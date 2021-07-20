#pragma once
#include "define.h"
#include "CAdmin.h"
#include "CWorker.h"
class CManager
{
	MYSQL* m_sql;
	CSockLx m_sock;
	CWorker m_worker = m_sql;
	CAdmin m_admin = m_sql;
	
	bool ConenctDB();
//	void Run(CSockLx* pSocka);
	bool OnReceive(CSockLx* pSocka);
	//c-style function 对立的是 C++ member function
	static PROC_RET theProc(void* p);
	
public:
	int Main();
	inline static CManager& getInstance()
	{
		static CManager m;
		return m;
	}
};

