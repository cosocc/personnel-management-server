
#include "CSockLx.h"
#pragma comment(lib,"ws2_32.lib")
BOOL CSockLx::Create(UINT nSocketPort, int nSocketType, LPCTSTR lpszSocketAddress)
{
    m_hSocket = socket(AF_INET, nSocketType, 0);
    if (INVALID_SOCKET == m_hSocket)
        return FALSE;
    uint16_t sdtbd = htons(nSocketPort);
    //struct sockaddr_in {
    //    short sin_family; 地址族 (必须是 AF_INET)。
    //    unsigned short sin_port;  IP 端口。
    //    struct in_addr sin_addr;  IP 地址。
    //    char sin_zero[8];     若要使框架相同大小的填充与 SOCKADDR。
    //};

    sockaddr_in sa = {AF_INET,sdtbd};
   
    if (lpszSocketAddress)
        sa.sin_addr.s_addr = inet_addr(lpszSocketAddress);
    return !bind(m_hSocket,(sockaddr*)&sa,sizeof(sa));
}

int CSockLx::SendTo(void* buff, int nLen, LPCTSTR sIP, UINT nPort)
{
    sockaddr_in sa = { AF_INET,htons(nPort) };
    sa.sin_addr.s_addr = inet_addr(sIP);

    return sendto(m_hSocket,(const char*)buff,nLen,0,(const sockaddr*)&sa,sizeof(sa));
}

int CSockLx::ReceiveFrom(void* buff, int nLen, LPTSTR sIP, UINT* pPort)
{
    SOCKADDR_IN sa;
    socklen_t len = sizeof(sa);
    int n = recvfrom(m_hSocket,(char*) buff, nLen, 0,(sockaddr*) &sa, &len);
    if (n > 0)
    {
        if (sIP)
            strcpy(sIP,inet_ntoa(sa.sin_addr));
        if (pPort)
            *pPort = htons(sa.sin_port);
    }
    return n;
}

BOOL CSockLx::GetSockName(char* sIP, UINT& nPort)
{
    SOCKADDR_IN sa;
    socklen_t len = sizeof(sa);
    int n = getsockname(m_hSocket, (sockaddr*)&sa, &len);
    if (n)
        return FALSE;
    if (sIP)
        strcpy(sIP, inet_ntoa(sa.sin_addr));
    
    nPort = htons(sa.sin_port);
    return TRUE;
}

BOOL CSockLx::GetPeerName(char* sIP, UINT& nPort)
{
    SOCKADDR_IN sa;
    socklen_t len = sizeof(sa);
    int n = getpeername(m_hSocket, (sockaddr*)&sa, &len);
    if (n)
        return FALSE;
    if (sIP)
        strcpy(sIP, inet_ntoa(sa.sin_addr));

    nPort = htons(sa.sin_port);
    return TRUE;
}

BOOL CSockLx::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
    SOCKADDR_IN sa = {AF_INET,htons(nHostPort)};
    sa.sin_addr.s_addr = inet_addr(lpszHostAddress);

    return !connect(m_hSocket,(sockaddr*)&sa,sizeof(sa));
}

BOOL CSockLx::Accept(CSockLx& socka, LPTSTR sIP, UINT* pPort)
{
    if (sIP || pPort)
    {
        SOCKADDR_IN sa;
        socklen_t len = sizeof(sa);
        socka.m_hSocket = accept(m_hSocket, (sockaddr*)&sa, &len);
        if (socka.m_hSocket == INVALID_SOCKET)
            return FALSE;
        if (sIP)
            strcpy(sIP, inet_ntoa(sa.sin_addr));
        if (pPort)
            *pPort = htons(sa.sin_port);
    }
    else
        socka.m_hSocket = accept(m_hSocket, nullptr, nullptr);
    return socka.m_hSocket != INVALID_SOCKET;
}

CSockLx::CSockLx():m_hSocket(INVALID_SOCKET)
{
#ifdef _WIN32
    WSAData wd;
    if (!WSAStartup(0x0202, &wd))
    {

    }
#endif

}

CSockLx::~CSockLx()
{
   closesocket(m_hSocket);
}
