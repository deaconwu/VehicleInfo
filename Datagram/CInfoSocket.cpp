#include "pch.h"
#include "CInfoSocket.h"

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

	memcpy(&m_serAddr, &serAddr, sizeof(m_serAddr));

	return m_pSocket;
}

SOCKET CInfoSocket::OnReConnect()
{
	if (INVALID_SOCKET == m_pSocket)
	{
		return INVALID_SOCKET;
	}

	if (m_serAddr.sin_port == 0
		|| m_serAddr.sin_addr.S_un.S_un_b.s_b1 == 0
		|| m_serAddr.sin_addr.S_un.S_un_b.s_b2 == 0
		|| m_serAddr.sin_addr.S_un.S_un_b.s_b3 == 0
		|| m_serAddr.sin_addr.S_un.S_un_b.s_b4 == 0)
	{
		return INVALID_SOCKET;
	}

	OnClose(false);

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return INVALID_SOCKET;
	}

	m_pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_pSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return INVALID_SOCKET;
	}

	if (connect(m_pSocket, (sockaddr *)&m_serAddr, sizeof(m_serAddr)) == SOCKET_ERROR)
	{
		closesocket(m_pSocket);
		WSACleanup();
		m_pSocket = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return m_pSocket;
}

VOID CInfoSocket::OnClose(bool bEmptyAddr)
{
	closesocket(m_pSocket);
	WSACleanup();
	m_pSocket = INVALID_SOCKET;

	if (bEmptyAddr)
		memset(&m_serAddr, 0, sizeof(m_serAddr));
}

INT CInfoSocket::OnReceive(char recvData[])
{
	if (INVALID_SOCKET == m_pSocket)
	{
		return -1;
	}

	INT iRcvBuf = 0;
	INT iRcvBufLen = sizeof(iRcvBuf);

	getsockopt(m_pSocket, SOL_SOCKET, SO_RCVBUF, (char*)&iRcvBuf, &iRcvBufLen);

	//iRcvBuf *= SIZE_MULTIPLE;
	//setsockopt(m_pSocket, SOL_SOCKET, SO_RCVBUF, (char*)&iRcvBuf, iRcvBufLen);
	

	INT recvSize = recv(m_pSocket, recvData, iRcvBuf, 0);
	//DWORD a = GetLastError();

	return recvSize;
}