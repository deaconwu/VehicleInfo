#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include "CInfoRecord.h"

DWORD WINAPI OnReceiveThread(LPVOID lparam);
DWORD WINAPI OnStatisticThread(LPVOID lparam);

CInfoRecord* CInfoRecord::m_pInstance = NULL;

long SetRecvData(const char* pRecv, STRECVDATA& stRecv)
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
		}

		uchar = *(pRecv + offset);	//驱动电机故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
		}

		uchar = *(pRecv + offset);	//发动机故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
		}

		uchar = *(pRecv + offset);	//其它故障总数
		offset += 1;
		if (uchar > 0)
		{
			offset += uchar * sizeof(uint32_t);
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

CInfoRecord::CInfoRecord():m_bLockFlag(false), m_vehicleNum(0)
{
	memset(m_chVin, 0, sizeof(m_chVin));
	memset(m_circleQue, 0, sizeof(m_circleQue));
	memset(m_dataType8, 0, sizeof(m_dataType8));
	memset(m_dataType9, 0, sizeof(m_dataType9));
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
	if (m_vehicleNum >= 200000)
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

	m_circleQue[insertPos].pElem = (STRECVDATA*)malloc(sizeof(STRECVDATA) * QUEUE_SIZE);
	if (NULL == m_circleQue[insertPos].pElem)
	{
		return -1;
	}
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

void CInfoRecord::RecordInfo(long pos, STRECVDATA stRecv)
{
	if (pos >= m_vehicleNum || pos < 0)
	{
		return;
	}

	do
	{
	} while (m_bLockFlag);

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
}

long CInfoRecord::RecordInfoType8(long pos, const char* pRecv)
{
	if (pos >= m_vehicleNum || pos < 0)
		return 0;

	if (*pRecv == 0 || *pRecv > 250)
		return 0;

	long offset = 0;
	uint16_t ushort;
	uint16_t ushortsw;

	do
	{

	} while (m_bLockFlag);

	if (NULL != m_dataType8[pos].pF8_1)
	{
		for (int i = 0; i < m_dataType8[pos].F8_0; i++)
		{
			if (NULL != m_dataType8[pos].pF8_1[i].pF8_1_6)
			{
				free(m_dataType8[pos].pF8_1[i].pF8_1_6);
				m_dataType8[pos].pF8_1[i].pF8_1_6 = NULL;
			}
		}

		free(m_dataType8[pos].pF8_1);
		m_dataType8[pos].pF8_1 = NULL;
	}

	m_dataType8[pos].pF8_1 = (RecvDataType8_1*)malloc((*pRecv) * sizeof(RecvDataType8_1));
	if (NULL == m_dataType8[pos].pF8_1)
	{
		return 0;
	}

	memset(m_dataType8[pos].pF8_1, 0, (*pRecv) * sizeof(RecvDataType8_1));
	m_dataType8[pos].F8_0 = *pRecv;
	offset += sizeof(m_dataType8[pos].F8_0);

	//if (m_dataType8[pos].F8_0 > 1)
	//	printf("\n");

	for (uint8_t i = 0; i < m_dataType8[pos].F8_0; i++)
	{
		m_dataType8[pos].pF8_1[i].pF8_1_6 = NULL;
		m_dataType8[pos].pF8_1[i].F8_1_0 = *(pRecv + offset);
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_0);
		if (m_dataType8[pos].pF8_1[i].F8_1_0 == 0 || m_dataType8[pos].pF8_1[i].F8_1_0 > 250)
		{
			return offset;
		}

		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		m_dataType8[pos].pF8_1[i].F8_1_1 = ushortsw;
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_1);
		if (m_dataType8[pos].pF8_1[i].F8_1_1 > 10000)
		{
			return offset;
		}

		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		m_dataType8[pos].pF8_1[i].F8_1_2 = ushortsw;
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_2);
		if (m_dataType8[pos].pF8_1[i].F8_1_2 > 20000)
		{
			return offset;
		}

		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		m_dataType8[pos].pF8_1[i].F8_1_3 = ushortsw;
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_3);

		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		m_dataType8[pos].pF8_1[i].F8_1_4 = ushortsw;
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_4);

		m_dataType8[pos].pF8_1[i].F8_1_5 = *(pRecv + offset);
		offset += sizeof(m_dataType8[pos].pF8_1[i].F8_1_5);
		if (m_dataType8[pos].pF8_1[i].F8_1_5 == 0 || m_dataType8[pos].pF8_1[i].F8_1_5 > 200)
		{
			return offset;
		}

		m_dataType8[pos].pF8_1[i].pF8_1_6 = (uint16_t*)malloc(m_dataType8[pos].pF8_1[i].F8_1_5 * sizeof(uint16_t));
		if (NULL == m_dataType8[pos].pF8_1[i].pF8_1_6)
		{
			free(m_dataType8[pos].pF8_1);
			m_dataType8[pos].pF8_1 = NULL;
			return offset;
		}
		memset(m_dataType8[pos].pF8_1[i].pF8_1_6, 0, m_dataType8[pos].pF8_1[i].F8_1_5 * sizeof(uint16_t));

		for (int x = 0; x < m_dataType8[pos].pF8_1[i].F8_1_5; x++)	//单体电池电压
		{
			ushort = *(pRecv + offset);
			ushortsw = SWAPWORD(ushort);
			m_dataType8[pos].pF8_1[i].pF8_1_6[x] = ushortsw;
			offset += sizeof(ushortsw);
			if (m_dataType8[pos].pF8_1[i].pF8_1_6[x] > 60000)
			{
				return offset;
			}
		}
	}

	return offset;
}

