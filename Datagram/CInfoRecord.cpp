#include "pch.h"
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "CInfoRecord.h"
#include "UserMessage.h"

DWORD WINAPI OnReceiveThread(LPVOID lparam);

DWORD WINAPI OnParseThread(LPVOID lparam);

CInfoRecord* CInfoRecord::m_pInstance = NULL;

STDATAGRAMQUEUE g_queDataGram;

ULONGLONG g_lRecvSizeSum = 0;

HANDLE g_hMutex = NULL;

CInfoRecord::CInfoRecord():m_bLockFlag(false), m_vehicleNumSum(0), m_vehicleNumAppend(0), m_datagramNum(0), m_hThreadRecv(NULL), m_hThreadParse(NULL),
							m_pDataGramPre(NULL), m_iPreSize(0)
{
	memset(m_chVin, 0, sizeof(m_chVin));
	memset(m_circleQue, 0, sizeof(m_circleQue));
	memset(m_dataType8, 0, sizeof(m_dataType8));
	memset(m_dataType9, 0, sizeof(m_dataType9));
	memset(&g_queDataGram, 0, sizeof(STDATAGRAMQUEUE));
	memset(&m_bTodayJoin, 0, sizeof(m_bTodayJoin));
}

void CInfoRecord::ReadVin()
{
	TCHAR tchPath[1024] = {};
	GetCurrentDirectory(1024, tchPath);

	char chPath[1024] = {};
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchPath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchPath, -1, chPath, iLength, NULL, NULL);
	memcpy(&chPath[iLength-1], "\\vinsort.txt", sizeof("\\vinsort.txt"));

	DWORD dwProgress = 1;
	FILE *fpRead = fopen(chPath, "r");
	if (NULL == fpRead)
	{
		dwProgress = 101;
		PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		return;
	}

	PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);

	while (!feof(fpRead))
	{
		char strVin[1024] = {};
		fgets(strVin, 1024, fpRead);

		memcpy(m_chVin[m_vehicleNumSum], strVin, VIN_LENGTH);
		m_vehicleNumSum += 1;
	}

	dwProgress = 10;
	PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);

	for (long i=0; i< m_vehicleNumSum; i++)
	{
		STRECVDATA* pElem = (STRECVDATA*)malloc(sizeof(STRECVDATA) * QUEUE_SIZE);
		if (NULL != pElem)
		{
			m_circleQue[i].pElem = pElem;
			memset(m_circleQue[i].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);
		}

		if (i == m_vehicleNumSum-1)
		{
			dwProgress = 100;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum*90/100)
		{
			dwProgress = 90;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 80 / 100)
		{
			dwProgress = 80;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 70 / 100)
		{
			dwProgress = 70;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 60 / 100)
		{
			dwProgress = 60;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 50 / 100)
		{
			dwProgress = 50;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 40 / 100)
		{
			dwProgress = 40;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 30 / 100)
		{
			dwProgress = 30;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
		else if (i == m_vehicleNumSum * 15 / 100)
		{
			dwProgress = 15;
			PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
		}
	}

	fclose(fpRead);

	Sleep(500);
	dwProgress = 101;
	PostMessage(m_hWnd, UM_LOADVINS, (WPARAM)&dwProgress, NULL);
}

void CInfoRecord::WriteVin()
{
	TCHAR tchPath[1024] = {};
	GetCurrentDirectory(1024, tchPath);
	char chPath[1024] = {};
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchPath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchPath, -1, chPath, iLength, NULL, NULL);
	memcpy(&chPath[iLength - 1], "\\vinsort.txt", sizeof("\\vinsort.txt"));

	FILE *fpWrite = fopen(chPath, "wb+");
	for (long i = 0; i < m_vehicleNumSum; i++)
	{
		fprintf(fpWrite, "%s", m_chVin[i]);
		if (i != m_vehicleNumSum - 1)
			fprintf(fpWrite, "\n");
	}
	fclose(fpWrite);
}

