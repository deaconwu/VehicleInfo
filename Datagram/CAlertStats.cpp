//#include <thread>
#include "pch.h"
#include "CAlertStats.h"
#include "UserMessage.h"
#include "CInfoRecord.h"
#include <tchar.h>

DWORD WINAPI OnStatsAlertRankThread(LPVOID lparam);

DWORD WINAPI OnStatsAlertCategoryThread(LPVOID lparam);

static HANDLE g_hMutex = CreateMutex(NULL, FALSE, _T("AlertStats"));

CAlertStats* CAlertStats::m_pInstance = NULL;

static STCIRCLEQUEUE g_circleQue[MAX_VEHICLENUM] = {};
static uint8_t g_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1];
static long g_vehicleNum = 0;

static long FindVinPos(uint8_t pVin[])
{
	long left = 0;
	long right = g_vehicleNum - 1;
	long mid = -1;

	while (left <= right)
	{
		mid = (left + right) / 2;
		if (memcmp((char*)pVin, (char*)g_chVin[mid], VIN_LENGTH) < 0)
		{
			right = mid - 1;
		}
		else if (memcmp((char*)pVin, (char*)g_chVin[mid], VIN_LENGTH) > 0)
		{
			left = mid + 1;
		}
		else
		{
			return mid;
		}
	}

	return -1;
}

static bool CheckAlertFlag(uint32_t iData, int iType)
{
	uint8_t iFlag = iData;

	switch (iType)
	{
	case 0:
	{
		iFlag = iData % 2;
		break;
	}
	case 1:
	{
		iFlag = (iData / 2) % 2;
		break;
	}
	case 2:
	{
		iFlag = (iData / 4) % 2;
		break;
	}
	case 3:
	{
		iFlag = (iData / 8) % 2;
		break;
	}
	case 4:
	{
		iFlag = (iData / 16) % 2;
		break;
	}
	case 5:
	{
		iFlag = (iData / 32) % 2;
		break;
	}
	case 6:
	{
		iFlag = (iData / 64) % 2;
		break;
	}
	case 7:
	{
		iFlag = (iData / 128) % 2;
		break;
	}
	case 8:
	{
		iFlag = (iData / 256) % 2;
		break;
	}
	case 9:
	{
		iFlag = (iData / 512) % 2;
		break;
	}
	case 10:
	{
		iFlag = (iData / 1024) % 2;
		break;
	}
	case 11:
	{
		iFlag = (iData / 2048) % 2;
		break;
	}
	case 12:
	{
		iFlag = (iData / 4096) % 2;
		break;
	}
	case 13:
	{
		iFlag = (iData / 8192) % 2;
		break;
	}
	case 14:
	{
		iFlag = (iData / 16384) % 2;
		break;
	}
	case 15:
	{
		iFlag = (iData / 32768) % 2;
		break;
	}
	case 16:
	{
		iFlag = (iData / 65536) % 2;
		break;
	}
	case 17:
	{
		iFlag = (iData / 131072) % 2;
		break;
	}
	case 18:
	{
		iFlag = (iData / 262144) % 2;
		break;
	}
	default:
		break;
	}

	return (iFlag > 0);
}

