#include "stdafx.h"
#include "GameServer.h"



void		PlayerPartyClass::CreateParty(RwUInt32 LeaderHandle)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	PartyData * pdd = new PartyData;

//	pdd->LeaderHandle = LeaderHandle;
//	pdd->LeaderName = LeaderName;
//	pd->PartyName = PartyName;
//	pd->pSession = pSession;

	m_partyList.push_back(pdd);

	//return true;
}






