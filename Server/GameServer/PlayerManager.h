#pragma once
#include "SharedType.h"
#include "NtlThread.h"
#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H


/*
Order of Members
1-Class Pointers/StructsPointers
2-Voids
3-Int,double
*/
class CPlayerManager : public CNtlRunObject
{
public:
	CPlayerManager();
	~CPlayerManager();

public:
	PlayersMain*    GetPlayer(RwUInt32 playerHandle);
	PlayersMain*	GetPlayerByID(RwUInt32 playerId);
	void			Init();
	void			Release();
	void			AddNewPlayer(RwUInt32 playerHandle,HSESSION PlayerSession, int CharID, int AccountID);
	void            RemovePlayer(RwUInt32 playerHandle);
	void			CreatePlayerThread();
	void			Run();
	int				GetTotalPlayers();
	void			SendSyncPacket(RwUInt32 PlayerHandle, sVECTOR3 loc, sVECTOR3 dir, BYTE byMovFlag);
private:
	CNtlThread * pThread;
	PlayersMain* cPlayersMain;
	bool m_bRun;
	int PlayerCounter;
	std::map<RwUInt32, PlayersMain*> m_map_Player ;
	typedef std::map<RwUInt32, PlayersMain*>::const_iterator itterType;
	itterType i;
	

}; 
extern CPlayerManager * g_pPlayerManager;
#endif //PLAYER_MANAGER_H