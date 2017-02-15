#pragma once

#include "SharedType.h"
#include "MobTable.h"
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <windows.h>

static RwUInt32 m_uiSerialId = 5;
static RwUInt32 m_iSerialId = 1000000;
static RwUInt32 m_uiTargetSerialId = 0;
class PlayersMain;
typedef struct _SBattleData
{
	RwUInt32			uiSerialId;
	RwUInt32			m_uiTargetSerialId;
	RwBool				bAttackMode;
	bool				bAttackerIsPlayer;
	bool				bDefenderIsPlayer;
	int	index_x;
	int index_z;

	DWORD				dwCurrTime;
}SBattleData;

typedef struct _SSkillData
{
	RwUInt32			pCharSkillTarget;
	RwUInt32			m_uiSkillTblId;
	RwUInt32			m_bySkillActiveType;
	DWORD				m_uiSkillTime;
}SSkillData;

#define MONSTER_ATTACK_UPDATE_TICK		2000
#define SKILL_TYPE_CASTING				1
#define SKILL_TYPE_NONE					0

static RwUInt32 m_iCurrentHp		=		10000;

//Commands
enum consoleCommands {
    eANNOUNCE = 0,
    eEMERGENCY,
    eADDITEM,
    eSPAWNMOB,
	eSPAWNNPC,
	eADDSKILL,
	eSCRIPTEXE,
	eHELP
};
#ifndef GS_FUNCTIONS_CLASS_H
#define GS_FUNCTIONS_CLASS_H


class CGameServer;
class CClientSession;
class CNtlPacket;

class GsFunctionsClass
{
public:
	GsFunctionsClass(){};
	~GsFunctionsClass(){};

public:

	bool						DeleteItemByUIdPlacePos(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 UniqueID, RwUInt32 Place, RwUInt32 Pos);
	bool						UpdateCharMoney(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 ChangeType, RwUInt32 MoneyAmount, RwUInt32 AvatarHandle);
	DWORD						CalculePowerLevel(sMOB_TBLDAT* pMOBtData);
	DWORD						CalculePowerLevelPlayer(PlayersMain *plr);
	//System Functions
	void						printError(const char* err);
	void						printOk(const char* err);
	void						printDebug(const char* dbg);	
  	//Items
	void						CreateUpdateItem(PlayersMain *plr, int stackCount, TBLIDX itemID, bool deleteItem, HSESSION ClientSession, int iUniqueHandle = 0, int byGrade = 0, int byRank = 1, int byBattleAttrib = 0, int place = 1, int pos = 0);
 	//Skills Like Debug
 	void						DebugSkillType(BYTE skillActType);
  	int							GetTotalSlotSkill(int charID);
 	int							GetBattleResultEffect(int RpEffect);
	//Quest
	void						QuestStarted(RwUInt32 CharID, NTL_TS_T_ID tIdQuest, NTL_TS_TC_ID tCurrentId, NTL_TS_TC_ID tNextAct, BYTE tsType, DWORD dwParam);
	void						NewQuest(RwUInt32 CharID, NTL_TS_T_ID tCurQuest, BYTE tsType, DWORD dwParam);
	//Time Quest
	TBLIDX						GetTmq(PlayersMain *plr);
	RwUInt8						GetTmqLevel(PlayersMain *plr);
	//MISC
	void						SendItemEffect(CClientSession * pSession,TBLIDX effectTbl,TBLIDX ItemTbl);
	//Command Handler
	int							consoleCommandHandler(std::string command);
	consoleCommands				parseConsoleEnums(std::string commands);
	void						commandFinished();

};

#endif