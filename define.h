#pragma once

#include "CSockLx.h"
#include <mysql.h>
#include <iostream>
#include <sstream>
#include <list>



enum {SERVER_PORT=35671};
//网络协议编号
enum {WK_PRINT=0x100,
	WK_INSERT,
	WK_DELETE,
	WK_MODIFY,
	WK_CHECK_NUMB,
	WK_FIND_NUMB,
	WK_FIND_NAME,
	WK_FIND_SALA,
	WK_FIND_DATE,

	ADM_LOGIN=0x200,
	ADM_PRINT,
	ADM_INSERT,
	ADM_DELETE,
	ADM_CHPASS,
};

struct SInfo
{
	int nNumb;
	char sName[20];
	float fSala;
	char sDate[20];
};



