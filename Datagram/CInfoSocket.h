#ifndef __CINFOSOCKET_H__
#define __CINFOSOCKET_H__

#include <windows.h>
#include <WinSock.h>
#include "CInfoSingleton.h"
#include "InfoType.h"

#pragma comment(lib,"ws2_32.lib") 

class CInfoSocket //: public CSingleton<CInfoSocket>
{
public:

	~CInfoSocket()
	{
// 		if (NULL != m_pInstance)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = NULL;
// 		}
	}

	static CInfoSocket* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CInfoSocket;

		return m_pInstance;
	}

	bool CheckClose() { return m_pSocket == INVALID_SOCKET; }

	SOCKET OnConnect(const sockaddr_in serAddr, HWND hWnd);

	SOCKET OnReConnect();

	INT OnReceive(char recvData[]);

	VOID OnClose(bool bEmptyAddr=true);

private:

	CInfoSocket()
	{
		memset(&m_serAddr, 0, sizeof(m_serAddr));
		m_pSocket = INVALID_SOCKET;
		m_hWnd = NULL;
	}

	SOCKET m_pSocket;
	sockaddr_in m_serAddr;

	HWND m_hWnd;

	static CInfoSocket* m_pInstance;
};

#endif