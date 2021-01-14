#ifndef _CSHAREMEM_H_
#define _CSHAREMEM_H_

#include "InfoType.h"

class CShareMem
{
public:

	~CShareMem()
	{
// 		if (NULL != m_pInstance)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = NULL;
// 		}
	}

	static CShareMem* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CShareMem;

		return m_pInstance;
	}

	void CreateFileMap(long* pVehicleNum, uint8_t* pVinArr, STRECVDATATYPE1TO7* pElem);

	void UnMapView();

	void LockShareVins();

	void UnlockShareVins();

	void LockShareInfoType();

	void UnlockShareInfoType();

private:
	CShareMem();

	HANDLE  m_hFileMapVins;	//�ļ�ӳ����
	LPVOID  m_hViewBufVins;	//�ļ�ӳ��ӳ�䵽��ַ�ռ���׵�ַ

	HANDLE  m_hFileMapInfoType;
	LPVOID  m_hViewBufInfoType;

	HANDLE	m_hVinsMutex;
	HANDLE	m_hInfoTypeMutex;

	static CShareMem* m_pInstance;
};

#endif
