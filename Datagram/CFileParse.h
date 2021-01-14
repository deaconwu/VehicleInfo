#pragma once

#include "InfoType.h"
#include <windows.h>

class CFileParse
{
public:
	~CFileParse()
	{
// 		if (NULL != m_pInstance)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = NULL;
// 		}
	}

	static CFileParse* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CFileParse;

		return m_pInstance;
	}

	void OnLauchParse();

	void WriteVin();

	long FindVinPos(uint8_t pVin[]);

	long InsertVinAndSort(uint8_t pVin[]);

private:
	CFileParse();

	void ReadVin();

	HANDLE m_hThreadParse;

	long m_vehicleNum;

	uint8_t m_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1];

	static CFileParse* m_pInstance;
};