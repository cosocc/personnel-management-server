#pragma once
#include "define.h"
class CWorker
{
	MYSQL* &m_sql;
	inline void FillData(MYSQL_ROW p, SInfo& d)
	{
		d.nNumb = atoi(*p++);//"1008"
		strcpy(d.sName, *p++);//’≈…∫
		d.fSala = static_cast<float>(atof(*p++));//cast 
		strcpy(d.sDate, *p);//’≈…∫
	}
	bool Select(const char* sSQL, CSockLx* pSocka);
public:
	bool OnPrint(CSockLx* pSocka);
	bool OnInsert(CSockLx* pSocka);
	bool OnDelete(CSockLx* pSocka);
	bool OnModify(CSockLx* pSocka);
	bool OnCheck(CSockLx* pSocka);
	bool OnFindNumb(CSockLx* pSocka, bool bSendInfo=true);
	bool OnFindName(CSockLx* pSocka);
	bool OnFindSalary(CSockLx* pSocka);
	bool OnFindDate(CSockLx* pSocka);
	CWorker(MYSQL* &sql) :m_sql(sql)
	{

	}
};
