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

HANDLE g_hMutex = CreateMutex(NULL, FALSE, _T("InfoRecord"));

long SetRecvData(const char* pRecv, STRECVDATA& stRecv, long leftOffset)
{
	uint8_t infoType = *pRecv;
	long offset = sizeof(infoType);
	uint8_t uchar;
	uint16_t ushort;
	uint16_t ushortsw;
	uint32_t uintsw;

	switch (infoType)
	{
	case 1:
	{
		stRecv.F1_0 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_0);
		stRecv.F1_1 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_1);
		stRecv.F1_2 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_2);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F1_3 = ushortsw;

		offset += sizeof(stRecv.F1_3);
		uchar = *(pRecv + offset);
		uintsw = uchar * 16777216;
		uchar = *(pRecv + offset + 1);
		uintsw += uchar * 65536;
		uchar = *(pRecv + offset + 2);
		uintsw += uchar * 256;
		uchar = *(pRecv + offset + 3);
		uintsw += uchar;
		stRecv.F1_4 = uintsw;

		offset += sizeof(stRecv.F1_4);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F1_5 = ushortsw;

		offset += sizeof(stRecv.F1_5);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F1_6 = ushortsw;

		offset += sizeof(stRecv.F1_6);
		stRecv.F1_7 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_7);
		stRecv.F1_8 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_8);
		stRecv.F1_9 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_9);
		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		stRecv.F1_10 = ushortsw;

		offset += sizeof(stRecv.F1_10);
		stRecv.F1_11 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_11);
		stRecv.F1_12 = *(pRecv + offset);

		offset += sizeof(stRecv.F1_12);
		break;
	}
	case 2:
	{
		offset += 2;	//跳过驱动电机个数、驱动电机序号
		stRecv.F2_0 = *(pRecv + offset);

		offset += sizeof(stRecv.F2_0);
		stRecv.F2_1 = *(pRecv + offset);

		offset += sizeof(stRecv.F2_1);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F2_2 = ushortsw;

		offset += sizeof(stRecv.F2_2);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F2_3 = ushortsw;

		offset += sizeof(stRecv.F2_3);
		stRecv.F2_4 = *(pRecv + offset);

		offset += sizeof(stRecv.F2_4);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F2_5 = ushortsw;

		offset += sizeof(stRecv.F2_5);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F2_6 = ushortsw;

		offset += sizeof(stRecv.F2_6);

		break;
	}
	case 5:
	{
		offset += 1; //跳过定位状态

		uchar = *(pRecv + offset);
		uintsw = uchar * 16777216;
		uchar = *(pRecv + offset + 1);
		uintsw += uchar * 65536;
		uchar = *(pRecv + offset + 2);
		uintsw += uchar * 256;
		uchar = *(pRecv + offset + 3);
		uintsw += uchar;
		stRecv.F5_0 = uintsw;

		offset += sizeof(stRecv.F5_0);
		uchar = *(pRecv + offset);
		uintsw = uchar * 16777216;
		uchar = *(pRecv + offset + 1);
		uintsw += uchar * 65536;
		uchar = *(pRecv + offset + 2);
		uintsw += uchar * 256;
		uchar = *(pRecv + offset + 3);
		uintsw += uchar;
		stRecv.F5_1 = uintsw;

		offset += sizeof(stRecv.F5_1);
		//stRecv.F5_2 = *(pRecv + offset);

		//offset += sizeof(stRecv.F5_2);
		break;
	}
	case 6:
	{
		//offset += 1;	//跳过最高电压电池子系统号
		uchar = *(pRecv + offset);
		ushortsw = uchar;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar * 256;
		stRecv.F6_0 = ushortsw;

		offset += sizeof(stRecv.F6_0);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F6_1 = ushortsw;

		//offset += 1;	//跳过最低电压电池子系统号
		offset += sizeof(stRecv.F6_1);
		uchar = *(pRecv + offset);
		ushortsw = uchar;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar * 256;
		stRecv.F6_2 = ushortsw;

		offset += sizeof(stRecv.F6_2);
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;
		stRecv.F6_3 = ushortsw;

		offset += 1;	//跳过最高温度子系统号
		offset += sizeof(stRecv.F6_3);
		stRecv.F6_4 = *(pRecv + offset);

		offset += sizeof(stRecv.F6_4);
		stRecv.F6_5 = *(pRecv + offset);

		offset += 1;	//跳过最低温度子系统号
		offset += sizeof(stRecv.F6_5);
		stRecv.F6_6 = *(pRecv + offset);

		offset += sizeof(stRecv.F6_6);
		stRecv.F6_7 = *(pRecv + offset);

		offset += sizeof(stRecv.F6_7);

		break;
	}
	case 7:
	{
		offset += 1; //跳过最高报警等级
		uchar = *(pRecv + offset);
		uintsw = uchar * 16777216;
		uchar = *(pRecv + offset + 1);
		uintsw += uchar * 65536;
		uchar = *(pRecv + offset + 2);
		uintsw += uchar * 256;
		uchar = *(pRecv + offset + 3);
		uintsw += uchar;
		stRecv.F7_0 = uintsw;
		offset += sizeof(stRecv.F7_0);

		uchar = *(pRecv + offset);	//可充电储能装置故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
			if (offset >= leftOffset)
				break;
		}

		uchar = *(pRecv + offset);	//驱动电机故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
			if (offset >= leftOffset)
				break;
		}

		uchar = *(pRecv + offset);	//发动机故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
			if (offset >= leftOffset)
				break;
		}

		uchar = *(pRecv + offset);	//其它故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
			if (offset >= leftOffset)
				break;
		}

		break;
	}
	default:
		break;
	}

	return offset;
}