static void RankSort(int iType, STMSGALERTRANKSEQ& msgSeq)
{
	WaitForSingleObject(g_hMutex, INFINITE);

	memset(g_chVin, 0, sizeof(g_chVin));
	memset(g_circleQue, 0, sizeof(g_circleQue));
	CInfoRecord::GetInstance()->GetVinInfo(g_chVin);
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(g_circleQue);
	g_vehicleNum = vehicleNum;

	PSTALERTDATALINK pNode = NULL;
	PSTALERTDATALINK pLast = NULL;
	uint8_t iRankNum = 0;

	for (long i = 0; i < vehicleNum; i++)
	{
		uint32_t iAlertTimes = 0;

		long rear = g_circleQue[i].rear;
		for (long j = rear - 1; j >= 0; j--)
		{
			if (g_circleQue[i].pElem[j].F7_0 > 0 && iType < 0)
			{
				iAlertTimes += 1;
			}
			else if (CheckAlertFlag(g_circleQue[i].pElem[j].F7_0, iType))
			{
				iAlertTimes += 1;
			}
		}

		if (iAlertTimes > 0)	//与链表每个节点比较
		{
			if (pNode == NULL)
			{
				//链表为空
				pNode = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
				if (NULL == pNode)
					return;

				pNode->pPre = NULL;
				pNode->pNext = NULL;
				pLast = pNode;

				pLast->iAlertTimes = iAlertTimes;
				//CInfoRecord::GetInstance()->FetchVinCode(i, pLast->chVin);
				memcpy(pLast->chVin, g_chVin[i], (VIN_LENGTH + 1) * sizeof(uint8_t));
				iRankNum += 1;
			}
			else if (iRankNum < ALERTTIMES_REANK_NUM)
			{
				PSTALERTDATALINK pPrev = pLast;
				while (pPrev != NULL)
				{
					if (iAlertTimes <= pPrev->iAlertTimes)
					{
						PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
						pNew->iAlertTimes = iAlertTimes;
						memcpy(pNew->chVin, g_chVin[i], (VIN_LENGTH + 1) * sizeof(uint8_t));
						//CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
						pNew->pNext = NULL;

						if (pPrev->pNext != NULL)
						{
							pPrev->pNext->pPre = pNew;
							pNew->pNext = pPrev->pNext;
						}
						else
						{
							//新结点插入链尾
							pLast = pNew;
						}

						pNew->pPre = pPrev;
						pPrev->pNext = pNew;

						iRankNum += 1;
						break;
					}

					pPrev = pPrev->pPre;
				}

				if (pPrev == NULL)
				{
					//排名最大
					PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
					pNew->iAlertTimes = iAlertTimes;
					//CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
					memcpy(pNew->chVin, g_chVin[i], (VIN_LENGTH + 1) * sizeof(uint8_t));
					pNew->pPre = NULL;
					pNew->pNext = pNode;
					pNode->pPre = pNew;
					pNode = pNew;
					iRankNum += 1;
				}
			}
			else
			{
				if (iAlertTimes <= pLast->iAlertTimes)	//比不过最小的，排不上名
					continue;

				PSTALERTDATALINK pPrev = pLast;
				while (pPrev != NULL)
				{
					if (iAlertTimes <= pPrev->iAlertTimes)
					{
						PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
						pNew->iAlertTimes = iAlertTimes;
						//CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
						memcpy(pNew->chVin, g_chVin[i], (VIN_LENGTH + 1) * sizeof(uint8_t));
						pNew->pNext = NULL;

						if (pPrev->pNext != NULL)
						{
							pPrev->pNext->pPre = pNew;
							pNew->pNext = pPrev->pNext;
						}
						else
						{
							//新结点插入链尾
							pLast = pNew;
						}

						pNew->pPre = pPrev;
						pPrev->pNext = pNew;

						pLast = pLast->pPre;
						free(pLast->pNext);
						pLast->pNext = NULL;

						break;
					}

					pPrev = pPrev->pPre;
				}

				if (pPrev == NULL)
				{
					//排名最大
					PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
					pNew->iAlertTimes = iAlertTimes;
					//CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
					memcpy(pNew->chVin, g_chVin[i], (VIN_LENGTH + 1) * sizeof(uint8_t));
					pNew->pPre = NULL;
					pNew->pNext = pNode;
					pNode->pPre = pNew;
					pNode = pNew;

					pLast = pLast->pPre;
					free(pLast->pNext);
					pLast->pNext = NULL;
				}
			}
		}
	}

	while (pNode != NULL && pNode->iAlertTimes > 0)
	{
		memcpy(msgSeq.stNode[msgSeq.iNum].chVin, pNode->chVin, VIN_LENGTH * sizeof(uint8_t));
		msgSeq.stNode[msgSeq.iNum].iAlertTimes = pNode->iAlertTimes;

		PSTALERTDATALINK pDel = pNode;
		pNode = pNode->pNext;
		free(pDel);

		msgSeq.iNum++;
	}

	ReleaseMutex(g_hMutex);
}

