//***********************************************************************************
//
//	File		:	NtlPacketSYS.h
//
//	Begin		:	2007-01-17
//
//	Copyright	:	®œ NTL-Inc Co., Ltd
//
//	Author		:	Hyun Woo, Koo   ( zeroera@ntl-inc.com )
//
//	Desc		:	
//
//***********************************************************************************

#pragma once


//------------------------------------------------------------------
// Ω√Ω∫≈€ opcode
//------------------------------------------------------------------
enum eOPCODE_SYS
{
	SYS_OPCODE_BEG = 0,
	SYS_ALIVE,
	SYS_PING,
	SYS_HANDSHAKE = 0x0003,
	SYS_HANDSHAKE_REPLY = 0x0004,
	SYS_AUTH = 0x0010,

	SYS_OPCODE_END_DUMMY,
	SYS_OPCODE_END = SYS_OPCODE_END_DUMMY - 1
};
//------------------------------------------------------------------
