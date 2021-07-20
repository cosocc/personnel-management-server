#include "CManager.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"libmysql.lib")

bool CManager::ConenctDB()
{
	m_sql = mysql_init(nullptr);
	if (m_sql == NULL)
	{
		printf("Error %u: %s\n", mysql_errno(m_sql), mysql_error(m_sql));
		return false;
	}
	//host参数可以用："localhost",".","192.168.0.88","127.0.0.1","lvxin"
    #ifdef _WIN32
	if (mysql_real_connect(m_sql, "localhost", "root", "123456", "worker", 0, NULL, 0) == NULL)
    #else
	if (mysql_real_connect(m_sql, "localhost", "root", "85236360", "worker", 0, NULL, 0) == NULL)
    #endif
	{
		printf("Error %u: %s\n", mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
		return false;
	}
	mysql_query(m_sql, "set names gbk");//FILE* pf;
	return true;
}

bool CManager::OnReceive(CSockLx* pSocka)                        //第一步协议命令接收
{//协议解析函数：一旦false就是中断！
	
	int nCmd = 0;
	if (pSocka->Receive(&nCmd, sizeof(nCmd))!= sizeof(nCmd))
		return false;
	
	switch (nCmd)                                               //协议转发
	{
	case WK_PRINT:
		return m_worker.OnPrint(pSocka);
	case WK_INSERT:
		return m_worker.OnInsert(pSocka);
	case WK_CHECK_NUMB:
		return m_worker.OnCheck(pSocka);
	case WK_DELETE:
		return m_worker.OnDelete(pSocka);
	case WK_MODIFY:
		return m_worker.OnModify(pSocka);
	case WK_FIND_NUMB:
		return m_worker.OnFindNumb(pSocka);
	case WK_FIND_NAME:
		return m_worker.OnFindName(pSocka);
	case WK_FIND_SALA:
		return m_worker.OnFindSalary(pSocka);
	case WK_FIND_DATE:
		return m_worker.OnFindDate(pSocka);
	case ADM_LOGIN:
		return m_admin.OnLogin(pSocka);                       //进入登录协议   对应的函数
	case ADM_PRINT:
		return m_admin.OnPrint(pSocka);
	case ADM_INSERT:
		return m_admin.OnInsert(pSocka);
	case ADM_DELETE:
		return m_admin.OnDelete(pSocka);
	}
	return false;
}
PROC_RET CManager::theProc(void* p)
{	

	//auto& m = getInstance();
	CManager& m = getInstance();
	//cout <<"\r\n" << typeid(m).name() << endl;	
	CSockLx* pSocka = (CSockLx*)p;	

	while (m.OnReceive(pSocka));

	//std::cout << pSocka->cm_state.sName << std::endl;
	//std::list<SAdmin>::iterator  iter;
	//for (iter =m.m_admin.m_state.begin();iter!=m.m_admin.m_state.end();iter++)
	//{
	//	std::cout << iter->sName << std::endl;
	//}
	pSocka->cm_state.nState = 1;
	char s[256];
	sprintf(s, "UPDATE `t_admin` SET `f_state` = '%d' WHERE(`f_name` = '%s');", pSocka->cm_state.nState, pSocka->cm_state.sName);
	mysql_query(m.m_sql,s);

	cout <<"线程退出，socka="<< *pSocka << endl;
	delete pSocka;


}
//
//void CManager::Run(CSockLx* pSocka)
//{
//	
//}

int CManager::Main()
{
	if (!ConenctDB())
		return -1;
	if (!m_sock.Create(SERVER_PORT))
	{
		cout << "创建端口失败：" << m_sock.GetLastError() << endl;
		return -1;
	}
	m_sock.Listen();
	//CSockLx socka;
	CSockLx* pSocka = new CSockLx;
	char sIP[32];
	UINT nPort;
	while (m_sock.Accept(*pSocka, sIP, &nPort))//m_sock.Accept(...)
	{
		cout << sIP << "-" << nPort << "连接" <<endl;
#ifdef _WIN32
		_beginthread(theProc,0, pSocka);
#else
		pthread_t id;
		pthread_create(&id, nullptr, theProc, pSocka);
#endif
		pSocka = new CSockLx;
	}
    return 0;
}
