// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#pragma warning( disable:4651 )
#pragma warning( disable:4652 )
#pragma warning( disable:4653 )
#pragma warning( disable:4748 )
#pragma warning( disable:4100 )
#pragma warning( disable:4996 )

#pragma comment (lib, "../NtlDatabase/mysql/lib/mysqlcppconn.lib") 
#pragma comment (lib, "../NtlDatabase/mysql/lib/mysqlcppconn-static.lib")


#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include <wincrypt.h>


// TODO: reference additional headers your program requires here
#include "NtlBase.h"