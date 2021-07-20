#include "CAdmin.h"
//_stricmp(
bool CAdmin::OnLogin(CSockLx* pSocka)
{//账号不区分大小写，密码区分
    SAdmin a;
    //int n = sizeof(a) - sizeof(int);
    int n = sizeof(a);
    if(pSocka ->Receive(&a,n)<n)
        return false;
    //接收客户端登陆请求并且查询账号权限
    char sSQL[256];
    _strlwr(a.sName);//
    sprintf(sSQL, "SELECT f_prior FROM t_admin WHERE f_name='%s' AND f_pass='%s'", a.sName, a.sPass);
    
    //如果查询成功，返回0。如果出现错误，返回非0值。
    int result = mysql_query(m_sql, sSQL);
    if (result)
    {
        printf("%s Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    //具有多个结果的MYSQL_RES结果集合。如果出现错误，返回NULL。
    auto res = mysql_store_result(m_sql);
    if (!res)
    {
        printf("%s store result Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    a.nState = 0;
    pSocka->cm_state = a;
    char s[256];
    sprintf(s, "UPDATE `t_admin` SET `f_state` = '%d' WHERE(`f_name` = '%s');", a.nState,a.sName);
    mysql_query(m_sql, s);
    

    int nPrior = -1;
    //检索结果集的下一行。在mysql_store_result()之后使用时，如果没有要检索的行，mysql_fetch_row()返回NULL。在mysql_use_result()之后使用时，
    //如果没有要检索的行或出现了错误，mysql_fetch_row()返回NULL。 行内值的数目由mysql_num_fields(result)给出。
    //如果行中保存了调用mysql_fetch_row()返回的值，将按照row[0]到row[mysql_num_fields(result) - 1]，访问这些值的指针。行中的NULL值由NULL指针指明。
    //可以通过调用mysql_fetch_lengths()来获得行中字段值的长度。对于空字段以及包含NULL的字段，长度为0。通过检查字段值的指针，能够区分它们。
    //如果指针为NULL，字段为NULL，否则字段为空。
    MYSQL_ROW p = mysql_fetch_row(res);
    if (p)
        nPrior = atoi(p[0]);//"0" "1"
    pSocka->Send(&nPrior, sizeof(nPrior));
    
    return true; 
}

bool CAdmin::OnPrint(CSockLx* pSocka)
{
    auto sSQL = "SELECT *FROM t_admin";
    if (mysql_query(m_sql, sSQL))
    {
        printf("%s Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
     //*从服务器上检索结果集时，就提取了行，并为之分配内存，存储到客户机中随后调用 
     // mysql_fetch_row() 就再也不会返回错误，因为它仅仅是把行脱离了已经保留结果集的数据结构。
     // mysql_fetch_row() 返回 NULL 始终表示已经到达结果集的末端。
    auto res = mysql_store_result(m_sql);
    if (!res)
    {
        printf("%s store result Error %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
        return false;
    }
    int nCount = static_cast<int>(res->row_count);
    pSocka->Send(&nCount, sizeof(nCount));
    SAdmin a;
    MYSQL_ROW p;
    //读取服务器上的信息
    while (p = mysql_fetch_row(res))
    {
        strcpy(a.sName, *p++);
        strcpy(a.sPass, *p++);
        a.nPrior = atoi(*p++);
        a.nState = atoi(*p);
        pSocka->Send(&a, sizeof(a));
    }
    return true;
}

bool CAdmin::OnInsert(CSockLx* pSocka)
{//入库前统一小写
    //_strlwr()
    SAdmin a;
    if(pSocka ->Receive(&a,sizeof(a))< sizeof(a))
        return false;
    _strlwr(a.sName);
    char sSQL[256];
    sprintf(sSQL, "INSERT INTO t_admin VALUES('%s','%s',%d,%d)", a.sName, a.sPass, a.nPrior,1);
    int nRes = 0;
    if (mysql_query(m_sql,sSQL))
    {
        nRes = mysql_errno(m_sql);
        if (nRes != 1062)
        {
            printf("%s %u: %s\n", sSQL, nRes, mysql_error(m_sql));//CError(nError,Discription
            return false;
        }
    }
    pSocka->Send(&nRes, sizeof(nRes));
    return true;
}

bool CAdmin::OnDelete(CSockLx* pSocka)
{
    char sName[32];

    int n = pSocka->Receive(sName, sizeof(sName));
    if(n <=0)
        return false;
    sName[n-1] = 0;
    int nRes = 0;
    _strlwr(sName);
    if (strcmp(sName, "admin"))
    {//强制保留admin账号
        char sSQL[256];
        sprintf(sSQL, "DELETE FROM t_admin WHERE f_name='%s'", sName);
        if (mysql_query(m_sql, sSQL))
        {
            printf("%s %u: %s\n", sSQL, mysql_errno(m_sql), mysql_error(m_sql));//CError(nError,Discription
            return false;
        }
        nRes = (int)mysql_affected_rows(m_sql);
    }
    pSocka->Send(&nRes, sizeof(nRes));
    return true;
}
