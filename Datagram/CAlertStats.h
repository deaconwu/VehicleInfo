#pragma once

#include "InfoType.h"
#include <windows.h>

class CAlertStats
{
public:
	CAlertStats();

	~CAlertStats();

	static CAlertStats* GetInstance();

	void OnLaunchAlertRank(HWND hWnd, int iType);

	void OnLaunchAlertCategory(HWND hWnd, uint8_t pVin[]);

	void OnStopAlertRank() { m_bStopAlertRank = true; }

	void OnStopAlertCategory() { m_bStopAlertCategory = true; }

	bool GetStopAlertRankFlag() { return m_bStopAlertRank; }

	bool GetStopAlertCategoryFlag() { return m_bStopAlertCategory; }

	void NotifyType(int iType) { m_iType = iType; }

	void NotifyVim(uint8_t pVin[]);

	void GetType(int &iType) { iType = m_iType; }

	void GetVin(uint8_t pVin[]);

private:

	HANDLE m_hThreadAlertRank;

	HANDLE m_hThreadAlertCategory;

	static CAlertStats* m_pInstance;

	bool m_bStopAlertRank;
	bool m_bStopAlertCategory;

	int m_iType;
	uint8_t m_chVin[VIN_LENGTH + 1];
};