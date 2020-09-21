#ifndef __CINFOSOCKET_H__
#define __CINFOSOCKET_H__

#include <windows.h>
#include <WinSock.h>
#include "CInfoSingleton.h"
#include "InfoType.h"

#pragma comment(lib,"ws2_32.lib") 

#define SWAPWORD(x) (uint16_t) (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8))

#define SWAPDWORD(x) (uint32_t) (((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24))

#define SWAPWORD2(pRecv) (*pRecv) + (*(pRecv+1)) * 256

#define SWAPDWORD2(pRecv) (*pRecv) + (*(pRecv+1)) * 256 + (*(pRecv+2)) * 65536 + (*(pRecv+3)) * 16777216

uint16_t SWAPWORD1(const char* pRecv);

uint32_t SWAPDWORD1(const char* pRecv);


class CInfoSocket //: public CSingleton<CInfoSocket>
{
public:
	CInfoSocket() { m_pSocket = INVALID_SOCKET; }
	~CInfoSocket()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static CInfoSocket* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CInfoSocket;

		return m_pInstance;
	}

	SOCKET OnConnect();

	INT OnReceive(char recvData[]);

	VOID OnClose();

private:
	SOCKET m_pSocket;
	static CInfoSocket* m_pInstance;
};

#endif