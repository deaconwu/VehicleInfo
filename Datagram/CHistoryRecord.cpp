#include "CHistoryRecord.h"
#include "CInfoRecord.h"

DWORD WINAPI OnHisRecThread(LPVOID lparam);

CHistoryRecord* CHistoryRecord::m_pInstance = NULL;

static STCIRCLEQUEUE g_circleQue[MAX_VEHICLENUM] = {};
static uint8_t g_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1];

CHistoryRecord::CHistoryRecord() : m_vehicleNumLastWeek(0), m_vehicleNumThisWeek(0)
{
	memset(m_chVinLastWeek, 0, sizeof(m_chVinLastWeek));
	memset(m_chVinThisWeek, 0, sizeof(m_chVinThisWeek));
	memset(m_iLatestMileage, 0, sizeof(m_iLatestMileage));
}

void CHistoryRecord::OnRecord(WORD wDayOfWeek)
{
	if (NULL != m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	memset(g_circleQue, 0, sizeof(g_circleQue));
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(g_circleQue);

	memset(g_chVin, 0, sizeof(g_chVin));
	CInfoRecord::GetInstance()->GetVinInfo(g_chVin);

	CInfoRecord::GetInstance()->OnReset();

	STDAYVEHICLE stDayVehicle;
	stDayVehicle.wDayOfWeek = wDayOfWeek;
	stDayVehicle.lVehicleNum = vehicleNum;

	DWORD dwThreadId;
	m_hThread = CreateThread(NULL, NULL, OnHisRecThread, (LPVOID)&stDayVehicle, 0, &dwThreadId);
}

void CHistoryRecord::OnMove()
{
	//本周记录转移到上周
	memset(m_chVinLastWeek, 0, sizeof(m_chVinLastWeek));
	memcpy(m_chVinLastWeek, m_chVinThisWeek, sizeof(m_chVinLastWeek));
	m_vehicleNumLastWeek = m_vehicleNumThisWeek;

	//清空本周记录
	memset(m_chVinThisWeek, 0, sizeof(m_chVinThisWeek));
	m_vehicleNumThisWeek = 0;
}

void CHistoryRecord::YesterdayAppend(long lVehicleNum)
{
	long pos;
	for (long i=0; i<lVehicleNum; i++)
	{
		pos = FindVinPos(g_chVin[i]);
		if (pos < 0)
		{
			pos = InsertVinAndSort(g_chVin[i]);
		}

		long rear = g_circleQue[i].rear;

		m_iLatestMileage[pos] = g_circleQue[i].pElem[rear - 1].F1_4;
	}
}

uint32_t CHistoryRecord::GetLatestMileage(uint8_t pVin[])
{
	long pos = FindVinPos(pVin);
	if (pos < 0)
		return 0L;

	return m_iLatestMileage[pos];
}

long CHistoryRecord::FindVinPos(uint8_t pVin[])
{
	long left = 0;
	long right = m_vehicleNumThisWeek - 1;
	long mid = -1;

	while (left <= right)
	{
		mid = (left + right) / 2;
		if (memcmp((char*)pVin, (char*)m_chVinThisWeek[mid], VIN_LENGTH) < 0)
		{
			right = mid - 1;
		}
		else if (memcmp((char*)pVin, (char*)m_chVinThisWeek[mid], VIN_LENGTH) > 0)
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

long CHistoryRecord::InsertVinAndSort(uint8_t pVin[])
{
	if (m_vehicleNumThisWeek >= MAX_VEHICLENUM)
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

	if (m_vehicleNumThisWeek == 0)
	{
		memcpy(m_chVinThisWeek[m_vehicleNumThisWeek], pVin, VIN_LENGTH);
		m_vehicleNumThisWeek += 1;

		return 0;
	}

	long insertPos = 0;

	for (long i = 0; i < m_vehicleNumThisWeek; i++)
	{
		if (memcmp((char*)pVin, (char*)m_chVinThisWeek[i], VIN_LENGTH) > 0)
		{
			insertPos = i + 1;
		}
		else
		{
			break;
		}
	}

	for (long i = m_vehicleNumThisWeek; i > insertPos; i--)
	{
		memcpy(m_chVinThisWeek[i], m_chVinThisWeek[i - 1], VIN_LENGTH + 1);
		m_iLatestMileage[i] = m_iLatestMileage[i - 1];
	}

	memcpy(m_chVinThisWeek[insertPos], (char*)pVin, VIN_LENGTH + 1);
	m_iLatestMileage[insertPos] = 0;

	m_vehicleNumThisWeek += 1;

	return insertPos;
}

DWORD WINAPI OnHisRecThread(LPVOID lparam)
{
	STDAYVEHICLE* pDayVehicle = (STDAYVEHICLE*)lparam;

	CHistoryRecord::GetInstance()->YesterdayAppend(pDayVehicle->lVehicleNum);

	if (pDayVehicle->wDayOfWeek == 1)	//周一
	{
		CHistoryRecord::GetInstance()->OnMove();
	}

	return 0;
}