long CInfoRecord::RecordInfoType9(long pos, const char* pRecv)
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
		for (uint8_t i = 0; i < m_dataType9[pos].F9_0; i++)
		{
			if (NULL != m_dataType9[pos].pF9_1[i].pF9_1_2)
			{
				free(m_dataType9[pos].pF9_1[i].pF9_1_2);
				m_dataType9[pos].pF9_1[i].pF9_1_2 = NULL;
			}
		}

		free(m_dataType9[pos].pF9_1);
		m_dataType9[pos].pF9_1 = NULL;
	}

	long offset = 0;
	uint16_t ushort;
	uint16_t ushortsw;

	m_dataType9[pos].F9_0 = *pRecv;
	offset += sizeof(m_dataType9[pos].F9_0);

	m_dataType9[pos].pF9_1 = (RecvDataType9_1*)malloc(m_dataType9[pos].F9_0 * sizeof(RecvDataType9_1));
	if (NULL == m_dataType9[pos].pF9_1)
	{
		return 0;
	}

	//if (m_dataType9[pos].F9_0 > 1)
	//	printf("\n");

	memset(m_dataType9[pos].pF9_1, 0, m_dataType9[pos].F9_0 * sizeof(RecvDataType9_1));
	for (uint8_t i = 0; i < m_dataType9[pos].F9_0; i++)
	{
		m_dataType9[pos].pF9_1[i].pF9_1_2 = NULL;
		m_dataType9[pos].pF9_1[i].F9_1_0 = *(pRecv + offset);
		offset += sizeof(m_dataType9[pos].pF9_1[i].F9_1_0);

		if (m_dataType9[pos].pF9_1[i].F9_1_0 == 0 || m_dataType9[pos].pF9_1[i].F9_1_0 > 250)
		{
			return offset;
		}

		ushort = *(pRecv + offset);
		ushortsw = SWAPWORD(ushort);
		m_dataType9[pos].pF9_1[i].F9_1_1 = ushortsw;
		offset += sizeof(m_dataType9[pos].pF9_1[i].F9_1_1);
		if (m_dataType9[pos].pF9_1[i].F9_1_1 == 0 || m_dataType9[pos].pF9_1[i].F9_1_1 > 65531)
		{
			return offset;
		}

		m_dataType9[pos].pF9_1[i].pF9_1_2 = (uint8_t*)malloc(m_dataType9[pos].pF9_1[i].F9_1_1 * sizeof(uint8_t));
		if (NULL == m_dataType9[pos].pF9_1[i].pF9_1_2)
		{
			free(m_dataType9[pos].pF9_1);
			m_dataType9[pos].pF9_1 = NULL;
			return offset;
		}
		memset(m_dataType9[pos].pF9_1[i].pF9_1_2, 0, m_dataType9[pos].pF9_1[i].F9_1_1 * sizeof(uint8_t));

		for (uint8_t j = 0; j < m_dataType9[pos].pF9_1[i].F9_1_1; j++)
		{
			//各温度探针检测到的温度值
			m_dataType9[pos].pF9_1[i].pF9_1_2[j] = *(pRecv + offset);
			offset += sizeof(m_dataType9[pos].pF9_1[i].pF9_1_2[j]);

			if (m_dataType9[pos].pF9_1[i].pF9_1_2[j] > 250)
			{
				return offset;
			}
		}
	}

	return offset;
}

