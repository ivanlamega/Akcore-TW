#include "stdafx.h"
#include "Vector.h"
#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "GameServer.h"
#include "Attributes.h"
#include "Character.h"
#include "Inventory.h"
#include "SkillsQS.h"

#ifndef PLAYERS_MAIN_H
#define PLAYERS_MAIN_H   

class PlayersMain{
	///-------------Constructor & Destructor-------------///
	public:
		PlayersMain(HSESSION PlayerSession, int CharID, int AccountID);
		~PlayersMain();
		///-------------------------------------///
	private:
		//Members
    	static RwUInt8 byChainAttack;
		int AccountID;
		int RPCounter;
		int RpBallFull;
		int iWorldID;
		int CharIDForDuel;
		bool isDead;
		bool isSitted;
		bool isfighting;
		bool isInTutorial;
		bool bTradeOk;
		bool bPlayerInTrade;
		bool bDuelStatus;
		bool bSkillInUse;
		DWORD lastFightTime;
		DWORD ChargingID;
		DWORD PlayerThreadID;
		HANDLE ChargingThread;
		HANDLE PlayerThread;
		TBLIDX WorldTblx;
		TBLIDX tblEquipedChips[8];
		RwUInt32 avatarHandle;
		RwUInt32 mob_SpawnTime;
    	HSESSION PlayerSession;
		sPC_PROFILE* sPlayerProfile;
		sPC_TBLDAT* sPcData;
		sCHARSTATE* sPlayerState;
		sVECTOR3 vCurLoc;
		sVECTOR3 vLastLoc;
		sVECTOR3 vCurDir;
		sVECTOR3 vLastDir;
		string sPlayerName;
		string sGuildName;
		void FillProfileWithInfo();
		void FillProfileWithAttribute();	
		void FillCharState();
	public:
		PlayersInventory* cPlayerInventory;
		PlayersSkills* cPlayerSkills;
		PlayerAttributes* cPlayerAttribute;
		PlayersMain* GetRefreshPointer(){ return this; };
		CClientSession* myCCSession = NULL;
		void CreatePlayerProfile();
		void SavePlayerData(CGameServer* app);
		void SetChainAttack(int number);
		int GetCharID();
		int GetCharIdForDuel();
		int GetAccountID();
		bool GetPlayerDead();
		bool GetPlayerSitGetUp();
		bool GetPlayerFight();		
		bool GetTradeOK();
		bool GetPlayerIsInTrade();
		bool GetDuelStatus();
		bool GetSkillInUse();
		bool HaveAnySpaceInScouter();
		bool IsInTutorial();
		int GetRpBallCounter();
		int GetRpBallFull();
		int GetWorldID();
		DWORD GetChargingID();
		DWORD GetPlayerThreadID();
		DWORD GetLastFightTime();
		HANDLE GetChargingThread();
		HANDLE GetPlayerThread();
		TBLIDX GetWorldTblx();
		TBLIDX* GetEquipedChips();
		RwUInt32 GetAvatarHandle();
		RwUInt32 GetMob_SpawnTime();
		RwUInt8 GetChainAttack();
    	RwUInt8 ChainNumber(){ return byChainAttack; };
		HSESSION GetSession();
		sPC_PROFILE* GetPcProfile();
		sBOT_PROFILE* sGetMobProfile();
		sCHARSTATE* GetCharState();
		sVECTOR3 GetPlayerPosition();
		sVECTOR3 GetPlayerLastPosition();
		sVECTOR3 GetPlayerDirection();
		sVECTOR3 GetPlayerLastDirection();
		string GetPlayerName();
		string GetGuildName();				
		void SetAvatarHandle(RwUInt32 AvatarHandle);
		void SetCharIDForDuel(int CharID);
		void SetPlayerName(string sPlayerName);		
		void SetPlayerPosition(sVECTOR3 sPlayerPosition);
		void SetPlayerLastPosition(sVECTOR3 sPlayerPosition);
		void SetPlayerDirection(sVECTOR3 sPlayerDirection);
		void SetPlayerLastDirection(sVECTOR3 sPlayerDirection);
		void SetGuildName(string sGuildName);		
		void SendRpBallInformation();
		void SendRpBallUpdate(int moreOrLess);
		void SetWorldTblidx(TBLIDX tblWorld);
		void SetWorldID(int iWorldID);
		void SetRPBall();
		void SetRpBallFull(int moreOrLess);
		void SetMob_SpawnTime(RwUInt32 id);
		void SetChargingID(DWORD id);
		void SetPlayerThreadID(DWORD id);
		void SetLastTimeFight(DWORD value);
		void SetChargindThread(HANDLE thread);
		void SetPlayerThread(HANDLE thread);
		void SetLevelUP();
		void SetPlayerSit(bool SitGetUp);
		void SetPlayerDead(bool isDead);
		void SetPlayerFight(bool isFighting);
		void SetTradeOK(bool bTradeOK);
		void SetPlayerIsInTrade(bool bIsInTrade);
		void SetPlayerIsInTutorial(bool bReally);
		void SetDuelStatus(bool bStatus);
		void SetSkillInUse(bool inUse);
		void SetPlayerDamage(int value);
		void SetCharState(int stateID);
		void SetStats(sPC_TBLDAT *pTblData);
		void UpdateBaseAttributeWithEquip(TBLIDX itemTBL, BYTE byRank, BYTE byGrade,bool bRemove = false);
		//Player Thread Functions
		void SendThreadUpdateEPLP();
		void SendThreadUpdateOnlyEP();
		void SendThreadUpdateOnlyLP();
		void SendThreadUpdateRP();
		void SendThreadUpdateDeathStatus();
		void SendThreadRevivalStatus();
};

#endif