long CInfoRecord::FindVinPos(uint8_t pVin[])
{
	long left = 0;
	long right = m_vehicleNumSum - 1;
	long mid = -1;

	while (left <= right)
	{
		mid = (left + right) / 2;
		if (memcmp((char*)pVin, (char*)m_chVin[mid], VIN_LENGTH) < 0)
		{
			right = mid - 1;
		}
		else if (memcmp((char*)pVin, (char*)m_chVin[mid], VIN_LENGTH) > 0)
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

long CInfoRecord::InsertVinAndSort(uint8_t pVin[])
{
	if (m_vehicleNumSum >= MAX_VEHICLENUM)
	{
		return -1;
	}

	for (int i = 0; i < VIN_LENGTH; i++)
	{
		if (pVin[i] < '0' || (pVin[i] > '9' && pVin[i] < 'A') || (pVin[i] > 'Z' && pVin[i] < 'a') || pVin[i]>'z')
			return -1;
	}

	while (m_bLockFlag)
	{
		printf("bLock");
	}

	STRECVDATA* pElem = (STRECVDATA*)malloc(sizeof(STRECVDATA) * QUEUE_SIZE);
	if (NULL == pElem)
	{
		return -1;
	}

	if (m_vehicleNumSum == 0)
	{
		memcpy(m_chVin[0], pVin, VIN_LENGTH);
		m_vehicleNumSum += 1;
		m_vehicleNumAppend += 1;
		m_bTodayJoin[0] = true;

		m_circleQue[0].pElem = pElem;
		memset(m_circleQue[0].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);

		return 0;
	}

	long iLeft = 0;
	long iRight = m_vehicleNumSum - 1;

	while (iLeft <= iRight)
	{
		long iMid = (iLeft + iRight) / 2;
		if (memcmp((char*)pVin, (char*)m_chVin[iMid], VIN_LENGTH) > 0)
		{
			iLeft = iMid + 1;
		}
		else
		{
			iRight = iMid - 1;
		}
	}

	for (long i= m_vehicleNumSum-1; i>=iLeft; i--)
	{
		memcpy(m_chVin[i+1], m_chVin[i], VIN_LENGTH + 1);

		m_circleQue[i+1].pElem = m_circleQue[i].pElem;
		m_circleQue[i+1].front = m_circleQue[i].front;
		m_circleQue[i+1].rear = m_circleQue[i].rear;

		m_dataType8[i+1] = m_dataType8[i];
		m_dataType9[i+1] = m_dataType9[i];

		m_bTodayJoin[i+1] = m_bTodayJoin[i];
	}

	memcpy(m_chVin[iLeft], (char*)pVin, VIN_LENGTH + 1);

	m_circleQue[iLeft].pElem = pElem;
	memset(m_circleQue[iLeft].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);
	m_circleQue[iLeft].front = 0;
	m_circleQue[iLeft].rear = 0;

	memset(&m_dataType8[iLeft], 0, sizeof(STRECVDATATYPE8));
	m_dataType8[iLeft].pF8_1 = NULL;

	memset(&m_dataType9[iLeft], 0, sizeof(STRECVDATATYPE9));
	m_dataType9[iLeft].pF9_1 = NULL;

	m_bTodayJoin[iLeft] = 1;

	m_vehicleNumSum += 1;
	m_vehicleNumAppend += 1;

	return iLeft;
}

long CInfoRecord::GetTodayJoinCount()
{
// 	DWORD exitCode;
// 	BOOL ret = ::GetExitCodeThread(m_hThreadRecv, &exitCode);
// 	if (exitCode != STILL_ACTIVE)
// 	{
// 		RestartThread();
// 		return 0;
// 	}

	long iJoinCount = 0;
	for (long i=0; i<m_vehicleNumSum; i++)
	{
		if (m_bTodayJoin[i])
			iJoinCount += 1;
	}

	return iJoinCount;
}

bool CInfoRecord::QueryLatestInfo(uint8_t pVin[], STRECVDATA &stData)
{
	m_bLockFlag = true;
	long pos = FindVinPos(pVin);
	if (pos < 0)
	{
		m_bLockFlag = false;
		return false;
	}

	long rear = m_circleQue[pos].rear;
	memcpy(&stData, &m_circleQue[pos].pElem[rear-1], sizeof(STRECVDATA));

	m_bLockFlag = false;

	return true;
}

long CInfoRecord::GetQueInfo(STCIRCLEQUEUE circleQue[])
{
	m_bLockFlag = true;
	memcpy(circleQue, m_circleQue, sizeof(m_circleQue));
	m_bLockFlag = false;

	return m_vehicleNumSum;
}

void CInfoRecord::GetVinInfo(uint8_t chVin[][VIN_LENGTH + 1])
{
	m_bLockFlag = true;
	memcpy(chVin, m_chVin, sizeof(m_chVin));
	m_bLockFlag = false;
}

void CInfoRecord::RecordInfo(long pos, STRECVDATA& stRecv)
{
	if (pos >= m_vehicleNumSum || pos < 0)
	{
		return;
	}

	while (m_bLockFlag)
	{
		printf("bLock");
	}

	if (NULL == m_circleQue[pos].pElem)
		return;

	if ((m_circleQue[pos].rear + 1) % QUEUE_SIZE == m_circleQue[pos].front)
	{
		//队列满，队头元素出队
		long front = m_circleQue[pos].front;
		memset(&m_circleQue[pos].pElem[front], 0, sizeof(STCIRCLEQUEUE));
		m_circleQue[pos].front = (m_circleQue[pos].front + 1) % QUEUE_SIZE;
	}

	long rear = m_circleQue[pos].rear;
	memcpy(&m_circleQue[pos].pElem[rear], &stRecv, sizeof(STRECVDATA));
	m_circleQue[pos].rear = (m_circleQue[pos].rear + 1) % QUEUE_SIZE;

	m_bTodayJoin[pos] = 1;
}

long CInfoRecord::RecordInfoType8(long pos, const char* pRecv)
{
	if (pos >= m_vehicleNumSum || pos < 0)
		return 0;

	if (*pRecv == 0 || *pRecv > 250)
		return 0;

	while (m_bLockFlag)
	{
		printf("bLock");
	}

	long offset = 0;
	uint8_t uchar;
	uint16_t ushortsw;

	//可充电储能子系统个数(目前只有1个)
	uchar = *pRecv;
	m_dataType8[pos].F8_0 = *pRecv;
	offset += 1;

	if (m_dataType8[pos].F8_0 == 0)
	{
		return offset;
	}

	if (NULL == m_dataType8[pos].pF8_1)
	{
		m_dataType8[pos].pF8_1 = (RecvDataType8_1*)malloc(sizeof(RecvDataType8_1));
		if (NULL == m_dataType8[pos].pF8_1)
		{
			return 0;
		}
		memset(m_dataType8[pos].pF8_1, 0, sizeof(RecvDataType8_1));
	}

	//可充电储能子系统号
	m_dataType8[pos].pF8_1->F8_1_0 = *(pRecv + offset);
	offset += 1;

	//可充电储能装置电压
	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_1 = ushortsw;
	offset += 2;

	//可充电储能装置电流
	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_2 = ushortsw;
	offset += 2;

	//单体电池总数
	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_3 = ushortsw;
	offset += 2;

	//本帧起始电池序号
	uchar = *(pRecv + offset);
	uint16_t iSequence1st = uchar * 256;
	uchar = *(pRecv + offset + 1);
	iSequence1st += uchar;
	offset += 2;

	//本帧单体电池总数
	uint8_t iCellNum = *(pRecv + offset);
	offset += 1;

	//单体电池电压列表
	RecvDataType8_1_4* pNode = NULL;
	if (NULL != m_dataType8[pos].pF8_1->pF8_1_4)
	{
		RecvDataType8_1_4* pLast = NULL;
		pNode = m_dataType8[pos].pF8_1->pF8_1_4;

		while (NULL != pNode)
		{
			if (pNode->pNext == NULL)
			{
				pLast = pNode;
			}

			if (pNode->F8_1_4_1 == iSequence1st)
			{
				//本帧起始电池序号存在，更改电压值

				if (pNode->F8_1_4_2 != iCellNum)
				{
					//电池总数有变，重新分配
					if (NULL != pNode->pF8_1_4_3)
					{
						free(pNode->pF8_1_4_3);
						pNode->pF8_1_4_3 = NULL;
					}

					if (iCellNum > 0)
						pNode->pF8_1_4_3 = (uint16_t*)malloc(iCellNum * sizeof(uint16_t));
				}

				pNode->F8_1_4_2 = iCellNum;
				break;
			}

			pNode = pNode->pNext;
		}

		if (NULL == pNode)
		{
			RecvDataType8_1_4* pNew = (RecvDataType8_1_4*)malloc(sizeof(RecvDataType8_1_4));
			pNew->pNext = NULL;
			pNew->F8_1_4_1 = iSequence1st;
			pNew->F8_1_4_2 = iCellNum;
			pNew->pF8_1_4_3 = NULL;
			if (iCellNum > 0)
				pNew->pF8_1_4_3 = (uint16_t*)malloc(iCellNum * sizeof(uint16_t));

			pLast->pNext = pNew;
			pNode = pNew;
		}
	}
	else
	{
		m_dataType8[pos].pF8_1->pF8_1_4 = (RecvDataType8_1_4*)malloc(sizeof(RecvDataType8_1_4));
		m_dataType8[pos].pF8_1->pF8_1_4->pNext = NULL;
		m_dataType8[pos].pF8_1->pF8_1_4->F8_1_4_1 = iSequence1st;
		m_dataType8[pos].pF8_1->pF8_1_4->F8_1_4_2 = iCellNum;
		m_dataType8[pos].pF8_1->pF8_1_4->pF8_1_4_3 = NULL;
		if (iCellNum > 0)
			m_dataType8[pos].pF8_1->pF8_1_4->pF8_1_4_3 = (uint16_t*)malloc(iCellNum * sizeof(uint16_t));

		pNode = m_dataType8[pos].pF8_1->pF8_1_4;
	}

	for (uint8_t i = 0; i < iCellNum; i++)
	{
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;

		pNode->pF8_1_4_3[i] = ushortsw;
		offset += 2;
	}

	return offset;
}

long CInfoRecord::RecordInfoType9(long pos, const char* pRecv)
{
	if (pos >= m_vehicleNumSum || pos < 0)
		return 0;

	if (*pRecv == 0)
		return 0;

	while (m_bLockFlag)
	{
		printf("bLock");
	}

	if (NULL != m_dataType9[pos].pF9_1)
	{
		if (NULL != m_dataType9[pos].pF9_1->pF9_1_2)
		{
			free(m_dataType9[pos].pF9_1->pF9_1_2);
			m_dataType9[pos].pF9_1->pF9_1_2 = NULL;
		}

		free(m_dataType9[pos].pF9_1);
		m_dataType9[pos].pF9_1 = NULL;
	}

	long offset = 0;
	uint8_t uchar;

	//可充电储能子系统个数(目前只有1个)
	m_dataType9[pos].F9_0 = *pRecv;
	offset += 1;

	m_dataType9[pos].pF9_1 = (RecvDataType9_1*)malloc(sizeof(RecvDataType9_1));
	if (NULL == m_dataType9[pos].pF9_1)
	{
		return 0;
	}

	memset(m_dataType9[pos].pF9_1, 0, sizeof(RecvDataType9_1));

	//可充电储能子系统号
	m_dataType9[pos].pF9_1->F9_1_0 = *(pRecv + offset);
	offset += 1;

	m_dataType9[pos].pF9_1->pF9_1_2 = NULL;

	//可充电储能温度探针个数
	uchar = *(pRecv + offset);
	uint16_t iProbNum = uchar * 256;
	uchar = *(pRecv + offset + 1);
	iProbNum += uchar;
	m_dataType9[pos].pF9_1->F9_1_1 = iProbNum;
	offset += 2;

	m_dataType9[pos].pF9_1->pF9_1_2 = (uint8_t*)malloc(m_dataType9[pos].pF9_1->F9_1_1 * sizeof(uint8_t));
	if (NULL == m_dataType9[pos].pF9_1->pF9_1_2)
	{
		free(m_dataType9[pos].pF9_1);
		m_dataType9[pos].pF9_1 = NULL;
		return offset;
	}
	memset(m_dataType9[pos].pF9_1->pF9_1_2, 0, m_dataType9[pos].pF9_1->F9_1_1 * sizeof(uint8_t));

	//可充电储能子系统各温度探针检测到的温度值
	for (uint16_t j = 0; j < iProbNum; j++)
	{
		m_dataType9[pos].pF9_1->pF9_1_2[j] = *(pRecv + offset);
		offset += 1;
	}

	return offset;
}

void CInfoRecord::OnMsgParse(char recvData[], int recvSize)
{
	int iLatestOffset = 0;
	PSTDATABUFFGRAM pNode = NULL;

	bool bLastToEnd = false;	//最后一条报文是否达到末尾
	int iCurOffset = 0;
	int iStartOffset = 0;

	//拆分出每条报文
	while (iLatestOffset < recvSize - 1)
	{
		bLastToEnd = false;
		if (iLatestOffset <= recvSize - 2 && memcmp(&recvData[iLatestOffset], "##", 2) != 0)
		{
			m_iPreSize += 1;
			iLatestOffset += 1;
			continue;
		}

		if (m_pDataGramPre != NULL)
		{
			if (m_iPreSize > 0)
			{
				//与前一条消息的最后一条报文衔接
				int dataSize = m_pDataGramPre->recvSize;
				memcpy(&m_pDataGramPre->recvData[dataSize], &recvData[0], m_iPreSize);
				m_pDataGramPre->recvSize += m_iPreSize;
				if (m_pDataGramPre->recvSize > 20000 || m_pDataGramPre->recvSize < 0)
				{
					printf("");
				}
			}

			WaitForSingleObject(g_hMutex, INFINITE);
			if (g_queDataGram.front == NULL)
			{
				g_queDataGram.rear = m_pDataGramPre;
				g_queDataGram.front = g_queDataGram.rear;
			}
			else
			{
				g_queDataGram.rear->pNext = m_pDataGramPre;
				g_queDataGram.rear = m_pDataGramPre;
			}
			ReleaseMutex(g_hMutex);

			m_datagramNum += 1;
		}

		m_iPreSize = 0;
		m_pDataGramPre = NULL;

		iStartOffset = iLatestOffset;
		iCurOffset = iLatestOffset;
		iCurOffset += 4;
		//起始符、命令单元
		if (iCurOffset >= recvSize - 1)
			break;
		iLatestOffset += 4;

		//vin码
		iCurOffset = iLatestOffset;
		iCurOffset += 17;
		if (iCurOffset >= recvSize - 1)
			break;
		iLatestOffset += 17;

		//加密方式
		iCurOffset = iLatestOffset;
		iCurOffset += 1;
		if (iCurOffset >= recvSize - 1)
			break;
		iLatestOffset += 1;

		//数据单元长度
		iCurOffset = iLatestOffset;
		iCurOffset += 2;
		if (iCurOffset >= recvSize - 1)
			break;

		UCHAR uchar = recvData[iLatestOffset];
		USHORT iDataLen = uchar * 256;
		uchar = recvData[iLatestOffset + 1];
		iDataLen += uchar;
		iLatestOffset += 2;

		//数据单元
		iCurOffset = iLatestOffset;
		iCurOffset += iDataLen;
		if (iCurOffset >= recvSize - 1)
			break;

		iLatestOffset += iDataLen;

		//校验码
		iCurOffset = iLatestOffset;
		iCurOffset += 1;
		if (iCurOffset >= recvSize - 1)
			break;

		iLatestOffset += 1;
		bLastToEnd = true;

		int iBuffLen = iLatestOffset - iStartOffset;
		pNode = (PSTDATABUFFGRAM)malloc(sizeof(STDATABUFFGRAM));
		memset(pNode, 0, sizeof(STDATABUFFGRAM));
		memcpy(pNode->recvData, &recvData[iStartOffset], iBuffLen * sizeof(char));
		pNode->recvSize = iBuffLen;
		pNode->pNext = NULL;
		if (iBuffLen > 20000 || iBuffLen<0)
		{
			printf("");
		}

		WaitForSingleObject(g_hMutex, INFINITE);
		if (g_queDataGram.front == NULL)
		{
			g_queDataGram.rear = pNode;
			g_queDataGram.front = g_queDataGram.rear;
		}
		else
		{
			g_queDataGram.rear->pNext = pNode;
			g_queDataGram.rear = pNode;
		}
		ReleaseMutex(g_hMutex);

		m_datagramNum += 1;
	}

	if (!bLastToEnd)
	{
		int iBuffLen = recvSize - iStartOffset;
		m_pDataGramPre = (PSTDATABUFFGRAM)malloc(sizeof(STDATABUFFGRAM));
		memset(m_pDataGramPre, 0, sizeof(STDATABUFFGRAM));
		memcpy(m_pDataGramPre->recvData, &recvData[iStartOffset], iBuffLen * sizeof(char));
		m_pDataGramPre->recvSize = iBuffLen;
		m_pDataGramPre->pNext = NULL;
	}

	g_lRecvSizeSum += recvSize;
}

bool CInfoRecord::OnRealTimeRecv(HWND hWnd, sockaddr_in serAddr)
{
	if (NULL != m_hThreadRecv)
	{
		CloseHandle(m_hThreadRecv);
		m_hThreadRecv = NULL;
	}

	if (NULL != m_hThreadParse)
	{
		CloseHandle(m_hThreadParse);
		m_hThreadParse = NULL;
	}

	if (NULL != g_hMutex)
	{
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}

	m_hWnd = hWnd;

	SOCKET pSocket = CInfoSocket::GetInstance()->OnConnect(serAddr, hWnd);
	if (pSocket == INVALID_SOCKET)
	{
		return false;
	}

	g_hMutex = CreateMutex(NULL, FALSE, _T("InfoRecord"));
	if (NULL == g_hMutex)
	{
		return false;
	}

	OpenMutex(MUTEX_ALL_ACCESS, true, NULL);

#ifndef _DEBUG
	//异步套接字实时接收，创建线程实时解析
 	DWORD dwThreadIdParse;
 	m_hThreadParse = CreateThread(NULL, NULL, OnParseThread, hWnd, 0, &dwThreadIdParse);

	if (NULL == m_hThreadParse)
		CInfoSocket::GetInstance()->OnClose();
#else
	//异步套接字定量接收存放文件，再调用CFileParse离线解析
	DWORD dwThreadIdRecv;
 	m_hThreadRecv = CreateThread(NULL, NULL, OnReceiveThread, hWnd, 0, &dwThreadIdRecv);

	if (NULL == m_hThreadRecv)
		CInfoSocket::GetInstance()->OnClose();
#endif

	return true;
}

bool CInfoRecord::OnStopRecv()
{
	OnClose();

	return true;
}

void CInfoRecord::OnClearDataGram()
{
	WaitForSingleObject(g_hMutex, INFINITE);
	while (g_queDataGram.front != NULL)
	{
		PSTDATABUFFGRAM pNode = g_queDataGram.front;
		g_queDataGram.front = g_queDataGram.front->pNext;

		free(pNode);
		pNode = NULL;
	}

	memset(&g_queDataGram, 0, sizeof(STDATAGRAMQUEUE));
	ReleaseMutex(g_hMutex);
}

void CInfoRecord::OnReset()
{
	for (long i = 0; i < m_vehicleNumSum; i++)
	{
		if (m_circleQue[i].pElem != NULL)
		{
// 			free(m_circleQue[i].pElem);
// 			m_circleQue[i].pElem = NULL;
			memset(m_circleQue[i].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);
			m_circleQue[i].front = 0;
			m_circleQue[i].rear = 0;
		}

		if (m_dataType8[i].pF8_1 != NULL)
		{
			if (NULL != m_dataType8[i].pF8_1->pF8_1_4)
			{
				RecvDataType8_1_4* pNode = m_dataType8[i].pF8_1->pF8_1_4;
				while (pNode != NULL)
				{
					RecvDataType8_1_4* pCur = pNode;
					pNode = pNode->pNext;
					free(pCur);
				}
				m_dataType8[i].pF8_1->pF8_1_4 = NULL;
			}

			free(m_dataType8[i].pF8_1);
			m_dataType8[i].pF8_1 = NULL;
		}

		if (m_dataType9[i].pF9_1 != NULL)
		{
			if (NULL != m_dataType9[i].pF9_1->pF9_1_2)
			{
				free(m_dataType9[i].pF9_1->pF9_1_2);
				m_dataType9[i].pF9_1->pF9_1_2 = NULL;
			}

			free(m_dataType9[i].pF9_1);
			m_dataType9[i].pF9_1 = NULL;
		}
	}

	//memset(m_circleQue, 0, sizeof(m_circleQue));
	memset(m_dataType8, 0, sizeof(m_dataType8));
	memset(m_dataType9, 0, sizeof(m_dataType9));

	memset(m_bTodayJoin, 0, sizeof(m_bTodayJoin));

	m_bLockFlag = false;
	m_datagramNum = 0;
	g_lRecvSizeSum = 0;
	m_vehicleNumAppend = 0;
}

void CInfoRecord::OnClose()
{
	CInfoSocket::GetInstance()->OnClose();
}

DWORD WINAPI OnReceiveThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;

	ULONGLONG saveSize = 0;
	FILE *fpWrite = fopen("datagram.dat", "wb+");

	while (1)
	{
		if (CInfoSocket::GetInstance()->CheckClose())
		{
			fclose(fpWrite);
			PostMessage(hWnd, UM_STOPRECV, NULL, 0);
			break;
		}

		if (saveSize >= (PARSE_FILE_SIZE*1.9))
		{
			fclose(fpWrite);
			PostMessage(hWnd, UM_FILEPARSE, NULL, 0);
			break;
		}

		char recvData[BUFFER_SIZE] = {};
		INT recvSize = CInfoSocket::GetInstance()->OnReceive(recvData);
		if (recvSize <= 0)
		{
			SOCKET pSocket = CInfoSocket::GetInstance()->OnReConnect();
			continue;
		}

		saveSize += recvSize;
		fwrite(recvData, recvSize, 1, fpWrite);
	}

	return 0;
}

DWORD WINAPI OnParseThread(LPVOID lparam)
{
	if (CInfoRecord::GetInstance()->GetVehicleNumSum() == 0)
	{
		CInfoRecord::GetInstance()->ReadVin();
	}

	HWND hWnd = (HWND)lparam;
	int num = 0;

	SYSTEMTIME st;
	memset(&st, 0, sizeof(st));
	SYSTEMTIME datePre;
	GetLocalTime(&datePre);

	uint8_t uchar;
	uint16_t ushortsw;
	uint32_t uintsw;

	while (1)
	{
		if (CInfoSocket::GetInstance()->CheckClose())
		{
			CInfoRecord::GetInstance()->WriteVin();
			PostMessage(hWnd, UM_STOPPARSE, NULL, 0);
			break;
		}

		if (CInfoRecord::GetInstance()->GetVehicleNumSum() >= MAX_VEHICLENUM)
		{
			CInfoRecord::GetInstance()->WriteVin();
			PostMessage(hWnd, UM_STOPPARSE, NULL, 0);
			break;
		}

		if (g_queDataGram.front == NULL)
		{
			continue;
		}

		GetLocalTime(&st);

		//触发主线程统计历史数据
// 		if (datePre.wDay != st.wDay || datePre.wMonth != st.wMonth || datePre.wYear != st.wYear)
// 		{
// 			SendMessage(hWnd, UM_HISTORY, (WPARAM)&st.wDayOfWeek, NULL);
// 		}
// 
// 		datePre.wYear = st.wYear;
// 		datePre.wMonth = st.wMonth;
// 		datePre.wDay = st.wDay;

		char recvData[BUFFER_SIZE] = {};
		INT recvSize = 0;

		WaitForSingleObject(g_hMutex, INFINITE);
		PSTDATABUFFGRAM pNode = g_queDataGram.front;
		memcpy(recvData, pNode->recvData, pNode->recvSize * sizeof(char));
		recvSize = pNode->recvSize;

		//报文结点出队
		g_queDataGram.front = g_queDataGram.front->pNext;
		free(pNode);
		pNode = NULL;
		if (g_queDataGram.front == NULL)
			g_queDataGram.rear = NULL;
		ReleaseMutex(g_hMutex);

		INT latestOffset = 0;
		if (memcmp(&recvData[latestOffset], "###", 3) == 0)
		{
			latestOffset += 1;
		}

		while (latestOffset < recvSize)
		{
			if (memcmp(&recvData[latestOffset], "##", 2) != 0)
			{
				latestOffset += 1;
			}
			else
			{
				break;
			}
		}

		//起始符号
		latestOffset += 2;

		//命令标识
		uint8_t iCmdFlag = recvData[latestOffset];
		latestOffset += 1;
		if (iCmdFlag != 2 && iCmdFlag != 3)
			continue;

		//应答标识
		uint8_t iRespFlag = recvData[latestOffset];
		latestOffset += 1;
		if (iRespFlag != 254)
			continue;

		//vin码
		uint8_t strVin[VIN_LENGTH + 1] = {};
		memcpy(strVin, &recvData[latestOffset], VIN_LENGTH);
		latestOffset += 17;

		//数据单元加密方式
		latestOffset += 1;

		//数据单元长度
		uchar = recvData[latestOffset];
		uint16_t iDataLen = uchar * 256;
		uchar = recvData[latestOffset + 1];
		iDataLen += uchar;
		latestOffset += 2;

		STRECVDATA infoData;
		memset(&infoData, 0, sizeof(infoData));

	/*******数据单元格式 实时信息上报*******/

		//数据采集时间(年月日时分秒)
		memcpy(infoData.F8_0, &recvData[latestOffset], 6);

		long pos = CInfoRecord::GetInstance()->FindVinPos(strVin);
		if (pos < 0)
		{
			pos = CInfoRecord::GetInstance()->InsertVinAndSort(strVin);

			//vin码无效，丢弃该报文
			if (pos < 0)
				continue;
		}

		latestOffset += 6;

		bool bSetFlag[10] = {};
		INT curOffset = latestOffset;
		INT leftOffset = iDataLen - 6;

		while (latestOffset < (iDataLen - 6) + curOffset)
		{
			if (leftOffset <= 1)
			{
				break;
			}

			uint8_t infoType = recvData[latestOffset];
			leftOffset -= 1;
			if (leftOffset <= 0)
				break;
			latestOffset += 1;

			if (infoType == 1 && !bSetFlag[1])
			{
				bSetFlag[1] = true;

				// 0__车辆状态
				uchar = recvData[latestOffset];
				infoData.F1_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__充电状态
				uchar = recvData[latestOffset];
				infoData.F1_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__运行模式
				uchar = recvData[latestOffset];
				infoData.F1_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 3__车速
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 5__累计里程
				uchar = recvData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = recvData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = recvData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = recvData[latestOffset + 3];
				uintsw += uchar;
				infoData.F1_4 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 9__总电压
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 11__总电流
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_6 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 13__SOC
				uchar = recvData[latestOffset];
				infoData.F1_7 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 14__DC/DC状态
				uchar = recvData[latestOffset];
				infoData.F1_8 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 15__挡位
				uchar = recvData[latestOffset];
				infoData.F1_9 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 16__绝缘电阻
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_10 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;;

				// 18__加速踏板行程值
				uchar = recvData[latestOffset];
				infoData.F1_11 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 19__制动踏板状态
				uchar = recvData[latestOffset];
				infoData.F1_12 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 2 && !bSetFlag[2])
			{
				bSetFlag[2] = true;

				//跳过驱动电机个数、驱动电机序号
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 0__驱动电机状态
				uchar = recvData[latestOffset];
				infoData.F2_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__驱动电机控制器温度
				uchar = recvData[latestOffset];
				infoData.F2_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__驱动电机转速
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_2 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 4__驱动电机转矩
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 6__驱动电机温度
				uchar = recvData[latestOffset];
				infoData.F2_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 7__电机控制器输入电压
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 9__直流控制器直流母线电流
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_6 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 3)
			{
				//燃料电池电压
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池电流
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池温度探针总数
				uchar = recvData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = recvData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//探针温度值
				for (uint16_t i = 0; i < iProbNum; i++)
				{
					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}

				if (leftOffset <= 0)
					break;

				//氢系统中最高温度
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢系统中最高温度探针代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高浓度
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高浓度传感器代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高压力
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高压力传感器代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//高压DC/DC状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 4)
			{
				//发动机状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//曲轴转速
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 5)
			{
				//跳过定位状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__经度
				uchar = recvData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = recvData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = recvData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = recvData[latestOffset + 3];
				uintsw += uchar;
				infoData.F5_0 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 4__纬度
				uchar = recvData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = recvData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = recvData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = recvData[latestOffset + 3];
				uintsw += uchar;
				infoData.F5_1 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;
			}
			else if (infoType == 6)
			{
				//跳过最高电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__最高电压单体号
				uchar = recvData[latestOffset];
				infoData.F6_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__电池单体电压最高值
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_1 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最低电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 4__最低电压单体号
				uchar = recvData[latestOffset];
				infoData.F6_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 6__电池单体电压最低值
				uchar = recvData[latestOffset];
				ushortsw = uchar * 256;
				uchar = recvData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最高温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 8__最高温度探针号
				uchar = recvData[latestOffset];
				infoData.F6_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 9__最高温度值
				uchar = recvData[latestOffset];
				infoData.F6_5 = uchar;;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//跳过最低温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 10__最低温度探针号
				uchar = recvData[latestOffset];
				infoData.F6_6 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 11__最低温度值
				uchar = recvData[latestOffset];
				infoData.F6_7 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 7)
			{
				//跳过最高报警等级
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//通用报警标志
				uchar = recvData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = recvData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = recvData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = recvData[latestOffset + 3];
				uintsw += uchar;
				infoData.F7_0 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				//可充电储能装置故障总数
				uchar = recvData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//驱动电机故障总数
				uchar = recvData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//发动机故障总数
				uchar = recvData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//其它故障总数
				uchar = recvData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}
			}
			else if (infoType == 8)
			{
				INT iLocalOffset = latestOffset;

				//可充电储能子系统个数(目前只有1个)
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能装置电压
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能装置电流
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//单体电池总数
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧起始电池序号;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧单体电池总数
				uint8_t iCellNum = recvData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//单体电池电压列表
				for (uint8_t i = 0; i < iCellNum; i++)
				{
					leftOffset -= 2;
					if (leftOffset <= 0)
						break;
					latestOffset += 2;
				}

				if (leftOffset <= 0)
					break;

				CInfoRecord::GetInstance()->RecordInfoType8(pos, &recvData[iLocalOffset]);
			}
			else if (infoType == 9)
			{
				INT iLocalOffset = latestOffset;

				//可充电储能子系统个数(目前只有1个);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能温度探针个数
				uchar = recvData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = recvData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能子系统各温度探针检测到的温度值
				for (uint16_t j = 0; j < iProbNum; j++)
				{
					if (latestOffset >= recvSize)
						break;

					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}

				if (leftOffset <= 0)
					break;

				CInfoRecord::GetInstance()->RecordInfoType9(pos, &recvData[iLocalOffset]);
			}
			else
			{
				break;
			}
		}

		if (bSetFlag[1] && bSetFlag[2])
		{
			CInfoRecord::GetInstance()->RecordInfo(pos, infoData);
		}

		if (infoData.F7_0 > 0)
		{
			if (CInfoSocket::GetInstance()->CheckClose())
				break;

			STALERTDATAPOST alertPost = {};
			memcpy(alertPost.chVin, strVin, sizeof(strVin));
			alertPost.F7_0 = infoData.F7_0;
			PostMessage(hWnd, UM_ALERT, (WPARAM)&alertPost, 0);
		}
	}

	return 0;
}