void CInfoRecord::OnStatistic()
{
	STCIRCLEQUEUE circleQue[20000];
	long vehicleNum = GetQueInfo(circleQue);


}

void CInfoRecord::OnRealTimeRecv(HWND hWnd)
{
	SOCKET pSocket = CInfoSocket::GetInstance()->OnConnect();
	if (pSocket == INVALID_SOCKET)
	{
		return;
	}

	DWORD dwThreadId = 0;
	DWORD dwMainThread = GetCurrentThreadId();

	HANDLE hThread = CreateThread(NULL, NULL, OnReceiveThread, hWnd, 0, &dwThreadId);

	//CInfoSocket::GetInstance()->OnClose();
}

DWORD WINAPI OnReceiveThread(LPVOID lparam)
{
	HWND hWnd = (HWND)lparam;
	int num = 0;
	while (1)
	{
		//if (num > 10)
		//	break;
		SYSTEMTIME st;
		GetLocalTime(&st);

		char recvData[BUFFER_SIZE] = {};
		INT recvSize = CInfoSocket::GetInstance()->OnReceive(recvData);
		DWORD a = GetLastError();
		if (recvSize <= 0)
		{
			continue;
		}

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
			long pos = CInfoRecord::GetInstance()->FindVinPos(strVin);
			if (pos < 0)
			{
				pos = CInfoRecord::GetInstance()->InsertVinAndSort(strVin);
				if (pos < 0)
					continue;
			}

			latestOffset += 18;

			//数据单元长度
			uint16_t ushort = 0;
			uint16_t ushortsw = 0;
			memcpy(&ushort, &recvData[latestOffset], sizeof(uint16_t));
			ushortsw = SWAPWORD(ushort);
			latestOffset += 2;

			STRECVDATA infoData;
			memset(&infoData, 0, sizeof(STRECVDATA));

			//数据采集时间(年月日时分秒)
			memcpy(infoData.F8_0, &recvData[24], sizeof(infoData.F8_0));
			if (infoData.F8_0[0] > (st.wYear % 100)
				|| infoData.F8_0[1] > st.wMonth
				|| infoData.F8_0[2] > st.wDay)
			{
				infoData.F8_0[0] = (uint8_t)st.wYear % 100;
				infoData.F8_0[1] = (uint8_t)st.wMonth;
				infoData.F8_0[2] = (uint8_t)st.wDay;

				infoData.F8_0[3] = (uint8_t)st.wHour;
				infoData.F8_0[4] = (uint8_t)st.wMinute;
				infoData.F8_0[5] = (uint8_t)st.wSecond;
			}
			latestOffset += 6;

			bool bSetFlag[10] = {};
			long curOffset = latestOffset;

			//信息类型信息体
			while (latestOffset<(ushortsw-24)+curOffset && latestOffset<recvSize-1)
			{
				uint8_t infoType = recvData[latestOffset];
				long localOffset = 0;

				if (infoType == 1 || infoType == 2 || infoType == 5 || infoType == 6 || infoType == 7)
				{
					localOffset = SetRecvData(&recvData[latestOffset], infoData);
				}
				else if (infoType == 8)
				{
					latestOffset += 1;
					localOffset = CInfoRecord::GetInstance()->RecordInfoType8(pos, &recvData[latestOffset]);
				}
				else if (infoType == 9)
				{
					latestOffset += 1;
					localOffset = CInfoRecord::GetInstance()->RecordInfoType9(pos, &recvData[latestOffset]);
				}
				else
				{
					break;
				}

				latestOffset += localOffset;
			}

			CInfoRecord::GetInstance()->RecordInfo(pos, infoData);

			//触发主线程统计车辆数据
			//PostMessage(hWnd, UM_STATISTIC, NULL, NULL);
			//if (infoData.F7_0 > 0)
			//	CInfoRecord::GetInstance()->WriteVin();
			if (infoData.F7_0 > 0)
				PostMessage(hWnd, UM_ALERT, (WPARAM)infoData.F7_0, (LPARAM)strVin);
		}

		num++;
	}

	CInfoSocket::GetInstance()->OnClose();

	return 0;
}