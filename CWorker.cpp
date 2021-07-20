#include "CWorker.h"
using namespace std;
//	
//ss << "SELECT* FROM t_worker ORDER BY " << ps[n - 1];

bool CWorker::Select(const char* sSQL, CSockLx* pSocka)
{
    if (mysql_query(m_sql, sSQL))
    {
        printf("%s Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    auto res = mysql_store_result(m_sql);
    if (!res)
    {
        printf("%s store result Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    int nCount = static_cast<int>(res->row_count);
    pSocka->Send(&nCount, sizeof(nCount));
    SInfo d;
    MYSQL_ROW p;
    while (p = mysql_fetch_row(res))
    {
        FillData(p, d);
        pSocka->Send(&d, sizeof(d));
    }
    return true;
}

bool CWorker::OnPrint(CSockLx* pSocka)
{
    int nOrder = 0,nDesc=0;
    if (pSocka->Receive(&nOrder, sizeof(int)) < (int)sizeof(int))
        return false;
    if (nOrder < 0 || nOrder>3)
    {
        cout << "Error order by = " << nOrder << endl;
        return false;
    }
        
    static const char* ps[] = { "f_numb","f_name","f_sala","f_date",nullptr };
    stringstream ss;
    ss << "SELECT * FROM t_worker ORDER BY " << ps[nOrder];
   
    return Select(ss.str().c_str(), pSocka);
}

bool CWorker::OnInsert(CSockLx* pSocka)
{
    SInfo d;
    if(pSocka ->Receive(&d,sizeof(d))<sizeof(d))
        return false;
    stringstream ss;
    ss << "INSERT INTO t_worker VALUES(" << d.nNumb << ",'" << d.sName << "'," << d.fSala << ",'" << d.sDate << "')";
    if (mysql_query(m_sql, ss.str().c_str()))
    {
        printf("%s %u: %s\n",  ss.str().c_str(), mysql_errno(m_sql),mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    return true;
}
//sprintf C语言 sstream C++
bool CWorker::OnDelete(CSockLx* pSocka)
{
    int nNumb;
    if (pSocka->Receive(&nNumb, sizeof(nNumb)) < sizeof(nNumb))
        return false;
    //stringstream ss;
    //ss << "DELETE FROM t_worker WHERE f_numb=" << nNumb;
    char s[256];
    sprintf(s, "DELETE FROM t_worker WHERE f_numb=%d", nNumb);
    if (mysql_query(m_sql, s))
    {
        printf("%s %u: %s\n", s, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    //mysql_affected_rows()
    return true;
}
//sprintf sstream
bool CWorker::OnModify(CSockLx* pSocka)
{
    SInfo d;
    if (pSocka->Receive(&d, sizeof(d)) < sizeof(d))
        return false;
    stringstream ss;
    ss << "UPDATE t_worker SET f_name='" << d.sName << "',f_sala=" << d.fSala << ",f_date='" << d.sDate
        << "' WHERE f_numb = " << d.nNumb;
    if (mysql_query(m_sql, ss.str().c_str()))
    {
        printf("%s %u: %s\n", ss.str().c_str(), mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }

    //mysql_affected_rows()
    return true;
}

bool CWorker::OnCheck(CSockLx* pSocka)
{
    int nNumb;
    if (pSocka->Receive(&nNumb, sizeof(nNumb)) < sizeof(nNumb))
        return false;
    stringstream ss;
    ss << "SELECT f_numb FROM t_worker WHERE f_numb=" << nNumb;
    if (mysql_query(m_sql, ss.str().c_str()))
    {
        printf("%s query error %u: %s\n", ss.str().c_str(), mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    auto res = mysql_store_result(m_sql);
    if (!res)
    {
        printf("%s result error %u: %s\n", ss.str().c_str(), mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    int nCount = static_cast<int>(res->row_count);
    pSocka->Send(&nCount, sizeof(nCount));

    return true;
}

bool CWorker::OnFindNumb(CSockLx* pSocka,bool bSendInfo)
{//单选
    int nNumb;
    if (pSocka->Receive(&nNumb, sizeof(nNumb)) < sizeof(nNumb))
        return false;
    stringstream ss;
    ss << "SELECT * FROM t_worker WHERE f_numb=" << nNumb;
    return Select(ss.str().c_str(),pSocka);
}

bool CWorker::OnFindName(CSockLx* pSocka)
{
    char sName[32];
    int n = pSocka->Receive(sName, sizeof(sName));
    if (n <=0)
        return false;
    sName[n]=0;
    char s[256];
    sprintf(s, "SELECT * FROM t_worker WHERE f_name LIKE '%%%s%%'", sName);// C:\\windows
    return Select(s, pSocka);
}

bool CWorker::OnFindSalary(CSockLx* pSocka)
{
    float fMin, fMax;
    if (pSocka->Receive(&fMin, sizeof(fMin)) < sizeof(fMin))
        return false;
    if (pSocka->Receive(&fMax, sizeof(fMax)) < sizeof(fMax))
        return false;
    char s[256];
    sprintf(s, "SELECT * FROM t_worker WHERE f_sala>=%g AND f_sala <=%g",fMin,fMax);// C:\\windows
    return Select(s, pSocka);
}
//粘包问题：
//1、发固定长度：sizeof(sMin),sizeof(sMax)
//发固定长度：12个：1999-09-08,sizeof(sMax)
//2、间隔符号：|或者,等 std::string::find 或者strchr
//3、中间有回发间隔,ack,
bool CWorker::OnFindDate(CSockLx* pSocka)
{//
    char sMin[32], sMax[32];//1999-9-82008-10-25
    int n = pSocka->Receive(sMin, sizeof(sMin));
    if (n <=0)
        return false;
    sMin[n] = 0;
    int ack = 0;
    pSocka->Send(&ack, sizeof(ack));
    n = pSocka->Receive(sMax, sizeof(sMax));
    if (n <= 0)
        return false;
    sMax[n] = 0;
    char s[256];
    sprintf(s, "SELECT * FROM t_worker WHERE f_date>='%s' AND f_date <='%s'", sMin, sMax);// C:\\windows
    return Select(s, pSocka);
}
