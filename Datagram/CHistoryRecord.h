#ifndef _C_HISTORY_RECORD_H_
#define _C_HISTORY_RECORD_H_

#include <windows.h>
#include "InfoType.h"

class CHistoryRecord
{
public:
	~CHistoryRecord()
	{
// 		if (NULL != m_pInstance)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = NULL;
// 		}
	}

	static CHistoryRecord* GetInstance()
	{
		if (0 == m_pInstance)
			m_pInstance = new CHistoryRecord;

		return m_pInstance;
	}

	void OnRecord(uint16_t wDayOfWeek);

	void YesterdayAppend(long lVehicleNum);

	void OnMove();

	uint32_t GetLatestMileage(uint8_t pVin[]);

private:
	CHistoryRecord();

	long FindVinPos(uint8_t pVin[]);

	long InsertVinAndSort(uint8_t pVin[]);

	HANDLE m_hThread;

	static CHistoryRecord* m_pInstance;

	//上周数据
	uint8_t m_chVinLastWeek[MAX_VEHICLENUM][VIN_LENGTH + 1]; //每辆车vin码
	long m_vehicleNumLastWeek;

	//本周数据
	uint8_t m_chVinThisWeek[MAX_VEHICLENUM][VIN_LENGTH + 1];
	long m_vehicleNumThisWeek;

	//最近数据(今日以外)
	uint32_t m_iLatestMileage[MAX_VEHICLENUM];
};

#endif