#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include "logindate.h"
#ifdef _WIN32
#include <process.h>
#include <winsock2.h>
typedef int socklen_t;
typedef void PROC_RET;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif
typedef void *PROC_RET;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef LPSTR LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT* PFLOAT;
typedef BOOL * PBOOL;
typedef BOOL * LPBOOL;
typedef BYTE * PBYTE;
typedef BYTE * LPBYTE;
typedef int * PINT;
typedef int * LPINT;
typedef WORD * PWORD;
typedef WORD * LPWORD;
typedef long * LPLONG;
typedef DWORD * PDWORD;
typedef DWORD * LPDWORD;
typedef void * LPVOID;
typedef const void * LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int* PUINT;
typedef struct sockaddr_in SOCKADDR_IN;


typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
inline int WSAGetLastError()
{
	return errno;
}
inline void closesocket(SOCKET s)
{
	close(s);
}
inline void _strlwr(char* str)
{
	char* p = str;
	while (*p)
	{
		if (*p >= 'A' && *p <= 'Z')
			*p ^= 32;//
		++p;
	}
}
#endif

class CSockLx
{
	SOCKET m_hSocket;//HANDLE 
	
public:
	SAdmin cm_state;
	BOOL Create(UINT nSocketPort = 0,int nSocketType = SOCK_STREAM,LPCTSTR lpszSocketAddress = NULL);
	operator SOCKET ()
	{
		return m_hSocket;
	}
	int SendTo(void* buff, int nLen, LPCTSTR sIP, UINT nPort);
	inline int Receive(void* buff, int nLen)
	{
		return recv(m_hSocket, (char*)buff, nLen, 0);
	}
	inline int Send(void* buff, int nLen)
	{
		return send(m_hSocket, (char*)buff, nLen,0);
	}
	int ReceiveFrom(void* buff, int nLen, LPTSTR sIP = NULL, UINT* pPort = NULL);
	inline static int GetLastError()
	{
		return WSAGetLastError();
	}
	BOOL GetSockName(char* sIP, UINT& nPort);
	BOOL GetPeerName(char* sIP, UINT& nPort);

	inline BOOL Listen(int nBacklog = 8)
	{
		return !listen(m_hSocket, nBacklog);
	}
	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Accept(CSockLx& socka, LPTSTR sIP = NULL, UINT* pPort = NULL);

	CSockLx();
	~CSockLx();

};

