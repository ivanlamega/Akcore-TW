#include "StdAfx.h"
#include "NtlPacketUA.h"

const char * s_packetName_UA[] =
{
	DECLARE_PACKET_NAME( UA_LOGIN_REQ ),
	DECLARE_PACKET_NAME( UA_LOGIN_REQ_KOREA_CJ ),
	DECLARE_PACKET_NAME( UA_LOGIN_REQ_TAIWAN_CT ),
	DECLARE_PACKET_NAME( UA_LOGIN_REQ_CHINA_SD  ),
	DECLARE_PACKET_NAME( UA_LOGIN_CREATEUSER_REQ ),
	DECLARE_PACKET_NAME( UA_LOGIN_DISCONNECT_REQ ),
};


//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_UA(WORD wOpCode)
{
	if( wOpCode < UA_OPCODE_BEGIN )
	{
		return "NOT DEFINED UA PACKET NAME : OPCODE LOW";
	}

	if( wOpCode > UA_OPCODE_END )
	{
		return "NOT DEFINED UA PACKET NAME : OPCODE HIGH";
	}

	int nIndex = wOpCode - UA_LOGIN_REQ;
	if( nIndex >= _countof( s_packetName_UA ) )
	{
		return "UA OPCODE BUFFER OVERFLOW";
	}

	return s_packetName_UA[ nIndex ];
}
//------------------------------------------------------------------
