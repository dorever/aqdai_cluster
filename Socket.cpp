#include "stdafx.h"
#include "SockLx.h"
#ifdef _WIN32
#pragma comment (lib,"ws2_32.lib")
#endif

CSockLx::CSockLx()
{
#ifdef _WIN32
	WSAData wd;
	WSAStartup(0x0202, &wd);
#endif
	m_hSocket = INVALID_SOCKET;
}


CSockLx::~CSockLx()
{
	Close();
}

BOOL CSockLx::Create(UINT nSocketPort, int nSocketType, LPCTSTR lpszSocketAddress)
{//socket & bind
	m_hSocket = socket(AF_INET, nSocketType, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		return FALSE;
	}
	sockaddr_in sa = { AF_INET,htons(nSocketPort) };
	if(lpszSocketAddress)
		Pton(lpszSocketAddress, sa.sin_addr);

	return !bind(m_hSocket, (sockaddr*)&sa, sizeof(sa));
}

BOOL CSockLx::Accept(CSockLx & socka, LPTSTR sIP, UINT * nPort)
{
	sockaddr_in sa = { AF_INET };
	socklen_t nLen = sizeof(sa);
	socka.m_hSocket = accept(m_hSocket, (sockaddr*)&sa, &nLen);
	if (INVALID_SOCKET == socka.m_hSocket)
		return FALSE;
	if (sIP)
		Ntop(sa.sin_addr, sIP);
	if (nPort)
		*nPort = htons(sa.sin_port);
	return TRUE;
}

BOOL CSockLx::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{//封装也交隐藏
	sockaddr_in sa = { AF_INET,htons(nHostPort) };
	Pton(lpszHostAddress, sa.sin_addr);
	return !connect(m_hSocket, (sockaddr*)&sa, sizeof(sa));
}

int CSockLx::SendTo(const void * lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	sockaddr_in sa = { AF_INET,htons(nHostPort) };
	Pton(lpszHostAddress, sa.sin_addr);
	sendto(m_hSocket, (const char*)lpBuf, nBufLen, 0, (sockaddr*)&sa, sizeof(sa));
	return 0;
}

int CSockLx::ReceiveFrom(void * lpBuf, int nBufLen, LPTSTR  rSocketAddress, UINT & rSocketPort, int nFlags)
{
	sockaddr_in sa = { AF_INET };
	socklen_t nLen = sizeof(sa);
	int nRet = recvfrom(m_hSocket, (char*)lpBuf, nBufLen, 0, (sockaddr*)&sa, &nLen);
	if (nRet <= 0)
		return nRet;

	rSocketPort = htons(sa.sin_port);
	if (rSocketAddress)
		Ntop(sa.sin_addr, rSocketAddress);
	return nRet;
}

BOOL CSockLx::GetPeerName(LPTSTR rSocketAddress, UINT & rSocketPort)
{
	sockaddr_in sa = { AF_INET };
	socklen_t nLen = sizeof(sa);
	if (getpeername(m_hSocket, (sockaddr*)&sa, &nLen) < 0)
		return FALSE;
	rSocketPort = htons(sa.sin_port);
	if (rSocketAddress)
		Ntop(sa.sin_addr, rSocketAddress);
	return TRUE;

}

BOOL CSockLx::GetSockName(LPTSTR rSocketAddress, UINT & rSocketPort)
{
	sockaddr_in sa = { AF_INET };
	socklen_t nLen = sizeof(sa);
	if (getsockname(m_hSocket, (sockaddr*)&sa, &nLen) < 0)
		return FALSE;
	rSocketPort = htons(sa.sin_port);
	if (rSocketAddress)
		Ntop(sa.sin_addr, rSocketAddress);
	return TRUE;
}
