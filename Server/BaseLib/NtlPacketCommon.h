#pragma once

#include "NtlSharedCommon.h"
#include "SharedType.h"
#include "SharedDef.h"

#include <string>
#include <map>

#pragma warning(disable : 4328)



//------------------------------------------------------------------
//
//------------------------------------------------------------------
struct sNTLPACKETHEADER
{
	sNTLPACKETHEADER(WORD wGivenOpCode) :
		wOpCode(wGivenOpCode) {}

	WORD			wOpCode;
};
//------------------------------------------------------------------



//------------------------------------------------------------------
//
//------------------------------------------------------------------
#define BEGIN_PROTOCOL(opcode)						\
struct s##opcode :									\
	public sNTLPACKETHEADER							\
{													\
	s##opcode() :									\
		sNTLPACKETHEADER(opcode)					\
	{												\
	}

#define END_PROTOCOL()	};
//------------------------------------------------------------------


//------------------------------------------------------------------
//
//------------------------------------------------------------------
#define BEGIN_PROTOCOL_IDENTITY( opcode, identity )	\
typedef s##identity s##opcode						\

#define END_PROTOCOL_IDENTITY()	;
//------------------------------------------------------------------


//------------------------------------------------------------------
#define DECLARE_PACKET_NAME( opcode )	{ #opcode }
//------------------------------------------------------------------

//------------------------------------------------------------------
///PACKETNAME_INSERT
///Adds opcode name and int to a string so we can see them in the log. 
//------------------------------------------------------------------
#define	PACKETNAME_INSERT(name)		m_mapPacketName.insert(PacketEnumNameMap::value_type(name, #name))



