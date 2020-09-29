#pragma once

#include <windows.h>
#include "InfoType.h"

class CHistoryRecord
{
public:
	CHistoryRecord();
	~CHistoryRecord()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static CHistoryRecord* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CHistoryRecord;

		return m_pInstance;
	}

	void OnRecord(WORD wDayOfWeek);

	void YesterdayAppend(long lVehicleNum);

	void OnMove();

	uint32_t GetLatestMileage(uint8_t pVin[]);

private:

	long FindVinPos(uint8_t pVin[]);

	long InsertVinAndSort(uint8_t pVin[]);

	HANDLE m_hThread;

	static CHistoryRecord* m_pInstance;

	//��������
	uint8_t m_chVinLastWeek[MAX_VEHICLENUM][VIN_LENGTH + 1]; //ÿ����vin��
	long m_vehicleNumLastWeek;

	//��������
	uint8_t m_chVinThisWeek[MAX_VEHICLENUM][VIN_LENGTH + 1];
	long m_vehicleNumThisWeek;

	//�������(��������)
	uint32_t m_iLatestMileage[MAX_VEHICLENUM];
};