static void AlertCategory(uint8_t pVin[], STMSGALERTCATEGORY &msgCategory)
{
	WaitForSingleObject(g_hMutex, INFINITE);

	memset(g_chVin, 0, sizeof(g_chVin));
	memset(g_circleQue, 0, sizeof(g_circleQue));
	CInfoRecord::GetInstance()->GetVinInfo(g_chVin);
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(g_circleQue);
	g_vehicleNum = vehicleNum;

	long iVinPos = FindVinPos(pVin);
	if (iVinPos < 0)
		return;

	int iType = -1;	//遍历每种报警类型

	while (iType <= 18)
	{
		uint32_t iAlertTimesSelf = 0;
		uint32_t iRank = 0;

		//统计该车辆各类报警次数
		long rear = g_circleQue[iVinPos].rear;
		for (long i = rear - 1; i >= 0; i--)
		{
			if (g_circleQue[iVinPos].pElem[i].F7_0 > 0 && iType < 0)
			{
				iAlertTimesSelf += 1;
			}
			else if (CheckAlertFlag(g_circleQue[iVinPos].pElem[i].F7_0, iType))
			{
				iAlertTimesSelf += 1;
			}
		}

		if (iAlertTimesSelf == 0)
		{
			iType += 1;
			continue;
		}

		iRank = 1;

		//遍历统计每辆车报警次数，得到该车辆所在每类报警的排名
		for (long i = 0; i < g_vehicleNum; i++)
		{
			if (i == iVinPos)
				continue;

			uint32_t iAlertTimesOther = 0;

			long rear = g_circleQue[i].rear;
			for (long j = rear - 1; j >= 0; j--)
			{
				if (g_circleQue[i].pElem[j].F7_0 > 0 && iType < 0)
				{
					iAlertTimesOther += 1;
				}
				else if (CheckAlertFlag(g_circleQue[i].pElem[j].F7_0, iType))
				{
					iAlertTimesOther += 1;
				}
			}

			if (iAlertTimesOther > iAlertTimesSelf)
			{
				iRank += 1;
			}
		}

		msgCategory.iAlertTimes[iType + 1] = iAlertTimesSelf;
		msgCategory.iRank[iType + 1] = iRank;

		iType += 1;
	}

	ReleaseMutex(g_hMutex);
}

CAlertStats* CAlertStats::GetInstance()
{
	if (0 == m_pInstance)
		m_pInstance = new CAlertStats;

	return m_pInstance;
}

CAlertStats::CAlertStats() : m_hThreadAlertRank(NULL), m_hThreadAlertCategory(NULL),
							m_bStopAlertRank(true), m_bStopAlertCategory(true)
{

}

CAlertStats::~CAlertStats()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CAlertStats::NotifyVim(uint8_t pVin[])
{ 
	memcpy(m_chVin, pVin, VIN_LENGTH * sizeof(uint8_t));
}

void CAlertStats::GetVin(uint8_t pVin[])
{
	memcpy(pVin, m_chVin, VIN_LENGTH * sizeof(uint8_t));
}

void CAlertStats::OnLaunchAlertRank(HWND hWnd, int iType)
{
	if (NULL != m_hThreadAlertRank)
	{
		CloseHandle(m_hThreadAlertRank);
		m_hThreadAlertRank = NULL;
	}

	m_bStopAlertRank = false;
	m_iType = iType;

	DWORD dwThreadId;
	m_hThreadAlertRank = CreateThread(NULL, NULL, OnStatsAlertRankThread, hWnd, 0, &dwThreadId);
}

void CAlertStats::OnLaunchAlertCategory(HWND hWnd, uint8_t pVin[])
{
	if (NULL != m_hThreadAlertCategory)
	{
		CloseHandle(m_hThreadAlertCategory);
		m_hThreadAlertCategory = NULL;
	}

	m_bStopAlertCategory = false;
	memcpy(m_chVin, pVin, VIN_LENGTH * sizeof(uint8_t));

	DWORD dwThreadId;
	m_hThreadAlertCategory = CreateThread(NULL, NULL, OnStatsAlertCategoryThread, hWnd, 0, &dwThreadId);
}

DWORD WINAPI OnStatsAlertRankThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;

	while (1)
	{
		if (CAlertStats::GetInstance()->GetStopAlertRankFlag())
		{
			break;
		}

		STMSGALERTRANKSEQ msgSeq;
		memset(&msgSeq, 0, sizeof(msgSeq));
		int iType = 0;
		CAlertStats::GetInstance()->GetType(iType);
		RankSort(iType, msgSeq);
		::SendMessage(hWnd, UM_ALERTRANK, (WPARAM)&msgSeq, 0);

		Sleep(1200);
	}

	return 0;
}

DWORD WINAPI OnStatsAlertCategoryThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;

	uint8_t pVin[VIN_LENGTH + 1] = {};
	CAlertStats::GetInstance()->GetVin(pVin);

	while (1)
	{
		if (CAlertStats::GetInstance()->GetStopAlertCategoryFlag())
		{
			break;
		}

		STMSGALERTCATEGORY msgCategory;
		memset(&msgCategory, 0, sizeof(msgCategory));

		AlertCategory(pVin, msgCategory);

		::SendMessage(hWnd, UM_ALERTCATEGORY, (WPARAM)&msgCategory, 0);

		Sleep(1200);
	}

	return 0;
}