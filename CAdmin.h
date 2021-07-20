#pragma once
#include "define.h"
class CAdmin
{
	MYSQL* &m_sql;
	

public:
	bool OnLogin(CSockLx* pSocka);
	bool OnPrint(CSockLx* pSocka);
	bool OnInsert(CSockLx* pSocka);
	bool OnDelete(CSockLx* pSocka);
	CAdmin(MYSQL*& sql) :m_sql(sql)
	{

	}
};

