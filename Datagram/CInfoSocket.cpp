#include "pch.h"
#include "CInfoSocket.h"

uint16_t SWAPWORD1(const char* pRecv)
{
	uint8_t uchar = *pRecv;
	uint16_t sw = uchar;
	uchar = *(pRecv + 1);
	sw += uchar * 256;
	return sw;
}

uint32_t SWAPDWORD1(const char* pRecv)
{
	uint8_t uchar = *pRecv;
	uint32_t uintsw = uchar;
	uchar = *(pRecv + 1);
	uintsw += uchar * 256;
	uchar = *(pRecv + 2);
	uintsw += uchar * 65536;
	uchar = *(pRecv + 3);
	uintsw += uchar * 16777216;

	return uintsw;
}

CInfoSocket* CInfoSocket::m_pInstance = NULL;

SOCKET CInfoSocket::OnConnect(const sockaddr_in serAddr)
{
	if (m_pSocket != INVALID_SOCKET)
		OnClose();

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return INVALID_SOCKET;
	}

	//创建套接字
	m_pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_pSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return INVALID_SOCKET;
	}

	if (connect(m_pSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		closesocket(m_pSocket);
		WSACleanup();
		m_pSocket = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return m_pSocket;
}

VOID CInfoSocket::OnClose()
{
	closesocket(m_pSocket);
	WSACleanup();
	m_pSocket = INVALID_SOCKET;
}

INT CInfoSocket::OnReceive(char recvData[])
{
	if (INVALID_SOCKET == m_pSocket)
	{
		return -1;
	}

	INT optVal = 0;
	INT optLen = sizeof(optVal);
	getsockopt(m_pSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen);

	INT recvSize = recv(m_pSocket, recvData, optVal, 0);
	DWORD a = GetLastError();

	return recvSize;
}