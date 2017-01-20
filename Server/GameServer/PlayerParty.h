#ifndef PLAYER_PARTY_CLASS_H
#define PLAYER_PARTY_CLASS_H

#include "NtlSfx.h"
#include "SharedDef.h"

#include <list>

class CGameServer;
class CClientSession;
class CNtlPacket;

class PlayerPartyClass
{
public:
	PlayerPartyClass(){};
	~PlayerPartyClass(){};

	void						CreateParty(RwUInt32 LeaderHandle);
typedef struct _PartyData
{
//	WCHAR*			PartyName;
//	std::string		LeaderName;
//	std::string		MemberName;
	RwUInt32		LeaderHandle;
//	TBLIDX			MemberHandle;
//	BYTE			Race;
//	BYTE			Class;
//	BYTE			Level;
//	WORD			CurLP;
//	WORD			MaxLP;
//	WORD			CurEP;
//	WORD			MaxEP;
//	DWORD			Zenny;
//	TBLIDX			worldTblidx;
//	WORLDID			worldId;
//	sVECTOR3		CurLoc;
//	BYTE			ItemLootingMethod;
//	BYTE			ZennyLootingMethod;
//	sPARTY_MEMBER_INFO	MemberInfo;
//	CClientSession * pSession;

}PartyData;
typedef std::list<PartyData*> PARTYLIST;
typedef PARTYLIST::iterator PARTYLISTIT;
PARTYLIST					m_partyList;
};

#endif