#pragma once

#include "InfoType.h"
#include <windows.h>

class CFileParse
{
public:
	CFileParse();
	~CFileParse()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static CFileParse* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CFileParse;

		return m_pInstance;
	}

	void OnLauchParse();

private:
	HANDLE m_hThreadParse;

	static CFileParse* m_pInstance;
};