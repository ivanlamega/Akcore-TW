#pragma once

#include "NtlPacketCommon.h"
#include "CSArchitecture.h"

enum eOPCODE_UA
{
	UA_OPCODE_BEGIN = 100,

	UA_LOGIN_REQ, //= UA_OPCODE_BEGIN,// Added for TW
	UA_LOGIN_REQ_KOREA_CJ,
	//Added for TW
	UA_LOGIN_REQ_TAIWAN_CT,
	UA_LOGIN_REQ_CHINA_SD,

	UA_LOGIN_CREATEUSER_REQ,
	UA_LOGIN_DISCONNECT_REQ,

	UA_OPCODE_DUMMY,
	UA_OPCODE_END = UA_OPCODE_DUMMY - 1
};

//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_UA(WORD wOpCode);
//------------------------------------------------------------------

#pragma pack(1)
//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_REQ)
	WCHAR		awchUserId[NTL_MAX_SIZE_USERID_UNICODE + 1];
	WCHAR		awchPasswd[NTL_MAX_SIZE_USERPW_UNICODE + 1];
	unsigned int unknown1;
	WORD		wLVersion;
	WORD		wRVersion;
	BYTE		abyMacAddress[DBO_MAX_ADAPTER_ADDRESS_LENGTH];
	RwUInt8		unKnown2;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_REQ_KOREA_CJ)
	DWORD		dwCodePage;
	WORD		wLVersion;
	WORD		wRVersion;
	BYTE		abyMacAddress[DBO_MAX_ADAPTER_ADDRESS_LENGTH];
	WORD		wCpCookieLength;
	char		szCpCookie[NTL_MAX_SIZE_CP_COOKIE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_REQ_TAIWAN_CT)
DWORD		dwCodePage;
WORD		wLVersion;
WORD		wRVersion;
BYTE		abyMacAddress[DBO_MAX_ADAPTER_ADDRESS_LENGTH];
WORD		wCpCookieLength;
char		szCpCookie[NTL_MAX_SIZE_CP_COOKIE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_REQ_CHINA_SD)
DWORD		dwCodePage;
WORD		wLVersion;
WORD		wRVersion;
BYTE		abyMacAddress[DBO_MAX_ADAPTER_ADDRESS_LENGTH];
WORD		wCpCookieLength;
char		szCpCookie[NTL_MAX_SIZE_CP_COOKIE + 1];
END_PROTOCOL()

//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_CREATEUSER_REQ)
	WCHAR		awchUserId[NTL_MAX_SIZE_USERID_UNICODE + 1];
	WCHAR		awchPasswd[NTL_MAX_SIZE_USERPW_UNICODE + 1];
	DWORD		dwCodePage;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UA_LOGIN_DISCONNECT_REQ)
	WCHAR				awchUserId[NTL_MAX_SIZE_USERID_UNICODE + 1];
	bool				bIsEnteringCharacterServer;
END_PROTOCOL()
//------------------------------------------------------------------

#pragma pack()