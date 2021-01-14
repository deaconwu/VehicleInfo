#include "pch.h"
#include "CShareMem.h"

CShareMem* CShareMem::m_pInstance = NULL;

CShareMem::CShareMem() : m_hFileMapVins(NULL), m_hViewBufVins(NULL), m_hFileMapInfoType(NULL), m_hViewBufInfoType(NULL), m_hVinsMutex(NULL), m_hInfoTypeMutex(NULL)
{
}

void CShareMem::CreateFileMap(long* pVehicleNum, uint8_t* pVinArr, STRECVDATATYPE1TO7* pElem)
{
	//创建共享文件对象
	WCHAR chStrVin[20] = L"share_vins";
	m_hFileMapVins = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(long)+MAX_VEHICLENUM*VIN_LENGTH, chStrVin);
	if (NULL == m_hFileMapVins)
		return;

	m_hViewBufVins = MapViewOfFile(m_hFileMapVins, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == m_hViewBufVins)
		return;

	pVehicleNum = (long*)m_hViewBufVins;
	pVinArr = (uint8_t*)m_hViewBufVins;
	pVinArr = pVinArr + sizeof(long);

	//大于4GB文件的内存映射
	WCHAR chStrInfoType[20] = L"share_infotype";
	
	TCHAR tchPath[1024] = {};
	GetCurrentDirectory(1024, tchPath);

	char chPath[1024] = {};
	int iLength = WideCharToMultiByte(CP_ACP, 0, tchPath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchPath, -1, chPath, iLength, NULL, NULL);
	memcpy(&chPath[iLength - 1], "\\map.txt", sizeof("\\map.txt"));

	iLength = MultiByteToWideChar(CP_ACP, 0, chPath, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, chPath, -1, tchPath, iLength);

	CFile hFile;
	hFile.Open(tchPath, CFile::modeReadWrite | CFile::modeCreate);
	hFile.SetLength(MAX_VEHICLENUM*QUEUE_SIZE * sizeof(STRECVDATATYPE1TO7));

	m_hFileMapInfoType = CreateFileMapping(hFile.m_hFile, NULL, PAGE_READWRITE, 0, 0, chStrInfoType);
	if (NULL == m_hFileMapInfoType)
		return;
	m_hViewBufInfoType = MapViewOfFile(m_hFileMapInfoType, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == m_hViewBufInfoType)
		return;
	pElem = (STRECVDATATYPE1TO7*)m_hViewBufInfoType;

	hFile.Close();

	m_hVinsMutex = CreateMutex(NULL, FALSE, _T("vins_share"));
	m_hInfoTypeMutex = CreateMutex(NULL, FALSE, _T("infotype_share"));
}

void CShareMem::UnMapView()
{
	UnmapViewOfFile(m_hViewBufVins);
	UnmapViewOfFile(m_hViewBufInfoType);

	CloseHandle(m_hVinsMutex);
	m_hVinsMutex = NULL;

	CloseHandle(m_hInfoTypeMutex);
	m_hInfoTypeMutex = NULL;
}

void CShareMem::LockShareVins()
{
	WaitForSingleObject(m_hVinsMutex, INFINITE);
}

void CShareMem::UnlockShareVins()
{
	ReleaseMutex(m_hVinsMutex);
}

void CShareMem::LockShareInfoType()
{
	WaitForSingleObject(m_hInfoTypeMutex, INFINITE);
}

void CShareMem::UnlockShareInfoType()
{
	ReleaseMutex(m_hInfoTypeMutex);
}