void CInfoRecord::WriteVin()
{
	FILE *fpWrite = fopen("vinsort.txt", "wb+");
	for (long i = 0; i < m_vehicleNum; i++)
	{
		fprintf(fpWrite, "%s", m_chVin[i]);
		if (i != m_vehicleNum - 1)
			fprintf(fpWrite, "\n");
	}
	fclose(fpWrite);
}

CInfoRecord::CInfoRecord():m_bLockFlag(false), m_vehicleNum(0), m_datagramNum(0), m_hThreadRecv(NULL), m_hThreadParse(NULL)
{
	memset(m_chVin, 0, sizeof(m_chVin));
	memset(m_circleQue, 0, sizeof(m_circleQue));
	memset(m_dataType8, 0, sizeof(m_dataType8));
	memset(m_dataType9, 0, sizeof(m_dataType9));
	memset(&g_queDataGram, 0, sizeof(STDATAGRAMQUEUE));
}

long CInfoRecord::FindVinPos(uint8_t pVin[])
{
	long left = 0;
	long right = m_vehicleNum - 1;
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

void CInfoRecord::SortVin()
{
	for (long i = 0; i < m_vehicleNum; i++)
	{
		for (long j = i + 1; j < m_vehicleNum; j++)
		{
			//printf("i:%ld, j:%ld", i, j);
			if (strcmp((char*)m_chVin[i], (char*)m_chVin[j]) > 0)
			{
				uint8_t chTmp[VIN_LENGTH + 1] = { "" };
				memcpy(chTmp, m_chVin[i], VIN_LENGTH + 1);
				memcpy(m_chVin[i], m_chVin[j], VIN_LENGTH + 1);
				memcpy(m_chVin[j], chTmp, VIN_LENGTH + 1);
			}
		}
	}
}

long CInfoRecord::InsertVinAndSort(uint8_t pVin[])
{
	if (m_vehicleNum >= MAX_VEHICLENUM)
	{
		return -1;
	}

	if (pVin[0] != 'L')
		return -1;

	if (pVin[1] <'A' || (pVin[1] > 'Z' && pVin[1] < 'a') || pVin[1]>'z')
		return -1;

	for (int i = 2; i < VIN_LENGTH; i++)
	{
		if (pVin[i] < '0' || (pVin[i] > '9' && pVin[i] < 'A') || (pVin[i] > 'Z' && pVin[i] < 'a') || pVin[i]>'z')
			return -1;
	}

	do
	{

	}while (m_bLockFlag);

	if (m_vehicleNum == 0)
	{
		memcpy(m_chVin[m_vehicleNum], pVin, VIN_LENGTH);
		m_vehicleNum += 1;

		m_circleQue[0].pElem = (STRECVDATA*)malloc(sizeof(STRECVDATA) * QUEUE_SIZE);
		if (NULL == m_circleQue[0].pElem)
		{
			return -1;
		}
		memset(m_circleQue[0].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);

		return 0;
	}

	long insertPos = 0;

	for (long i = 0; i < m_vehicleNum; i++)
	{
		if (memcmp((char*)pVin, (char*)m_chVin[i], VIN_LENGTH) > 0)
		{
			insertPos = i + 1;
		}
		else
		{
			break;
		}
	}

	STRECVDATA* pElem = (STRECVDATA*)malloc(sizeof(STRECVDATA) * QUEUE_SIZE);
	if (NULL == pElem)
	{
		return -1;
	}

	for (long i = m_vehicleNum; i > insertPos; i--)
	{
		memcpy(m_chVin[i], m_chVin[i - 1], VIN_LENGTH + 1);

		m_circleQue[i].pElem = m_circleQue[i - 1].pElem;
		m_circleQue[i].front = m_circleQue[i - 1].front;
		m_circleQue[i].rear = m_circleQue[i - 1].rear;

		m_dataType8[i] = m_dataType8[i - 1];
		m_dataType9[i] = m_dataType9[i - 1];
	}

	memcpy(m_chVin[insertPos], (char*)pVin, VIN_LENGTH + 1);

	m_circleQue[insertPos].pElem = pElem;
	memset(m_circleQue[insertPos].pElem, 0, sizeof(STRECVDATA) * QUEUE_SIZE);
	m_circleQue[insertPos].front = 0;
	m_circleQue[insertPos].rear = 0;

	memset(&m_dataType8[insertPos], 0, sizeof(STRECVDATATYPE8));
	m_dataType8[insertPos].pF8_1 = NULL;

	memset(&m_dataType9[insertPos], 0, sizeof(STRECVDATATYPE9));
	m_dataType9[insertPos].pF9_1 = NULL;

	m_vehicleNum += 1;

	return insertPos;
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

	return m_vehicleNum;
}

void CInfoRecord::GetVinInfo(uint8_t chVin[][VIN_LENGTH + 1])
{
	m_bLockFlag = true;
	memcpy(chVin, m_chVin, sizeof(m_chVin));
	m_bLockFlag = false;
}

void CInfoRecord::RecordInfo(long pos, STRECVDATA stRecv)
{
	if (pos >= m_vehicleNum || pos < 0)
	{
		return;
	}

	do
	{
	} while (m_bLockFlag);

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

	m_datagramNum += 1;
}

long CInfoRecord::RecordInfoType8(long pos, const char* pRecv, long leftOffset)
{
	if (pos >= m_vehicleNum || pos < 0)
		return 0;

	if (*pRecv == 0 || *pRecv > 250)
		return 0;

	long offset = 0;
	uint8_t uchar;
	uint16_t ushortsw;

	do
	{

	} while (m_bLockFlag);

	m_dataType8[pos].F8_0 = *pRecv;
	offset += sizeof(m_dataType8[pos].F8_0);

	if (NULL == m_dataType8[pos].pF8_1)
	{
		m_dataType8[pos].pF8_1 = (RecvDataType8_1*)malloc(sizeof(RecvDataType8_1));
		if (NULL == m_dataType8[pos].pF8_1)
		{
			return 0;
		}
		memset(m_dataType8[pos].pF8_1, 0, sizeof(RecvDataType8_1));
	}

	m_dataType8[pos].pF8_1->F8_1_0 = *(pRecv + offset);
	offset += sizeof(m_dataType8[pos].pF8_1->F8_1_0);
	if (m_dataType8[pos].pF8_1->F8_1_0 == 0 || m_dataType8[pos].pF8_1->F8_1_0 > 250)
	{
		return offset;
	}

	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_1 = ushortsw;
	offset += sizeof(m_dataType8[pos].pF8_1->F8_1_1);
	if (m_dataType8[pos].pF8_1->F8_1_1 > 10000)
	{
		printf("invalid\n");
		//return offset;
	}

	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_2 = ushortsw;
	offset += sizeof(m_dataType8[pos].pF8_1->F8_1_2);
	if (m_dataType8[pos].pF8_1->F8_1_2 > 20000)
	{
		//return offset;
		printf("invalid\n");
	}

	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType8[pos].pF8_1->F8_1_3 = ushortsw;
	offset += sizeof(m_dataType8[pos].pF8_1->F8_1_3);

	uchar = *(pRecv + offset);
	uint16_t iSequence1st = uchar * 256;
	uchar = *(pRecv + offset + 1);
	iSequence1st += uchar;

	offset += sizeof(iSequence1st);
	uint8_t iCellNum = *(pRecv + offset);
	offset += sizeof(iCellNum);

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
				offset += sizeof(pNode->F8_1_4_2);
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

	for (uint8_t i=0; i<iCellNum; i++)
	{
		uchar = *(pRecv + offset);
		ushortsw = uchar * 256;
		uchar = *(pRecv + offset + 1);
		ushortsw += uchar;

		pNode->pF8_1_4_3[i] = ushortsw;
		offset += sizeof(ushortsw);
	}

	return offset;
}

long CInfoRecord::RecordInfoType9(long pos, const char* pRecv, long leftOffset)
{
	if (pos >= m_vehicleNum || pos < 0)
		return 0;

	if (*pRecv == 0)
		return 0;

	do
	{

	} while (m_bLockFlag);

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
	uint16_t ushortsw;

	m_dataType9[pos].F9_0 = *pRecv;
	offset += sizeof(m_dataType9[pos].F9_0);

	m_dataType9[pos].pF9_1 = (RecvDataType9_1*)malloc(sizeof(RecvDataType9_1));
	if (NULL == m_dataType9[pos].pF9_1)
	{
		return 0;
	}

	memset(m_dataType9[pos].pF9_1, 0, sizeof(RecvDataType9_1));
	
	m_dataType9[pos].pF9_1->pF9_1_2 = NULL;
	m_dataType9[pos].pF9_1->F9_1_0 = *(pRecv + offset);
	offset += sizeof(m_dataType9[pos].pF9_1->F9_1_0);

	if (m_dataType9[pos].pF9_1->F9_1_0 == 0 || m_dataType9[pos].pF9_1->F9_1_0 > 250)
	{
		return offset;
	}

	uchar = *(pRecv + offset);
	ushortsw = uchar * 256;
	uchar = *(pRecv + offset + 1);
	ushortsw += uchar;
	m_dataType9[pos].pF9_1->F9_1_1 = ushortsw;
	offset += sizeof(m_dataType9[pos].pF9_1->F9_1_1);
	if (m_dataType9[pos].pF9_1->F9_1_1 == 0 || m_dataType9[pos].pF9_1->F9_1_1 > 65531)
	{
		return offset;
	}

	m_dataType9[pos].pF9_1->pF9_1_2 = (uint8_t*)malloc(m_dataType9[pos].pF9_1->F9_1_1 * sizeof(uint8_t));
	if (NULL == m_dataType9[pos].pF9_1->pF9_1_2)
	{
		free(m_dataType9[pos].pF9_1);
		m_dataType9[pos].pF9_1 = NULL;
		return offset;
	}
	memset(m_dataType9[pos].pF9_1->pF9_1_2, 0, m_dataType9[pos].pF9_1->F9_1_1 * sizeof(uint8_t));

	for (uint8_t j = 0; j < m_dataType9[pos].pF9_1->F9_1_1; j++)
	{
		if (offset >= leftOffset)
		{
			break;
		}

		//各温度探针检测到的温度值
		m_dataType9[pos].pF9_1->pF9_1_2[j] = *(pRecv + offset);
		offset += sizeof(m_dataType9[pos].pF9_1->pF9_1_2[j]);

		if (m_dataType9[pos].pF9_1->pF9_1_2[j] > 250)
		{
			printf("invalid\n");
			//return offset;
		}
	}

	return offset;
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

	SOCKET pSocket = CInfoSocket::GetInstance()->OnConnect(serAddr);
	if (pSocket == INVALID_SOCKET)
	{
		return false;
	}

	DWORD dwThreadIdRecv;
	m_hThreadRecv = CreateThread(NULL, NULL, OnReceiveThread, hWnd, 0, &dwThreadIdRecv);

	DWORD dwThreadIdParse;
	m_hThreadParse = CreateThread(NULL, NULL, OnParseThread, hWnd, 0, &dwThreadIdParse);

	if (NULL == m_hThreadRecv || NULL == m_hThreadParse)
		CInfoSocket::GetInstance()->OnClose();

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
	for (long i = 0; i < m_vehicleNum; i++)
	{
		if (m_circleQue[i].pElem != NULL)
		{
			free(m_circleQue[i].pElem);
			m_circleQue[i].pElem = NULL;
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

	memset(m_chVin, 0, sizeof(m_chVin));
	memset(m_circleQue, 0, sizeof(m_circleQue));
	memset(m_dataType8, 0, sizeof(m_dataType8));
	memset(m_dataType9, 0, sizeof(m_dataType9));

	m_bLockFlag = false;
	m_vehicleNum = 0;
	m_datagramNum = 0;
}

void CInfoRecord::OnClose()
{
	CInfoSocket::GetInstance()->OnClose();
}

DWORD WINAPI OnReceiveThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;

	while (1)
	{
		if (CInfoSocket::GetInstance()->CheckClose())
		{
			PostMessage(hWnd, UM_STOPRECV, NULL, 0);
			break;
		}

		char recvData[BUFFER_SIZE] = {};
		INT recvSize = CInfoSocket::GetInstance()->OnReceive(recvData);
		DWORD dwLastErr = GetLastError();

		if (recvSize <= 0)
		{
			continue;
		}

		PSTDATABUFFGRAM pNode = (PSTDATABUFFGRAM)malloc(sizeof(STDATABUFFGRAM));
		memcpy(pNode->recvData, recvData, recvSize * sizeof(char));
		pNode->recvSize = recvSize;
		pNode->pNext = NULL;

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
	}

	return 0;
}

DWORD WINAPI OnParseThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;
	int num = 0;

	SYSTEMTIME st;
	memset(&st, 0, sizeof(st));
	SYSTEMTIME datePre;
	GetLocalTime(&datePre);

	while (1)
	{
		if (CInfoSocket::GetInstance()->CheckClose())
		{
			PostMessage(hWnd, UM_STOPPARSE, NULL, 0);
			break;
		}

		if (g_queDataGram.front == NULL)
			continue;

		GetLocalTime(&st);

		//触发主线程统计历史数据
		if (datePre.wDay != st.wDay || datePre.wMonth != st.wMonth || datePre.wYear != st.wYear)
		{
			SendMessage(hWnd, UM_HISTORY, (WPARAM)&st.wDayOfWeek, NULL);
		}

		datePre.wYear = st.wYear;
		datePre.wMonth = st.wMonth;
		datePre.wDay = st.wDay;

		WaitForSingleObject(g_hMutex, INFINITE);
		PSTDATABUFFGRAM pNode = g_queDataGram.front;
		g_queDataGram.front = g_queDataGram.front->pNext;

		char recvData[BUFFER_SIZE] = {};
		memcpy(recvData, pNode->recvData, pNode->recvSize * sizeof(char));
		INT recvSize = pNode->recvSize;

		free(pNode);
		pNode = NULL;
		ReleaseMutex(g_hMutex);

		long lastOffset = 0;
		long latestOffset = 0;

		while (latestOffset < recvSize)
		{
			if (memcmp(&recvData[latestOffset], "##", 2) != 0)
			{
				latestOffset += 1;
				continue;
			}

			lastOffset = latestOffset;
			latestOffset += 4;

			uint8_t strVin[VIN_LENGTH + 1] = {};
			memcpy(strVin, &recvData[latestOffset], VIN_LENGTH);

			latestOffset += 18;

			//数据单元长度
			uint8_t uchar = recvData[latestOffset];
			uint16_t ushortsw = uchar * 256;
			uchar = recvData[latestOffset + 1];
			ushortsw += uchar;
			latestOffset += 2;

			STRECVDATA infoData;
			memset(&infoData, 0, sizeof(STRECVDATA));

			//数据采集时间(年月日时分秒)
			memcpy(infoData.F8_0, &recvData[24], sizeof(infoData.F8_0));
			if (infoData.F8_0[0] != (st.wYear % 100)
				|| infoData.F8_0[1] != (uint8_t)st.wMonth
				|| infoData.F8_0[2] != (uint8_t)st.wDay)
			{
				//不是今天的不处理
				//latestOffset += ushortsw + 1;
				//continue;
				infoData.F8_0[0] = (st.wYear % 100);
				infoData.F8_0[1] = (uint8_t)st.wMonth;
				infoData.F8_0[2] = (uint8_t)st.wDay;
				infoData.F8_0[3] = (uint8_t)st.wHour;
				infoData.F8_0[4] = (uint8_t)st.wMinute;
				infoData.F8_0[5] = (uint8_t)st.wSecond;
			}

			latestOffset += 6;

			long pos = CInfoRecord::GetInstance()->FindVinPos(strVin);
			if (pos < 0)
			{
				pos = CInfoRecord::GetInstance()->InsertVinAndSort(strVin);
				if (pos < 0)
					continue;
			}

			bool bSetFlag[10] = {};
			long curOffset = latestOffset;

			//信息类型信息体
			while (latestOffset < (ushortsw - 6) + curOffset && latestOffset < recvSize - 1)
			{
				if (memcmp(&recvData[latestOffset], "##", 2) == 0)
					break;

				uint8_t infoType = recvData[latestOffset];
				long localOffset = 0;
				long leftOffset = (ushortsw - 6) + curOffset - latestOffset;

				if (infoType == 1 || infoType == 2 || infoType == 5 || infoType == 6 || infoType == 7)
				{
					localOffset = SetRecvData(&recvData[latestOffset], infoData, leftOffset);
				}
				else if (infoType == 8)
				{
					latestOffset += 1;
					localOffset = CInfoRecord::GetInstance()->RecordInfoType8(pos, &recvData[latestOffset], leftOffset);
				}
				else if (infoType == 9)
				{
					latestOffset += 1;
					localOffset = CInfoRecord::GetInstance()->RecordInfoType9(pos, &recvData[latestOffset], leftOffset);
				}
				else
				{
					latestOffset += leftOffset + 1;
					break;
				}

				latestOffset += localOffset;
			}

			CInfoRecord::GetInstance()->RecordInfo(pos, infoData);

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
	}

	return 0;
}