#include "stdafx.h"
#include "NtlTokenizer.h"

#include "PacketGameServer.h"

#include "NtlPacketTU.h"
#include "NtlPacketUT.h"
#include "GameServer.h"

typedef std::list<SBattleData*> ListAttackBegin;
typedef ListAttackBegin::iterator BATTLEIT;
ListAttackBegin				m_listAttackBegin;
SSkillData *pSkillData = NULL;
#define PACKET_TRACE(opcode, packet) NTL_PRINT(PRINT_SYSTEM, "%s [%u] Size[%u]", NtlGetPacketName_GU(opcode), opcode, sizeof(packet));

static float
dbo_move_pos_to_float(uint32_t n)
{
	float f;
	unsigned char *p = (unsigned char *)&n;
	/* this works for little endian only */
	f = (p[1] << 24) + ((p[0] & 0x7f) << 16) + (p[3] << 8) + (p[2]);
	((char *)&f)[3] |= (p[0] & 0x80);	/* first byte sign bit for a float */
	f /= 100.0;
	return f;
}

#define DBO_MOVE_DIR_TO_FLOAT(n) \
	(n / (float) 10000.0)


float GetDistance(sVECTOR3 location1, sVECTOR3 location2)
{
	float lengthx = fabs(location1.x - location2.x);
	float lengthz = fabs(location1.z - location2.z);
	return sqrt((lengthx * lengthx) + (lengthz * lengthz));
}
float GetDistance(CNtlVector location1, sVECTOR3 location2)
{
	float lengthx = fabs(location1.x - location2.x);
	float lengthz = fabs(location1.z - location2.z);
	return sqrt((lengthx * lengthx) + (lengthz * lengthz));
}

//--------------------------------------------------------------------------------------//
//		Log into Game Server
//--------------------------------------------------------------------------------------//
void CClientSession::SendGameEnterReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- LOAD CHAT SERVER --- \n");
	sUG_GAME_ENTER_REQ * req = (sUG_GAME_ENTER_REQ *)pPacket->GetPacketData();

	avatarHandle = AcquireSerialId();

	g_pPlayerManager->AddNewPlayer(avatarHandle, this->GetHandle(), req->charId, req->accountId);

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	plr->CreatePlayerProfile();
	this->cPlayersMain = plr;
	plr->myCCSession = this;
	CNtlPacket packet(sizeof(sGU_GAME_ENTER_RES));

	app->db->prepare("UPDATE characters SET IsOnline = 1,TutorialFlag = ? WHERE CharID = ?");
	app->db->setInt(1, (req->bTutorialMode == true ? 0 : 1));
	app->db->setInt(2, plr->GetCharID());
	app->db->execute();

	sGU_GAME_ENTER_RES * res = (sGU_GAME_ENTER_RES *)packet.GetPacketData();

	res->wOpCode = GU_GAME_ENTER_RES;
	res->wResultCode = GAME_SUCCESS;
	strcpy_s(res->achCommunityServerIP, sizeof(res->achCommunityServerIP), app->GetConfigFileExternalIP());
	res->wCommunityServerPort = 20400;

	packet.SetPacketLen(sizeof(sGU_GAME_ENTER_RES));
	PACKET_TRACE(GU_GAME_ENTER_RES, packet);
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Send avatar char info
//--------------------------------------------------------------------------------------//
void CClientSession::CheckPlayerStat(CGameServer * app, sPC_TBLDAT *pTblData, int level,RwUInt32 playerHandle)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(playerHandle);
	app->db->prepare("UPDATE characters SET BaseStr = ?, BaseCon = ?, BaseFoc = ?, BaseDex = ?,BaseSol = ?, BaseEng = ? WHERE CharID = ?");
	app->db->setInt(1, pTblData->byStr + (pTblData->fLevel_Up_Str * level));
	app->db->setInt(2, pTblData->byCon + (pTblData->fLevel_Up_Con * level));
	app->db->setInt(3, pTblData->byFoc + (pTblData->fLevel_Up_Foc * level));
	app->db->setInt(4, pTblData->byDex + (pTblData->fLevel_Up_Dex * level));
	app->db->setInt(5, pTblData->bySol + (pTblData->fLevel_Up_Sol * level));
	app->db->setInt(6, pTblData->byEng + (pTblData->fLevel_Up_Eng * level));
	app->db->setInt(7, plr->GetCharID());
	app->db->execute();

	app->db->prepare("UPDATE characters SET BaseAttackRate = ?, BaseAttackSpeedRate = ?, BaseEnergyDefence = ?, BaseEnergyOffence = ?,BasePhysicalDefence = ?, BasePhysicalOffence = ? WHERE CharID = ?");
	app->db->setInt(1, pTblData->wAttack_Rate);
	app->db->setInt(2, pTblData->wAttack_Speed_Rate);
	app->db->setInt(3, pTblData->wBasic_Energy_Defence + (pTblData->byLevel_Up_Energy_Defence * level));
	app->db->setInt(4, pTblData->wBasic_Energy_Offence + (pTblData->byLevel_Up_Energy_Offence * level));
	app->db->setInt(5, pTblData->wBasic_Physical_Defence + (pTblData->byLevel_Up_Physical_Defence * level));
	app->db->setInt(6, pTblData->wBasic_Physical_Offence + (pTblData->byLevel_Up_Physical_Offence * level));
	app->db->setInt(7, plr->GetCharID());
	app->db->execute();

	WORD basiclife = pTblData->wBasic_LP + (pTblData->byLevel_Up_LP * level);
	WORD levelcon = pTblData->byCon + (pTblData->fLevel_Up_Con * level);
	WORD LP = basiclife + ((levelcon * level) * 4.7);

	WORD basicenergy = pTblData->wBasic_EP + (pTblData->byLevel_Up_EP * level);
	WORD leveleng = pTblData->byEng + (pTblData->fLevel_Up_Eng * level);
	WORD EP = basicenergy + ((leveleng * level) * 4.2);

	app->db->prepare("UPDATE characters SET BaseMaxLP = ?, BaseMaxEP = ?, BaseMaxRP = ?, BaseDodgeRate = ?, BaseAttackRate = ?, BaseBlockRate = ?, BasePhysicalCriticalRate = ?, BaseEnergyCriticalRate = ? WHERE CharID = ?");
	app->db->setInt(1, LP);
	app->db->setInt(2, EP);
	app->db->setInt(3, pTblData->wBasic_RP + (pTblData->byLevel_Up_RP * level));
	app->db->setInt(4, pTblData->wDodge_Rate);
	app->db->setInt(5, pTblData->wAttack_Rate);
	app->db->setInt(6, pTblData->wBlock_Rate);
	app->db->setInt(7, 10);
	app->db->setInt(8, 10);
	app->db->setInt(9, plr->GetCharID());
	app->db->execute();

	plr->SetStats(pTblData);
}
void CClientSession::SendAvatarCharInfo(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- LOAD CHARACTER INFO FOR GAMESERVER --- \n");
	CNtlPacket packet(sizeof(sGU_AVATAR_CHAR_INFO));
	sGU_AVATAR_CHAR_INFO * res = (sGU_AVATAR_CHAR_INFO *)packet.GetPacketData();

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	app->db->prepare("UPDATE characters SET OnlineID = ? WHERE CharID = ?");
	app->db->setInt(1, plr->GetAvatarHandle());
	app->db->setInt(2, plr->GetCharID());
	app->db->execute();

	res->wOpCode = GU_AVATAR_CHAR_INFO;
	res->handle = this->GetavatarHandle();


	memcpy(&res->sPcProfile, plr->GetPcProfile(), sizeof(sPC_PROFILE));
	memcpy(&res->sCharState, plr->GetCharState(), sizeof(sCHARSTATE));
	res->wCharStateSize = sizeof(sCHARSTATE_BASE);	
	//res->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	//res->sCharState.sCharStateBase.aspectState.sAspectStateBase.byAspectStateId = 0xff;
	//res->sCharState.sCharStateBase.bFightMode = false;
	//res->sPcProfile.avatarAttribute.wBaseMaxAp = 450000;
	//res->sPcProfile.avatarAttribute.wLastMaxAp = 450000;
	//res->sPcProfile.bIsGameMaster = true;
	//res->sPcProfile.bIsAdult = false;
	//res->sPcProfile.byLevel = 1;
	//res->sPcProfile.dwCurAp = 100000;//app->db->getInt("CurAp");//New AP TW
//	res->sPcProfile.dwCurExp = 0;
	packet.SetPacketLen(sizeof(sGU_AVATAR_CHAR_INFO));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	
	plr->SendRpBallInformation();
	PACKET_TRACE(GU_AVATAR_CHAR_INFO, packet);
	std::string log = "Player " + plr->GetPlayerName() + " have join the game\n";
	this->gsf->printDebug(log.c_str());
	plr = NULL;
	delete plr;
}

//--------------------------------------------------------------------------------------//
//		Send Avatar Iteminfo
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarItemInfo(CNtlPacket * pPacket, CGameServer * app)
{
	printf("Send item info\n");
	CNtlPacket packet(sizeof(sGU_AVATAR_ITEM_INFO));
	sGU_AVATAR_ITEM_INFO * res = (sGU_AVATAR_ITEM_INFO *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sITEM_PROFILE asItemProfile[NTL_MAX_COUNT_USER_HAVE_INVEN_ITEM];
	
	res->wOpCode = GU_AVATAR_ITEM_INFO;
	plr->cPlayerInventory->GetInventory();
	res->byBeginCount = plr->cPlayerInventory->GetTotalItemsCount();
	res->byItemCount = plr->cPlayerInventory->GetTotalItemsCount();
	printf("res->BeginCount %u \n  Res->ItemCount %u \n"), (unsigned char)res->byItemCount, (unsigned char)res->byBeginCount;
	for (int i = 0; i < res->byItemCount; i++)
	{
		res->aItemProfile[i].handle = plr->cPlayerInventory->GetInventory()[i].handle;
		res->aItemProfile[i].tblidx = plr->cPlayerInventory->GetInventory()[i].tblidx;
		res->aItemProfile[i].byPlace = plr->cPlayerInventory->GetInventory()[i].byPlace;
		res->aItemProfile[i].byPos = plr->cPlayerInventory->GetInventory()[i].byPos;
		res->aItemProfile[i].byStackcount = plr->cPlayerInventory->GetInventory()[i].byStackcount;
		res->aItemProfile[i].byRank = plr->cPlayerInventory->GetInventory()[i].byRank;
		res->aItemProfile[i].byGrade = plr->cPlayerInventory->GetInventory()[i].byGrade;
		res->aItemProfile[i].byCurDur = plr->cPlayerInventory->GetInventory()[i].byCurDur;
		res->aItemProfile[i].byBattleAttribute = 2;
		res->aItemProfile[i].byDurationType = 100;
		asItemProfile[1].aitemEffect[1].wType = 2;
		asItemProfile[1].aitemEffect[1].dwValue = 8;
		asItemProfile[1].aitemExtraEffect[1].wType = 2;
		asItemProfile[1].aitemExtraEffect[1].dwValue = 8;
	}	
		
	
	
			
	packet.AdjustPacketLen(sizeof(sNTLPACKETHEADER) + (2 * sizeof(BYTE)) + (res->byItemCount * sizeof(sITEM_PROFILE)));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_AVATAR_ITEM_INFO, packet);
	plr = NULL;
	delete plr;
}
void CClientSession::SendSlotInfo(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_QUICK_SLOT_INFO));
	sGU_QUICK_SLOT_INFO * res = (sGU_QUICK_SLOT_INFO *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	res->wOpCode = GU_QUICK_SLOT_INFO;
	res->byQuickSlotCount = plr->cPlayerSkills->GetQuickSlotCount();
	for (int i = 0; i < res->byQuickSlotCount; i++)
	{
		res->asQuickSlotData[i].bySlot = plr->cPlayerSkills->GetQuickSlot()[i].bySlot;
		res->asQuickSlotData[i].byType = plr->cPlayerSkills->GetQuickSlot()[i].byType;
		res->asQuickSlotData[i].hItem = plr->cPlayerSkills->GetQuickSlot()[i].hItem;
		res->asQuickSlotData[i].tblidx = plr->cPlayerSkills->GetQuickSlot()[i].tblidx;
	}
	packet.AdjustPacketLen(sizeof(sNTLPACKETHEADER) + (2 * sizeof(BYTE)) + (res->byQuickSlotCount * (sizeof(sQUICK_SLOT_DATA))));

	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_QUICK_SLOT_INFO, packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Send Avatar Skillinfo Fix By Marco
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarSkillInfo(CNtlPacket * pPacket, CGameServer * app)
{
	printf("Send skill info\n");

	CNtlPacket packet(sizeof(sGU_AVATAR_SKILL_INFO));
	sGU_AVATAR_SKILL_INFO * res = (sGU_AVATAR_SKILL_INFO *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	res->bySkillCount = plr->cPlayerSkills->GetSkillCount();
	for (int i = 0; i < res->bySkillCount; i++)
	{
		res->aSkillInfo[i].bIsRpBonusAuto = plr->cPlayerSkills->GetSkills()[i].bIsRpBonusAuto;
		res->aSkillInfo[i].byRpBonusType = plr->cPlayerSkills->GetSkills()[i].byRpBonusType;
		res->aSkillInfo[i].bySlotId = plr->cPlayerSkills->GetSkills()[i].bySlotId;
		res->aSkillInfo[i].dwTimeRemaining = plr->cPlayerSkills->GetSkills()[i].dwTimeRemaining;
		res->aSkillInfo[i].nExp = plr->cPlayerSkills->GetSkills()[i].nExp;
		res->aSkillInfo[i].tblidx = plr->cPlayerSkills->GetSkills()[i].tblidx;
		//res->aSkillInfo[i].tblidx = 0x51af; // this is Fly skyll, dont exist in tblx
		//res->aSkillInfo[i].tblidx = 0x4ef3; // Dash skill but wonrk fine in data base
	}
	
	
	//res->bySkillCount += 2;
	//res->aSkillInfo[res->bySkillCount -1].tblidx = 0x51af;// this is Fly skyll, dont exist in tblx
	//res->aSkillInfo[res->bySkillCount -2].tblidx = 0x4ef3; // Dash skill but wonrk fine in data base 
	
	res->wOpCode = GU_AVATAR_SKILL_INFO;
	packet.SetPacketLen(sizeof(sGU_AVATAR_SKILL_INFO));

	PACKET_TRACE(GU_AVATAR_SKILL_INFO, packet);
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		SendAvatarHTBInfo Fix By Marco
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarHTBInfo(CNtlPacket * pPacket, CGameServer* app)
{
	CNtlPacket packet(sizeof(sGU_AVATAR_HTB_INFO));
	sGU_AVATAR_HTB_INFO * res = (sGU_AVATAR_HTB_INFO *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	
	res->byHTBSkillCount = plr->cPlayerSkills->GetHTBSkillCount();
	for (int i = 0; i < res->byHTBSkillCount; i++)
	{
		res->aHTBSkillnfo[i].bySlotId = plr->cPlayerSkills->GetHTBSkills()[i].bySlotId;
		res->aHTBSkillnfo[i].dwTimeRemaining = plr->cPlayerSkills->GetHTBSkills()[i].dwTimeRemaining;
		res->aHTBSkillnfo[i].skillId = plr->cPlayerSkills->GetHTBSkills()[i].skillId;
	}
	//memset(&res, 0, sizeof(sGU_AVATAR_HTB_INFO));
	
	::ZeroMemory(res, sizeof(sGU_AVATAR_HTB_INFO));

	//res->byHTBSkillCount = 0;
	/*res->aHTBSkillnfo[0].bySlotId = 0;
	res->aHTBSkillnfo[0].dwTimeRemaining = 0;
	res->aHTBSkillnfo[0].skillId = 0;*/
	res->wOpCode = GU_AVATAR_HTB_INFO;
	packet.SetPacketLen(sizeof(sGU_AVATAR_HTB_INFO));
	PACKET_TRACE(GU_AVATAR_HTB_INFO, packet);
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		SendCharRevivalRequest Need Fix
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharRevivalReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	plr->SendThreadRevivalStatus();
	sUG_CHAR_REVIVAL_REQ * req = (sUG_CHAR_REVIVAL_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_CHAR_REVIVAL_RES));
	sGU_CHAR_REVIVAL_RES * res = (sGU_CHAR_REVIVAL_RES *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_AVATAR_WORLD_INFO));
	sGU_AVATAR_WORLD_INFO * res2 = (sGU_AVATAR_WORLD_INFO *)packet2.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* pStand = (sGU_UPDATE_CHAR_STATE*)packet3.GetPacketData();

	CNtlPacket packet4(sizeof(sGU_OBJECT_DESTROY));
	sGU_OBJECT_DESTROY* pCorpse = (sGU_OBJECT_DESTROY*)packet4.GetPacketData();

	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();

	res2->wOpCode = GU_AVATAR_WORLD_INFO;
	res2->worldInfo.tblidx = app->db->getInt("WorldTable");
	res2->worldInfo.worldID = app->db->getInt("WorldID");
	res2->worldInfo.hTriggerObjectOffset = 100000;
	res2->worldInfo.sRuleInfo.byRuleType = GAMERULE_NORMAL;
	res2->vCurLoc.x = app->db->getInt("CurLocX");
	res2->vCurLoc.y = app->db->getInt("CurLocY");
	res2->vCurLoc.z = app->db->getInt("CurLocZ");
	res2->vCurDir.x = app->db->getInt("CurDirX");
	res2->vCurDir.y = app->db->getInt("CurDirY");
	res2->vCurDir.z = app->db->getInt("CurDirZ");

	plr->SetWorldID(app->db->getInt("WorldID"));
	plr->SetWorldTblidx(app->db->getInt("WorldTable"));

	pStand->handle = this->GetavatarHandle();
	pStand->wOpCode = GU_UPDATE_CHAR_STATE;
	pStand->sCharState.sCharStateBase.byStateID = CHARSTATE_FLAG_STUNNED;
	pStand->sCharState.sCharStateBase.vCurLoc.x = res2->vCurLoc.x;
	pStand->sCharState.sCharStateBase.vCurLoc.y = res2->vCurLoc.y;
	pStand->sCharState.sCharStateBase.vCurLoc.z = res2->vCurLoc.z;
	pStand->sCharState.sCharStateBase.vCurDir.x = res2->vCurDir.x;
	pStand->sCharState.sCharStateBase.vCurDir.y = res2->vCurDir.y;
	pStand->sCharState.sCharStateBase.vCurDir.z = res2->vCurDir.z;

	res->wOpCode = GU_CHAR_REVIVAL_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_CHAR_REVIVAL_RES));
	packet2.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet2);
	PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet2);
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_CHAR_REVIVAL_RES, packet);
	g_pApp->Send(this->GetHandle(), &packet3);
	PACKET_TRACE(GU_UPDATE_CHAR_STATE, packet3);




	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		SendAvatarBuffInfo Need test
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarBuffInfo(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_AVATAR_BUFF_INFO));
	sGU_AVATAR_BUFF_INFO * res = (sGU_AVATAR_BUFF_INFO *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	res->byBuffCount = plr->cPlayerSkills->GetSkillBuffCount();
	for (int i = 0; i < res->byBuffCount; i++)
	{
		res->aBuffInfo[i].bySourceType = plr->cPlayerSkills->GetSkillBuff()[i].bySourceType;
		res->aBuffInfo[i].dwInitialDuration = plr->cPlayerSkills->GetSkillBuff()[i].dwInitialDuration;
		res->aBuffInfo[i].dwTimeRemaining = plr->cPlayerSkills->GetSkillBuff()[i].dwTimeRemaining;
		res->aBuffInfo[i].afEffectValue[0] = plr->cPlayerSkills->GetSkillBuff()[i].afEffectValue[0];
		res->aBuffInfo[i].afEffectValue[1] = plr->cPlayerSkills->GetSkillBuff()[i].afEffectValue[1];
		res->aBuffInfo[i].sourceTblidx = plr->cPlayerSkills->GetSkillBuff()[i].sourceTblidx;
	}
	res->wOpCode = GU_AVATAR_BUFF_INFO;

	packet.SetPacketLen(sizeof(sGU_AVATAR_BUFF_INFO));
	PACKET_TRACE(GU_AVATAR_BUFF_INFO, packet);
	g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		SendAvatarQuestList need test
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarQuestList(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_AVATAR_QUEST_PROGRESS_INFO));
	sGU_AVATAR_QUEST_PROGRESS_INFO * res = (sGU_AVATAR_QUEST_PROGRESS_INFO *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_AVATAR_QUEST_COMPLETE_INFO));
	sGU_AVATAR_QUEST_COMPLETE_INFO * res2 = (sGU_AVATAR_QUEST_COMPLETE_INFO *)packet2.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_AVATAR_QUEST_INVENTORY_INFO));
	sGU_AVATAR_QUEST_INVENTORY_INFO * res3 = (sGU_AVATAR_QUEST_INVENTORY_INFO *)packet3.GetPacketData();
	int iQuestCounter = 0;

	app->db->prepare("SELECT * FROM charquestlist WHERE charId = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	int iQuestList = 0;

	while (app->db->fetch())
	{
		int questID = app->db->getInt("questID");
		int currentStep = app->db->getInt("currentStep");
		int nextStep = app->db->getInt("nextStep");
		if (nextStep == 255)
		{
			unsigned char& c = res2->completeInfo.abyQCInfo[questID / eCOMPLETE_QUEST_QUEST_PER_BYTE];
			int nShift = (questID % eCOMPLETE_QUEST_QUEST_PER_BYTE) * eCOMPLETE_QUEST_STATE_MAX;

			c |= (eCOMPLETE_QUEST_STATE_CLEAR << nShift);
			iQuestCounter++;
		}
		else
		{
			res->progressInfo[iQuestList].tId = questID;
			res->progressInfo[iQuestList].byVer = 1;
			res->progressInfo[iQuestList].uData.sQInfoV0.wQState = 0;//ALWAYS 0
			res->progressInfo[iQuestList].uData.sQInfoV0.sMainTSP.tcCurId = currentStep;//Quest Pointer current
			res->progressInfo[iQuestList].uData.sQInfoV0.sMainTSP.tcPreId = nextStep;//Quest Pointer Next
			res->progressInfo[iQuestList].uData.sQInfoV0.sSSM.auiSSM[iQuestList] = questID;
			//Time Quest?
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[0].tcId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[0].taId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[1].tcId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[1].taId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[2].tcId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[2].taId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[3].tcId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sETSlot.asExceptTimer[3].taId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sSToCEvtData.tcId = 0xff;
			res->progressInfo[iQuestList].uData.sQInfoV0.sSToCEvtData.taId = 0xff;
			if (app->db->getInt("dwEventData") == 391)
			{
			res->progressInfo[iQuestList].uData.sQInfoV0.sSToCEvtData.tcId = currentStep;
			res->progressInfo[iQuestList].uData.sQInfoV0.sSToCEvtData.taId = app->db->getInt("dwEventData");
			}
			///
			res->progressInfo[iQuestList].uData.sQInfoV0.tcQuestInfo = currentStep;
			res->progressInfo[iQuestList].uData.sQInfoV0.taQuestInfo = nextStep;
			res->progressInfo[iQuestList].uData.sQInfoV0.tgExcCGroup = 0;
			iQuestList++;
		}
	}
	res2->wOpCode = GU_AVATAR_QUEST_COMPLETE_INFO;

	res->byProgressCount = iQuestList;
	res->wOpCode = GU_AVATAR_QUEST_PROGRESS_INFO;
	packet.SetPacketLen(sizeof(sGU_AVATAR_QUEST_PROGRESS_INFO));
	packet2.SetPacketLen(sizeof(sGU_AVATAR_QUEST_COMPLETE_INFO));
	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet);

	plr = NULL;
	delete plr;

}

//--------------------------------------------------------------------------------------//
//		SendAvatarInfoEnd
//--------------------------------------------------------------------------------------//
void CClientSession::SendAvatarInfoEnd(CNtlPacket * pPacket)
{
	//printf("--- GU_AVATAR_INFO_END --- \n");
	CNtlPacket packet(sizeof(sGU_AVATAR_INFO_END));
	sGU_AVATAR_INFO_END * res = (sGU_AVATAR_INFO_END *)packet.GetPacketData();

	res->wOpCode = GU_AVATAR_INFO_END;
	packet.SetPacketLen(sizeof(sGU_AVATAR_INFO_END));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_AVATAR_INFO_END, packet);
}
//--------------------------------------------------------------------------------------//
//		NetMarbleMEmberSHip - Luiz45
//--------------------------------------------------------------------------------------//
void CClientSession::SendNetMarbleMemberShipNFY(CNtlPacket* pPacket, CGameServer* app)
{
	CNtlPacket packet(sizeof(sGU_ENTER_WORLD_RES));
	sGU_ENTER_WORLD_RES* res = (sGU_ENTER_WORLD_RES*)packet.GetPacketData();
	res->wOpCode = GU_ENTER_WORLD_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_ENTER_WORLD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	//printf("--- sGU_NETMARBLEMEMBERIP_NFY --- \n");

	CNtlPacket packet1(sizeof(sGU_NETMARBLEMEMBERIP_NFY));
	sGU_NETMARBLEMEMBERIP_NFY* res1 = (sGU_NETMARBLEMEMBERIP_NFY*)packet1.GetPacketData();
	res1->wOpCode = GU_NETMARBLEMEMBERIP_NFY;
	packet1.SetPacketLen(sizeof(sGU_NETMARBLEMEMBERIP_NFY));
	g_pApp->Send(this->GetHandle(), &packet1);


	PACKET_TRACE(GU_NETMARBLEMEMBERIP_NFY, packet);
	
}
//--------------------------------------------------------------------------------------//
//		Server Contents ? - Luiz45
//--------------------------------------------------------------------------------------//
void CClientSession::SendServerContents(CNtlPacket* pPacket, CGameServer* app)
{
	//printf("--- GU_CONTENTS_ONOFF --- \n");
	BYTE contents_onoff[] = { 0xe4, 0x15, 0x00, 0x00, 0x40, 0xb1, 0xff, 0x12, 0x00, 0xf8, 0x01, 0x00, 0x00, 0x90, 0x00, 0x06, 0xda, 0x08, 0x00, 0x00, 0xd7, 0x08, 0x00, 0x00, 0xdb, 0x08, 0x00, 0x00, 0xd8, 0x08, 0x00, 0x00, 0xdc, 0x08, 0x00, 0x00, 0xd9, 0x08, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };
	CNtlPacket packet(0x10e);
	cout << "SERVER CONTENTS SHOULD == OPCODE 5604" << endl;
	packet.SetPacket(contents_onoff, 0x10e);
	cout << "Contents_onoff size is " << sizeof(contents_onoff) << endl;
	cout << "Should be 270" << endl;
	packet.SetPacketLen(0x10c);

	packet.GetPacketHeader()->bEncrypt = false;
	PushHandshakePacket(&packet);
	PACKET_TRACE(GU_SERVER_CONTENTS_ONOFF, packet);
}

//--------------------------------------------------------------------------------------//
//		Login into World
//--------------------------------------------------------------------------------------//
void CClientSession::SendWorldEnterReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- sGU_AVATAR_WORLD_INFO --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
	sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();

	res->wOpCode = GU_AVATAR_WORLD_INFO;
	res->byDojoCount = 0;
	for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
	{
		res->sDojoData[n].guildId = 0xffffffff;
		res->sDojoData[n].dojoTblidx = 0xffffffff;
	}
	//Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
	if (app->db->getInt("TutorialFlag") == 1)
	{
		res->worldInfo.tblidx = 1; // app->db->getInt("WorldTable");
		res->worldInfo.worldID = 1;//app->db->getInt("WorldID");
		res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
		res->worldInfo.sRuleInfo.byRuleType = GAMERULE_NORMAL;
		res->vCurLoc.x = plr->GetPlayerPosition().x;
		res->vCurLoc.y = plr->GetPlayerPosition().y;
		res->vCurLoc.z = plr->GetPlayerPosition().z;
		res->vCurDir.x = plr->GetPlayerDirection().x;
		res->vCurDir.y = plr->GetPlayerDirection().y;
		res->vCurDir.z = plr->GetPlayerDirection().z;
		plr->SetWorldID(app->db->getInt("WorldID"));
		plr->SetWorldTblidx(app->db->getInt("WorldTable"));
	}
	else
	{
		CNewbieTable* pNewBieTable = app->g_pTableContainer->GetNewbieTable();
		sNEWBIE_TBLDAT* pNewbieTbldat = reinterpret_cast<sNEWBIE_TBLDAT*>(pNewBieTable->GetNewbieTbldat(app->db->getInt("Race"), app->db->getInt("Class")));
		res->worldInfo.tblidx = pNewbieTbldat->tutorialWorld;
		res->worldInfo.worldID = pNewbieTbldat->world_Id;
		res->worldInfo.hTriggerObjectOffset = 100000;
		res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TUTORIAL;
		//Hard Coded
		res->vCurLoc.x = (-1)*78.90;
		res->vCurLoc.y = 46.95;
		res->vCurLoc.z = (-1) * 168.35;
		res->vCurDir.x = (-1) * 0.95;
		res->vCurDir.y = 0;
		res->vCurDir.z = 0.30;
		plr->SetWorldID(pNewbieTbldat->world_Id);
		plr->SetWorldTblidx(pNewbieTbldat->tutorialWorld);
		plr->SetPlayerIsInTutorial(true);
	}

	packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
	plr = NULL;
	delete plr;

}
void CClientSession::SendWorldEnterReq1(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- sGU_AVATAR_WORLD_INFO --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
	sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();

	res->wOpCode = GU_AVATAR_WORLD_INFO;
	res->byDojoCount = 0;
	for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
	{
		res->sDojoData[n].guildId = 0xffffffff;
		res->sDojoData[n].dojoTblidx = 0xffffffff;
	}
	//Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
	if (app->db->getInt("TutorialFlag") == 1)
	{

		res->worldInfo.tblidx = app->db->getInt("WorldTable");
		res->worldInfo.worldID = app->db->getInt("WorldID");
		res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
		res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
		res->vCurLoc.x = app->db->getDouble("tlqlocX");
		res->vCurLoc.y = app->db->getDouble("tlqlocY");
		res->vCurLoc.z = app->db->getDouble("tlqlocZ");
		res->vCurDir.x = app->db->getDouble("tlqdirX");
		res->vCurDir.y = app->db->getDouble("tlqdirY");
		res->vCurDir.z = app->db->getDouble("tlqdirZ");
		plr->SetWorldID(app->db->getInt("WorldID"));
		plr->SetWorldTblidx(app->db->getInt("WorldTable"));
	}
	else
	{
		CNewbieTable* pNewBieTable = app->g_pTableContainer->GetNewbieTable();
		sNEWBIE_TBLDAT* pNewbieTbldat = reinterpret_cast<sNEWBIE_TBLDAT*>(pNewBieTable->GetNewbieTbldat(app->db->getInt("Race"), app->db->getInt("Class")));
		res->worldInfo.tblidx = pNewbieTbldat->tutorialWorld;
		res->worldInfo.worldID = pNewbieTbldat->world_Id;
		res->worldInfo.hTriggerObjectOffset = 100000;
		res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TUTORIAL;
		//Hard Coded
		res->vCurLoc.x = (-1)*78.90;
		res->vCurLoc.y = 46.95;
		res->vCurLoc.z = (-1) * 168.35;
		res->vCurDir.x = (-1) * 0.95;
		res->vCurDir.y = 0;
		res->vCurDir.z = 0.30;
		plr->SetWorldID(pNewbieTbldat->world_Id);
		plr->SetWorldTblidx(pNewbieTbldat->tutorialWorld);
		plr->SetPlayerIsInTutorial(true);
	}

	packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
	plr = NULL;
	delete plr;

}
//--------------------------------------------------------------------------------------//
//		Character ready request need fix
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharReadyReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- sGU_OBJECT_CREATE --- \n");
	//SPAN PLAYERS
	CNtlPacket packet(sizeof(SpawnPlayer));
	SpawnPlayer * res = (SpawnPlayer *)packet.GetPacketData();
	CNtlPacket packet1(sizeof(sGU_OBJECT_CREATE));
	sGU_OBJECT_CREATE * res1 = (sGU_OBJECT_CREATE *)packet1.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();

	wcscpy_s(plr->GetPcProfile()->awchName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(plr->GetPlayerName()).c_str());

	dbo_data_table_pc *pc = new dbo_data_table_pc();
	pc->load("data/table_pc_data.rdf");
	const dbo_data_table_pc_st *pcDat = pc->pc_data_get(app->db->getInt("Race"), app->db->getInt("Class"), app->db->getInt("Gender"));

	res->wOpCode = GU_OBJECT_CREATE;
	res->Handle = this->GetavatarHandle();
	res->Type = OBJTYPE_PC;
	res->Tblidx = pcDat->id;
	res->Adult = app->db->getBoolean("Adult");
	wcscpy_s(res->Name, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("CharName")).c_str());
	wcscpy_s(res->GuildName, NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE, s2ws(app->db->getString("GuildName")).c_str());
	res->appear.Face = app->db->getInt("Face");
	res->appear.Hair = app->db->getInt("Hair");
	res->appear.HairColor = app->db->getInt("HairColor");
	res->appear.SkinColor = app->db->getInt("SkinColor");
	res->curLP = app->db->getInt("CurLP");
	res->maxLP = app->db->getInt("BaseMaxLP");
	res->curEP = app->db->getInt("CurEP");
	res->maxEP = app->db->getInt("BaseMaxEP");
	res->level = app->db->getInt("Level");
	//res->Speed = (float)app->db->getDouble("LastRunSpeed");
	//res->sObjectInfo.pcBrief.wAttackSpeedRate = app->db->getInt("BaseAttackSpeedRate");
	res->Loc[0] = (float)app->db->getDouble("CurLocX");
	res->Loc[1] = (float)app->db->getDouble("CurLocY");
	res->Loc[2] = (float)app->db->getDouble("CurLocZ");
	res->Dir[0] = (float)app->db->getDouble("CurDirX");
	res->Dir[1] = (float)app->db->getDouble("CurDirY");
	res->Dir[2] = (float)app->db->getDouble("CurDirZ");
	res->Unknown2[0] = 0;
	res->Unknown2[1] = 0;
	res->Unknown2[2] = 0;
	res->Unknown2[3] = 0;
	res->Unknown2[4] = 0;
	res->Unknown2[5] = 0;
	for (int i = 0; i <= 179; i++)
	{
		res->Unknown[i] = 0;
	}
	res->StateID = 0;
	res->AspectID = 255;
	res->mascotID = 6000071;
	res->Size = 10;


	//plr->SetGuildName(app->db->getString("GuildName"));

	for (int i = 0; i < NTL_MAX_EQUIP_ITEM_SLOT; i++)
	{
		app->db->prepare("select * from items WHERE place=7 AND pos=? AND owner_id=?");
		app->db->setInt(1, i);
		app->db->setInt(2, plr->GetCharID());
		app->db->execute();
		app->db->fetch();
		if (app->db->rowsCount() == 0)
		{
			res->sItemBrief[i].tblidx = INVALID_TBLIDX;
		}
		else
		{

			res->sItemBrief[i].tblidx = app->db->getInt("tblidx");
		}

	}

	memcpy(&this->characterspawnInfo, res, sizeof(SpawnPlayer));
	packet.SetPacketLen(sizeof(SpawnPlayer));
	memcpy(&this->characterspawnInfo, res1, sizeof(sGU_OBJECT_CREATE));
	packet1.SetPacketLen(sizeof(sGU_OBJECT_CREATE));

	app->AddUser(plr->GetPlayerName().c_str(), this);
	app->UserBroadcastothers(&packet, this);
	//app->UserBroadcastothers(&packet1, this);
	app->UserBroadcasFromOthers(GU_OBJECT_CREATE, this);
	CClientSession::SendNpcCreate(pPacket, app);
	CClientSession::SendMonsterCreate(pPacket, app);
}


//--------------------------------------------------------------------------------------//
//		Auth community Server
//--------------------------------------------------------------------------------------//
void CClientSession::SendAuthCommunityServer(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- GU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES --- \n");
	CNtlPacket packet(sizeof(sGU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES));
	sGU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES * res = (sGU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES *)packet.GetPacketData();

	res->wOpCode = GU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES;
	res->wResultCode = GAME_SUCCESS;
	strcpy_s((char*)res->abyAuthKey, NTL_MAX_SIZE_AUTH_KEY, "dbo");
	packet.SetPacketLen(sizeof(sGU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES, packet);
}

//--------------------------------------------------------------------------------------//
//		SPAWN NPC
//--------------------------------------------------------------------------------------//
void CClientSession::SendNpcCreate(CNtlPacket * pPacket, CGameServer * app)
{	
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	g_pMobManager->SpawnNpcAtLogin(pPacket, plr->myCCSession);
}
//NPC TLQ3 test
void CClientSession::SendNpcTLQ3Create(CNtlPacket * pPacket, CGameServer * app)
{
	//NPC 1 TLQ 3

	CNtlPacket packet1220(sizeof(SpawnNPC));
	SpawnNPC * res1220 = (SpawnNPC *)packet1220.GetPacketData();

	res1220->wOpCode = GU_OBJECT_CREATE;
	res1220->Type = OBJTYPE_NPC;
	res1220->Handle = 2000;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1220->Tblidx = 2972109;
	res1220->Loc[0] = -45.910000;// curpos.x;
	res1220->Loc[1] = 22.040001; //curpos.y;
	res1220->Loc[2] = -0.580000;// curpos.z;
	res1220->Dir[0] = 0.950666;// curpos.x;
	res1220->Dir[1] = -0.0; //curpos.y;
	res1220->Dir[2] = -0.310217;// curpos.z;
	res1220->Size = 10;
	res1220->Unknown3[0] = 0;
	res1220->Unknown4[0] = 0;
	packet1220.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1220);

	//NPC 2 TLQ 3

	CNtlPacket packet1221(sizeof(SpawnNPC));
	SpawnNPC * res1221 = (SpawnNPC *)packet1221.GetPacketData();

	res1221->wOpCode = GU_OBJECT_CREATE;
	res1221->Type = OBJTYPE_NPC;
	res1221->Handle = 2001;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1221->Tblidx = 3031213;
	res1221->Loc[0] = -43.180000;// curpos.x;
	res1221->Loc[1] = 22.059999; //curpos.y;
	res1221->Loc[2] = -1.510000;// curpos.z;
	res1221->Dir[0] = -0.498284;// curpos.x;
	res1221->Dir[1] = -0.0; //curpos.y;
	res1221->Dir[2] = -0.867014;// curpos.z;
	res1221->Size = 10;
	res1221->Unknown3[0] = 0;
	res1221->Unknown4[0] = 0;
	packet1221.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1221);

	//NPC 3 TLQ 3

	CNtlPacket packet1222(sizeof(SpawnNPC));
	SpawnNPC * res1222 = (SpawnNPC *)packet1222.GetPacketData();

	res1222->wOpCode = GU_OBJECT_CREATE;
	res1222->Type = OBJTYPE_NPC;
	res1222->Handle = 2002;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1222->Tblidx = 3183109;
	res1222->Loc[0] = -11.550000;// curpos.x;
	res1222->Loc[1] = 22.000000; //curpos.y;
	res1222->Loc[2] = -16.370001;// curpos.z;
	res1222->Dir[0] = -0.000000;// curpos.x;
	res1222->Dir[1] = -0.0; //curpos.y;
	res1222->Dir[2] = 1.000000;// curpos.z;
	res1222->Size = 10;
	res1222->Unknown3[0] = 0;
	res1222->Unknown4[0] = 0;
	packet1222.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1222);

	//NPC 4 TLQ 3

	CNtlPacket packet1223(sizeof(SpawnNPC));
	SpawnNPC * res1223 = (SpawnNPC *)packet1223.GetPacketData();

	res1223->wOpCode = GU_OBJECT_CREATE;
	res1223->Type = OBJTYPE_NPC;
	res1223->Handle = 2003;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1223->Tblidx = 3143115;
	res1223->Loc[0] = -7.870000;// curpos.x;
	res1223->Loc[1] = 22.000000; //curpos.y;
	res1223->Loc[2] = -35.400002;// curpos.z;
	res1223->Dir[0] = -0.777941;// curpos.x;
	res1223->Dir[1] = -0.0; //curpos.y;
	res1223->Dir[2] = -0.628337;// curpos.z;
	res1223->Size = 10;
	res1223->Unknown3[0] = 0;
	res1223->Unknown4[0] = 0;
	packet1223.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1223);

	//NPC 5 TLQ 3

	CNtlPacket packet1224(sizeof(SpawnNPC));
	SpawnNPC * res1224 = (SpawnNPC *)packet1224.GetPacketData();

	res1224->wOpCode = GU_OBJECT_CREATE;
	res1224->Type = OBJTYPE_NPC;
	res1224->Handle = 2004;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1224->Tblidx = 3131209;
	res1224->Loc[0] = -8.220000;// curpos.x;
	res1224->Loc[1] = 22.000000; //curpos.y;
	res1224->Loc[2] = -36.150002;// curpos.z;
	res1224->Dir[0] = 0.932568;// curpos.x;
	res1224->Dir[1] = -0.0; //curpos.y;
	res1224->Dir[2] = -0.360994;// curpos.z;
	res1224->Size = 10;
	res1224->Unknown3[0] = 0;
	res1224->Unknown4[0] = 0;
	packet1224.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1224);

	//NPC 6 TLQ 3

	CNtlPacket packet1225(sizeof(SpawnNPC));
	SpawnNPC * res1225 = (SpawnNPC *)packet1225.GetPacketData();

	res1225->wOpCode = GU_OBJECT_CREATE;
	res1225->Type = OBJTYPE_NPC;
	res1225->Handle = 2005;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1225->Tblidx = 3171104;
	res1225->Loc[0] = -8.160000;// curpos.x;
	res1225->Loc[1] = 22.000000; //curpos.y;
	res1225->Loc[2] = -41.270000;// curpos.z;
	res1225->Dir[0] = 0.960000;// curpos.x;
	res1225->Dir[1] = -0.0; //curpos.y;
	res1225->Dir[2] = 0.280000;// curpos.z;
	res1225->Size = 10;
	res1225->Unknown3[0] = 0;
	res1225->Unknown4[0] = 0;
	packet1225.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1225);

	//NPC 7 TLQ 3

	CNtlPacket packet1226(sizeof(SpawnNPC));
	SpawnNPC * res1226 = (SpawnNPC *)packet1226.GetPacketData();

	res1226->wOpCode = GU_OBJECT_CREATE;
	res1226->Type = OBJTYPE_NPC;
	res1226->Handle = 2006;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1226->Tblidx = 3184103;
	res1226->Loc[0] = -8.960000;// curpos.x;
	res1226->Loc[1] = 22.000000; //curpos.y;
	res1226->Loc[2] = -39.189999;// curpos.z;
	res1226->Dir[0] = 0.995974;// curpos.x;
	res1226->Dir[1] = -0.0; //curpos.y;
	res1226->Dir[2] = 0.089638;// curpos.z;
	res1226->Size = 10;
	res1226->Unknown3[0] = 0;
	res1226->Unknown4[0] = 0;
	packet1226.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1226);

	//NPC 8 TLQ 3

	CNtlPacket packet1227(sizeof(SpawnNPC));
	SpawnNPC * res1227 = (SpawnNPC *)packet1227.GetPacketData();

	res1227->wOpCode = GU_OBJECT_CREATE;
	res1227->Type = OBJTYPE_NPC;
	res1227->Handle = 2007;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1227->Tblidx = 3613103;
	res1227->Loc[0] = -5.990000;// curpos.x;
	res1227->Loc[1] = 22.000000; //curpos.y;
	res1227->Loc[2] = -43.160000;// curpos.z;
	res1227->Dir[0] = 0.328135;// curpos.x;
	res1227->Dir[1] = -0.0; //curpos.y;
	res1227->Dir[2] = 0.944631;// curpos.z;
	res1227->Size = 10;
	res1227->Unknown3[0] = 0;
	res1227->Unknown4[0] = 0;
	packet1227.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1227);

	//NPC 9 TLQ 3

	CNtlPacket packet1228(sizeof(SpawnNPC));
	SpawnNPC * res1228 = (SpawnNPC *)packet1228.GetPacketData();

	res1228->wOpCode = GU_OBJECT_CREATE;
	res1228->Type = OBJTYPE_NPC;
	res1228->Handle = 2008;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1228->Tblidx = 1852102;
	res1228->Loc[0] = -2.860000;// curpos.x;
	res1228->Loc[1] = 22.000000; //curpos.y;
	res1228->Loc[2] = -38.580002;// curpos.z;
	res1228->Dir[0] = -0.998752;// curpos.x;
	res1228->Dir[1] = -0.0; //curpos.y;
	res1228->Dir[2] = 0.049938;// curpos.z;
	res1228->Size = 10;
	res1228->Unknown3[0] = 0;
	res1228->Unknown4[0] = 0;
	packet1228.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1228);

	//NPC 10 TLQ 3

	CNtlPacket packet1229(sizeof(SpawnNPC));
	SpawnNPC * res1229 = (SpawnNPC *)packet1229.GetPacketData();

	res1229->wOpCode = GU_OBJECT_CREATE;
	res1229->Type = OBJTYPE_NPC;
	res1229->Handle = 2009;//AcquireSerialId();//app->mob->AcquirseMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1229->Tblidx = 1853102;
	res1229->Loc[0] = -4.660000;// curpos.x;
	res1229->Loc[1] = 22.000000; //curpos.y;
	res1229->Loc[2] = -36.480000;// curpos.z;
	res1229->Dir[0] = -0.929164;// curpos.x;
	res1229->Dir[1] = -0.0; //curpos.y;
	res1229->Dir[2] = 0.369667;// curpos.z;
	res1229->Size = 10;
	res1229->Unknown3[0] = 0;
	res1229->Unknown4[0] = 0;
	packet1229.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1229);

	//NPC 11 TLQ 3

	CNtlPacket packet1230(sizeof(SpawnNPC));
	SpawnNPC * res1230 = (SpawnNPC *)packet1230.GetPacketData();

	res1230->wOpCode = GU_OBJECT_CREATE;
	res1230->Type = OBJTYPE_NPC;
	res1230->Handle = 2010;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1230->Tblidx = 1851102;
	res1230->Loc[0] = -4.980000;// curpos.x;
	res1230->Loc[1] = 22.000000; //curpos.y;
	res1230->Loc[2] = -40.500000;// curpos.z;
	res1230->Dir[0] = -0.995974;// curpos.x;
	res1230->Dir[1] = -0.0; //curpos.y;
	res1230->Dir[2] = -0.089638;// curpos.z;
	res1230->Size = 10;
	res1230->Unknown3[0] = 0;
	res1230->Unknown4[0] = 0;
	packet1230.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1230);

	//NPC 12 TLQ 3

	CNtlPacket packet1231(sizeof(SpawnNPC));
	SpawnNPC * res1231 = (SpawnNPC *)packet1231.GetPacketData();

	res1231->wOpCode = GU_OBJECT_CREATE;
	res1231->Type = OBJTYPE_NPC;
	res1231->Handle = 2011;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1231->Tblidx = 1471103;
	res1231->Loc[0] = -20.040001;// curpos.x;
	res1231->Loc[1] = 22.000000; //curpos.y;
	res1231->Loc[2] = -113.430000;// curpos.z;
	res1231->Dir[0] = 0.559301;// curpos.x;
	res1231->Dir[1] = -0.0; //curpos.y;
	res1231->Dir[2] = 0.828964;// curpos.z;
	res1231->Size = 10;
	res1231->Unknown3[0] = 0;
	res1231->Unknown4[0] = 0;
	packet1231.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1231);

	//NPC 13 TLQ 3

	CNtlPacket packet1232(sizeof(SpawnNPC));
	SpawnNPC * res1232 = (SpawnNPC *)packet1232.GetPacketData();

	res1232->wOpCode = GU_OBJECT_CREATE;
	res1232->Type = OBJTYPE_NPC;
	res1232->Handle = 2012;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1232->Tblidx = 3251101;
	res1232->Loc[0] = -122.169998;// curpos.x;
	res1232->Loc[1] = 22.120001; //curpos.y;
	res1232->Loc[2] = -12.760000;// curpos.z;
	res1232->Dir[0] = -0.855732;// curpos.x;
	res1232->Dir[1] = -0.0; //curpos.y;
	res1232->Dir[2] = -0.517419;// curpos.z;
	res1232->Size = 10;
	res1232->Unknown3[0] = 0;
	res1232->Unknown4[0] = 0;
	packet1232.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1232);

	//NPC 14 TLQ 3

	CNtlPacket packet1233(sizeof(SpawnNPC));
	SpawnNPC * res1233 = (SpawnNPC *)packet1233.GetPacketData();

	res1233->wOpCode = GU_OBJECT_CREATE;
	res1233->Type = OBJTYPE_NPC;
	res1233->Handle = 2013;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1233->Tblidx = 3252101;
	res1233->Loc[0] = -121.610001;// curpos.x;
	res1233->Loc[1] = 22.090000; //curpos.y;
	res1233->Loc[2] = -13.890000;// curpos.z;
	res1233->Dir[0] = -0.977802;// curpos.x;
	res1233->Dir[1] = -0.0; //curpos.y;
	res1233->Dir[2] = -0.209529;// curpos.z;
	res1233->Size = 10;
	res1233->Unknown3[0] = 0;
	res1233->Unknown4[0] = 0;
	packet1233.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1233);

	//NPC 15 TLQ 3

	CNtlPacket packet1234(sizeof(SpawnNPC));
	SpawnNPC * res1234 = (SpawnNPC *)packet1234.GetPacketData();

	res1234->wOpCode = GU_OBJECT_CREATE;
	res1234->Type = OBJTYPE_NPC;
	res1234->Handle = 2014;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1234->Tblidx = 3173109;
	res1234->Loc[0] = 2.430000;// curpos.x;
	res1234->Loc[1] = 22.020000; //curpos.y;
	res1234->Loc[2] = -1.060000;// curpos.z;
	res1234->Dir[0] = -0.120331;// curpos.x;
	res1234->Dir[1] = -0.0; //curpos.y;
	res1234->Dir[2] = 0.992734;// curpos.z;
	res1234->Size = 10;
	res1234->Unknown3[0] = 0;
	res1234->Unknown4[0] = 0;
	packet1234.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1234);

	//NPC 16 TLQ 3

	CNtlPacket packet1235(sizeof(SpawnNPC));
	SpawnNPC * res1235 = (SpawnNPC *)packet1235.GetPacketData();

	res1235->wOpCode = GU_OBJECT_CREATE;
	res1235->Type = OBJTYPE_NPC;
	res1235->Handle = 2015;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1235->Tblidx = 1471104;
	res1235->Loc[0] = 91.419998;// curpos.x;
	res1235->Loc[1] = 22.059999; //curpos.y;
	res1235->Loc[2] = -7.900000;// curpos.z;
	res1235->Dir[0] = -0.221187;// curpos.x;
	res1235->Dir[1] = -0.0; //curpos.y;
	res1235->Dir[2] = 0.975232;// curpos.z;
	res1235->Size = 10;
	res1235->Unknown3[0] = 0;
	res1235->Unknown4[0] = 0;
	packet1235.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1235);

	//NPC 17 TLQ 3

	CNtlPacket packet1236(sizeof(SpawnNPC));
	SpawnNPC * res1236 = (SpawnNPC *)packet1236.GetPacketData();

	res1236->wOpCode = GU_OBJECT_CREATE;
	res1236->Type = OBJTYPE_NPC;
	res1236->Handle = 2016;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1236->Tblidx = 1471105;
	res1236->Loc[0] = 96.550003;// curpos.x;
	res1236->Loc[1] = 22.040001; //curpos.y;
	res1236->Loc[2] = -84.510002;// curpos.z;
	res1236->Dir[0] = 0.540758;// curpos.x;
	res1236->Dir[1] = -0.0; //curpos.y;
	res1236->Dir[2] = 0.841178;// curpos.z;
	res1236->Size = 10;
	res1236->Unknown3[0] = 0;
	res1236->Unknown4[0] = 0;
	packet1236.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1236);

	//NPC 18 TLQ 3

	CNtlPacket packet1237(sizeof(SpawnNPC));
	SpawnNPC * res1237 = (SpawnNPC *)packet1237.GetPacketData();

	res1237->wOpCode = GU_OBJECT_CREATE;
	res1237->Type = OBJTYPE_NPC;
	res1237->Handle = 2017;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1237->Tblidx = 1181102;
	res1237->Loc[0] = 92.519997;// curpos.x;
	res1237->Loc[1] = 22.200001; //curpos.y;
	res1237->Loc[2] = -92.699997;// curpos.z;
	res1237->Dir[0] = 0.998752;// curpos.x;
	res1237->Dir[1] = -0.0; //curpos.y;
	res1237->Dir[2] = 0.049938;// curpos.z;
	res1237->Size = 10;
	res1237->Unknown3[0] = 0;
	res1237->Unknown4[0] = 0;
	packet1237.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1237);

	//NPC 19 TLQ 3

	CNtlPacket packet1238(sizeof(SpawnNPC));
	SpawnNPC * res1238 = (SpawnNPC *)packet1238.GetPacketData();

	res1238->wOpCode = GU_OBJECT_CREATE;
	res1238->Type = OBJTYPE_NPC;
	res1238->Handle = 2018;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1238->Tblidx = 3189103;
	res1238->Loc[0] = 96.529999;// curpos.x;
	res1238->Loc[1] = 22.200001; //curpos.y;
	res1238->Loc[2] = -90.330002;// curpos.z;
	res1238->Dir[0] = -0.867014;// curpos.x;
	res1238->Dir[1] = -0.0; //curpos.y;
	res1238->Dir[2] = -0.498284;// curpos.z;
	res1238->Size = 10;
	res1238->Unknown3[0] = 0;
	res1238->Unknown4[0] = 0;
	packet1238.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1238);

	//NPC 20 TLQ 3

	CNtlPacket packet1239(sizeof(SpawnNPC));
	SpawnNPC * res1239 = (SpawnNPC *)packet1239.GetPacketData();

	res1239->wOpCode = GU_OBJECT_CREATE;
	res1239->Type = OBJTYPE_NPC;
	res1239->Handle = 2019;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1239->Tblidx = 3171105;
	res1239->Loc[0] = 97.000000;// curpos.x;
	res1239->Loc[1] = 22.200001; //curpos.y;
	res1239->Loc[2] = -92.360001;// curpos.z;
	res1239->Dir[0] = -0.975232;// curpos.x;
	res1239->Dir[1] = -0.0; //curpos.y;
	res1239->Dir[2] = 0.221187;// curpos.z;
	res1239->Size = 10;
	res1239->Unknown3[0] = 0;
	res1239->Unknown4[0] = 0;
	packet1239.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1239);

	//NPC 21 TLQ 3

	CNtlPacket packet1240(sizeof(SpawnNPC));
	SpawnNPC * res1240 = (SpawnNPC *)packet1240.GetPacketData();

	res1240->wOpCode = GU_OBJECT_CREATE;
	res1240->Type = OBJTYPE_NPC;
	res1240->Handle = 2020;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1240->Tblidx = 3242110;
	res1240->Loc[0] = 75.320000;// curpos.x;
	res1240->Loc[1] = 22.200001; //curpos.y;
	res1240->Loc[2] = -92.430000;// curpos.z;
	res1240->Dir[0] = -0.981720;// curpos.x;
	res1240->Dir[1] = -0.0; //curpos.y;
	res1240->Dir[2] = -0.190333;// curpos.z;
	res1240->Size = 10;
	res1240->Unknown3[0] = 0;
	res1240->Unknown4[0] = 0;
	packet1240.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1240);

	//NPC 22 TLQ 3

	CNtlPacket packet1241(sizeof(SpawnNPC));
	SpawnNPC * res1241 = (SpawnNPC *)packet1241.GetPacketData();

	res1241->wOpCode = GU_OBJECT_CREATE;
	res1241->Type = OBJTYPE_NPC;
	res1241->Handle = 2021;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1241->Tblidx = 1631102;
	res1241->Loc[0] = 75.209999;// curpos.x;
	res1241->Loc[1] = 22.200001; //curpos.y;
	res1241->Loc[2] = -95.190002;// curpos.z;
	res1241->Dir[0] = -0.920691;// curpos.x;
	res1241->Dir[1] = -0.0; //curpos.y;
	res1241->Dir[2] = 0.390293;// curpos.z;
	res1241->Size = 10;
	res1241->Unknown3[0] = 0;
	res1241->Unknown4[0] = 0;
	packet1241.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1241);

	//NPC 23 TLQ 3

	CNtlPacket packet1242(sizeof(SpawnNPC));
	SpawnNPC * res1242 = (SpawnNPC *)packet1242.GetPacketData();

	res1242->wOpCode = GU_OBJECT_CREATE;
	res1242->Type = OBJTYPE_NPC;
	res1242->Handle = 2022;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1242->Tblidx = 7071110;
	res1242->Loc[0] = 73.739998;// curpos.x;
	res1242->Loc[1] = 22.200001; //curpos.y;
	res1242->Loc[2] = -106.550003;// curpos.z;
	res1242->Dir[0] = -0.559301;// curpos.x;
	res1242->Dir[1] = -0.0; //curpos.y;
	res1242->Dir[2] = -0.828964;// curpos.z;
	res1242->Size = 10;
	res1242->Unknown3[0] = 0;
	res1242->Unknown4[0] = 0;
	packet1242.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1242);

	//NPC 24 TLQ 3

	CNtlPacket packet1243(sizeof(SpawnNPC));
	SpawnNPC * res1243 = (SpawnNPC *)packet1243.GetPacketData();

	res1243->wOpCode = GU_OBJECT_CREATE;
	res1243->Type = OBJTYPE_NPC;
	res1243->Handle = 2023;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1243->Tblidx = 7072104;
	res1243->Loc[0] = 73.800003;// curpos.x;
	res1243->Loc[1] = 22.200001; //curpos.y;
	res1243->Loc[2] = -109.730003;// curpos.z;
	res1243->Dir[0] = -0.340136;// curpos.x;
	res1243->Dir[1] = -0.0; //curpos.y;
	res1243->Dir[2] = 0.940376;// curpos.z;
	res1243->Size = 10;
	res1243->Unknown3[0] = 0;
	res1243->Unknown4[0] = 0;
	packet1243.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1243);

	//NPC 25 TLQ 3

	CNtlPacket packet1244(sizeof(SpawnNPC));
	SpawnNPC * res1244 = (SpawnNPC *)packet1244.GetPacketData();

	res1244->wOpCode = GU_OBJECT_CREATE;
	res1244->Type = OBJTYPE_NPC;
	res1244->Handle = 2024;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1244->Tblidx = 7076102;
	res1244->Loc[0] = 71.290001;// curpos.x;
	res1244->Loc[1] = 22.200001; //curpos.y;
	res1244->Loc[2] = -108.400002;// curpos.z;
	res1244->Dir[0] = 0.977802;// curpos.x;
	res1244->Dir[1] = -0.0; //curpos.y;
	res1244->Dir[2] = 0.209529;// curpos.z;
	res1244->Size = 10;
	res1244->Unknown3[0] = 0;
	res1244->Unknown4[0] = 0;
	packet1244.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1244);

	//NPC 26 TLQ 3

	CNtlPacket packet1245(sizeof(SpawnNPC));
	SpawnNPC * res1245 = (SpawnNPC *)packet1245.GetPacketData();

	res1245->wOpCode = GU_OBJECT_CREATE;
	res1245->Type = OBJTYPE_NPC;
	res1245->Handle = 2025;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1245->Tblidx = 7091110;
	res1245->Loc[0] = 92.550003;// curpos.x;
	res1245->Loc[1] = 22.200001; //curpos.y;
	res1245->Loc[2] = -112.449997;// curpos.z;
	res1245->Dir[0] = -0.777941;// curpos.x;
	res1245->Dir[1] = -0.0; //curpos.y;
	res1245->Dir[2] = 0.628337;// curpos.z;
	res1245->Size = 10;
	res1245->Unknown3[0] = 0;
	res1245->Unknown4[0] = 0;
	packet1245.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1245);

	//NPC 27 TLQ 3

	CNtlPacket packet1246(sizeof(SpawnNPC));
	SpawnNPC * res1246 = (SpawnNPC *)packet1246.GetPacketData();

	res1246->wOpCode = GU_OBJECT_CREATE;
	res1246->Type = OBJTYPE_NPC;
	res1246->Handle = 2026;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1246->Tblidx = 1751104;
	res1246->Loc[0] = 90.389999;// curpos.x;
	res1246->Loc[1] = 22.200001; //curpos.y;
	res1246->Loc[2] = -111.500000;// curpos.z;
	res1246->Dir[0] = 0.628337;// curpos.x;
	res1246->Dir[1] = -0.0; //curpos.y;
	res1246->Dir[2] = -0.777941;// curpos.z;
	res1246->Size = 10;
	res1246->Unknown3[0] = 0;
	res1246->Unknown4[0] = 0;
	packet1246.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1246);

	//NPC 28 TLQ 3

	CNtlPacket packet1247(sizeof(SpawnNPC));
	SpawnNPC * res1247 = (SpawnNPC *)packet1247.GetPacketData();

	res1247->wOpCode = GU_OBJECT_CREATE;
	res1247->Type = OBJTYPE_NPC;
	res1247->Handle = 2027;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1247->Tblidx = 3183110;
	res1247->Loc[0] = 107.230003;// curpos.x;
	res1247->Loc[1] = 22.200001; //curpos.y;
	res1247->Loc[2] = -114.720001;// curpos.z;
	res1247->Dir[0] = 0.340136;// curpos.x;
	res1247->Dir[1] = -0.0; //curpos.y;
	res1247->Dir[2] = 0.940376;// curpos.z;
	res1247->Size = 10;
	res1247->Unknown3[0] = 0;
	res1247->Unknown4[0] = 0;
	packet1247.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1247);

	//NPC 29 TLQ 3

	CNtlPacket packet1248(sizeof(SpawnNPC));
	SpawnNPC * res1248 = (SpawnNPC *)packet1248.GetPacketData();

	res1248->wOpCode = GU_OBJECT_CREATE;
	res1248->Type = OBJTYPE_NPC;
	res1248->Handle = 2028;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1248->Tblidx = 3221207;
	res1248->Loc[0] = 106.349998;// curpos.x;
	res1248->Loc[1] = 22.200001; //curpos.y;
	res1248->Loc[2] = -113.400002;// curpos.z;
	res1248->Dir[0] = 0.977802;// curpos.x;
	res1248->Dir[1] = -0.0; //curpos.y;
	res1248->Dir[2] = 0.209529;// curpos.z;
	res1248->Size = 10;
	res1248->Unknown3[0] = 0;
	res1248->Unknown4[0] = 0;
	packet1248.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1248);

	//NPC 30 TLQ 3

	CNtlPacket packet1249(sizeof(SpawnNPC));
	SpawnNPC * res1249 = (SpawnNPC *)packet1249.GetPacketData();

	res1249->wOpCode = GU_OBJECT_CREATE;
	res1249->Type = OBJTYPE_NPC;
	res1249->Handle = 2029;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1249->Tblidx = 1411101;
	res1249->Loc[0] = 110.260002;// curpos.x;
	res1249->Loc[1] = 22.200001; //curpos.y;
	res1249->Loc[2] = -112.239998;// curpos.z;
	res1249->Dir[0] = -0.681604;// curpos.x;
	res1249->Dir[1] = -0.0; //curpos.y;
	res1249->Dir[2] = 0.731722;// curpos.z;
	res1249->Size = 10;
	res1249->Unknown3[0] = 0;
	res1249->Unknown4[0] = 0;
	packet1249.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1249);

	//NPC 31 TLQ 3

	CNtlPacket packet1250(sizeof(SpawnNPC));
	SpawnNPC * res1250 = (SpawnNPC *)packet1250.GetPacketData();

	res1250->wOpCode = GU_OBJECT_CREATE;
	res1250->Type = OBJTYPE_NPC;
	res1250->Handle = 2030;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1250->Tblidx = 6811101;
	res1250->Loc[0] = 122.260002;// curpos.x;
	res1250->Loc[1] = 23.400000; //curpos.y;
	res1250->Loc[2] = -102.650002;// curpos.z;
	res1250->Dir[0] = 0.995974;// curpos.x;
	res1250->Dir[1] = -0.0; //curpos.y;
	res1250->Dir[2] = 0.089638;// curpos.z;
	res1250->Size = 10;
	res1250->Unknown3[0] = 0;
	res1250->Unknown4[0] = 0;
	packet1250.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1250);

	//NPC 32 TLQ 3

	CNtlPacket packet1251(sizeof(SpawnNPC));
	SpawnNPC * res1251 = (SpawnNPC *)packet1251.GetPacketData();

	res1251->wOpCode = GU_OBJECT_CREATE;
	res1251->Type = OBJTYPE_NPC;
	res1251->Handle = 2031;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1251->Tblidx = 3351101;
	res1251->Loc[0] = 127.870003;// curpos.x;
	res1251->Loc[1] = 22.200001; //curpos.y;
	res1251->Loc[2] = -95.180000;// curpos.z;
	res1251->Dir[0] = -0.769039;// curpos.x;
	res1251->Dir[1] = -0.0; //curpos.y;
	res1251->Dir[2] = 0.639202;// curpos.z;
	res1251->Size = 10;
	res1251->Unknown3[0] = 0;
	res1251->Unknown4[0] = 0;
	packet1251.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1251);

	//NPC 33 TLQ 3

	CNtlPacket packet1252(sizeof(SpawnNPC));
	SpawnNPC * res1252 = (SpawnNPC *)packet1252.GetPacketData();

	res1252->wOpCode = GU_OBJECT_CREATE;
	res1252->Type = OBJTYPE_NPC;
	res1252->Handle = 2032;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1252->Tblidx = 1311102;
	res1252->Loc[0] = -44.580002;// curpos.x;
	res1252->Loc[1] = 22.030001; //curpos.y;
	res1252->Loc[2] = 0.120000;// curpos.z;
	res1252->Dir[0] = 0.410781;// curpos.x;
	res1252->Dir[1] = -0.0; //curpos.y;
	res1252->Dir[2] = -0.911734;// curpos.z;
	res1252->Size = 10;
	res1252->Unknown3[0] = 0;
	res1252->Unknown4[0] = 0;
	packet1252.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1252);

	//NPC 34 TLQ 3

	CNtlPacket packet1253(sizeof(SpawnNPC));
	SpawnNPC * res1253 = (SpawnNPC *)packet1253.GetPacketData();

	res1253->wOpCode = GU_OBJECT_CREATE;
	res1253->Type = OBJTYPE_NPC;
	res1253->Handle = 2033;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1253->Tblidx = 1471102;
	res1253->Loc[0] = -25.389999;// curpos.x;
	res1253->Loc[1] = 22.000000; //curpos.y;
	res1253->Loc[2] = 18.670000;// curpos.z;
	res1253->Dir[0] = 0.069829;// curpos.x;
	res1253->Dir[1] = -0.0; //curpos.y;
	res1253->Dir[2] = -0.997559;// curpos.z;
	res1253->Size = 10;
	res1253->Unknown3[0] = 0;
	res1253->Unknown4[0] = 0;
	packet1253.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1253);

	//NPC 35 TLQ 3

	CNtlPacket packet1254(sizeof(SpawnNPC));
	SpawnNPC * res1254 = (SpawnNPC *)packet1254.GetPacketData();

	res1254->wOpCode = GU_OBJECT_CREATE;
	res1254->Type = OBJTYPE_NPC;
	res1254->Handle = 2034;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1254->Tblidx = 1901102;
	res1254->Loc[0] = -17.350000;// curpos.x;
	res1254->Loc[1] = 22.000000; //curpos.y;
	res1254->Loc[2] = 16.730000;// curpos.z;
	res1254->Dir[0] = -0.069829;// curpos.x;
	res1254->Dir[1] = -0.0; //curpos.y;
	res1254->Dir[2] = -0.997559;// curpos.z;
	res1254->Size = 10;
	res1254->Unknown3[0] = 0;
	res1254->Unknown4[0] = 0;
	packet1254.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1254);

	//NPC 36 TLQ 3

	CNtlPacket packet1255(sizeof(SpawnNPC));
	SpawnNPC * res1255 = (SpawnNPC *)packet1255.GetPacketData();

	res1255->wOpCode = GU_OBJECT_CREATE;
	res1255->Type = OBJTYPE_NPC;
	res1255->Handle = 2035;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1255->Tblidx = 7091109;
	res1255->Loc[0] = -23.850000;// curpos.x;
	res1255->Loc[1] = 22.000000; //curpos.y;
	res1255->Loc[2] = 3.430000;// curpos.z;
	res1255->Dir[0] = 0.000000;// curpos.x;
	res1255->Dir[1] = -0.0; //curpos.y;
	res1255->Dir[2] = 1.000000;// curpos.z;
	res1255->Size = 10;
	res1255->Unknown3[0] = 0;
	res1255->Unknown4[0] = 0;
	packet1255.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1255);

	//NPC 37 TLQ 3

	CNtlPacket packet1256(sizeof(SpawnNPC));
	SpawnNPC * res1256 = (SpawnNPC *)packet1256.GetPacketData();

	res1256->wOpCode = GU_OBJECT_CREATE;
	res1256->Type = OBJTYPE_NPC;
	res1256->Handle = 2036;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1256->Tblidx = 3181105;
	res1256->Loc[0] = 1.320000;// curpos.x;
	res1256->Loc[1] = 22.000000; //curpos.y;
	res1256->Loc[2] = 0.710000;// curpos.z;
	res1256->Dir[0] = 0.617382;// curpos.x;
	res1256->Dir[1] = -0.0; //curpos.y;
	res1256->Dir[2] = -0.786664;// curpos.z;
	res1256->Size = 10;
	res1256->Unknown3[0] = 0;
	res1256->Unknown4[0] = 0;
	packet1256.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1256);

	//NPC 38 TLQ 3

	CNtlPacket packet1257(sizeof(SpawnNPC));
	SpawnNPC * res1257 = (SpawnNPC *)packet1257.GetPacketData();

	res1257->wOpCode = GU_OBJECT_CREATE;
	res1257->Type = OBJTYPE_NPC;
	res1257->Handle = 2037;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1257->Tblidx = 3221206;
	res1257->Loc[0] = 34.009998;// curpos.x;
	res1257->Loc[1] = 22.059999; //curpos.y;
	res1257->Loc[2] = 4.070000;// curpos.z;
	res1257->Dir[0] = 0.000000;// curpos.x;
	res1257->Dir[1] = -0.0; //curpos.y;
	res1257->Dir[2] = 1.000000;// curpos.z;
	res1257->Size = 10;
	res1257->Unknown3[0] = 0;
	res1257->Unknown4[0] = 0;
	packet1257.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1257);

	//NPC 39 TLQ 3

	CNtlPacket packet1258(sizeof(SpawnNPC));
	SpawnNPC * res1258 = (SpawnNPC *)packet1258.GetPacketData();

	res1258->wOpCode = GU_OBJECT_CREATE;
	res1258->Type = OBJTYPE_NPC;
	res1258->Handle = 2038;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1258->Tblidx = 3142213;
	res1258->Loc[0] = 52.180000;// curpos.x;
	res1258->Loc[1] = 22.020000; //curpos.y;
	res1258->Loc[2] = 19.129999;// curpos.z;
	res1258->Dir[0] = 0.992734;// curpos.x;
	res1258->Dir[1] = -0.0; //curpos.y;
	res1258->Dir[2] = -0.120331;// curpos.z;
	res1258->Size = 10;
	res1258->Unknown3[0] = 0;
	res1258->Unknown4[0] = 0;
	packet1258.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1258);

	//NPC 40 TLQ 3

	CNtlPacket packet1259(sizeof(SpawnNPC));
	SpawnNPC * res1259 = (SpawnNPC *)packet1259.GetPacketData();

	res1259->wOpCode = GU_OBJECT_CREATE;
	res1259->Type = OBJTYPE_NPC;
	res1259->Handle = 2039;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1259->Tblidx = 3242109;
	res1259->Loc[0] = 53.549999;// curpos.x;
	res1259->Loc[1] = 22.110001; //curpos.y;
	res1259->Loc[2] = 14.450000;// curpos.z;
	res1259->Dir[0] = 0.390293;// curpos.x;
	res1259->Dir[1] = -0.0; //curpos.y;
	res1259->Dir[2] = 0.920691;// curpos.z;
	res1259->Size = 10;
	res1259->Unknown3[0] = 0;
	res1259->Unknown4[0] = 0;
	packet1259.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1259);

	//NPC 41 TLQ 3

	CNtlPacket packet1260(sizeof(SpawnNPC));
	SpawnNPC * res1260 = (SpawnNPC *)packet1260.GetPacketData();

	res1260->wOpCode = GU_OBJECT_CREATE;
	res1260->Type = OBJTYPE_NPC;
	res1260->Handle = 2040;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1260->Tblidx = 3189102;
	res1260->Loc[0] = 58.720001;// curpos.x;
	res1260->Loc[1] = 22.110001; //curpos.y;
	res1260->Loc[2] = 13.940000;// curpos.z;
	res1260->Dir[0] = -0.310217;// curpos.x;
	res1260->Dir[1] = -0.0; //curpos.y;
	res1260->Dir[2] = 0.950666;// curpos.z;
	res1260->Size = 10;
	res1260->Unknown3[0] = 0;
	res1260->Unknown4[0] = 0;
	packet1260.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1260);

	//NPC 42 TLQ 3

	CNtlPacket packet1261(sizeof(SpawnNPC));
	SpawnNPC * res1261 = (SpawnNPC *)packet1261.GetPacketData();

	res1261->wOpCode = GU_OBJECT_CREATE;
	res1261->Type = OBJTYPE_NPC;
	res1261->Handle = 2041;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1261->Tblidx = 4192102;
	res1261->Loc[0] = 62.660000;// curpos.x;
	res1261->Loc[1] = 22.110001; //curpos.y;
	res1261->Loc[2] = 16.799999;// curpos.z;
	res1261->Dir[0] = -0.992734;// curpos.x;
	res1261->Dir[1] = -0.0; //curpos.y;
	res1261->Dir[2] = 0.120331;// curpos.z;
	res1261->Size = 10;
	res1261->Unknown3[0] = 0;
	res1261->Unknown4[0] = 0;
	packet1261.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1261);

	//NPC 43 TLQ 3

	CNtlPacket packet1262(sizeof(SpawnNPC));
	SpawnNPC * res1262 = (SpawnNPC *)packet1262.GetPacketData();

	res1262->wOpCode = GU_OBJECT_CREATE;
	res1262->Type = OBJTYPE_NPC;
	res1262->Handle = 2042;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1262->Tblidx = 3173202;
	res1262->Loc[0] = 53.410000;// curpos.x;
	res1262->Loc[1] = 22.110001; //curpos.y;
	res1262->Loc[2] = 15.200000;// curpos.z;
	res1262->Dir[0] = 0.660628;// curpos.x;
	res1262->Dir[1] = -0.0; //curpos.y;
	res1262->Dir[2] = 0.750713;// curpos.z;
	res1262->Size = 10;
	res1262->Unknown3[0] = 0;
	res1262->Unknown4[0] = 0;
	packet1262.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1262);

	//NPC 44 TLQ 3

	CNtlPacket packet1263(sizeof(SpawnNPC));
	SpawnNPC * res1263 = (SpawnNPC *)packet1263.GetPacketData();

	res1263->wOpCode = GU_OBJECT_CREATE;
	res1263->Type = OBJTYPE_NPC;
	res1263->Handle = 2043;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1263->Tblidx = 1181101;
	res1263->Loc[0] = 56.939999;// curpos.x;
	res1263->Loc[1] = 22.040001; //curpos.y;
	res1263->Loc[2] = 18.809999;// curpos.z;
	res1263->Dir[0] = -0.209529;// curpos.x;
	res1263->Dir[1] = -0.0; //curpos.y;
	res1263->Dir[2] = -0.977802;// curpos.z;
	res1263->Size = 10;
	res1263->Unknown3[0] = 0;
	res1263->Unknown4[0] = 0;
	packet1263.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1263);

	//NPC 45 TLQ 3

	CNtlPacket packet1264(sizeof(SpawnNPC));
	SpawnNPC * res1264 = (SpawnNPC *)packet1264.GetPacketData();

	res1264->wOpCode = GU_OBJECT_CREATE;
	res1264->Type = OBJTYPE_NPC;
	res1264->Handle = 2044;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1264->Tblidx = 7071109;
	res1264->Loc[0] = 3.170000;// curpos.x;
	res1264->Loc[1] = 22.000000; //curpos.y;
	res1264->Loc[2] = 18.780001;// curpos.z;
	res1264->Dir[0] = 0.221187;// curpos.x;
	res1264->Dir[1] = -0.0; //curpos.y;
	res1264->Dir[2] = -0.975232;// curpos.z;
	res1264->Size = 10;
	res1264->Unknown3[0] = 0;
	res1264->Unknown4[0] = 0;
	packet1264.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1264);

	//NPC 46 TLQ 3

	CNtlPacket packet1265(sizeof(SpawnNPC));
	SpawnNPC * res1265 = (SpawnNPC *)packet1265.GetPacketData();

	res1265->wOpCode = GU_OBJECT_CREATE;
	res1265->Type = OBJTYPE_NPC;
	res1265->Handle = 2045;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1265->Tblidx = 1751103;
	res1265->Loc[0] = 50.590000;// curpos.x;
	res1265->Loc[1] = 22.070000; //curpos.y;
	res1265->Loc[2] = 16.580000;// curpos.z;
	res1265->Dir[0] = 0.911734;// curpos.x;
	res1265->Dir[1] = -0.0; //curpos.y;
	res1265->Dir[2] = 0.410781;// curpos.z;
	res1265->Size = 10;
	res1265->Unknown3[0] = 0;
	res1265->Unknown4[0] = 0;
	packet1265.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1265);

	//NPC 47 TLQ 3

	CNtlPacket packet1266(sizeof(SpawnNPC));
	SpawnNPC * res1266 = (SpawnNPC *)packet1266.GetPacketData();

	res1266->wOpCode = GU_OBJECT_CREATE;
	res1266->Type = OBJTYPE_NPC;
	res1266->Handle = 2046;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1266->Tblidx = 2951203;
	res1266->Loc[0] = 2.870000;// curpos.x;
	res1266->Loc[1] = 22.000000; //curpos.y;
	res1266->Loc[2] = 16.860001;// curpos.z;
	res1266->Dir[0] = 0.000000;// curpos.x;
	res1266->Dir[1] = -0.0; //curpos.y;
	res1266->Dir[2] = 1.000000;// curpos.z;
	res1266->Size = 10;
	res1266->Unknown3[0] = 0;
	res1266->Unknown4[0] = 0;
	packet1266.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(this->GetHandle(), &packet1266);
}
//--------------------------------------------------------------------------------------//
//		SPAWN MOBS
//--------------------------------------------------------------------------------------//
void CClientSession::SendMonsterCreate(CNtlPacket * pPacket, CGameServer * app)
{
	g_pMobManager->SpawnMonsterAtLogin(pPacket, this);
}
//Mob TLQ3 Test
void CClientSession::SendMonsterTLQ3Create(CNtlPacket * pPacket, CGameServer * app)
{
	//tlq mob test
	CNtlPacket packet18(sizeof(SpawnMOB));
	SpawnMOB * res18 = (SpawnMOB *)packet18.GetPacketData();

	res18->wOpCode = GU_OBJECT_CREATE;
	res18->Type = OBJTYPE_MOB;
	res18->Handle = 2700;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res18->Tblidx = 4131101;
	res18->Loc[0] = -122.110001;// curpos.x;
	res18->Loc[1] = 22.120001; //curpos.y;
	res18->Loc[2] = -12.710000;// curpos.z;
	res18->Dir[0] = -0.855732;
	res18->Dir[1] = -0.0;
	res18->Dir[2] = -0.517419;
	res18->Size = 10;
	res18->curEP = 500;
	res18->maxEP = 500;
	res18->curLP = 600;
	res18->maxLP = 600;
	res18->Level = 2;
	res18->StateID = 35;

	packet18.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(this->GetHandle(), &packet18);
	//2
	CNtlPacket packet19(sizeof(SpawnMOB));
	SpawnMOB * res19 = (SpawnMOB *)packet19.GetPacketData();

	res19->wOpCode = GU_OBJECT_CREATE;
	res19->Type = OBJTYPE_MOB;
	res19->Handle = 2701;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res19->Tblidx = 4132101;
	res19->Loc[0] = -121.680000;// curpos.x;
	res19->Loc[1] = 22.090000; //curpos.y;
	res19->Loc[2] = -13.840000;// curpos.z;
	res19->Dir[0] = -0.977802;
	res19->Dir[1] = -0.0;
	res19->Dir[2] = 0.209529;
	res19->Size = 10;
	res19->curEP = 500;
	res19->maxEP = 500;
	res19->curLP = 600;
	res19->maxLP = 600;
	res19->Level = 2;
	res19->StateID = 35;

	packet19.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(this->GetHandle(), &packet19);

	//3
	CNtlPacket packet20(sizeof(SpawnMOB));
	SpawnMOB * res20 = (SpawnMOB *)packet20.GetPacketData();

	res20->wOpCode = GU_OBJECT_CREATE;
	res20->Type = OBJTYPE_MOB;
	res20->Handle = 2702;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res20->Tblidx = 4132101;
	res20->Loc[0] = -76.739998;// curpos.x;
	res20->Loc[1] = 22.000000; //curpos.y;
	res20->Loc[2] = 3.030000;// curpos.z;
	res20->Dir[0] = -0.529101;
	res20->Dir[1] = -0.0;
	res20->Dir[2] = 0.848559;
	res20->Size = 10;
	res20->curEP = 500;
	res20->maxEP = 500;
	res20->curLP = 600;
	res20->maxLP = 600;
	res20->Level = 2;
	res20->StateID = 35;

	packet20.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(this->GetHandle(), &packet20);

	//4
	CNtlPacket packet21(sizeof(SpawnMOB));
	SpawnMOB * res21 = (SpawnMOB *)packet21.GetPacketData();

	res21->wOpCode = GU_OBJECT_CREATE;
	res21->Type = OBJTYPE_MOB;
	res21->Handle = 2703;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res21->Tblidx = 4131101;
	res21->Loc[0] = -79.940002;// curpos.x;
	res21->Loc[1] = 22.020000; //curpos.y;
	res21->Loc[2] = 1.030000;// curpos.z;
	res21->Dir[0] = -0.258902;
	res21->Dir[1] = -0.0;
	res21->Dir[2] = 0.965904;
	res21->Size = 10;
	res21->curEP = 500;
	res21->maxEP = 500;
	res21->curLP = 600;
	res21->maxLP = 600;
	res21->Level = 2;
	res21->StateID = 35;

	packet21.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(this->GetHandle(), &packet21);

	//5
	CNtlPacket packet22(sizeof(SpawnMOB));
	SpawnMOB * res22 = (SpawnMOB *)packet22.GetPacketData();

	res22->wOpCode = GU_OBJECT_CREATE;
	res22->Type = OBJTYPE_MOB;
	res22->Handle = 2704;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res22->Tblidx = 1591101;
	res22->Loc[0] = -79.449997;// curpos.x;
	res22->Loc[1] = 22.000000; //curpos.y;
	res22->Loc[2] = 3.700000;// curpos.z;
	res22->Dir[0] = -0.617382;
	res22->Dir[1] = -0.0;
	res22->Dir[2] = 0.786664;
	res22->Size = 10;
	res22->curEP = 500;
	res22->maxEP = 500;
	res22->curLP = 600;
	res22->maxLP = 600;
	res22->Level = 2;
	res22->StateID = 35;

	packet22.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(this->GetHandle(), &packet22);
}
//--------------------------------------------------------------------------------------//
//		SendEnterWorldComplete
//--------------------------------------------------------------------------------------//
void CClientSession::SendEnterWorldComplete(CNtlPacket * pPacket)
{
	printf("--- GU_ENTER_WORLD_COMPLETE --- \n");

	CNtlPacket packet(sizeof(sGU_ENTER_WORLD_COMPLETE));
	sGU_ENTER_WORLD_COMPLETE * res = (sGU_ENTER_WORLD_COMPLETE *)packet.GetPacketData();

	res->wOpCode = GU_ENTER_WORLD_COMPLETE;

	packet.SetPacketLen(sizeof(sGU_ENTER_WORLD_COMPLETE));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_ENTER_WORLD_COMPLETE, packet);

	CNtlPacket packet2(sizeof(sGU_AVATAR_RP_DECREASE_START_NFY));
	sGU_AVATAR_RP_DECREASE_START_NFY * res2 = (sGU_AVATAR_RP_DECREASE_START_NFY *)packet2.GetPacketData();
	res2->wOpCode = GU_AVATAR_RP_DECREASE_START_NFY;
	packet2.SetPacketLen(sizeof(sGU_ENTER_WORLD_COMPLETE));
	g_pApp->Send(this->GetHandle(), &packet2);


}

//--------------------------------------------------------------------------------------//
//		Tutorial Hint request
//--------------------------------------------------------------------------------------//
void CClientSession::SendTutorialHintReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TUTORIAL_HINT_UPDATE_REQ * req = (sUG_TUTORIAL_HINT_UPDATE_REQ *)pPacket->GetPacketData();
	//req->dwTutorialHint;
	printf("--- TUTORIAL HINT REQUEST %i --- \n", req->dwTutorialHint);

	CNtlPacket packet(sizeof(sGU_TUTORIAL_HINT_UPDATE_RES));
	sGU_TUTORIAL_HINT_UPDATE_RES * res = (sGU_TUTORIAL_HINT_UPDATE_RES *)packet.GetPacketData();

	//app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	//app->db->setInt(1, this->characterID);
	//app->db->execute();
	//app->db->fetch();
	res->wOpCode = GU_TUTORIAL_HINT_UPDATE_RES;
	res->wResultCode = GAME_SUCCESS;
	res->dwTutorialHint = 0;//req->dwTutorialHint;

	packet.SetPacketLen(sizeof(sGU_TUTORIAL_HINT_UPDATE_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_TUTORIAL_HINT_UPDATE_RES, packet);

}

//--------------------------------------------------------------------------------------//
//		Char Ready
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharReady(CNtlPacket * pPacket)
{
	//printf("--- SEND CHAR READY --- \n");

	CNtlPacket packet(sizeof(sUG_CHAR_READY));
	sUG_CHAR_READY * res = (sUG_CHAR_READY *)packet.GetPacketData();

	res->wOpCode = UG_CHAR_READY;
	res->byAvatarType = 0;

	packet.SetPacketLen(sizeof(sUG_CHAR_READY));
	int rc = g_pApp->Send(this->GetHandle(), &packet);

}

//--------------------------------------------------------------------------------------//
//		Char Move
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharMove(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- SEND CHAR MOVE --- \n");

	sUG_CHAR_MOVE * req = (sUG_CHAR_MOVE*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_CHAR_MOVE));
	sGU_CHAR_MOVE * res = (sGU_CHAR_MOVE *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_MOVE;
	res->handle = this->GetavatarHandle();
	res->vCurLoc.x = dbo_move_pos_to_float(req->pos_move_x);
	res->vCurLoc.y = dbo_move_pos_to_float(req->pos_move_y);
	res->vCurLoc.z = dbo_move_pos_to_float(req->pos_move_z);
	res->vCurDir.x = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_x);
	res->vCurDir.y = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_y);
	res->vCurDir.z = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_z);
	res->move_type = req->move_type;
	res->move_flag = NTL_MOVE_KEYBOARD_FIRST;
	res->relleno[0] = 0;
	plr->SetPlayerLastDirection(plr->GetPlayerDirection());
	plr->SetPlayerLastPosition(plr->GetPlayerPosition());
	plr->SetPlayerPosition(res->vCurLoc);
	plr->SetPlayerDirection(res->vCurDir);

	packet.SetPacketLen(sizeof(sGU_CHAR_MOVE));
	app->UserBroadcastothers(&packet, this);
	
	UpdateCharState(this->GetavatarHandle(), CHARSTATE_MOVING);
	PACKET_TRACE(GU_CHAR_MOVE, packet);
	
	


	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Char Destination Move
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharDestMove(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- CHARACTER REQUEST DEST MOVE --- \n");

	sUG_CHAR_DEST_MOVE * req = (sUG_CHAR_DEST_MOVE*)pPacket->GetPacketData();

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_CHAR_DEST_MOVE));
	sGU_CHAR_DEST_MOVE * res = (sGU_CHAR_DEST_MOVE *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_DEST_MOVE;
	res->handle = this->GetavatarHandle();
	res->dwTimeStamp = req->dwTimeStamp;
	res->vCurLoc.x = req->vCurLoc.x;
	res->vCurLoc.y = req->vCurLoc.y;
	res->vCurLoc.z = req->vCurLoc.z;
	
	res->byMoveFlag = NTL_MOVE_MOUSE_MOVEMENT;
	res->bHaveSecondDestLoc = false;
	res->byDestLocCount = 10;
	for (int i = 0; i < res->byDestLocCount; i++)
	{
		res->avDestLoc[i].x = req->vDestLoc.x;
		res->avDestLoc[i].y = req->vDestLoc.y;
		res->avDestLoc[i].z = req->vDestLoc.z;
	}
	
	packet.SetPacketLen(sizeof(sGU_CHAR_DEST_MOVE));
	app->UserBroadcastothers(&packet, this);

	//UpdateCharState(this->GetavatarHandle(), CHARSTATE_MOVING);

	PACKET_TRACE(GU_CHAR_DEST_MOVE, packet);
	plr = NULL;
	delete plr;

	
}

//--------------------------------------------------------------------------------------//
//		Char Move Sync
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharMoveSync(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- CHARACTER MOVE SYNC --- \n");
	sUG_CHAR_AIR_MOVE_SYNC * req = (sUG_CHAR_AIR_MOVE_SYNC *)pPacket->GetPacketData();

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_CHAR_AIR_MOVE_SYNC));
	sGU_CHAR_AIR_MOVE_SYNC * res = (sGU_CHAR_AIR_MOVE_SYNC*)packet.GetPacketData();

	res->wOpCode = GU_CHAR_AIR_MOVE_SYNC;
	res->handle = this->GetavatarHandle();
	res->vCurLoc.x = dbo_move_pos_to_float(req->pos_move_x);
	res->vCurLoc.y = dbo_move_pos_to_float(req->pos_move_y);
	res->vCurLoc.z = dbo_move_pos_to_float(req->pos_move_z);
	res->vCurDir.x = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_x);
	res->vCurDir.y = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_y);
	res->vCurDir.z = DBO_MOVE_DIR_TO_FLOAT(req->dir_move_z);

	packet.SetPacketLen(sizeof(sGU_CHAR_AIR_MOVE_SYNC));
	app->UserBroadcastothers(&packet, this);

	plr->SetPlayerLastDirection(plr->GetPlayerDirection());
	plr->SetPlayerLastPosition(plr->GetPlayerPosition());
	plr->SetPlayerPosition(res->vCurLoc);
	plr->SetPlayerDirection(res->vCurDir);


	
	printf("move: %f %f %f, %f %f %f, %d\n",
		dbo_move_pos_to_float(req->pos_move_x),
		dbo_move_pos_to_float(req->pos_move_y),
		dbo_move_pos_to_float(req->pos_move_z),
		DBO_MOVE_DIR_TO_FLOAT(req->dir_move_x),
		DBO_MOVE_DIR_TO_FLOAT(req->dir_move_y),
		DBO_MOVE_DIR_TO_FLOAT(req->dir_move_z));

	PACKET_TRACE(GU_CHAR_AIR_MOVE_SYNC, packet);
	//UpdateCharState(this->GetavatarHandle(), CHARSTATE_STANDING);

	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Char Change Heading
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharChangeHeading(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- CHARACTER CHANGE HEADING --- \n");
	sUG_CHAR_CHANGE_HEADING * req = (sUG_CHAR_CHANGE_HEADING*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());


	CNtlPacket packet(sizeof(sGU_CHAR_CHANGE_HEADING));
	sGU_CHAR_CHANGE_HEADING * res = (sGU_CHAR_CHANGE_HEADING *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_CHANGE_HEADING;
	res->handle = this->GetavatarHandle();
	res->vNewHeading.x = req->vCurrentHeading.x;
	res->vNewHeading.y = req->vCurrentHeading.y;
	res->vNewHeading.z = req->vCurrentHeading.z;
	res->vNewLoc.x = req->vCurrentPosition.x;
	res->vNewLoc.y = req->vCurrentPosition.y;
	res->vNewLoc.z = req->vCurrentPosition.z;
	packet.SetPacketLen(sizeof(sGU_CHAR_CHANGE_HEADING));
	app->UserBroadcastothers(&packet, this);

	


	//UpdateCharState(this->GetavatarHandle(), CHARSTATE_STANDING);
	PACKET_TRACE(GU_CHAR_CHANGE_HEADING, packet);
}
//--------------------------------------------------------------------------------------//
//		Char Jump
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharJump(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- CHARACTER JUMP --- \n");
	sUG_CHAR_CHANGE_HEADING * req = (sUG_CHAR_CHANGE_HEADING*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_CHAR_JUMP));
	sGU_CHAR_JUMP * res = (sGU_CHAR_JUMP *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_JUMP;
	res->handle = this->GetavatarHandle();
	res->vCurrentHeading.x = req->vCurrentHeading.x;
	res->vCurrentHeading.y = req->vCurrentHeading.y;
	res->vCurrentHeading.z = req->vCurrentHeading.z;

	res->vJumpDir.x = req->vCurrentPosition.x;
	res->vJumpDir.y = req->vCurrentPosition.y;
	res->vJumpDir.z = req->vCurrentPosition.z;

	res->byMoveDirection = 1;
	//plr->SetPlayerFight(false);

	packet.SetPacketLen(sizeof(sGU_CHAR_JUMP));
	app->UserBroadcastothers(&packet, this);
	PACKET_TRACE(GU_CHAR_JUMP, packet);


	plr = NULL;
	delete plr;

}

//--------------------------------------------------------------------------------------//
//		Change Char Direction on floating
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharChangeDirOnFloating(CNtlPacket * pPacket, CGameServer * app)
{
	//	printf("--- Change Char Direction on floating --- \n");
	sUG_CHAR_CHANGE_DIRECTION_ON_FLOATING * req = (sUG_CHAR_CHANGE_DIRECTION_ON_FLOATING*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_CHANGE_DIRECTION_ON_FLOATING));
	sGU_CHAR_CHANGE_DIRECTION_ON_FLOATING * res = (sGU_CHAR_CHANGE_DIRECTION_ON_FLOATING *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_CHANGE_DIRECTION_ON_FLOATING;
	res->hSubject = this->GetavatarHandle();
	res->vCurDir.x = req->vCurDir.x;
	res->vCurDir.y = req->vCurDir.y;
	res->vCurDir.z = req->vCurDir.z;
	res->byMoveDirection = req->byMoveDirection;

	packet.SetPacketLen(sizeof(sGU_CHAR_CHANGE_DIRECTION_ON_FLOATING));
	app->UserBroadcastothers(&packet, this);

	
}
//--------------------------------------------------------------------------------------//
//		Char falling
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharFalling(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- character falling --- \n");
	sUG_CHAR_FALLING * req = (sUG_CHAR_FALLING*)pPacket->GetPacketData();

	req->wOpCode = UG_CHAR_FALLING;
	req->bIsFalling = true;

	req->vCurLoc.x;
	req->vCurLoc.y;
	req->vCurLoc.z;
	req->vCurDir.x;
	req->vCurDir.z;
	req->byMoveDirection;

	



}

//--------------------------------------------------------------------------------------//
//		GM Command
//--------------------------------------------------------------------------------------//
void CClientSession::RecvServerCommand(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle()); 
	if (plr->GetPcProfile()->bIsGameMaster == true)
	{
		sUG_SERVER_COMMAND * pServerCmd = (sUG_SERVER_COMMAND*)pPacket->GetPacketData();

		char chBuffer[1024];
		wcout << pServerCmd->awchCommand << endl;
		cout << pServerCmd->awchCommand << endl;
		::WideCharToMultiByte(GetACP(), 0, pServerCmd->awchCommand, -1, chBuffer, 1024, NULL, NULL);
		wchar_t wstr[1024];
		std::string str = "";

		CNtlTokenizer lexer(chBuffer);

		if (!lexer.IsSuccess())
			return;

		enum ECmdParseState
		{
			SERVER_CMD_NONE,
			SERVER_CMD_KEY,
			SERVER_CMD_END,
		};

		ECmdParseState eState = SERVER_CMD_KEY;
		int iOldLine = 0;
		int iLine;

		while (1)
		{
			std::string strToken = lexer.PeekNextToken(NULL, &iLine);
			cout << strToken.c_str() << endl;
			cout << strToken << endl;
			if (strToken == "")
				break;

			switch (eState)
			{
			case SERVER_CMD_KEY:
				if (strToken == "@setspeed")
				{
					printf("received char speed command");
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					float fSpeed = (float)atof(strToken.c_str());
					CClientSession::SendUpdateCharSpeed(fSpeed, app);
					return;
				}

				else if (strToken == "@checkspawn")
				{
					printf("Spawning Check\n");
					PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

					sVECTOR3 curpos = plr->GetPlayerPosition();
					printf("CurPos x[%u] y[%u] z[%u]\n", curpos.x, curpos.y, curpos.z);
					g_pMobManager->RunSpawnCheck(pPacket, curpos, plr->myCCSession);
					return;
				}

				else if (strToken == "@addmob")//fixed
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int uiMobId = (unsigned int)atoi(strToken.c_str());
					lexer.PopToPeek();
					printf("Executing Mob Func\n");
					CClientSession::CreateMonsterById(uiMobId);
					printf("Executed\n");
					return;
				}
				else if (strToken == "@addnpc")//Fixed
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int uiMobId = (unsigned int)atoi(strToken.c_str());
					lexer.PopToPeek();
					printf("Executing NPC Func\n");
					CClientSession::CreateNPCById(uiMobId);
					printf("Executed\n");
					return;
				}
				else if (strToken == "@createitem")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int uiTblId = (unsigned int)atof(strToken.c_str());
					//SendAddItem(uiTblId);
					return;
				}
				else if (strToken == "@learnskill")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int tblidx = (unsigned int)atof(strToken.c_str());
					AddSkillById(tblidx);
					return;
				}
				else if (strToken == "@learnhtb")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int uiTblId = (unsigned int)atof(strToken.c_str());
					//	SendCharLearnHTBRes(uiTblId);
					return;
				}
				else if (strToken == "@refreshlp")
				{
					//	app->db->prepare("SELECT LastMaxLp FROM characters WHERE CharID = ?");
					//	app->db->setInt(1, this->characterID);
					//	app->db->execute();
					//	app->db->fetch();
					//	int max_lp = app->db->getInt("LastMaxLp");

					return;
				}
				else if (strToken == "@setscale")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					float fScale = (float)atof(strToken.c_str());
					//	CNtlSob *pSobObj = GetNtlSobManager()->GetSobObject(m_uiTargetSerialId);
					//	if(pSobObj)
					//	{
					//		CNtlSobProxy *pSobProxy = pSobObj->GetSobProxy();
					//		pSobProxy->SetScale(fScale);
					//	}

					return;
				}
				else if (strToken == "@is")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					//	CNtlBehaviorProjSteal::m_ffIncSpeed = (RwReal)atof(strToken.c_str());
				}
				else if (strToken == "@iw")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					//	CNtlBehaviorProjSteal::m_fWaitCheckTime = (RwReal)atof(strToken.c_str());
				}
				else if (strToken == "@heal")
				{
					CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
					sGU_UPDATE_CHAR_LP_EP* res = (sGU_UPDATE_CHAR_LP_EP*)packet.GetPacketData();
					PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
					res->handle = this->GetavatarHandle();
					res->dwLpEpEventId = 0;
					res->wCurEP = 6780;
					res->wCurLP = 8750;
					res->wMaxEP = 6785;
					res->wMaxLP = 8754;
					res->wOpCode = GU_UPDATE_CHAR_LP_EP;

					packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
					g_pApp->Send(this->GetHandle(), &packet);
					app->UserBroadcastothers(&packet, this);
					return;
				}
				else if (strToken == "@setlevel")
				{
					lexer.PopToPeek();
					strToken = lexer.PeekNextToken(NULL, &iLine);
					unsigned int level = (unsigned int)atof(strToken.c_str());

					PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
					CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
					sGU_UPDATE_CHAR_SP * res2 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();

					plr->GetPcProfile()->dwCurExp -= plr->GetPcProfile()->dwMaxExpInThisLevel;
					plr->GetPcProfile()->dwMaxExpInThisLevel += (plr->GetPcProfile()->dwMaxExpInThisLevel * 1);

					CNtlPacket packet1(sizeof(sGU_UPDATE_CHAR_LEVEL));
					sGU_UPDATE_CHAR_LEVEL * response1 = (sGU_UPDATE_CHAR_LEVEL*)packet1.GetPacketData();
					response1->byPrevLevel = plr->GetPcProfile()->byLevel;
					plr->GetPcProfile()->byLevel = level;
					response1->byCurLevel = plr->GetPcProfile()->byLevel;
					response1->dwMaxExpInThisLevel = plr->GetPcProfile()->dwMaxExpInThisLevel;
					response1->handle = plr->GetAvatarHandle();
					response1->wOpCode = GU_UPDATE_CHAR_LEVEL;

					packet1.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LEVEL));
					g_pApp->Send(this->GetHandle(), &packet1);

					plr->SetLevelUP();
					//plr->cPlayerAttribute->UpdateAvatarAttributes(plr->GetAvatarHandle());
					plr->GetPcProfile()->dwSpPoint = level;
					app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);
					app->qry->UpdatePlayerLevel(plr->GetPcProfile()->byLevel, plr->GetCharID(), plr->GetPcProfile()->dwCurExp, plr->GetPcProfile()->dwMaxExpInThisLevel);
					//response->dwCurExp = plr->GetPcProfile()->dwCurExp;
					plr->SetRPBall();
					//plr->SendRpBallInformation();
					res2->wOpCode = GU_UPDATE_CHAR_SP;
					res2->dwSpPoint = plr->GetPcProfile()->dwSpPoint;

					packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
					g_pApp->Send(this->GetHandle(), &packet2);

					CNtlPacket packet4(sizeof(sGU_UPDATE_CHAR_STATE));
					sGU_UPDATE_CHAR_STATE* res4 = (sGU_UPDATE_CHAR_STATE*)packet4.GetPacketData();

					res4->handle = this->GetavatarHandle();
					res4->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
					res4->wOpCode = GU_UPDATE_CHAR_STATE;

					packet4.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
					g_pApp->Send(this->GetHandle(), &packet4);
					app->UserBroadcastothers(&packet1, this);
					return;
				}
				else if (strToken == "@announce")
				{
					const char* sMsg = NULL;
					while (42)
					{
						strToken = lexer.PeekNextToken(NULL, &iLine);
						str += strToken;
						if (strToken == "")
						{
							break;
						}

					}
					std::wstring widestr = std::wstring(str.begin(), str.end());
					SendServerAnnouncement(widestr.c_str(), app);
				}
				else if (strToken == "@notice")
				{
					const char* sMsg = NULL;
					while (42)
					{
						strToken = lexer.PeekNextToken(NULL, &iLine);
						str += strToken;
						if (strToken == "")
						{
							break;
						}

					}
					std::wstring widestr = std::wstring(str.begin(), str.end());
					SendServerBroadcast(widestr.c_str(), app);
				}
				break;
			}

			lexer.PopToPeek();
		}
	}
	else
	{
		printf("NOT HAVE GM ACCESS");
	}
}
//--------------------------------------------------------------------------------------//
//		Update Char speed *dont work*
//--------------------------------------------------------------------------------------//
void CClientSession::SendUpdateCharSpeed(float fSpeed, CGameServer * app)
{
	printf("Update char speed \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_SPEED));
	sGU_UPDATE_CHAR_SPEED * res = (sGU_UPDATE_CHAR_SPEED *)packet.GetPacketData();

	res->wOpCode = GU_UPDATE_CHAR_SPEED;
	res->handle = this->GetavatarHandle();
	res->fLastWalkingSpeed = fSpeed * .50;
	res->fLastRunningSpeed = fSpeed;
	res->fLastFlySpeed = fSpeed * .50;
	res->fLastFlyBoostSpeed = fSpeed;
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SPEED));
	app->UserBroadcast(&packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Select target
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharTargetSelect(CNtlPacket * pPacket)
{
	sUG_CHAR_TARGET_SELECT * req = (sUG_CHAR_TARGET_SELECT*)pPacket->GetPacketData();
	m_uiTargetSerialId = req->hTarget;
	printf("UG_CHAR_TARGET_SELECT %i \n", m_uiTargetSerialId);
}
//--------------------------------------------------------------------------------------//
//		Select target
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharTargetFacing(CNtlPacket * pPacket)
{
	printf("UG_CHAR_TARGET_FACING \n");

	sUG_CHAR_TARGET_SELECT * req = (sUG_CHAR_TARGET_SELECT*)pPacket->GetPacketData();
	m_uiTargetSerialId = req->hTarget;
}
//--------------------------------------------------------------------------------------//
//		target info
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharTargetInfo(CNtlPacket * pPacket)
{
	printf("UG_CHAR_TARGET_INFO \n");
	sUG_CHAR_TARGET_SELECT * req = (sUG_CHAR_TARGET_SELECT*)pPacket->GetPacketData();
	m_uiTargetSerialId = req->hTarget;
	
}
//--------------------------------------------------------------------------------------//
//		Send game leave request
//--------------------------------------------------------------------------------------//
void CClientSession::SendGameLeaveReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- CHARACTER REQUEST LEAVE GAME --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	plr->SetPlayerFight(false);
	plr->SetPlayerLeaving(true);
	Sleep(500);
	app->db->prepare("UPDATE characters SET IsOnline = 0, OnlineID = 0 WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();

	CNtlPacket packet(sizeof(sGU_OBJECT_DESTROY));
	sGU_OBJECT_DESTROY * sPacket = (sGU_OBJECT_DESTROY *)packet.GetPacketData();

	sPacket->wOpCode = GU_OBJECT_DESTROY;
	sPacket->handle = this->GetavatarHandle();
	
	packet.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
	app->UserBroadcastothers(&packet, this);
	plr->SavePlayerData(app);
	app->RemoveUser(plr->GetPlayerName().c_str());
	g_pPlayerManager->RemovePlayer(avatarHandle);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Char exit request
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharExitReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- Char exit request --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	plr->SetPlayerFight(false);
	plr->SetPlayerLeaving(true);
	Sleep(500);
	app->db->prepare("UPDATE characters SET IsOnline = 0, OnlineID = 0 WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();

//	plr->SavePlayerData(app);
	// log out of game
	CNtlPacket packet1(sizeof(sGU_OBJECT_DESTROY));
	sGU_OBJECT_DESTROY * sPacket = (sGU_OBJECT_DESTROY *)packet1.GetPacketData();

	sPacket->wOpCode = GU_OBJECT_DESTROY;
	sPacket->handle = this->GetavatarHandle();
	this->cPlayersMain = NULL;
	packet1.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
	app->UserBroadcastothers(&packet1, this);
	//g_pPlayerManager->RemovePlayer(this->GetavatarHandle());
	

	// log in to char server
	CNtlPacket packet(sizeof(sGU_CHAR_EXIT_RES));
	sGU_CHAR_EXIT_RES * res = (sGU_CHAR_EXIT_RES *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_EXIT_RES;
	res->wResultCode = GAME_SUCCESS;
	strcpy_s((char*)res->achAuthKey, NTL_MAX_SIZE_AUTH_KEY, "Dbo");
	res->byServerInfoCount = 1;
	strcpy_s(res->aServerInfo[0].szCharacterServerIP, NTL_MAX_LENGTH_OF_IP, app->GetConfigFileExternalIP());
	res->aServerInfo[0].wCharacterServerPortForClient = 20300;
	res->aServerInfo[0].dwLoad = 0;
	g_pPlayerManager->RemovePlayer(avatarHandle);
	app->RemoveUser(plr->GetPlayerName().c_str());
	packet.SetPacketLen(sizeof(sGU_CHAR_EXIT_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}

//--------------------------------------------------------------------------------------//
//		Char sit down
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharSitDown(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- Char sit down request --- \n");
	/*PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_CHAR_SITDOWN));
	sGU_CHAR_SITDOWN * sPacket = (sGU_CHAR_SITDOWN *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* pSit = (sGU_UPDATE_CHAR_STATE*)packet2.GetPacketData();

	pSit->handle = this->GetavatarHandle();
	pSit->wOpCode = GU_UPDATE_CHAR_STATE;
	pSit->sCharState.sCharStateBase.byStateID = CHARSTATE_SITTING;
	pSit->sCharState.sCharStateBase.vCurLoc.x = plr->GetPlayerPosition().x;
	pSit->sCharState.sCharStateBase.vCurLoc.y = plr->GetPlayerPosition().y;
	pSit->sCharState.sCharStateBase.vCurLoc.z = plr->GetPlayerPosition().z;
	pSit->sCharState.sCharStateBase.vCurDir.x = plr->GetPlayerDirection().x;
	pSit->sCharState.sCharStateBase.vCurDir.y = plr->GetPlayerDirection().y;
	pSit->sCharState.sCharStateBase.vCurDir.z = plr->GetPlayerDirection().z;

	sPacket->wOpCode = GU_CHAR_SITDOWN;
	sPacket->handle = this->GetavatarHandle();

	packet.SetPacketLen(sizeof(sGU_CHAR_SITDOWN));
	packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	app->UserBroadcastothers(&packet, this);
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
	plr->SetPlayerSit(true);
	app->UserBroadcastothers(&packet2, this);
	plr = NULL;
	delete plr;*/
}
//--------------------------------------------------------------------------------------//
//		Char stand up
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharStandUp(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- Char stand up request --- \n");
	/*PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_CHAR_STANDUP));
	sGU_CHAR_STANDUP * sPacket = (sGU_CHAR_STANDUP *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* pSitUp = (sGU_UPDATE_CHAR_STATE*)packet2.GetPacketData();

	pSitUp->handle = this->GetavatarHandle();
	pSitUp->wOpCode = GU_UPDATE_CHAR_STATE;
	pSitUp->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	pSitUp->sCharState.sCharStateBase.vCurLoc.x = plr->GetPlayerPosition().x;
	pSitUp->sCharState.sCharStateBase.vCurLoc.y = plr->GetPlayerPosition().y;
	pSitUp->sCharState.sCharStateBase.vCurLoc.z = plr->GetPlayerPosition().z;
	pSitUp->sCharState.sCharStateBase.vCurDir.x = plr->GetPlayerDirection().x;
	pSitUp->sCharState.sCharStateBase.vCurDir.y = plr->GetPlayerDirection().y;
	pSitUp->sCharState.sCharStateBase.vCurDir.z = plr->GetPlayerDirection().z;

	sPacket->wOpCode = GU_CHAR_STANDUP;
	sPacket->handle = this->GetavatarHandle();
	packet.SetPacketLen(sizeof(sGU_CHAR_STANDUP));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
	app->UserBroadcastothers(&packet, this);
	app->UserBroadcastothers(&packet2, this);
	plr->SetPlayerSit(false);
	plr = NULL;
	delete plr;*/
}
//--------------------------------------------------------------------------------------//
//		char start mail
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharMailStart(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- char start mail --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_MAIL_START_REQ * req = (sUG_MAIL_START_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_MAIL_START_RES));
	sGU_MAIL_START_RES * res = (sGU_MAIL_START_RES *)packet.GetPacketData();

	app->db->prepare("SELECT MailIsAway FROM characters WHERE CharID=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();

	//res->hObject = req->hObject;
	res->wOpCode = GU_MAIL_START_RES;
	res->wResultCode = GAME_SUCCESS;
	res->bIsAway = app->db->getBoolean("MailIsAway");

	app->db->prepare("SELECT * FROM mail WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	if (app->db->rowsCount() > 0)
	{
		while (app->db->fetch())
		{
			CNtlPacket packet2(sizeof(sGU_MAIL_LOAD_DATA));
			sGU_MAIL_LOAD_DATA * res2 = (sGU_MAIL_LOAD_DATA *)packet2.GetPacketData();
			res2->wOpCode = GU_MAIL_LOAD_DATA;
			res2->sData.bIsAccept = app->db->getBoolean("bIsAccept");
			res2->sData.bIsLock = app->db->getBoolean("bIsLock");
			res2->sData.bIsRead = app->db->getBoolean("bIsRead");
			//	res2->sData.bySenderType = app->db->getInt("SenderType");
			res2->sData.byMailType = app->db->getInt("byMailType");
			wcscpy_s(res2->wszText, NTL_MAX_LENGTH_OF_MAIL_MESSAGE_IN_UNICODE, s2ws(app->db->getString("wszText")).c_str());
			wcscpy_s(res2->sData.wszFromName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("wszFromName")).c_str());
			res2->byTextSize = app->db->getInt("byTextSize");
			res2->sData.dwZenny = app->db->getInt("dwZenny");
			res2->byTextSize = app->db->getInt("byTextSize");
			res2->sData.endTime = app->db->getInt("byDay");
			res2->sData.mailID = app->db->getInt("id");
			packet2.SetPacketLen(sizeof(sGU_MAIL_LOAD_DATA));
			g_pApp->Send(this->GetHandle(), &packet2);
			PACKET_TRACE(GU_MAIL_LOAD_DATA, packet2);

		}
	}

	packet.SetPacketLen(sizeof(sGU_MAIL_START_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_MAIL_START_RES, packet);

	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		load mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailLoadReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- load mails --- \n");

	sUG_MAIL_LOAD_REQ * req = (sUG_MAIL_LOAD_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_MAIL_LOAD_RES));
	sGU_MAIL_LOAD_RES * res = (sGU_MAIL_LOAD_RES *)packet.GetPacketData();

	res->wOpCode = GU_MAIL_LOAD_RES;
	res->hObject = req->hObject;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_MAIL_LOAD_RES));

	g_pApp->Send(this->GetHandle(), &packet);
	PACKET_TRACE(GU_MAIL_LOAD_RES, packet);
}
//--------------------------------------------------------------------------------------//
//		reload mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailReloadReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	//printf("--- reload mails --- \n");
	sUG_MAIL_RELOAD_REQ * req = (sUG_MAIL_RELOAD_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_MAIL_RELOAD_RES));
	sGU_MAIL_RELOAD_RES * res = (sGU_MAIL_RELOAD_RES *)packet.GetPacketData();

	// COUNT UNREAD MESSAGES START
	app->db->prepare("SELECT COUNT(*) AS countmsg FROM mail WHERE CharID = ? AND bIsRead=0");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();
	RwUInt32 count_unread_msg = app->db->getInt("countmsg");
	// COUNT UNREAD MESSAGES END

	app->db->prepare("SELECT * FROM mail WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	RwUInt32 i = 0;
	while (app->db->fetch())
	{
		res->aMailID[i] = app->db->getInt("id");
		i++;
	}

	res->byMailCount = app->db->rowsCount(); //count all mails
	res->byManagerCount = count_unread_msg; //amount of unread messages
	res->byNormalCount = 0;
	res->hObject = req->hObject;
	res->wOpCode = GU_MAIL_RELOAD_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_MAIL_RELOAD_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	this->SendCharMailStart(pPacket, app);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		read mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailReadReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- UG_MAIL_READ_REQ --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_MAIL_READ_REQ * req = (sUG_MAIL_READ_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT * FROM mail WHERE id = ?");
	app->db->setInt(1, req->mailID);
	app->db->execute();
	app->db->fetch();

	RwUInt32 itemid = app->db->getInt("item_id");

	CNtlPacket packet(sizeof(sGU_MAIL_READ_RES));
	sGU_MAIL_READ_RES * res = (sGU_MAIL_READ_RES *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_MAIL_LOAD_INFO));
	sGU_MAIL_LOAD_INFO * res2 = (sGU_MAIL_LOAD_INFO *)packet2.GetPacketData();


	res->byRemainDay = app->db->getInt("byDay");
	res->endTime = 100;
	res->hObject = req->hObject;
	res->mailID = req->mailID;
	res->wOpCode = GU_MAIL_READ_RES;
	res->wResultCode = GAME_SUCCESS;

	res2->wOpCode = GU_MAIL_LOAD_INFO;
	res2->sData.bIsAccept = app->db->getBoolean("bIsAccept");
	res2->sData.bIsLock = app->db->getBoolean("bIsLock");
	res2->sData.bIsRead = app->db->getBoolean("bIsRead");
	res2->byTextSize = app->db->getInt("byTextSize");
	res2->sData.byExpired = 100;
	res2->sData.bySenderType = eMAIL_SENDER_TYPE_BASIC;
	res2->sData.byMailType = app->db->getInt("byMailType");
	res2->sData.dwZenny = app->db->getInt("dwZenny");
	res2->sData.endTime = app->db->getInt("byDay");
	res2->sData.mailID = app->db->getInt("id");
	wcscpy_s(res2->sData.wszFromName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("wszFromName")).c_str());
	wcscpy_s(res2->wszText, NTL_MAX_LENGTH_OF_MAIL_MESSAGE_IN_UNICODE, s2ws(app->db->getString("wszText")).c_str());

	//	res2->sData.tCreateTime.second = app->db->getInt("byDay");
	//	res2->sData.tCreateTime.minute = app->db->getInt("byDay");
	//	res2->sData.tCreateTime.hour = app->db->getInt("byDay");
	//	res2->sData.tCreateTime.day = app->db->getInt("tCreateTime");
	//	res2->sData.tCreateTime.month = app->db->getInt("byDay");
	//	res2->sData.tCreateTime.year = app->db->getInt("byDay");

	if (app->db->getInt("byMailType") > 1){
		app->db->prepare("SELECT id,tblidx,count,rank,durability,grade FROM items WHERE id = ?");
		app->db->setInt(1, itemid);
		app->db->execute();
		app->db->fetch();
		res2->sData.sItemProfile.handle = app->db->getInt("id");
		res2->sData.sItemProfile.byCurDur = app->db->getInt("durability");
		res2->sData.sItemProfile.byStackcount = app->db->getInt("count");
		res2->sData.sItemProfile.byGrade = app->db->getInt("grade");
		res2->sData.sItemProfile.tblidx = app->db->getInt("tblidx");
		res2->sData.sItemProfile.byRank = app->db->getInt("rank");
	}

	packet2.SetPacketLen(sizeof(sGU_MAIL_LOAD_INFO));
	g_pApp->Send(this->GetHandle(), &packet2);

	//SET MAIL READ
	app->qry->SetMailRead(req->mailID);

	packet.SetPacketLen(sizeof(sGU_MAIL_READ_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		send mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailSendReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_MAIL_SEND_REQ * req = (sUG_MAIL_SEND_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_MAIL_SEND_RES));
	sGU_MAIL_SEND_RES * res = (sGU_MAIL_SEND_RES *)packet.GetPacketData();

	app->db->prepare("SELECT CharID,MailIsAway FROM characters WHERE CharName = ?");

	char targetname[NTL_MAX_SIZE_CHAR_NAME_UNICODE];
	wcstombs(targetname, req->wszTargetName, NTL_MAX_SIZE_CHAR_NAME_UNICODE);

	char *text = new char[req->byTextSize];
	wcstombs(text, req->wszText, req->byTextSize);
	text[req->byTextSize] = '\0';

	app->db->setString(1, targetname);
	app->db->execute();
	app->db->fetch();
	int id = app->db->getInt("CharID");

	wcscpy_s(res->wszTargetName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, req->wszTargetName);
	res->wOpCode = GU_MAIL_SEND_RES;
	res->hObject = req->hObject;

	if (app->db->getBoolean("MailIsAway") == 1){
		res->wResultCode = GAME_MAIL_TARGET_AWAY_STATE;
	}
	else {
		res->wResultCode = GAME_SUCCESS;
		app->db->prepare("INSERT INTO mail (CharID, byDay, byMailType, byTextSize, dwZenny, wszText, item_id, item_place, item_pos, wszTargetName, wszFromName, bIsAccept, bIsLock, bIsRead) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		app->db->setInt(1, id);
		app->db->setInt(2, req->byDay);
		app->db->setInt(3, req->byMailType);
		app->db->setInt(4, req->byTextSize);
		app->db->setInt(5, req->dwZenny);
		app->db->setString(6, text);
		app->db->setInt(7, req->sItemData.hItem);
		app->db->setInt(8, req->sItemData.byPlace);
		app->db->setInt(9, req->sItemData.byPos);
		app->db->setString(10, targetname);
		app->db->setString(11, plr->GetPlayerName().c_str());
		app->db->setInt(12, app->db->getBoolean("MailIsAway"));
		app->db->setInt(13, false);
		app->db->setInt(14, false);
		app->db->execute();

		if (req->byMailType == 2 || req->byMailType == 5){
			//SET OWNER ID TO 0
			app->qry->UpdateItemOwnerIdWithUniqueID(0, req->sItemData.hItem);
			//DEL ITEM PACKET
			this->gsf->DeleteItemByUIdPlacePos(pPacket, this, req->sItemData.hItem, req->sItemData.byPlace, req->sItemData.byPos);
		}
		else if (req->byMailType == 3){
			//UPDATE CHAR MONEY
			app->qry->SetMinusMoney(plr->GetCharID(), req->dwZenny);
			plr->GetPcProfile()->dwZenny -= req->dwZenny;
			//UPDATE MONEY PACKET
			this->gsf->UpdateCharMoney(pPacket, this, 16, plr->GetPcProfile()->dwZenny, this->GetavatarHandle());
		}
		else if (req->byMailType == 4){
			//UPDATE MONEY
			app->qry->SetMinusMoney(plr->GetCharID(), req->dwZenny);
			plr->GetPcProfile()->dwZenny -= req->dwZenny;
			//UPDATE MONEY PACKET
			this->gsf->UpdateCharMoney(pPacket, this, 16, plr->GetPcProfile()->dwZenny, this->GetavatarHandle());
			//SET OWNER ID TO 0
			app->qry->UpdateItemOwnerIdWithUniqueID(0, req->sItemData.hItem);
			//DEL ITEM PACKET
			this->gsf->DeleteItemByUIdPlacePos(pPacket, this, req->sItemData.hItem, req->sItemData.byPlace, req->sItemData.byPos);
		}

	}

	packet.SetPacketLen(sizeof(sGU_MAIL_SEND_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		delete mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailDelReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- UG_MAIL_DEL_REQ --- \n");

	sUG_MAIL_DEL_REQ * req = (sUG_MAIL_DEL_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_MAIL_DEL_RES));
	sGU_MAIL_DEL_RES * res = (sGU_MAIL_DEL_RES *)packet.GetPacketData();

	app->qry->DeleteFromMailByID(req->mailID);

	res->hObject = req->hObject;
	res->mailID = req->mailID;
	res->wOpCode = GU_MAIL_DEL_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_MAIL_DEL_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		receive item with mail
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailItemReceiveReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- UG_MAIL_ITEM_RECEIVE_REQ --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_MAIL_ITEM_RECEIVE_REQ * req = (sUG_MAIL_ITEM_RECEIVE_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT * FROM mail WHERE id = ?");
	app->db->setInt(1, req->mailID);
	app->db->execute();
	app->db->fetch();

	CNtlPacket packet(sizeof(sGU_MAIL_ITEM_RECEIVE_RES));
	sGU_MAIL_ITEM_RECEIVE_RES * res = (sGU_MAIL_ITEM_RECEIVE_RES *)packet.GetPacketData();

	res->hObject = req->hObject;
	res->mailID = req->mailID;
	res->wOpCode = GU_MAIL_ITEM_RECEIVE_RES;
	res->wResultCode = GAME_SUCCESS;


	if (app->db->getInt("byMailType") == 2){
		//CHANGE ITEM OWNER
		app->qry->ChangeItemOwnerByUIdPlacePos(plr->GetCharID(), app->db->getInt("item_id"), 0, 0); // 0 = place and pos
		//CREATE ITEM PACKET
		CNtlPacket packet1(sizeof(sGU_ITEM_CREATE));
		sGU_ITEM_CREATE * res1 = (sGU_ITEM_CREATE *)packet1.GetPacketData();

		res1->wOpCode = GU_ITEM_CREATE;
		packet1.SetPacketLen(sizeof(sGU_ITEM_CREATE));
		g_pApp->Send(this->GetHandle(), &packet1);
	}
	else if (app->db->getInt("byMailType") == 3){
		//UPDATE MONEY
		plr->GetPcProfile()->dwZenny += app->db->getInt("dwZenny");
		app->qry->SetPlusMoney(plr->GetCharID(), app->db->getInt("dwZenny"));
		//UPDATE MONEY PACKET
		this->gsf->UpdateCharMoney(pPacket, this, 17, plr->GetPcProfile()->dwZenny, this->GetavatarHandle());
	}
	else if (app->db->getInt("byMailType") == 4){
		//UPDATE MONEY
		app->qry->SetPlusMoney(plr->GetCharID(), app->db->getInt("dwZenny"));
		plr->GetPcProfile()->dwZenny += app->db->getInt("dwZenny");
		//UPDATE MONEY PACKET
		this->gsf->UpdateCharMoney(pPacket, this, 17, plr->GetPcProfile()->dwZenny, this->GetavatarHandle());
		//CHANGE ITEM OWNER
		app->qry->ChangeItemOwnerByUIdPlacePos(plr->GetCharID(), app->db->getInt("item_id"), 0, 0); // 0 = place and pos
		//CREATE ITEM PACKET
		CNtlPacket packet3(sizeof(sGU_ITEM_CREATE));
		sGU_ITEM_CREATE * res3 = (sGU_ITEM_CREATE *)packet3.GetPacketData();

		res3->wOpCode = GU_ITEM_CREATE;
		packet3.SetPacketLen(sizeof(sGU_ITEM_CREATE));
		g_pApp->Send(this->GetHandle(), &packet3);
	}

	app->qry->SetMailAccept(plr->GetCharID(), req->mailID);

	packet.SetPacketLen(sizeof(sGU_MAIL_ITEM_RECEIVE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		delete multiple mails
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailMultiDelReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- UG_MAIL_MULTI_DEL_REQ --- \n");

	sUG_MAIL_MULTI_DEL_REQ * req = (sUG_MAIL_MULTI_DEL_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_MAIL_MULTI_DEL_RES));
	sGU_MAIL_MULTI_DEL_RES * res = (sGU_MAIL_MULTI_DEL_RES *)packet.GetPacketData();

	for (RwInt32 j = 0; j < req->byCount; ++j)
	{
		app->qry->DeleteFromMailByID(req->aMailID[j]);

		res->wOpCode = GU_MAIL_MULTI_DEL_RES;
		res->wResultCode = GAME_SUCCESS;
		res->hObject = req->hObject;
		res->byCount = req->byCount;
		res->aMailID[j] = req->aMailID[j];

		packet.SetPacketLen(sizeof(sGU_MAIL_MULTI_DEL_RES));
		g_pApp->Send(this->GetHandle(), &packet);
	}

}
//--------------------------------------------------------------------------------------//
//		lock mail
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailLockReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- LOCK MAIL --- \n");
	sUG_MAIL_LOCK_REQ * req = (sUG_MAIL_LOCK_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT * FROM mail WHERE id = ?");
	app->db->setInt(1, req->mailID);
	app->db->execute();
	app->db->fetch();

	CNtlPacket packet(sizeof(sGU_MAIL_LOCK_RES));
	sGU_MAIL_LOCK_RES * res = (sGU_MAIL_LOCK_RES *)packet.GetPacketData();

	res->wOpCode = GU_MAIL_LOCK_RES;
	res->wResultCode = GAME_SUCCESS;
	res->hObject = req->hObject;
	res->mailID = req->mailID;
	res->bIsLock = req->bIsLock;

	app->qry->UpdateMailLock(req->mailID, res->bIsLock);

	packet.SetPacketLen(sizeof(sGU_MAIL_LOCK_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		RETURN MAIL
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharMailReturnReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- RETURN MAIL --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_MAIL_RETURN_REQ * req = (sUG_MAIL_RETURN_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT item_id FROM mail WHERE id = ?");
	app->db->setInt(1, req->mailID);
	app->db->execute();
	app->db->fetch();

	CNtlPacket packet(sizeof(sGU_MAIL_RETURN_RES));
	sGU_MAIL_RETURN_RES * res = (sGU_MAIL_RETURN_RES *)packet.GetPacketData();

	res->wOpCode = GU_MAIL_RETURN_RES;
	res->wResultCode = GAME_SUCCESS;
	res->hObject = req->hObject;
	res->mailID = req->mailID;

	app->db->prepare("CALL ReturnMail (?,?)");
	app->db->setInt(1, req->mailID);
	app->db->setString(2, plr->GetPlayerName().c_str());
	app->db->execute();

	packet.SetPacketLen(sizeof(sGU_MAIL_RETURN_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}


//--------------------------------------------------------------------------------------//
//		char away req
//--------------------------------------------------------------------------------------//
void	CClientSession::SendCharAwayReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- char away req --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_CHAR_AWAY_REQ * req = (sUG_CHAR_AWAY_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_AWAY_RES));
	sGU_CHAR_AWAY_RES * res = (sGU_CHAR_AWAY_RES *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_AWAY_RES;
	res->wResultCode = GAME_SUCCESS;
	res->bIsAway = req->bIsAway;

	app->qry->UpdateCharAwayStatus(plr->GetCharID(), req->bIsAway);

	packet.SetPacketLen(sizeof(sGU_CHAR_AWAY_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		char follow move
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharFollowMove(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- SEND CHAR FOLLOW MOVE --- \n");
	sUG_CHAR_FOLLOW_MOVE * req = (sUG_CHAR_FOLLOW_MOVE*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_FOLLOW_MOVE));
	sGU_CHAR_FOLLOW_MOVE * res = (sGU_CHAR_FOLLOW_MOVE *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_FOLLOW_MOVE;
	res->handle = this->GetavatarHandle();
	res->hTarget = this->GetTargetSerialId();
	res->fDistance = req->fDistance;
	res->byMovementReason = req->byMovementReason;
	res->byMoveFlag = NTL_MOVE_FLAG_RUN;

	packet.SetPacketLen(sizeof(sGU_CHAR_FOLLOW_MOVE));
	app->UserBroadcastothers(&packet, this);
	int rc = g_pApp->Send(this->GetHandle(), &packet);
}



//--------------------------------------------------------------------------------------//
//		Create Guild
//--------------------------------------------------------------------------------------//
void CClientSession::SendGuildCreateReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- create guild request --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_GUILD_CREATE_REQ * req = (sUG_GUILD_CREATE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_GUILD_CREATE_RES));
	sGU_GUILD_CREATE_RES * res = (sGU_GUILD_CREATE_RES *)packet.GetPacketData();

	res->wOpCode = GU_GUILD_CREATE_RES;
	printf("guild manager id: %i ", req->hGuildManagerNpc);

	app->db->prepare("CALL GuildCreate (?,?, @wResultCode, @cguildid, @charactername)");
	app->db->setString(1, Ntl_WC2MB(req->wszGuildName));
	app->db->setInt(2, plr->GetCharID());
	app->db->execute();
	app->db->execute("SELECT @wResultCode, @cguildid, @charactername");
	app->db->fetch();

	int result = app->db->getInt("@wResultCode");

	printf("create guild result %i \n ", result);
	res->wResultCode = result;

	packet.SetPacketLen(sizeof(sGU_GUILD_CREATE_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);

	if (result == 200) {

		// CREATE GUILD
		CNtlPacket packet2(sizeof(sTU_GUILD_CREATED_NFY));
		sTU_GUILD_CREATED_NFY * res2 = (sTU_GUILD_CREATED_NFY *)packet2.GetPacketData();
		res2->wOpCode = TU_GUILD_CREATED_NFY;
		memcpy(res2->wszGuildName, req->wszGuildName, sizeof(wchar_t)* NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE);
		packet2.SetPacketLen(sizeof(sTU_GUILD_CREATED_NFY));
		rc = g_pApp->Send(this->GetHandle(), &packet2);

		// GUILD INFORMATIONS
		CNtlPacket packet3(sizeof(sTU_GUILD_INFO));
		sTU_GUILD_INFO * res3 = (sTU_GUILD_INFO *)packet3.GetPacketData();

		res3->wOpCode = TU_GUILD_INFO;
		res3->guildInfo.dwGuildReputation = 0;
		res3->guildInfo.guildId = app->db->getInt("@cguildid");
		res3->guildInfo.guildMaster = plr->GetCharID();
		memcpy(res3->guildInfo.wszName, req->wszGuildName, sizeof(wchar_t)* NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE);
		wcscpy_s(res3->guildInfo.awchName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("@charactername")).c_str());
		packet3.SetPacketLen(sizeof(sTU_GUILD_INFO));
		rc = g_pApp->Send(this->GetHandle(), &packet3);

		// GUILD MEMBER INFORMATIONS
		CNtlPacket packet4(sizeof(sTU_GUILD_MEMBER_INFO));
		sTU_GUILD_MEMBER_INFO * res4 = (sTU_GUILD_MEMBER_INFO *)packet4.GetPacketData();

		res4->wOpCode = TU_GUILD_MEMBER_INFO;
		res4->guildMemberInfo.bIsOnline = true;
		res4->guildMemberInfo.charId = plr->GetCharID();
		wcscpy_s(res4->guildMemberInfo.wszMemberName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("@charactername")).c_str());
		packet4.SetPacketLen(sizeof(sTU_GUILD_MEMBER_INFO));
		rc = g_pApp->Send(this->GetHandle(), &packet4);
		app->UserBroadcastothers(&packet4, this);

		plr->SetGuildName(app->db->getString("@charactername"));

	}
	plr = NULL;
	delete plr;

}

//--------------------------------------------------------------------------------------//
//		Guild Add Function
//--------------------------------------------------------------------------------------//
void CClientSession::SendGuildFunctionAddReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- create guild request --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_GUILD_FUNCTION_ADD_REQ * req = (sUG_GUILD_FUNCTION_ADD_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_GUILD_FUNCTION_ADD_RES));
	sGU_GUILD_FUNCTION_ADD_RES* res = (sGU_GUILD_FUNCTION_ADD_RES*)packet.GetPacketData();
	
	res->wOpCode = GU_GUILD_FUNCTION_ADD_RES;
	printf("guild manager id: %i ", req->hGuildManagerNpc);
	app->db->prepare("SELECT GuildFunctions from guilds where GuildMaster = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	
	int CurrentFunctions = app->db->getInt("GuildFunctions");


	
	plr = NULL;
	delete plr;

}
//--------------------------------------------------------------------------------------//
//		Create Party
//--------------------------------------------------------------------------------------//
void CClientSession::SendCreatePartyReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- create party request --- \n");

	PlayerPartyClass *t = new PlayerPartyClass;

	t->m_partyList.begin();
	t->CreateParty(this->GetHandle());

	sUG_PARTY_CREATE_REQ * req = (sUG_PARTY_CREATE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PARTY_CREATE_RES));
	sGU_PARTY_CREATE_RES * res = (sGU_PARTY_CREATE_RES *)packet.GetPacketData();

	res->wOpCode = GU_PARTY_CREATE_RES;
	res->wResultCode = GAME_SUCCESS;
	memcpy(res->wszPartyName, req->wszPartyName, sizeof(wchar_t)* NTL_MAX_SIZE_PARTY_NAME_IN_UNICODE);

	packet.SetPacketLen(sizeof(sGU_PARTY_CREATE_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);

}
//--------------------------------------------------------------------------------------//
//		Disband Party
//--------------------------------------------------------------------------------------//
void CClientSession::SendDisbandPartyReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- disband party request --- \n");

	CNtlPacket packet(sizeof(sGU_PARTY_DISBAND_RES));
	sGU_PARTY_DISBAND_RES * res = (sGU_PARTY_DISBAND_RES *)packet.GetPacketData();

	res->wOpCode = GU_PARTY_DISBAND_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_PARTY_DISBAND_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);


	CNtlPacket packet2(sizeof(sGU_PARTY_DISBANDED_NFY));
	sGU_PARTY_DISBANDED_NFY * sPacket2 = (sGU_PARTY_DISBANDED_NFY *)packet2.GetPacketData();
	sPacket2->wOpCode = GU_PARTY_DISBANDED_NFY;

	packet2.SetPacketLen(sizeof(sGU_PARTY_DISBANDED_NFY));
	rc = g_pApp->Send(this->GetHandle(), &packet2);

}
//--------------------------------------------------------------------------------------//
//		Send party invite request
//--------------------------------------------------------------------------------------//
void CClientSession::SendPartyInviteReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- Send party invite request --- \n");
	sUG_PARTY_INVITE_REQ * req = (sUG_PARTY_INVITE_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT * FROM characters WHERE OnlineID = ? AND isOnline = 1");
	app->db->setInt(1, req->hTarget);
	app->db->execute();
	app->db->fetch();
	//this->plr->LastPartyHandle = req->hTarget;
	//Invite player
	CNtlPacket packet(sizeof(sGU_PARTY_INVITE_RES));
	sGU_PARTY_INVITE_RES * res = (sGU_PARTY_INVITE_RES *)packet.GetPacketData();

	res->wOpCode = GU_PARTY_INVITE_RES;
	res->wResultCode = GAME_SUCCESS;
	wcscpy_s(res->wszTargetName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("CharName")).c_str());

	packet.SetPacketLen(sizeof(sGU_PARTY_INVITE_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);

	//Send invitation request to player
	CNtlPacket packet2(sizeof(sGU_PARTY_INVITE_NFY));
	sGU_PARTY_INVITE_NFY * res2 = (sGU_PARTY_INVITE_NFY *)packet2.GetPacketData();

	res2->wOpCode = GU_PARTY_INVITE_NFY;
	res2->bFromPc = true;
	//wcscpy_s(res2->wszInvitorPcName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, plr->GetPcProfile()->awchName);

	packet2.SetPacketLen(sizeof(sGU_PARTY_INVITE_NFY));
	app->UserBroadcastothers(&packet2, this);


	/*sUG_PARTY_RESPONSE_INVITATION * req2 = (sUG_PARTY_RESPONSE_INVITATION*)pPacket->GetPacketData();
	CNtlPacket packet3(sizeof(sGU_PARTY_RESPONSE_INVITATION_RES));
	sGU_PARTY_RESPONSE_INVITATION_RES * res3 = (sGU_PARTY_RESPONSE_INVITATION_RES *)packet3.GetPacketData();

	res3->wOpCode = GU_PARTY_RESPONSE_INVITATION_RES;
	res3->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen( sizeof(sGU_PARTY_RESPONSE_INVITATION_RES) );
	rc = g_pApp->Send( this->GetHandle(), &packet3 );

	printf("response: %i ",req2->byResponse);
	if(req2->byResponse == 2) // User accepted party invite
	{
	CNtlPacket packet4(sizeof(sGU_PARTY_MEMBER_JOINED_NFY));
	sGU_PARTY_MEMBER_JOINED_NFY * res4 = (sGU_PARTY_MEMBER_JOINED_NFY *)packet4.GetPacketData();

	res2->wOpCode = GU_PARTY_MEMBER_JOINED_NFY;
	wcscpy_s(res4->memberInfo.awchMemberName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("CharName")).c_str());

	res4->memberInfo.byClass = app->db->getInt("Class");
	res4->memberInfo.byLevel = app->db->getInt("Level");
	res4->memberInfo.byRace = app->db->getInt("Race");
	res4->memberInfo.hHandle = app->db->getInt("OnlineID");
	res4->memberInfo.vCurLoc.x = (float)app->db->getDouble("CurLocX");
	res4->memberInfo.vCurLoc.y = (float)app->db->getDouble("CurLocY");
	res4->memberInfo.vCurLoc.z = (float)app->db->getDouble("CurLocZ");
	res4->memberInfo.wCurEP = app->db->getInt("CurEP");
	res4->memberInfo.wCurLP = app->db->getInt("CurLP");
	res4->memberInfo.wMaxEP = app->db->getInt("BaseMaxEp");
	res4->memberInfo.wMaxLP = app->db->getInt("BaseMaxLp");
	res4->memberInfo.worldId = app->db->getInt("WorldID");
	res4->memberInfo.dwZenny = app->db->getInt("Money");
	res4->memberInfo.worldTblidx = app->db->getInt("WorldTable");

	packet4.SetPacketLen( sizeof(sGU_PARTY_MEMBER_JOINED_NFY));
	rc = g_pApp->Send( this->GetHandle(), &packet );
	app->UserBroadcastothers(&packet4, this);
	//printf("user invited ");
	}*/
}
//--------------------------------------------------------------------------------------//
//		Party invitation response
//--------------------------------------------------------------------------------------//
void CClientSession::SendPartyResponse(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- Party invitation response --- \n");

	sUG_PARTY_RESPONSE_INVITATION * req2 = (sUG_PARTY_RESPONSE_INVITATION*)pPacket->GetPacketData();

	CNtlPacket packet3(sizeof(sGU_PARTY_RESPONSE_INVITATION_RES));
	sGU_PARTY_RESPONSE_INVITATION_RES * res3 = (sGU_PARTY_RESPONSE_INVITATION_RES *)packet3.GetPacketData();
	res3->wOpCode = GU_PARTY_RESPONSE_INVITATION_RES;
	res3->wResultCode = GAME_FAIL;

	/*if (req2->byResponse == 0 && this->plr->LastPartyHandle != -1) // User accepted party invite
	{
	CNtlPacket packet4(sizeof(sGU_PARTY_MEMBER_JOINED_NFY));
	sGU_PARTY_MEMBER_JOINED_NFY * res4 = (sGU_PARTY_MEMBER_JOINED_NFY *)packet4.GetPacketData();
	res4->wOpCode = GU_PARTY_MEMBER_JOINED_NFY;

	app->db->prepare("SELECT * FROM characters WHERE OnlineID = ? AND isOnline = 1");
	app->db->setInt(1, this->plr->LastPartyHandle);
	app->db->execute();
	app->db->fetch();

	wcscpy_s(res4->memberInfo.awchMemberName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("CharName")).c_str());
	printf("1");
	res4->memberInfo.byClass = app->db->getInt("Class");
	res4->memberInfo.byLevel = app->db->getInt("Level");
	res4->memberInfo.byRace = app->db->getInt("Race");
	res4->memberInfo.hHandle = app->db->getInt("OnlineID");
	res4->memberInfo.vCurLoc.x = (float)app->db->getDouble("CurLocX");
	res4->memberInfo.vCurLoc.y = (float)app->db->getDouble("CurLocY");
	res4->memberInfo.vCurLoc.z = (float)app->db->getDouble("CurLocZ");
	res4->memberInfo.wCurEP = app->db->getInt("CurEP");
	res4->memberInfo.wCurLP = app->db->getInt("CurLP");
	res4->memberInfo.wMaxEP = app->db->getInt("BaseMaxEp");
	res4->memberInfo.wMaxLP = app->db->getInt("BaseMaxLp");
	res4->memberInfo.worldId = app->db->getInt("WorldID");
	res4->memberInfo.dwZenny = app->db->getInt("Money");
	res4->memberInfo.worldTblidx = app->db->getInt("WorldTable");

	packet4.SetPacketLen(sizeof(sGU_PARTY_MEMBER_JOINED_NFY));
	g_pApp->Send(this->GetHandle(), &packet4);
	app->UserBroadcastothers(&packet4, this);
	res3->wResultCode = GAME_SUCCESS;
	}
	packet3.SetPacketLen(sizeof(sGU_PARTY_RESPONSE_INVITATION_RES));
	g_pApp->Send(this->GetHandle(), &packet3);
	this->plr->LastPartyHandle = -1;*/
}
//--------------------------------------------------------------------------------------//
//		Leave Party
//--------------------------------------------------------------------------------------//
void CClientSession::SendPartyLeaveReq(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- leave party request --- \n");

	CNtlPacket packet(sizeof(sGU_PARTY_LEAVE_RES));
	sGU_PARTY_LEAVE_RES * res = (sGU_PARTY_LEAVE_RES *)packet.GetPacketData();

	res->wOpCode = GU_PARTY_LEAVE_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_PARTY_DISBAND_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);


	CNtlPacket packet2(sizeof(sGU_PARTY_MEMBER_LEFT_NFY));
	sGU_PARTY_MEMBER_LEFT_NFY * sPacket2 = (sGU_PARTY_MEMBER_LEFT_NFY *)packet2.GetPacketData();
	sPacket2->wOpCode = GU_PARTY_MEMBER_LEFT_NFY;
	//sPacket2->hMember = GET PARTY-MEMBER ID


	packet2.SetPacketLen(sizeof(sGU_PARTY_MEMBER_LEFT_NFY));
	rc = g_pApp->Send(this->GetHandle(), &packet2);

	app->UserBroadcastothers(&packet2, this);
}

//--------------------------------------------------------------------------------------//
//		Execute trigger object
//--------------------------------------------------------------------------------------//
void CClientSession::SendExcuteTriggerObject(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- SendExcuteTriggerObject --- \n");

	sUG_TS_EXCUTE_TRIGGER_OBJECT * req = (sUG_TS_EXCUTE_TRIGGER_OBJECT*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_TS_EXCUTE_TRIGGER_OBJECT_RES));
	sGU_TS_EXCUTE_TRIGGER_OBJECT_RES * res = (sGU_TS_EXCUTE_TRIGGER_OBJECT_RES *)packet.GetPacketData();

	res->wOpCode = GU_TS_EXCUTE_TRIGGER_OBJECT_RES;
	res->wResultCode = GAME_SUCCESS;
	res->hTriggerObject = req->hTarget;

	printf("SOURCE: %i TARGET: %i EVTGENTYPE: %i ", req->hSource, req->hTarget, req->byEvtGenType);

	packet.SetPacketLen(sizeof(sGU_TS_EXCUTE_TRIGGER_OBJECT_RES));
	app->UserBroadcastothers(&packet, this);
	int rc = g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		Character bind to world
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharBindReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- UG_CHAR_BIND_REQ --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_CHAR_BIND_REQ * req = (sUG_CHAR_BIND_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_BIND_RES));
	sGU_CHAR_BIND_RES * res = (sGU_CHAR_BIND_RES *)packet.GetPacketData();

	app->db->prepare("CALL CharBind (?,?, @currentWorldID)");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bindObjectTblidx);
	app->db->execute();
	app->db->execute("SELECT @currentWorldID");
	app->db->fetch();
//	sOBJECT_TBLDAT* objMap = reinterpret_cast<sOBJECT_TBLDAT*>(app->g_pTableContainer->GetObjectTable(plr->GetWorldID())->FindData(req->bindObjectTblidx));
	res->wOpCode = GU_CHAR_BIND_RES;
	res->wResultCode = GAME_SUCCESS;
	res->byBindType = DBO_BIND_TYPE_FIRST;
	res->bindObjectTblidx = req->bindObjectTblidx;

	res->bindWorldId = plr->GetWorldID();

	packet.SetPacketLen(sizeof(sGU_CHAR_BIND_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		PORTAL START REQUEST
//--------------------------------------------------------------------------------------//
void CClientSession::SendPortalStartReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- PORTAL START REQUEST --- \n");
	sUG_PORTAL_START_REQ * req = (sUG_PORTAL_START_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PORTAL_START_RES));
	sGU_PORTAL_START_RES * res = (sGU_PORTAL_START_RES *)packet.GetPacketData();

	res->wOpCode = GU_PORTAL_START_RES;
	res->wResultCode = GAME_SUCCESS;
	res->hNpcHandle = req->handle;

	CNtlPacket packet2(sizeof(sGU_PORTAL_INFO));
	sGU_PORTAL_INFO * res2 = (sGU_PORTAL_INFO *)packet2.GetPacketData();

	CPortalTable* pPortalTbl = app->g_pTableContainer->GetPortalTable();
	int i = 0;
	for (CTable::TABLEIT itPortal = pPortalTbl->Begin(); itPortal != pPortalTbl->End(); ++itPortal)
	{
		sPORTAL_TBLDAT* pPortalTblData = (sPORTAL_TBLDAT*)itPortal->second;
		res2->aPortalID[i] = pPortalTblData->tblidx;
		res2->byCount = i;
		i++;
	}
	res2->wOpCode = GU_PORTAL_INFO;


	packet2.SetPacketLen(sizeof(sGU_PORTAL_INFO));
	int rc = g_pApp->Send(this->GetHandle(), &packet2);
	Sleep(1);
	packet.SetPacketLen(sizeof(sGU_PORTAL_START_RES));
	rc = g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		PORTAL ADD REQUEST
//--------------------------------------------------------------------------------------//
void CClientSession::SendPortalAddReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("PORTAL ADD REQUEST");
	sUG_PORTAL_ADD_REQ* req = (sUG_PORTAL_ADD_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PORTAL_ADD_RES));
	sGU_PORTAL_ADD_RES * res = (sGU_PORTAL_ADD_RES *)packet.GetPacketData();
	CWayPointTable* pos;
	res->wOpCode = GU_PORTAL_ADD_RES;
	res->wResultCode = GAME_SUCCESS;
	res->hNpcHandle = req->handle;
	res->PortalID = 255;

	packet.SetPacketLen(sizeof(sGU_PORTAL_ADD_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendPortalTelReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_PORTAL_REQ* req = (sUG_PORTAL_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PORTAL_RES));
	sGU_PORTAL_RES * res = (sGU_PORTAL_RES *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_CHAR_TELEPORT_RES));
	sGU_CHAR_TELEPORT_RES * res2 = (sGU_CHAR_TELEPORT_RES *)packet2.GetPacketData();
		
	CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res3 = (sGU_UPDATE_CHAR_STATE *)packet3.GetPacketData();

	packet.SetPacketLen(sizeof(sGU_PORTAL_RES));
	packet2.SetPacketLen(sizeof(sGU_CHAR_TELEPORT_RES));
	packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	int iMyPortalID = req->byPoint;
	iMyPortalID += 1;
	CPortalTable* myPortalTbl = app->g_pTableContainer->GetPortalTable();
	sPORTAL_TBLDAT* pPortalTblData = reinterpret_cast<sPORTAL_TBLDAT*>(myPortalTbl->FindData(iMyPortalID));
	if (pPortalTblData)
	{
		if (iMyPortalID == pPortalTblData->tblidx)
		{
			//pPortalTblData->adwPointZenny;
			//pPortalTblData->dwPointName;
			//pPortalTblData->szPointNameText;
			//pPortalTblData->vMap;
			res->vDir.x = pPortalTblData->vDir.x;
			res->vDir.y = pPortalTblData->vDir.y;
			res->vDir.z = pPortalTblData->vDir.z;
			res->vLoc.x = pPortalTblData->vLoc.x;
			res->vLoc.y = pPortalTblData->vLoc.y;
			res->vLoc.z = pPortalTblData->vLoc.z;
			res->byPoint = pPortalTblData->tblidx;
			res->worldID = pPortalTblData->worldId;
			res->wOpCode = GU_PORTAL_RES;
			res->wResultCode = GAME_SUCCESS;
			res->hNpcHandle = req->handle;//This need be our npc

			res2->bIsToMoveAnotherServer = false;
			//res2->sWorldInfo.sRuleInfo.byRuleType = GAMERULE_NORMAL;
			//res2->sWorldInfo.hTriggerObjectOffset = 100000;
			//res2->sWorldInfo.tblidx = this->plr->GetWorldTableID();
			//res2->sWorldInfo.worldID = res->worldID;
			res2->vNewDir.x = res->vDir.x;
			res2->vNewDir.y = res->vDir.y;
			res2->vNewDir.z = res->vDir.z;
			res2->vNewLoc.x = res->vLoc.x;
			res2->vNewLoc.y = res->vLoc.y;
			res2->vNewLoc.z = res->vLoc.z;
			res2->wOpCode = GU_CHAR_TELEPORT_RES;
			res2->wResultCode = GAME_SUCCESS;
			//res2->sWorldInfo.sRuleInfo.sTimeQuestRuleInfo;

			plr->SetPlayerLastPosition(plr->GetPlayerPosition());
			plr->SetPlayerLastDirection(plr->GetPlayerDirection());
			plr->SetPlayerDirection(res2->vNewDir);
			plr->SetPlayerPosition(res2->vNewLoc);
			app->db->prepare("UPDATE characters SET CurLocX=? , CurLocY=? , CurLocZ=? , CurDirX=? , CurDirZ=? WHERE CharID = ?");
			app->db->setInt(1, res->vLoc.x);
			app->db->setInt(2, res->vLoc.y);
			app->db->setInt(3, res->vLoc.z);
			app->db->setInt(4, res->vDir.x);
			app->db->setInt(5, res->vDir.z);
			app->db->setInt(6, plr->GetCharID());
			app->db->execute();

			g_pApp->Send(this->GetHandle(), &packet);
			g_pApp->Send(this->GetHandle(), &packet2);
		}
		else
		{
			res->wOpCode = GU_PORTAL_RES;
			res->wResultCode = GAME_PORTAL_NOT_EXIST;
			g_pApp->Send(this->GetHandle(), &packet);
			this->gsf->printError("An error is occured in SendPortalTelReq: GAME_PORTAL_NOT_EXIST");
		}
	}
	else
	{
		res->wOpCode = GU_PORTAL_RES;
		res->wResultCode = GAME_PORTAL_NOT_EXIST;
		g_pApp->Send(this->GetHandle(), &packet);
	}
	plr = NULL;
	delete plr;
}

//--------------------------------------------------------------------------------------//
//		ATTACK BEGIN
//--------------------------------------------------------------------------------------//
void CClientSession::SendAttackBegin(CNtlPacket * pPacket, CGameServer * app)
{

	sUG_CHAR_ATTACK_BEGIN* req = (sUG_CHAR_ATTACK_BEGIN *)pPacket->GetPacketData();
	
	CNtlPacket packet(sizeof(sGU_CHAR_FIGHTMODE));
	sGU_CHAR_FIGHTMODE * fightMode = (sGU_CHAR_FIGHTMODE *)packet.GetPacketData();

	fightMode->wOpCode = GU_CHAR_FIGHTMODE;
	fightMode->bFightMode = true;
	g_pApp->Send(this->GetHandle(), &packet);
	//this->gsf->printError("An error is occured in SendPortalTelReq: GAME_PORTAL_NOT_EXIST");

	CNtlPacket packet2(sizeof(sGU_CHAR_IS_BATTLECOMBATING));
	sGU_CHAR_IS_BATTLECOMBATING* battleCombat = (sGU_CHAR_IS_BATTLECOMBATING*)packet2.GetPacketData();
	battleCombat->wOpCode = GU_CHAR_FIGHTMODE;
	battleCombat->Unknown[0] = 1;
	battleCombat->Unknown[1] = 0;
	battleCombat->Unknown[2] = 0;
	battleCombat->Unknown[3] = 0;
	g_pApp->Send(this->GetHandle(), &packet2);

	printf("--- ATTACK BEGIN --- \n");
	this->cPlayersMain->SetPlayerFight(true);
	this->cPlayersMain->SetPlayerSit(false);
	UpdateCharState(this->GetavatarHandle(), CHARSTATE_STANDING);
	if (req->byType == 0)
	{
		if (app->IsUser(this->GetTargetSerialId()))
		{
			pServer->m_batle_mutex.Lock();
			app->AddAttackBegin(this->GetavatarHandle(), this->GetTargetSerialId(), true, true);
			pServer->m_batle_mutex.Unlock();
		}
		else
		{
			CMonster::MonsterData*data = g_pMobManager->GetMobByHandle(this->GetTargetSerialId());
			if (data)
			{
				//if (data->MonsterData->curPos.x < 140 && data->indexx>0 && data->indexz < 120 && data->indexz > 0)
				//{
					pServer->m_batle_mutex.Lock();
					printf("mob %u idx %i %i\n", this->GetTargetSerialId(), data->curPos.x, data->curPos.z);
					app->AddAttackBegin(this->GetavatarHandle(), this->GetTargetSerialId(), true, false, data->curPos.x, data->curPos.z);
					pServer->m_batle_mutex.Unlock();
				//}
				//else
					//printf("mob %u idx %i %i\n", this->GetTargetSerialId(), data->indexx, data->indexz);
			}
		}
	}
	else if (req->byType == 1)
	{
		printf("ATTACK FOR TYPE 1 NOT EXIST \n");
	}
	else if (req->byType == 2)
	{
		//pServer->m_batle_mutex.Lock();
		//app->AddAttackBegin(this->GetTargetSerialId(), this->GetavatarHandle(), );
		//pServer->m_batle_mutex.Unlock();
	}
}
//--------------------------------------------------------------------------------------//
//		ATTACK END
//---------------------------------------------------------------------------------------//
void CClientSession::SendAttackEnd(CNtlPacket * pPacket, CGameServer * app)
{
	//printf("--- ATTACK END --- \n");
	this->cPlayersMain->SetPlayerFight(false);
	sUG_CHAR_ATTACK_END* req = (sUG_CHAR_ATTACK_END *)pPacket->GetPacketData();
	if (req->byType == 0)
	{
		//app->m_game_mutex.Lock();
		app->RemoveAttackBegin(this->GetavatarHandle(), this->GetTargetSerialId());
		//app->m_game_mutex.Unlock();
	}
	else if (req->byType == 1)
	{
		//CNtlSobAvatar *pSobAvatar = GetNtlSLGlobal()->GetSobAvatar();
		//CNtlPetBuffer *pPetBuffer = pSobAvatar->GetPetBuffer();
		//RemoveAttackBegin(pPetBuffer->GetSerialId(0));
		this->gsf->printError("An error is occured in SendAttackEnd: req->byType == 1");
	}
}

void CGameServer::RemoveAttackBegin(RwUInt32 uiSerialId, RwUInt32 m_uiTargetSerialId)
{
	SBattleData *pBattleData;
	m_batle_mutex.Lock();
	for (BATTLEIT it = m_listAttackBegin.begin(); it != m_listAttackBegin.end(); it++)
	{
		pBattleData = (*it);
		if (pBattleData){
			if (pBattleData->uiSerialId == uiSerialId)
			{
					printf("RemoveAttackBegin SERIAL %u %u \n", uiSerialId,m_uiTargetSerialId);

				pBattleData->bAttackMode = false;
				pBattleData->uiSerialId = 0;
				m_batle_mutex.Unlock();
				return;
			}
		}
	}
	m_batle_mutex.Unlock();
}


void CGameServer::AddAttackBegin(RwUInt32 uiSerialId, RwUInt32 m_uiTargetSerialId, bool bIsplayer, bool bIsplayer2, float indexx, float indexz)
{
	SBattleData *pOldBattleData;
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->pSession->GetavatarHandle());

	//m_batle_mutex.Lock();
	for (BATTLEIT it = m_listAttackBegin.begin(); it != m_listAttackBegin.end(); it++)
	{
		pOldBattleData = (*it);
		if (pOldBattleData == 0)
		{
		}
		else
			if (pOldBattleData->uiSerialId == 0)
			{
				pOldBattleData->uiSerialId = uiSerialId;
				pOldBattleData->m_uiTargetSerialId = m_uiTargetSerialId;
				pOldBattleData->bAttackMode = true;
				pOldBattleData->bAttackerIsPlayer = bIsplayer;
				pOldBattleData->bDefenderIsPlayer = bIsplayer2;
				pOldBattleData->index_x = indexx;
				pOldBattleData->index_z = indexz;
				pOldBattleData->dwCurrTime = GetTickCount() - MONSTER_ATTACK_UPDATE_TICK;
				//	printf("AddAttackBegin SERIAL old %lu %lu \n", uiSerialId,m_uiTargetSerialId);
				//	m_batle_mutex.Unlock();
				return;
			}
	}
	SBattleData *pBattleData = new SBattleData;
		printf("AddAttackBegin SERIAL %lu %lu \n", uiSerialId,m_uiTargetSerialId);

	pBattleData->uiSerialId = uiSerialId;
	pBattleData->m_uiTargetSerialId = m_uiTargetSerialId;
	pBattleData->bAttackMode = true;
	pBattleData->bAttackerIsPlayer = bIsplayer;
	pBattleData->bDefenderIsPlayer = bIsplayer2;
	pBattleData->index_x = indexx;
	pBattleData->index_z = indexz;
	pBattleData->dwCurrTime = GetTickCount();
	if (pBattleData->bAttackerIsPlayer)
	{
		if (plr->GetLastFightTime() > GetTickCount())
		{
			printf("too soon to fight again");
			return;
		}
		plr->SetLastTimeFight(GetTickCount() + 1000);/*it0->second->plr->pcProfile->avatarAttribute.wBaseAttackSpeedRate*/;
	}

	m_listAttackBegin.push_back(pBattleData);
	//m_batle_mutex.Unlock();
}
//void CGameServer::SendCharActionAttack(SBattleData *pBattleData)
//{
//	static RwUInt8 byChainAttack = 1;
//	RwBool bDamageApply = true;
//	//m_game_mutex.Lock();
//	itterType it, it1;
//	CMonster::MonsterData* it2;
//	DWORD CurHP = 42;
//	//If we Find the Target as Mob or if the Attacker is Mob
//	if (g_pMobManager->FindCreature(m_uiTargetSerialId))
//	{
//		it2 = g_pMobManager->GetMobByHandle(m_uiTargetSerialId);
//		if (it2 != NULL)
//		{
//			CurHP = (RwUInt32)it2->CurLP;
//			//printf("First HP %d\n LP %d\n", CurHP, it2->CurLP);
//		}
//	}
//
//	if (pBattleData->bAttackerIsPlayer)
//	{
//		for (it = g_pPlayerManager->m_map_Player.begin(); it != g_pPlayerManager->m_map_Player.end(); it++)
//		{
//			if (it->second->GetAvatarHandle() == pBattleData->uiSerialId)
//				break;
//		}
//		if (it == g_pPlayerManager->m_map_Player.end())
//		{
//			//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//			pBattleData->uiSerialId = 0;
//			return;
//		}
//		if (it->second->GetPlayerFight() == false)return;
//		if (pBattleData->bDefenderIsPlayer)
//		{
//			for (it1 = g_pPlayerManager->m_map_Player.begin(); it1 != g_pPlayerManager->m_map_Player.end(); it1++)
//			{
//				if (it1->second->GetAvatarHandle() == pBattleData->m_uiTargetSerialId)
//					break;
//			}
//			if (it1 == g_pPlayerManager->m_map_Player.end())
//			{
//				//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//				pBattleData->uiSerialId = 0;
//				return;
//			}
//		}
//		else
//		{
//			//it2 = m_monsterlist[pBattleData->index_x][pBattleData->index_z].find(pBattleData->m_uiTargetSerialId);
//			//if (it2 == m_monsterlist[pBattleData->index_x][pBattleData->index_z].end())
//			//{
//			//	pBattleData->uiSerialId = 0;
//			//	//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//			//	return;
//			//}
//			//if (it2->second->wCurHP == 0)
//			//{
//			//	//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//			//	pBattleData->uiSerialId = 0;
//			//	return;
//			//}
//			///*if(it2->second->bIsFighting == false)
//			//{
//			//AddAttackBegin(pBattleData->m_uiTargetSerialId, pBattleData->uiSerialId, pBattleData->bDefenderIsPlayer, pBattleData->bAttackerIsPlayer);
//			//it2->second->bIsFighting = true;
//			//}*/
//		}
//	}
//	else
//	{
//		for (it = g_pPlayerManager->m_map_Player.begin(); it != g_pPlayerManager->m_map_Player.end(); it++)
//		{
//			if (it->second->GetAvatarHandle() == pBattleData->m_uiTargetSerialId)
//			{
//				if (it->second->GetPcProfile()->dwCurLP == 0)
//				{
//				//	RemoveAttackBegin(pBattleData->uiSerialId, 0);
//					pBattleData->uiSerialId = 0;
//					return;
//				}
//				break;
//			}
//		}
//		if (it == g_pPlayerManager->m_map_Player.end())
//		{
//			//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//			pBattleData->uiSerialId = 0;
//			return;
//		}
//		//it2 = m_monsterlist[pBattleData->index_x][pBattleData->index_z].find(pBattleData->uiSerialId);
//		//if (it2 == m_monsterlist[pBattleData->index_x][pBattleData->index_z].end())
//		//{
//		//	printf("mob disapeared from list??\n");
//		//	//RemoveAttackBegin(pBattleData->uiSerialId, 0);
//		//	pBattleData->uiSerialId = 0;
//		//	return;
//		//}	
//		if (it2->CurLP == 0 ) //|| it2->bIsFighting == false)
//		{
//
//			pBattleData->uiSerialId = 0;
//			return;
//		}
//		float distance = GetDistance(it2->curPos, it->second->GetPlayerLastPosition());
//		//if (distance > 2.0f)
//		//{
//		//	SendMobFollowMove(it2->first, it0->second->GetavatarHandle(), 1, 1/*NTL_FOLLOW_FIGHTING*/);
//		//	it2->second->folowID = it0->second->GetavatarHandle();
//		//	it2->second->vDest_Loc = it0->second->plr->GetPosition();
//		//	it2->second->dwLastMoveTime = GetTickCount();
//		//	return;
//		//}
//		//else 
//		//if (distance > 30.0f)
//		//{
//		//	//it2->second->folowID = 0;
//		//	pBattleData->uiSerialId = 0;
//		//	return;
//		//}
//		//if (distance > 2.0f)return;
//	}
//	//if(it0->second->isfighting == false)return;
//	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
//	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();
//	res->wOpCode = GU_CHAR_ACTION_ATTACK;
//	res->hSubject = pBattleData->uiSerialId;
//	res->hTarget = pBattleData->m_uiTargetSerialId;
//	res->bChainAttack = true;//pBattleData->bAttackerIsPlayer;
//
//	res->dwLpEpEventId = 999;
//	res->fReflectedDamage = 0;
//	res->byBlockedAction = DBO_GUARD_TYPE_INVALID;
//
//	//	res->wAttackResultValue = m_iCurrentHp;
//	for (int i = 0; i < 9; i++)	res->unknown[i] = 0;
//
//	res->vShift.x = 0;
//	res->vShift.y = 0;
//	res->vShift.z = 0;
//	if(res->bChainAttack)
//	res->byAttackSequence = byChainAttack%6+NTL_BATTLE_CHAIN_ATTACK_START;
//	else
//	res->byAttackSequence = rand()%2;
//	if (pBattleData->bAttackerIsPlayer)
//	{
//		if (it->second->GetLastFightTime() > GetTickCount())
//		{
//			printf("too soon to fight again");
//			return;
//		}
//		it->second->SetLastTimeFight(GetTickCount() + 1000);/*it0->second->plr->pcProfile->avatarAttribute.wBaseAttackSpeedRate*/;
//		res->wAttackResultValue = it->second->cPlayerAttribute->GetAvatarAttribute().byLastStr*2;//100;
//	}
//	//else
//	//{
//	//	if (it2->second->dwLastAttackTime > GetTickCount())return;
//	//	it2->second->dwLastAttackTime = GetTickCount() + it2->second->staticData->wAttackCoolTime;
//	//	res->wAttackResultValue = it2->second->staticData->byStr;//10;
//	//}
//
//	if (res->byAttackSequence == 6)
//	{
//		if (rand() % 2)
//			res->byAttackResult = BATTLE_ATTACK_RESULT_KNOCKDOWN;
//		else
//			res->byAttackResult = BATTLE_ATTACK_RESULT_SLIDING;
//	}
//	else
//	{
//		RwInt32 iRandValue = rand() % 5;
//		if (iRandValue <= 2)
//			res->byAttackResult = BATTLE_ATTACK_RESULT_HIT;
//		else if (iRandValue == 3)
//		{
//			bDamageApply = false;
//			res->wAttackResultValue = 0;
//			res->byAttackResult = BATTLE_ATTACK_RESULT_DODGE;
//		}
//		else
//		{
//			bDamageApply = false;
//			res->wAttackResultValue = 0;
//			res->byAttackResult = BATTLE_ATTACK_RESULT_BLOCK;
//		}
//	}
//
//	packet.SetPacketLen(sizeof(sGU_CHAR_ACTION_ATTACK));
//
//	if (pBattleData->bDefenderIsPlayer)
//	{	
//		UserBroadcastothers(&packet, pSession);
//		//UserBroadcastothersVisible(&packet, pBattleData->m_uiTargetSerialId);
//	}
//	else
//	{
//		Send(it->second->myCCSession->GetHandle(), &packet);
//		byChainAttack++;
//		UserBroadcast(&packet);
//		//MOBBroadcastothersVisible(&packet, pBattleData->m_uiTargetSerialId);
//	}
//	// update LP
//	if (bDamageApply)
//	{
//		if (pBattleData->bAttackerIsPlayer)
//		{
//			if (pBattleData->bDefenderIsPlayer)
//			{
//				//int healthp = it1->second->plr->pcProfile->wCurLP - res->wAttackResultValue;
//				//if (healthp < 1)healthp = 1;
//				//it1->second->plr->pcProfile->wCurLP = healthp;
//				//UpdateLP(pBattleData->m_uiTargetSerialId, healthp, it1->second->plr->pcProfile->avatarAttribute.wLastMaxLP, 0, 999);
//				//if (healthp <= 1)
//				//{
//				//	it0->second->isfighting = false;
//				//	it1->second->isfighting = false;
//				//	it1->second->SendCharUpdateState(CHARSTATE_CAMPING);
//
//				//	CNtlPacket packet(sizeof(sGU_FREEBATTLE_END_NFY));
//				//	sGU_FREEBATTLE_END_NFY * sPacket = (sGU_FREEBATTLE_END_NFY *)packet.GetPacketData();
//				//	sPacket->wOpCode = GU_FREEBATTLE_END_NFY;
//				//	sPacket->byFreeBattleResult = FREEBATTLE_RESULT_WIN;
//				//	packet.SetPacketLen(sizeof(sGU_FREEBATTLE_END_NFY));
//				//	Send(it0->second->GetHandle(), &packet);
//				//	sPacket->byFreeBattleResult = FREEBATTLE_RESULT_LOSE;
//				//	Send(it1->second->GetHandle(), &packet);
//				//	//	printf("endpvp %lu %lu\n",it0->second->GetavatarHandle(),it1->second->GetavatarHandle());
//				//	it0->second->SetPVPTarget(0);
//				//	it1->second->SetPVPTarget(0);
//				//	//RemoveAttackBegin(it1->second->GetavatarHandle(), 0);
//				//	//RemoveAttackBegin(it0->second->GetavatarHandle(), 0);
//				//	pBattleData->uiSerialId = 0;
//					return;
//				//}
//			}
//			else
//			{
//				DWORD healthp;
//				
//				if ((healthp = (it2->CurLP - res->wAttackResultValue) <= 0))
//					healthp = 0;
//
//				it2->CurLP = healthp;
//				pSession->SendCharUpdateLp(pPacket, this, healthp, pBattleData->m_uiTargetSerialId);
//					printf("monster battle lp %u\n", it2->CurLP);
//				if (healthp == 0 || it2->CurLP == 0)
//				{
//					printf("removing target %u\n",pBattleData->m_uiTargetSerialId);
//					//printf("removing %u\n",it2->first);
//					//printf("removing %u\n",it0->second->GetavatarHandle());
//					//printf("Spawngroup 0x%X party 0x%X\n", it2->second->spawnlist->spawnGroupId, it2->second->spawnlist->dwParty_Index);
//					g_pMobManager->UpdateDeathStatus(pBattleData->m_uiTargetSerialId, true);
//					//RemoveAttackBegin(it2->first, 0);
//					RemoveAttackBegin(it->second->GetAvatarHandle(), 0);
//					pBattleData->uiSerialId = 0;
//					it2->IsDead = true;
//					//it2->second->dwReviveTime = it2->second->dwDieTime + it2->second->spawnlist->wSpawn_Cool_Time * 10000 + 10000;
//					//it2->second->bIsFighting = false;
//					//SendExpUpdate(it2->second->staticData->wExp, it0->second);
//				}
//				//else
//					/*if (it2->second->bIsFighting == false)
//					{
//					it2->second->bIsFighting = true;
//					return AddAttackBegin(pBattleData->m_uiTargetSerialId, pBattleData->uiSerialId, pBattleData->bDefenderIsPlayer, pBattleData->bAttackerIsPlayer, pBattleData->index_x, pBattleData->index_z);
//					}*/
//			}
//		}
//		//else
//		//{
//		//	DWORD healthp = it->second->GetPcProfile()->dwCurLP - res->wAttackResultValue;
//		//	if (healthp < 0)healthp = 0;
//		//	it0->second->plr->pcProfile->wCurLP = healthp;
//		//	UpdateLP(pBattleData->m_uiTargetSerialId, healthp, it0->second->plr->pcProfile->avatarAttribute.wLastMaxLP, 0, 999);
//		//	if (healthp == 0)
//		//	{
//		//		it0->second->isfighting = false;
//		//		pBattleData->bAttackMode = false;
//		//		pBattleData->uiSerialId = 0;
//		//		pBattleData->m_uiTargetSerialId = 0;
//		//		it0->second->SendCharUpdateState(CHARSTATE_FAINTING);
//		//		//	RemoveAttackBegin(it2->first, 0);
//		//		//	RemoveAttackBegin(it0->second->GetavatarHandle(), 0);
//		//	}
//		//}
//	}
//	//m_game_mutex.Unlock();
//}


void CGameServer::SendCharActionAttack(SBattleData *pBattleData)
{

	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->pSession->GetavatarHandle());
	static RwUInt8 byChainAttack = 1;
	DWORD CurHP = 0;
	RwBool bDamageApply = true;
	float formula;
	plr->SetPlayerFight(true);

	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res2 = (sGU_UPDATE_CHAR_STATE *)packet2.GetPacketData();

	res->wOpCode = GU_CHAR_ACTION_ATTACK;
	res->hSubject = pBattleData->uiSerialId;
	res->hTarget = pBattleData->m_uiTargetSerialId;
	res->dwLpEpEventId = 255;
	res->byBlockedAction = 255;
	CMonster::MonsterData *lol = NULL;

	g_pMobManager->FindCreature(m_uiTargetSerialId);
	lol = g_pMobManager->GetMobByHandle(m_uiTargetSerialId);
	CurHP = lol->CurLP;
	printf("Mob HP %d of %d\n", CurHP, lol->MaxLP);

	formula = rand() % (plr->GetPcProfile()->avatarAttribute.byLastStr * 3) % (plr->GetPcProfile()->avatarAttribute.byLastStr * 6); //Fixed the formula for now. Player stat's have to be read correctly.

	res->wAttackResultValue = formula;
	res->fReflectedDamage = 0.0;
	res->vShift.x = 100;
	res->vShift.y = 0;
	res->vShift.z = 0;

	res->byAttackSequence = rand() % NTL_BATTLE_MAX_CHAIN_ATTACK_COUNT + NTL_BATTLE_CHAIN_ATTACK_START; //byChainAttack % NTL_BATTLE_MAX_CHAIN_ATTACK_COUNT + NTL_BATTLE_CHAIN_ATTACK_START
	printf("AttackSequence is %u\n", byChainAttack);
	res->bChainAttack = true;

	if (bDamageApply == true){
		CurHP -= res->wAttackResultValue;
		g_pMobManager->GetMobByHandle(m_uiTargetSerialId)->CurLP = CurHP;
		g_pMobManager->GetMobByHandle(m_uiTargetSerialId)->target = pBattleData->uiSerialId;
		pSession->SendCharUpdateLp(pPacket, app, lol->CurLP, pBattleData->m_uiTargetSerialId);
		lol->isAggro = true;
	}

	//this is to make sure the mob died. This can be written better so see if anyone can update this statement!
	if (lol->CurLP < (lol->MaxLP * 0.20))
	{
		lol->CurLP = 0;
		pSession->SendCharUpdateLp(pPacket, app, lol->CurLP, pBattleData->m_uiTargetSerialId);
		printf("HP IS 0!\n");
		bDamageApply = false;
		lol->IsDead = true;
		lol->isAggro = false;
		if (lol->CurLP != 0 && lol->CurLP < (lol->MaxLP * 0.10))
		{
			lol->CurLP = 0;
			lol->IsDead = true;
		}
		
	}

	//to get the mob to react to you attacking it and it attacks back.
	if (lol->isAggro == true && lol->IsDead != true) //lol->IsDead != true && lol->CurLP < lol->MaxLP
	{
		lol->FightMode = true;
		lol->Attack_rate = 1;
		lol->AttackSpeedRate = 1;
		lol->Attack_speed_rate = lol->AttackSpeedRate;
		res->wOpCode = GU_CHAR_ACTION_ATTACK;
		res->hSubject = pBattleData->m_uiTargetSerialId;
		res->hTarget = pBattleData->uiSerialId;
		res->dwLpEpEventId = 255;
		res->byBlockedAction = 255;

		float mob_formula;
		mob_formula = rand() % (lol->Level * 2) + (lol->Level * 4); //hardcoded at 10 for now.

		res->wAttackResultValue = mob_formula;
		res->fReflectedDamage = 0;
		res->byAttackResult = BATTLE_ATTACK_RESULT_HIT;
		res->vShift = plr->GetPlayerPosition();

		res->bChainAttack = 0;

		lol->chainAttackCount = 0;

		plr->SetPlayerFight(true);
		plr->GetPcProfile()->dwCurLP -= res->wAttackResultValue;

		packet.SetPacketLen(sizeof(sGU_CHAR_ACTION_ATTACK));
		g_pApp->Send(this->pSession->GetHandle(), &packet);
		app->UserBroadcast(&packet);
		
	}

	/*if (lol->isAggro == true && lol->IsDead != true && lol->FightMode == true && lol->curPos.x != plr->GetPlayerPosition().x && lol->curPos.z != plr->GetPlayerPosition().z)
	{
		res2->wOpCode = GU_UPDATE_CHAR_STATE;
		res2->handle = pBattleData->m_uiTargetSerialId;
		res2->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
		res2->sCharState.sCharStateBase.dwConditionFlag = CHARSTATE_FLAG_DESTMOVE;
		res2->sCharState.sCharStateBase.vCurDir.x = plr->GetPlayerDirection().x;
		res2->sCharState.sCharStateBase.vCurDir.y = plr->GetPlayerDirection().y;
		res2->sCharState.sCharStateBase.vCurDir.z = plr->GetPlayerDirection().z;
		res2->sCharState.sCharStateBase.vCurLoc.x = plr->GetPlayerPosition().x - 1;
		res2->sCharState.sCharStateBase.vCurLoc.y = plr->GetPlayerPosition().y;
		res2->sCharState.sCharStateBase.vCurLoc.z = plr->GetPlayerPosition().z - 1;
		res2->sCharState.sCharStateDetail.sCharStateDestMove;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.byMoveFlag = CHARSTATE_FLAG_DESTMOVE;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = res2->sCharState.sCharStateBase.vCurLoc.x;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = res2->sCharState.sCharStateBase.vCurLoc.y;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = res2->sCharState.sCharStateBase.vCurLoc.z;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.dwTimeStamp = 100;
		res2->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;

		packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
		g_pApp->Send(this->pSession->GetHandle(), &packet2);
		app->UserBroadcast(&packet2);

		printf("MOB IS MOVING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}*/
	//this is to play the fainting animation! This can also be written better, perhaps.
	if (lol->isAggro == false && lol->IsDead == true)
	{
		res2->handle = pBattleData->m_uiTargetSerialId;
		res2->wOpCode = GU_UPDATE_CHAR_STATE;
		res2->sCharState.sCharStateBase.byStateID = CHARSTATE_FAINTING;
		res2->sCharState.sCharStateBase.dwConditionFlag = CHARSTATE_FLAG_FAINTING;
		res2->sCharState.sCharStateDetail.sCharStateFainting;
		res2->sCharState.sCharStateDetail.sCharStateFainting.byReason = lol->IsDead;

		//this is what gives the player exp upon mob's death
		uint32_t Exp;
		Exp = lol->Level * 5; //this can be changed. When character's level up you should relog because you will freeze in place after level up.
		pSession->SendPlayerLevelUpCheck(app, Exp);

		packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
		g_pApp->Send(this->pSession->GetHandle(), &packet2);
		app->UserBroadcast(&packet2);
	}

	pSession->SendCharUpdateLp(pPacket, app, lol->CurLP, pBattleData->m_uiTargetSerialId);

	packet.SetPacketLen(sizeof(sGU_CHAR_ACTION_ATTACK));
	g_pApp->Send(this->pSession->GetHandle(), &packet);
	app->UserBroadcast(&packet);

	

}
void CClientSession::SendMobActionAttack(RwUInt32 uiSerialId, RwUInt32 m_uiTargetSerialId, CNtlPacket * pPacket)
{
	printf("MONSTER WILL REACT!!!\n");
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
/*	PlayersMain * plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CMonster::MonsterData * lol = NULL;
	static RwUInt8 byChainAttack = 1;
	DWORD PlayerCurHP = 0;
	RwBool bDamageApply = true;
	float formula;
	lol->isAggro = true;
	plr->SetPlayerFight(true);
	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();

	MobActivity::CreatureData * loldata = NULL;
	loldata->Attack(plr, app); */

}
void CClientSession::SendCharUpdateLp(CNtlPacket * pPacket, CGameServer * app, RwUInt32 wLp, RwUInt32 m_uiTargetSerialId)
{
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP));
	sGU_UPDATE_CHAR_LP * res = (sGU_UPDATE_CHAR_LP *)packet.GetPacketData();
//	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	res->wOpCode = GU_UPDATE_CHAR_LP;
	res->handle = m_uiTargetSerialId;
	if (g_pMobManager->FindCreature(m_uiTargetSerialId) == true)
	{
		CMonster::MonsterData *lol = g_pMobManager->GetMobByHandle(m_uiTargetSerialId);
		if (lol != NULL)
		{
			lol->FightMode = true;
			lol->CurLP = (DWORD)wLp;
			res->wCurLP = lol->CurLP;
			res->wMaxLP = lol->MaxLP;
			res->dwLpEpEventId = 255;
			packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP));
			app->UserBroadcastothers(&packet, this);
			g_pApp->Send(this->GetHandle(), &packet);
			if (lol->isAggro == false)
			{
				//lol->isAggro = true;
				// lol->target = plr->GetAvatarHandle();
			}
		}
	}
	else
	{
		this->gsf->printOk("PVP OK");
		PlayersMain *lol2 = g_pPlayerManager->GetPlayer(m_uiTargetSerialId);
		if (lol2)
		{
			res->wCurLP = lol2->GetPcProfile()->dwCurLP = wLp;
			res->wMaxLP = lol2->GetPcProfile()->avatarAttribute.wLastMaxLP;
			packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP));

			app->UserBroadcastothers(&packet, this);
			g_pApp->Send(this->GetHandle(), &packet);
		}
	}

	//plr = NULL;
	//delete plr;
}
void	CClientSession::SendMobLoot(CNtlPacket * pPacket, CGameServer * app, RwUInt32 m_uiTargetSerialId)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
	sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();

	//Legendary Drops
	//if (DropDat->aLegendaryDropTblidx[i] != INVALID_TBLIDX  && (random <= DropDat->afLegendaryTblidxRate[i]))
	{
		//	sLEGENDARY_DROP_TBLDAT* sDropDat = (sLEGENDARY_DROP_TBLDAT*)(sdrop->FindData(DropDat->aLegendaryDropTblidx[i]));
		//	cout << "lDropDat tblidx = " << sDropDat->aItem_Tblidx[i] << endl;
		//	for (int z = 0; z < NTL_MAX_LEGENDARY_DROP; z++)
		{
			//		if (sDropDat->aItem_Tblidx[z] != INVALID_TBLIDX && sDropDat->aItem_Tblidx[z] < 200000 && (random <= sDropDat->afDrop_Rate[i]))
			{
				//Randomizing numbers for  see if the player get a Fucking item
						std::random_device rd4;
						std::mt19937_64 mt4(rd4());
						std::uniform_int_distribution<int> distribution4(10, 15);
						//sITEM_TBLDAT* pLegendary = (sITEM_TBLDAT*)app->g_pTableContainer->GetItemTable()->FindData(sDropDat->aItem_Tblidx[z]);
						res->handle = AcquireItemSerialId();
						res->sObjectInfo.objType = OBJTYPE_DROPITEM;
						res->sObjectInfo.itemBrief.tblidx = 90005;
						res->sObjectInfo.itemBrief.byGrade = distribution4(mt4);
						res->sObjectInfo.itemBrief.byRank = ITEM_RANK_LEGENDARY;
						res->sObjectInfo.itemState.bIsNew = true;
						res->sObjectInfo.itemState.vCurLoc.x = 4719.310059;
						res->sObjectInfo.itemState.vCurLoc.y = -62.299999;
						res->sObjectInfo.itemState.vCurLoc.z = 4072.080078;
						res->wOpCode = GU_OBJECT_CREATE;
						//printf("Item Created %d\n\r", sDropDat->aItem_Tblidx[z]);
				packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
				g_pApp->Send(this->GetHandle(), &packet);
				//		app->AddNewItemDrop(res2->handle, sDropDat->aItem_Tblidx[z], res2->sObjectInfo.itemBrief.byGrade, res2->sObjectInfo.itemBrief.byRank);
			}
		}

		plr = NULL;
		delete plr;
	}
}
void CClientSession::SendCharUpdateFaintingState(CNtlPacket * pPacket, CGameServer * app, RwUInt32 uiSerialId, RwUInt32 m_uiTargetSerialId)
{
	//printf("char die: %i \n", m_uiTargetSerialId);
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	app->RemoveAttackBegin(uiSerialId, m_uiTargetSerialId);
	plr->SetPlayerFight(false);
	CNtlPacket packet(sizeof(SpawnMOB));
	SpawnMOB * res = (SpawnMOB *)packet.GetPacketData();
	if (g_pMobManager->FindCreature(m_uiTargetSerialId))
	{
		res->wOpCode = GU_UPDATE_CHAR_STATE;
		res->Handle = m_uiTargetSerialId;
		res->AspectID = CHARSTATE_FAINTING;
		////res->sCharState.sCharStateBase.vCurLoc.x = plr->GetPlayerPosition().x;
		//res->sCharState.sCharStateBase.vCurLoc.y = plr->GetPlayerPosition().y;
		//res->sCharState.sCharStateBase.vCurLoc.z = plr->GetPlayerPosition().z;
		//res->sCharState.sCharStateBase.vCurDir.x = plr->GetPlayerDirection().x;
		//res->sCharState.sCharStateBase.vCurDir.y = plr->GetPlayerDirection().y;
		//res->sCharState.sCharStateBase.vCurDir.z = plr->GetPlayerDirection().z;

		packet.SetPacketLen(sizeof(SpawnMOB));
		app->UserBroadcastothers(&packet, this);
		g_pApp->Send(this->GetHandle(), &packet);
		g_pMobManager->UpdateDeathStatus(m_uiTargetSerialId, true);
	}
	plr = NULL;
	delete plr;
}

void CClientSession::SendCharSkillRes(CNtlPacket * pPacket, CGameServer * app)
{
	SSkillData *pSkillData = new SSkillData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_CHAR_SKILL_REQ *pCharSkillReq = (sUG_CHAR_SKILL_REQ*)pPacket->GetPacketData();

	app->db->prepare("SELECT * FROM skills WHERE owner_id=? AND SlotID= ? ");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, pCharSkillReq->bySlotIndex);
	app->db->execute();
	app->db->fetch();

	int skillID = app->db->getInt("skill_id");
	CSkillTable * skillTable = app->g_pTableContainer->GetSkillTable();
	sSKILL_TBLDAT * skillDataOriginal = reinterpret_cast<sSKILL_TBLDAT*>(skillTable->FindData(skillID));
	plr->SetSkillInUse(true);
	
	pSkillData->pCharSkillTarget = pCharSkillReq->hTarget;
	pSkillData->m_uiSkillTblId = skillID;	//GetSkillId[m_Char.iClass][pCharSkillReq->bySlotIndex];
	pSkillData->m_bySkillActiveType = SKILL_TYPE_CASTING;
	pSkillData->m_uiSkillTime = timeGetTime();

	CNtlPacket packet(sizeof(sGU_CHAR_SKILL_RES));
	sGU_CHAR_SKILL_RES * sPacket = (sGU_CHAR_SKILL_RES *)packet.GetPacketData();

	sPacket->wOpCode = GU_CHAR_SKILL_RES;
	sPacket->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_CHAR_SKILL_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcast(&packet);
	plr = NULL;
	delete plr;
	switch (skillDataOriginal->bySkill_Active_Type)
	{
	case SKILL_ACTIVE_TYPE_DD:
	{
		printf("Skill Tipo Demage\n");
		SendCharSkillAction(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_BB:
	{
		printf("Buff\n");
		SendCharSkillCasting(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_CB:
	{
		printf("SKILL_ACTIVE_TYPE_CB\n");
		SendCharSkillAction(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_DB:
	{
		printf("SKILL_ACTIVE_TYPE_DB\n");
		SendCharSkillCasting(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_DC:
	{
		printf("SKILL_ACTIVE_TYPE_DC\n");
		SendCharSkillCasting(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_DH:
	{
		printf("SKILL_ACTIVE_TYPE_DH\n");
		SendCharSkillCasting(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	case SKILL_ACTIVE_TYPE_DOT:
	{
		printf("SKILL_ACTIVE_TYPE_DOT\n");
		SendCharSkillAction(pPacket, app, skillDataOriginal, pCharSkillReq->byRpBonusType);
	}break;
	}

	/*if ((skillDataOriginal->dwKeepTimeInMilliSecs != 0) || (skillDataOriginal->dwTransform_Use_Info_Bit_Flag==1))
	SendCharSkillCasting(pPacket, app, skillID);
	else
	SendCharSkillAction(pPacket, app, skillID);*/
}
//--------------------------------------------------------------------------------------//
//		Char Skill Send
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharSkillAction(CNtlPacket * pPacket, CGameServer * app, sSKILL_TBLDAT* SkillNow, int RpSelectedType)
{
	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_SKILL));
	sGU_CHAR_ACTION_SKILL * res = (sGU_CHAR_ACTION_SKILL *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CSkillTable *pSkillTbl = app->g_pTableContainer->GetSkillTable();

	res->wOpCode = GU_CHAR_ACTION_SKILL;
	res->handle = this->GetavatarHandle();
	res->wResultCode = GAME_SUCCESS;
	res->hAppointedTarget = this->GetTargetSerialId();
	res->skillId = SkillNow->tblidx;
	res->dwLpEpEventId = SkillNow->tblidx;
	res->bySkillResultCount = 1;

	res->byRpBonusType = RpSelectedType;
	res->aSkillResult[0].hTarget = this->GetTargetSerialId();
	res->aSkillResult[0].byAttackResult = this->gsf->GetBattleResultEffect(RpSelectedType);
	res->aSkillResult[0].effectResult1.fResultValue = SkillNow->fSkill_Effect_Value[0];
	res->aSkillResult[0].effectResult2.fResultValue = SkillNow->fSkill_Effect_Value[1];
	res->aSkillResult[0].byBlockedAction = 255;
	res->aSkillResult[1].hTarget = this->GetTargetSerialId() + 1;
	res->aSkillResult[1].byAttackResult = this->gsf->GetBattleResultEffect(RpSelectedType);
	res->aSkillResult[1].effectResult1.fResultValue = SkillNow->fSkill_Effect_Value[0];
	res->aSkillResult[1].effectResult2.fResultValue = SkillNow->fSkill_Effect_Value[1];
	res->aSkillResult[1].byBlockedAction = 255;

	//Char update EP
	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_EP));
	sGU_UPDATE_CHAR_EP * pUpdateEp = (sGU_UPDATE_CHAR_EP*)packet2.GetPacketData();
	plr->GetPcProfile()->wCurEP = (plr->GetPcProfile()->wCurEP - SkillNow->wRequire_EP);//Sub by Required EP
	pUpdateEp->handle = this->GetavatarHandle();
	pUpdateEp->wCurEP = plr->GetPcProfile()->wCurEP;
	pUpdateEp->wMaxEP = plr->GetPcProfile()->avatarAttribute.wBaseMaxEP;
	pUpdateEp->wOpCode = GU_UPDATE_CHAR_EP;

	//Condition for LP...skill is Optional
	CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_LP));
	if (SkillNow->wRequire_LP)
	{
		sGU_UPDATE_CHAR_LP * pUpdateLp = (sGU_UPDATE_CHAR_LP*)packet3.GetPacketData();
		plr->GetPcProfile()->dwCurLP -= SkillNow->wRequire_LP;//Sub by Required EP
		pUpdateLp->handle = this->GetavatarHandle();
		pUpdateLp->wCurLP = plr->GetPcProfile()->dwCurLP;
		pUpdateLp->wMaxLP = plr->GetPcProfile()->avatarAttribute.wBaseMaxLP;
		pUpdateLp->wOpCode = GU_UPDATE_CHAR_LP;

	}

	//Preparing packets
	packet.SetPacketLen(sizeof(sGU_CHAR_ACTION_SKILL));
	packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EP));
	packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet3);
	app->UserBroadcastothers(&packet2, this);
	app->UserBroadcastothers(&packet3, this);
	app->UserBroadcastothers(&packet, this);
	CClientSession::SendMobLoot(&packet, app, res->hAppointedTarget);
	float newLP = 10;
	//if (g_pMobManager->FindCreature) //MOBS
	{
		CMonster::MonsterData *lol = g_pMobManager->GetMobByHandle(res->hAppointedTarget);
		if (lol != NULL)
		{
			lol->FightMode = true;
			newLP = (float)lol->CurLP;
			newLP -= res->aSkillResult[0].effectResult1.DD_DOT_fDamage + 100;
			printf("LP: %f, damage: %f\n", newLP, res->aSkillResult[0].effectResult1.DD_DOT_fDamage + 100);
			if (newLP <= 0 || (newLP > lol->MaxLP))
			{
				//lol->IsDead = true;
				//	CClientSession::SendMobLoot(&packet, app, res->hAppointedTarget);
				this->gsf->printOk("DIE MOTHER FUCKER");
				//SendCharUpdateFaintingState(pPacket, app, this->GetavatarHandle(), res->hAppointedTarget);
			}
			else if (newLP > 0 && lol->IsDead == false)
			{
				SendCharUpdateLp(pPacket, app, newLP, res->hAppointedTarget);
			}
		}
		plr->SetSkillInUse(false);
		plr = NULL;
		delete plr;
	}
}
//-------------------------------------------------------------------//
//----------Fixed Casting Buff/Transform Skills - Luiz45-------------//
//-------------------------------------------------------------------//
void CClientSession::SendCharSkillCasting(CNtlPacket * pPacket, CGameServer * app, sSKILL_TBLDAT* SkillNow, int RpSelectedType)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	//Skill Events Prepare
	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_SKILL));
	CSkillTable *pSkillTbl = app->g_pTableContainer->GetSkillTable();
	sGU_CHAR_ACTION_SKILL * res = (sGU_CHAR_ACTION_SKILL *)packet.GetPacketData();

	res->skillId = SkillNow->tblidx;
	res->wResultCode = GAME_SUCCESS;
	res->byRpBonusType = RpSelectedType;//Untested
	res->wOpCode = GU_CHAR_ACTION_SKILL;
	res->handle = this->GetavatarHandle();//My Handle
	res->hAppointedTarget = this->GetTargetSerialId();//Get myself

	//Buff Events Prepare
	CNtlPacket packet2(sizeof(sGU_BUFF_REGISTERED));
	sGU_BUFF_REGISTERED * pBuffData = (sGU_BUFF_REGISTERED*)packet2.GetPacketData();
	pBuffData->wOpCode = GU_BUFF_REGISTERED;
	pBuffData->tblidx = SkillNow->tblidx;
	pBuffData->hHandle = this->GetavatarHandle();
	pBuffData->dwInitialDuration = SkillNow->dwKeepTimeInMilliSecs;
	pBuffData->dwTimeRemaining = SkillNow->dwCoolTimeInMilliSecs;
	pBuffData->afEffectValue[0] = SkillNow->fSkill_Effect_Value[0];
	pBuffData->afEffectValue[1] = SkillNow->fSkill_Effect_Value[1];
	pBuffData->bySourceType = DBO_OBJECT_SOURCE_SKILL;

	//Char update EP
	/*CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_EP));
	sGU_UPDATE_CHAR_EP * pUpdateEp = (sGU_UPDATE_CHAR_EP*)packet3.GetPacketData();
	plr->GetPcProfile()->wCurEP = (plr->GetPcProfile()->wCurEP - SkillNow->wRequire_EP);//Sub by Required EP
	pUpdateEp->handle = this->GetavatarHandle();
	pUpdateEp->wCurEP = plr->GetPcProfile()->wCurEP;
	pUpdateEp->wMaxEP = plr->GetPcProfile()->avatarAttribute.wBaseMaxEP;
	pUpdateEp->wOpCode = GU_UPDATE_CHAR_EP;

	//Condition for LP...skill is Optional
	CNtlPacket packet4(sizeof(sGU_UPDATE_CHAR_LP));
	if (SkillNow->wRequire_LP)
	{
		sGU_UPDATE_CHAR_LP * pUpdateLp = (sGU_UPDATE_CHAR_LP*)packet4.GetPacketData();
		plr->GetPcProfile()->dwCurLP = (plr->GetPcProfile()->dwCurLP - SkillNow->wRequire_LP);//Sub by Required LP
		pUpdateLp->handle = this->GetavatarHandle();
		pUpdateLp->wCurLP = plr->GetPcProfile()->dwCurLP;
		pUpdateLp->wMaxLP = plr->GetPcProfile()->avatarAttribute.wBaseMaxLP;
		pUpdateLp->wOpCode = GU_UPDATE_CHAR_LP;

	}*/

	packet.SetPacketLen(sizeof(sGU_CHAR_ACTION_SKILL));
	packet2.SetPacketLen(sizeof(sGU_BUFF_REGISTERED));
	//packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EP));
	//packet4.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP));

	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet);
	//g_pApp->Send(this->GetHandle(), &packet3);
	//g_pApp->Send(this->GetHandle(), &packet4);
	app->UserBroadcastothers(&packet2, this);
	app->UserBroadcastothers(&packet, this);
//	app->UserBroadcastothers(&packet3, this);
//	app->UserBroadcastothers(&packet4, this);
	plr = NULL;
	delete plr;
}

void CGameServer::UpdateClient(CNtlPacket * pPacket, CClientSession * pSession)
{
	// BASIC ATTACK
	SBattleData *pBattleData;
	m_batle_mutex.Lock();
	for (BATTLEIT it = m_listAttackBegin.begin(); it != m_listAttackBegin.end(); it++)
	{
		pBattleData = (*it);
		if (pBattleData->uiSerialId == 0)continue;
		if (pBattleData->m_uiTargetSerialId == 0)continue;
		if (pBattleData->bAttackMode == false)continue;
		if (GetTickCount() - pBattleData->dwCurrTime >= 200)// MONSTER_ATTACK_UPDATE_TICK)
		{
			printf("ATTACK %lu %lu\n", pBattleData->uiSerialId, pBattleData->m_uiTargetSerialId);
			pBattleData->dwCurrTime = GetTickCount();
			SendCharActionAttack(pBattleData);
		}
	}
	m_batle_mutex.Unlock();

}

//--------------------------------------------------------------------------------------//
//		Char toggle fight
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharToggleFighting(CNtlPacket * pPacket, CGameServer * app)
{
	//CHAR_TOGG_FIGHTING
	//app->mob->AddToWorld(pPacket, this);
	//sUG_CHAR_TOGG_FIGHTING * req = (sUG_CHAR_TOGG_FIGHTING *)pPacket->GetPacketData();
	this->cPlayersMain->SetPlayerFight(!this->cPlayersMain->GetPlayerFight());

	/*SBattleData *pBattleData;
	ListAttackBegin::iterator it;
	if (m_listAttackBegin.empty() == false)
	{
	for (it = m_listAttackBegin.begin(); it != m_listAttackBegin.end(); it++)
	{
	pBattleData = (*it);
	if (pBattleData->uiSerialId == this->GetTargetSerialId())
	{
	if (req->bFightMode)
	pBattleData->bAttackMode = TRUE;
	else
	pBattleData->bAttackMode = FALSE;
	}
	}
	}

	CNtlPacket packet2(sizeof(sGU_CHAR_FIGHTMODE));
	sGU_CHAR_FIGHTMODE * res = (sGU_CHAR_FIGHTMODE *)packet2.GetPacketData();
	res->handle = this->GetTargetSerialId();
	res->wOpCode = GU_CHAR_FIGHTMODE;
	if (req->bFightMode)
	res->bFightMode = false;
	else
	res->bFightMode = true;

	packet2.SetPacketLen(sizeof(sGU_CHAR_FIGHTMODE));
	g_pApp->Send(this->GetHandle(), &packet2);*/
}

//--------------------------------------------------------------------------------------//
//		SKILL SHOP REQUEST
//--------------------------------------------------------------------------------------//
void CClientSession::SendShopSkillReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_SHOP_SKILL_BUY_REQ * req = (sUG_SHOP_SKILL_BUY_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_SHOP_SKILL_BUY_RES));
	sGU_SHOP_SKILL_BUY_RES * res = (sGU_SHOP_SKILL_BUY_RES *)packet.GetPacketData();

	res->hNpchandle = req->hNpchandle;
	res->wOpCode = GU_SHOP_SKILL_BUY_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_SHOP_SKILL_BUY_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
}

//--------------------------------------------------------------------------------------//
//		Char learn skill fix by Marco
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharLearnSkillReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_SKILL_LEARN_REQ * req = (sUG_SKILL_LEARN_REQ*)pPacket->GetPacketData();

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packetSkill(sizeof(sGU_SKILL_LEARNED_NFY));
	sGU_SKILL_LEARNED_NFY * res = (sGU_SKILL_LEARNED_NFY*)packetSkill.GetPacketData();

	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	sSKILL_TBLDAT *pSkillSetTblData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(req->skillTblidx));

	res->wOpCode = GU_SKILL_LEARNED_NFY;
	res->skillId = pSkillSetTblData->tblidx;
	res->bySlot = pSkillSetTblData->bySlot_Index;
	packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
	g_pApp->Send(this->GetHandle(), &packetSkill);
	app->qry->InsertNewSkill(pSkillSetTblData->tblidx, plr->GetCharID(), pSkillSetTblData->bySlot_Index, 0, 1);

	CNtlPacket packetSkill1(sizeof(sGU_SKILL_LEARN_RES));
	sGU_SKILL_LEARN_RES * res1 = (sGU_SKILL_LEARN_RES *)packetSkill1.GetPacketData();

	res1->wOpCode = GU_SKILL_LEARN_RES;
	res1->wResultCode = 500;

	packetSkill1.SetPacketLen(sizeof(sGU_SKILL_LEARN_RES));
	g_pApp->Send(this->GetHandle(), &packetSkill1);

	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
	sGU_UPDATE_CHAR_SP * res3 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();

	//Update player's SP
	plr->GetPcProfile()->dwSpPoint -= 1;
	app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);

	//Send a response to client to get Update SP OK
	res3->wOpCode = GU_UPDATE_CHAR_SP;
	res3->dwSpPoint = plr->GetPcProfile()->dwSpPoint;

	packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
	g_pApp->Send(this->GetHandle(), &packet2);

	plr = NULL;
	delete plr;
}
void CClientSession::SendCharSkillByItemRes(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet20(sizeof(sGU_ITEM_USE_RES));
	sGU_ITEM_USE_RES * res20 = (sGU_ITEM_USE_RES*)packet20.GetPacketData();
	sUG_SKILL_LEARN_BY_ITEM_REQ * req = (sUG_SKILL_LEARN_BY_ITEM_REQ*)pPacket->GetPacketData();
	int CharID = plr->GetCharID();
	int level = 0;
	int Class = 0;
	
	CNtlPacket packetSkill(sizeof(sGU_SKILL_LEARNED_NFY));
	sGU_SKILL_LEARNED_NFY * res = (sGU_SKILL_LEARNED_NFY*)packetSkill.GetPacketData();
	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();
	 level = app->db->getInt("Level");
	 Class = app->db->getInt("Class");
	 res20->wOpCode = GU_ITEM_USE_RES;
	 //Bag1 16slot
	if (req->skillTblidx == 1)
	{
		req->test = 1;//pos
		req->skillTblidx = 0;//slot
	}
	else if(req->skillTblidx == 257)
	{
		req->test = 1;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 513)
	{
		req->test = 1;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 769)
	{
		req->test = 1;//bag
		req->skillTblidx = 3;//slot
	}
	else if  (req->skillTblidx == 1025)
	{
		req->test = 1;//bag
		req->skillTblidx = 4;//slot
	}
	else if  (req->skillTblidx == 1281)
	{
		req->test = 1;//bag
		req->skillTblidx = 5;//slot
	}
	else if  (req->skillTblidx == 1537)
	{
		req->test = 1;//bag
		req->skillTblidx = 6;//slot
	}
	else if  (req->skillTblidx == 1793)
	{
		req->test = 1;//bag
		req->skillTblidx = 7;//slot
	}
	else if  (req->skillTblidx == 2049)
	{
		req->test = 1;//bag
		req->skillTblidx = 8;//slot
	}
	else if  (req->skillTblidx == 2305)
	{
		req->test = 1;//bag
		req->skillTblidx = 9;//slot
	}
	else if  (req->skillTblidx == 2561)
	{
		req->test = 1;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2817)
	{
		req->test = 1;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3073)
	{
		req->test = 1;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3329)
	{
		req->test = 1;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3585)
	{
		req->test = 1;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3841)
	{
		req->test = 1;//bag
		req->skillTblidx = 15;//slot
	}
	////////////////////////////////////////////////////////////////////////////////////
	///					               BAG2											///
	///////////////////////////////////////////////////////////////////////////////////
	 if (req->skillTblidx == 0)
	{
		req->test = 2;//bag
		req->skillTblidx = 0;//slot
	}
	else if (req->skillTblidx == 258)
	{
		req->test = 2;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 514)
	{
		req->test = 2;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 770)
	{
		req->test = 2;//bag
		req->skillTblidx = 3;//slot
	}
	else if (req->skillTblidx == 1026)
	{
		req->test = 2;//bag
		req->skillTblidx = 4;//slot
	}
	else if (req->skillTblidx == 1282)
	{
		req->test = 2;//bag
		req->skillTblidx = 5;//slot
	}
	else if (req->skillTblidx == 1538)
	{
		req->test = 2;//bag
		req->skillTblidx = 6;//slot
	}
	else if (req->skillTblidx == 1794)
	{
		req->test = 2;//bag
		req->skillTblidx = 7;//slot
	}
	else if (req->skillTblidx == 2050)
	{
		req->test = 2;//bag
		req->skillTblidx = 8;//slot
	}
	else if (req->skillTblidx == 2306)
	{
		req->test = 2;//bag
		req->skillTblidx = 9;//slot
	}
	else if (req->skillTblidx == 2562)
	{
		req->test = 2;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2818)
	{
		req->test = 2;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3074)
	{
		req->test = 2;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3330)
	{
		req->test = 2;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3586)
	{
		req->test = 2;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3842)
	{
		req->test = 2;//bag
		req->skillTblidx = 15;//slot
	}
	else if (req->skillTblidx == 4098)
	{
		req->test = 2;//bag
		req->skillTblidx = 16;//slot
	}
	else if (req->skillTblidx == 4354)
	{
		req->test = 2;//bag
		req->skillTblidx = 17;//slot
	}
	else if (req->skillTblidx == 4610)
	{
		req->test = 2;//bag
		req->skillTblidx = 18;//slot
	}
	else if (req->skillTblidx == 4866)
	{
		req->test = 2;//bag
		req->skillTblidx = 19;//slot
	}
	else if (req->skillTblidx == 5122)
	{
		req->test = 2;//bag
		req->skillTblidx = 20;//slot
	}
	else if (req->skillTblidx == 5378)
	{
		req->test = 2;//bag
		req->skillTblidx = 21;//slot
	}
	else if (req->skillTblidx == 5634)
	{
		req->test = 2;//bag
		req->skillTblidx = 22;//slot
	}
	else if (req->skillTblidx == 5890)
	{
		req->test = 2;//bag
		req->skillTblidx = 23;//slot
	}
	else if (req->skillTblidx == 6146)
	{
		req->test = 2;//bag
		req->skillTblidx = 24;//slot
	}
	else if (req->skillTblidx == 6402)
	{
		req->test = 2;//bag
		req->skillTblidx = 25;//slot
	}
	else if (req->skillTblidx == 6658)
	{
		req->test = 2;//bag
		req->skillTblidx = 26;//slot
	}
	else if (req->skillTblidx == 6914)
	{
		req->test = 2;//bag
		req->skillTblidx = 27;//slot
	}
	else if (req->skillTblidx == 7170)
	{
		req->test = 2;//bag
		req->skillTblidx = 28;//slot
	}
	else if (req->skillTblidx == 7426)
	{
		req->test = 2;//bag
		req->skillTblidx = 29;//slot
	}
	else if (req->skillTblidx == 7682)
	{
		req->test = 2;//bag
		req->skillTblidx = 30;//slot
	}
	else if (req->skillTblidx == 7938)
	{
		req->test = 2;//bag
		req->skillTblidx = 31;//slot
	}
	//////////////////////////////////////////////////////////////////////////
	//							BAG 3									  ///
	////////////////////////////////////////////////////////////////////////
	 if (req->skillTblidx == 3)
	{
		req->test = 3;//bag
		req->skillTblidx = 0;//slot
	}
	else if (req->skillTblidx == 259)
	{
		req->test = 3;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 515)
	{
		req->test = 3;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 771)
	{
		req->test = 3;//bag
		req->skillTblidx = 3;//slot
	}
	else if (req->skillTblidx == 1027)
	{
		req->test = 3;//bag
		req->skillTblidx = 4;//slot
	}
	else if (req->skillTblidx == 1283)
	{
		req->test = 3;//bag
		req->skillTblidx = 5;//slot
	}
	else if (req->skillTblidx == 1539)
	{
		req->test = 3;//bag
		req->skillTblidx = 6;//slot
	}
	else if (req->skillTblidx == 1795)
	{
		req->test = 3;//bag
		req->skillTblidx = 7;//slot
	}
	else if (req->skillTblidx == 2051)
	{
		req->test = 3;//bag
		req->skillTblidx = 8;//slot
	}
	else if (req->skillTblidx == 2307)
	{
		req->test = 3;//bag
		req->skillTblidx = 9;//slot
	}
	else if (req->skillTblidx == 2563)
	{
		req->test = 3;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2819)
	{
		req->test = 3;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3075)
	{
		req->test = 3;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3331)
	{
		req->test = 3;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3587)
	{
		req->test = 3;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3843)
	{
		req->test = 3;//bag
		req->skillTblidx = 15;//slot
	}
	else if (req->skillTblidx == 4099)
	{
		req->test = 3;//bag
		req->skillTblidx = 16;//slot
	}
	else if (req->skillTblidx == 4355)
	{
		req->test = 3;//bag
		req->skillTblidx = 17;//slot
	}
	else if (req->skillTblidx == 4611)
	{
		req->test = 3;//bag
		req->skillTblidx = 18;//slot
	}
	else if (req->skillTblidx == 4867)
	{
		req->test = 3;//bag
		req->skillTblidx = 19;//slot
	}
	else if (req->skillTblidx == 5123)
	{
		req->test = 3;//bag
		req->skillTblidx = 20;//slot
	}
	else if (req->skillTblidx == 5379)
	{
		req->test = 3;//bag
		req->skillTblidx = 21;//slot
	}
	else if (req->skillTblidx == 5635)
	{
		req->test = 3;//bag
		req->skillTblidx = 22;//slot
	}
	else if (req->skillTblidx == 5891)
	{
		req->test = 3;//bag
		req->skillTblidx = 23;//slot
	}
	else if (req->skillTblidx == 6147)
	{
		req->test = 3;//bag
		req->skillTblidx = 24;//slot
	}
	else if (req->skillTblidx == 6403)
	{
		req->test = 3;//bag
		req->skillTblidx = 25;//slot
	}
	else if (req->skillTblidx == 6659)
	{
		req->test = 3;//bag
		req->skillTblidx = 26;//slot
	}
	else if (req->skillTblidx == 6915)
	{
		req->test = 3;//bag
		req->skillTblidx = 27;//slot
	}
	else if (req->skillTblidx == 7171)
	{
		req->test = 3;//bag
		req->skillTblidx = 28;//slot
	}
	else if (req->skillTblidx == 7427)
	{
		req->test = 3;//bag
		req->skillTblidx = 29;//slot
	}
	else if (req->skillTblidx == 7683)
	{
		req->test = 3;//bag
		req->skillTblidx = 30;//slot
	}
	else if (req->skillTblidx == 7939)
	{
		req->test = 3;//bag
		req->skillTblidx = 31;//slot
	}
	//////////////////////////////////////////////////////////////////////////
	//							BAG 4									  ///
	////////////////////////////////////////////////////////////////////////
	 if (req->skillTblidx == 4)
	{
		req->test = 4;//bag
		req->skillTblidx = 0;//slot
	}
	else if (req->skillTblidx == 260)
	{
		req->test = 4;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 516)
	{
		req->test = 4;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 772)
	{
		req->test = 4;//bag
		req->skillTblidx = 3;//slot
	}
	else if (req->skillTblidx == 1028)
	{
		req->test = 4;//bag
		req->skillTblidx = 4;//slot
	}
	else if (req->skillTblidx == 1284)
	{
		req->test = 4;//bag
		req->skillTblidx = 5;//slot
	}
	else if (req->skillTblidx == 1540)
	{
		req->test = 4;//bag
		req->skillTblidx = 6;//slot
	}
	else if (req->skillTblidx == 1796)
	{
		req->test = 4;//bag
		req->skillTblidx = 7;//slot
	}
	else if (req->skillTblidx == 2052)
	{
		req->test = 4;//bag
		req->skillTblidx = 8;//slot
	}
	else if (req->skillTblidx == 2308)
	{
		req->test = 4;//bag
		req->skillTblidx = 9;//slot
	}
	else if (req->skillTblidx == 2564)
	{
		req->test = 4;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2820)
	{
		req->test = 4;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3076)
	{
		req->test = 4;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3332)
	{
		req->test = 4;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3588)
	{
		req->test = 4;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3844)
	{
		req->test = 4;//bag
		req->skillTblidx = 15;//slot
	}
	else if (req->skillTblidx == 4100)
	{
		req->test = 4;//bag
		req->skillTblidx = 16;//slot
	}
	else if (req->skillTblidx == 4356)
	{
		req->test = 4;//bag
		req->skillTblidx = 17;//slot
	}
	else if (req->skillTblidx == 4612)
	{
		req->test = 4;//bag
		req->skillTblidx = 18;//slot
	}
	else if (req->skillTblidx == 4868)
	{
		req->test = 4;//bag
		req->skillTblidx = 19;//slot
	}
	else if (req->skillTblidx == 5124)
	{
		req->test = 4;//bag
		req->skillTblidx = 20;//slot
	}
	else if (req->skillTblidx == 5380)
	{
		req->test = 4;//bag
		req->skillTblidx = 21;//slot
	}
	else if (req->skillTblidx == 5636)
	{
		req->test = 4;//bag
		req->skillTblidx = 22;//slot
	}
	else if (req->skillTblidx == 5892)
	{
		req->test = 4;//bag
		req->skillTblidx = 23;//slot
	}
	else if (req->skillTblidx == 6148)
	{
		req->test = 4;//bag
		req->skillTblidx = 24;//slot
	}
	else if (req->skillTblidx == 6404)
	{
		req->test = 4;//bag
		req->skillTblidx = 25;//slot
	}
	else if (req->skillTblidx == 6660)
	{
		req->test = 4;//bag
		req->skillTblidx = 26;//slot
	}
	else if (req->skillTblidx == 6916)
	{
		req->test = 4;//bag
		req->skillTblidx = 27;//slot
	}
	else if (req->skillTblidx == 7172)
	{
		req->test = 4;//bag
		req->skillTblidx = 28;//slot
	}
	else if (req->skillTblidx == 7428)
	{
		req->test = 4;//bag
		req->skillTblidx = 29;//slot
	}
	else if (req->skillTblidx == 7684)
	{
		req->test = 4;//bag
		req->skillTblidx = 30;//slot
	}
	else if (req->skillTblidx == 7940)
	{
		req->test = 4;//bag
		req->skillTblidx = 31;//slot
	}
	//////////////////////////////////////////////////////////////////////////
	//							BAG 5									  ///
	////////////////////////////////////////////////////////////////////////
	 if (req->skillTblidx == 5)
	{
		req->test = 5;//bag
		req->skillTblidx = 0;//slot
	}
	else if (req->skillTblidx == 261)
	{
		req->test = 5;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 517)
	{
		req->test = 5;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 773)
	{
		req->test = 5;//bag
		req->skillTblidx = 3;//slot
	}
	else if (req->skillTblidx == 1029)
	{
		req->test = 5;//bag
		req->skillTblidx = 4;//slot
	}
	else if (req->skillTblidx == 1285)
	{
		req->test = 5;//bag
		req->skillTblidx = 5;//slot
	}
	else if (req->skillTblidx == 1541)
	{
		req->test = 5;//bag
		req->skillTblidx = 6;//slot
	}
	else if (req->skillTblidx == 1797)
	{
		req->test = 5;//bag
		req->skillTblidx = 7;//slot
	}
	else if (req->skillTblidx == 2053)
	{
		req->test = 5;//bag
		req->skillTblidx = 8;//slot
	}
	else if (req->skillTblidx == 2309)
	{
		req->test = 5;//bag
		req->skillTblidx = 9;//slot
	}
	else if (req->skillTblidx == 2565)
	{
		req->test = 5;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2821)
	{
		req->test = 5;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3077)
	{
		req->test = 5;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3333)
	{
		req->test = 5;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3589)
	{
		req->test = 5;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3845)
	{
		req->test = 5;//bag
		req->skillTblidx = 15;//slot
	}
	else if (req->skillTblidx == 4101)
	{
		req->test = 5;//bag
		req->skillTblidx = 16;//slot
	}
	else if (req->skillTblidx == 4357)
	{
		req->test = 5;//bag
		req->skillTblidx = 17;//slot
	}
	else if (req->skillTblidx == 4613)
	{
		req->test = 5;//bag
		req->skillTblidx = 18;//slot
	}
	else if (req->skillTblidx == 4869)
	{
		req->test = 5;//bag
		req->skillTblidx = 19;//slot
	}
	else if (req->skillTblidx == 5125)
	{
		req->test = 5;//bag
		req->skillTblidx = 20;//slot
	}
	else if (req->skillTblidx == 5381)
	{
		req->test = 5;//bag
		req->skillTblidx = 21;//slot
	}
	else if (req->skillTblidx == 5637)
	{
		req->test = 5;//bag
		req->skillTblidx = 22;//slot
	}
	else if (req->skillTblidx == 5893)
	{
		req->test = 5;//bag
		req->skillTblidx = 23;//slot
	}
	else if (req->skillTblidx == 6149)
	{
		req->test = 5;//bag
		req->skillTblidx = 24;//slot
	}
	else if (req->skillTblidx == 6405)
	{
		req->test = 5;//bag
		req->skillTblidx = 25;//slot
	}
	else if (req->skillTblidx == 6661)
	{
		req->test = 5;//bag
		req->skillTblidx = 26;//slot
	}
	else if (req->skillTblidx == 6917)
	{
		req->test = 5;//bag
		req->skillTblidx = 27;//slot
	}
	else if (req->skillTblidx == 7173)
	{
		req->test = 5;//bag
		req->skillTblidx = 28;//slot
	}
	else if (req->skillTblidx == 7429)
	{
		req->test = 5;//bag
		req->skillTblidx = 29;//slot
	}
	else if (req->skillTblidx == 7685)
	{
		req->test = 5;//bag
		req->skillTblidx = 30;//slot
	}
	else if (req->skillTblidx == 7941)
	{
		req->test = 5;//bag
		req->skillTblidx = 31;//slot
	}
	//////////////////////////////////////////////////////////////////////////
	//							BAG 6									  ///
	////////////////////////////////////////////////////////////////////////
	 if (req->skillTblidx == 6)
	{
		req->test = 6;//bag
		req->skillTblidx = 0;//slot
	}
	else if (req->skillTblidx == 262)
	{
		req->test = 6;//bag
		req->skillTblidx = 1;//slot
	}
	else if (req->skillTblidx == 518)
	{
		req->test = 6;//bag
		req->skillTblidx = 2;//slot
	}
	else if (req->skillTblidx == 774)
	{
		req->test = 6;//bag
		req->skillTblidx = 3;//slot
	}
	else if (req->skillTblidx == 1030)
	{
		req->test = 6;//bag
		req->skillTblidx = 4;//slot
	}
	else if (req->skillTblidx == 1286)
	{
		req->test = 6;//bag
		req->skillTblidx = 5;//slot
	}
	else if (req->skillTblidx == 1542)
	{
		req->test = 6;//bag
		req->skillTblidx = 6;//slot
	}
	else if (req->skillTblidx == 1798)
	{
		req->test = 6;//bag
		req->skillTblidx = 7;//slot
	}
	else if (req->skillTblidx == 2054)
	{
		req->test = 6;//bag
		req->skillTblidx = 8;//slot
	}
	else if (req->skillTblidx == 2310)
	{
		req->test = 6;//bag
		req->skillTblidx = 9;//slot
	}
	if (req->skillTblidx == 2566)
	{
		req->test = 6;//bag
		req->skillTblidx = 10;//slot
	}
	else if (req->skillTblidx == 2822)
	{
		req->test = 6;//bag
		req->skillTblidx = 11;//slot
	}
	else if (req->skillTblidx == 3078)
	{
		req->test = 6;//bag
		req->skillTblidx = 12;//slot
	}
	else if (req->skillTblidx == 3334)
	{
		req->test = 6;//bag
		req->skillTblidx = 13;//slot
	}
	else if (req->skillTblidx == 3590)
	{
		req->test = 6;//bag
		req->skillTblidx = 14;//slot
	}
	else if (req->skillTblidx == 3846)
	{
		req->test = 6;//bag
		req->skillTblidx = 15;//slot
	}
	else if (req->skillTblidx == 4102)
	{
		req->test = 6;//bag
		req->skillTblidx = 16;//slot
	}
	else if (req->skillTblidx == 4358)
	{
		req->test = 6;//bag
		req->skillTblidx = 17;//slot
	}
	else if (req->skillTblidx == 4614)
	{
		req->test = 6;//bag
		req->skillTblidx = 18;//slot
	}
	else if (req->skillTblidx == 4870)
	{
		req->test = 6;//bag
		req->skillTblidx = 19;//slot
	}
	else if (req->skillTblidx == 5126)
	{
		req->test = 6;//bag
		req->skillTblidx = 20;//slot
	}
	else if (req->skillTblidx == 5382)
	{
		req->test = 6;//bag
		req->skillTblidx = 21;//slot
	}
	else if (req->skillTblidx == 5638)
	{
		req->test = 6;//bag
		req->skillTblidx = 22;//slot
	}
	else if (req->skillTblidx == 5894)
	{
		req->test = 6;//bag
		req->skillTblidx = 23;//slot
	}
	else if (req->skillTblidx == 6150)
	{
		req->test = 6;//bag
		req->skillTblidx = 24;//slot
	}
	else if (req->skillTblidx == 6406)
	{
		req->test = 6;//bag
		req->skillTblidx = 25;//slot
	}
	else if (req->skillTblidx == 6662)
	{
		req->test = 6;//bag
		req->skillTblidx = 26;//slot
	}
	else if (req->skillTblidx == 6918)
	{
		req->test = 6;//bag
		req->skillTblidx = 27;//slot
	}
	else if (req->skillTblidx == 7174)
	{
		req->test = 6;//bag
		req->skillTblidx = 28;//slot
	}
	else if (req->skillTblidx == 7430)
	{
		req->test = 6;//bag
		req->skillTblidx = 29;//slot
	}
	else if (req->skillTblidx == 7686)
	{
		req->test = 6;//bag
		req->skillTblidx = 30;//slot
	}
	else if (req->skillTblidx == 7942)
	{
		req->test = 6;//bag
		req->skillTblidx = 31;//slot
	}
	//Finish all bag 
	
	int item = 0;
	app->db->prepare("SELECT * FROM items WHERE owner_id = ? AND place = ? AND pos = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->test);
	app->db->setInt(3, req->skillTblidx);
	app->db->execute();
	app->db->fetch();

	item = app->db->getInt("tblidx");//disable for get bag id or crash
	printf("class %d \n", Class);
	printf("item %d \n test %d \n test1 %d \n tblidx %d \n ", req->skillTblidx, req->test, req->test1, item );
	//CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	//sSKILL_TBLDAT *pSkillSetTblData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(req->skillTblidx));
	//Start skill Item Martial Artist / figther / sword
	if (item == 11120142)//Fly skill MA
	{
		if ((Class == 0 || Class == 7 || Class == 8))//if class is 0 MA 7 figther 8 Sworsdman
		{
			if (level >= 30)//se o level for maior ou igual
			{

				res->wOpCode = GU_SKILL_LEARNED_NFY;
				res->skillId = 20911;
				res->bySlot = 50;
				packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
				g_pApp->Send(this->GetHandle(), &packetSkill);

				//app->qry->InsertNewSkill(res->skillId, plr->GetCharID(), res->bySlot, 0, 1);

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->test);
				app->db->setInt(3, req->skillTblidx);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;
			}
			else//falha nivel
			{
				printf("fail nivel");
				res20->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
			}
		}
		else//falha class
		{
			res20->wResultCode = GAME_ITEM_CLASS_FAIL;
		}
	}

	else if (item == 11120118)//power UP M.A
	{
		if ((Class == 0 || Class == 7 || Class == 8))//if class is 0 MA 7 figther 8 Sworsdman
		{
			if (level >= 4)
			{
				res->wOpCode = GU_SKILL_LEARNED_NFY;
				res->skillId = 20221;
				res->bySlot = 51;
				packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
				g_pApp->Send(this->GetHandle(), &packetSkill);

				app->qry->InsertNewSkill(res->skillId, plr->GetCharID(), res->bySlot, 0, 1);

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->test);
				app->db->setInt(3, req->skillTblidx);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;
			}
			else
			{
				res20->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
			}
		}
		else
		{
			res20->wResultCode = GAME_ITEM_CLASS_FAIL;
		}
	}
	else if (item == 11120124)//Guard skill M.A
	{
		if ((Class == 0 || Class == 7 || Class == 8))//if class is 0 MA 7 figther 8 Sworsdman
		{
			if (level >= 9)
			{
				res->wOpCode = GU_SKILL_LEARNED_NFY;
				res->skillId = 20201;
				res->bySlot = 52;
				packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
				g_pApp->Send(this->GetHandle(), &packetSkill);

				app->qry->InsertNewSkill(res->skillId, plr->GetCharID(), res->bySlot, 0, 1);

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->test);
				app->db->setInt(3, req->skillTblidx);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;
			}
			else
			{
				res20->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
			}
		}
		else
		{
			res20->wResultCode = GAME_ITEM_CLASS_FAIL;
		}
	}
	else if (item == 11120130)//Dash skill M.A
	{
		if ((Class == 0 || Class == 7 || Class == 8))//if class is 0 MA 7 figther 8 Sworsdman
		{
			if (level >= 14)
			{
				res->wOpCode = GU_SKILL_LEARNED_NFY;
				res->skillId = 20211;
				res->bySlot = 53;
				packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
				g_pApp->Send(this->GetHandle(), &packetSkill);

				app->qry->InsertNewSkill(res->skillId, plr->GetCharID(), res->bySlot, 0, 1);

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->test);
				app->db->setInt(3, req->skillTblidx);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;
			}
			else
			{
				res20->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
			}
		}
		else
		{
			res20->wResultCode = GAME_ITEM_CLASS_FAIL;
		}
	}
	else if (item == 11120136)//Contra atack skill M.A
	{
		if ((Class == 0 || Class == 7 || Class == 8))//if class is 0 MA 7 figther 8 Sworsdman
		{
			if (level >= 9)
			{
				res->wOpCode = GU_SKILL_LEARNED_NFY;
				res->skillId = 10021;
				res->bySlot = 54;
				packetSkill.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
				g_pApp->Send(this->GetHandle(), &packetSkill);

				app->qry->InsertNewSkill(res->skillId, plr->GetCharID(), res->bySlot, 0, 1);

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->test);
				app->db->setInt(3, req->skillTblidx);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;
			}
			else
			{
				res20->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
			}
		}
		else
		{
			res20->wResultCode = GAME_ITEM_CLASS_FAIL;
		}

	}
	//Spiritualist / Turtle / Crane

	packet20.SetPacketLen(sizeof(sGU_ITEM_USE_RES));
	g_pApp->Send(this->GetHandle(), &packet20);
	
	CNtlPacket packetSkill1(sizeof(sGU_SKILL_LEARN_RES));
	sGU_SKILL_LEARN_RES * res1 = (sGU_SKILL_LEARN_RES *)packetSkill1.GetPacketData();

	res1->wOpCode = GU_SKILL_LEARN_RES;
	res1->wResultCode = GAME_SUCCESS;

	packetSkill1.SetPacketLen(sizeof(sGU_SKILL_LEARN_RES));
	g_pApp->Send(this->GetHandle(), &packetSkill1);
	


	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		Char learn HTB skill
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharSkillHTBLearn(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_HTB_LEARN_REQ * req = (sUG_HTB_LEARN_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packetHTB(sizeof(sGU_HTB_LEARN_RES));
	sGU_HTB_LEARN_RES * res = (sGU_HTB_LEARN_RES*)packetHTB.GetPacketData();

	CHTBSetTable * pHTBTable = app->g_pTableContainer->GetHTBSetTable();
	sHTB_SET_TBLDAT *pHTBSetTblData = reinterpret_cast<sHTB_SET_TBLDAT*>(pHTBTable->FindData(req->skillId));

	res->skillId = pHTBSetTblData->tblidx;
	res->bySkillSlot = pHTBSetTblData->bySlot_Index;
	res->wOpCode = GU_HTB_LEARN_RES;
	res->wResultCode = GAME_SUCCESS;
	packetHTB.SetPacketLen(sizeof(sGU_HTB_LEARN_RES));
	g_pApp->Send(this->GetHandle(), &packetHTB);
	app->qry->InsertNewSkill(pHTBSetTblData->tblidx, plr->GetCharID(), pHTBSetTblData->bySlot_Index, 0, 0);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		MOVE ITEM FIXED NEED SOME VERIFICATION
//--------------------------------------------------------------------------------------//
void CClientSession::SendItemMoveReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf(" move item \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_ITEM_MOVE_REQ * req = (sUG_ITEM_MOVE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_ITEM_MOVE_RES));
	sGU_ITEM_MOVE_RES * res = (sGU_ITEM_MOVE_RES *)packet.GetPacketData();

	app->db->prepare("SELECT * FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySrcPlace);
	app->db->setInt(3, req->bySrcPos);
	app->db->execute();
	app->db->fetch();
	RwUInt32 uniqueID = app->db->getInt("id");

	if (app->qry->CheckIfCanMoveItemThere(plr->GetCharID(), req->byDestPlace, req->byDestPos) == false){
		res->wResultCode = GAME_MOVE_CANT_GO_THERE;
	}
	else {
		app->qry->UpdateItemPlaceAndPos(uniqueID, req->byDestPlace, req->byDestPos);
		res->wResultCode = GAME_SUCCESS;
	}

	res->wOpCode = GU_ITEM_MOVE_RES;
	res->hSrcItem = uniqueID;
	res->bySrcPlace = req->bySrcPlace;
	res->bySrcPos = req->bySrcPos;
	res->hDestItem = -1;
	res->byDestPlace = req->byDestPlace;
	res->byDestPos = req->byDestPos;
	//res->handle = req->handle;

	packet.SetPacketLen(sizeof(sGU_ITEM_MOVE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;



}

//--------------------------------------------------------------------------------------//
//		DELETE ITEM FIXED
//--------------------------------------------------------------------------------------//
void CClientSession::SendItemDeleteReq(CNtlPacket * pPacket, CGameServer * app)
{
	// GET DELETE ITEM
	sUG_ITEM_DELETE_REQ * req = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_ITEM_DELETE_RES));
	sGU_ITEM_DELETE_RES * res = (sGU_ITEM_DELETE_RES *)packet.GetPacketData();

	app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySrcPlace);
	app->db->setInt(3, req->bySrcPos);
	app->db->execute();
	app->db->fetch();

	RwUInt32 u_itemid = app->db->getInt("id");
	RwUInt32 item_place = app->db->getInt("place");
	RwUInt32 item_pos = app->db->getInt("pos");

	res->wOpCode = GU_ITEM_DELETE_RES;
	res->wResultCode = GAME_SUCCESS;
	res->byPlace = req->bySrcPlace;
	res->byPos = req->bySrcPos;

	packet.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	// DELETE ITEM
	app->qry->DeleteItemById(u_itemid);

	CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
	sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

	res2->bySrcPlace = item_place;
	res2->bySrcPos = item_pos;
	res2->hSrcItem = u_itemid;
	res2->wOpCode = GU_ITEM_DELETE;
	plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

	packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
	g_pApp->Send(this->GetHandle(), &packet2);

	plr = NULL;
	delete plr;
}

//--------------------------------------------------------------------------------------//
//		STACK ITEM
//--------------------------------------------------------------------------------------//
void CClientSession::SendItemStackReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_ITEM_MOVE_STACK_REQ * req = (sUG_ITEM_MOVE_STACK_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	// GET DATA FROM MYSQL
	app->db->prepare("SELECT id,tblidx FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySrcPlace);
	app->db->setInt(3, req->bySrcPos);
	app->db->execute();
	app->db->fetch();
	unsigned int uniqueID = app->db->getInt("id");
	unsigned int item1ID = app->db->getInt("tblidx");

	app->db->prepare("SELECT id,count,tblidx FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->byDestPlace);
	app->db->setInt(3, req->byDestPos);
	app->db->execute();
	app->db->fetch();
	unsigned int uniqueID2 = app->db->getInt("id");
	unsigned int item2ID = app->db->getInt("tblidx");

	if (item1ID == item2ID)
	{
		// UPDATE ITEMS
		CNtlPacket packet(sizeof(sGU_ITEM_MOVE_STACK_RES));
		sGU_ITEM_MOVE_STACK_RES * res = (sGU_ITEM_MOVE_STACK_RES *)packet.GetPacketData();

		CNtlPacket packet1(sizeof(sGU_ITEM_DELETE));
		sGU_ITEM_DELETE * res1 = (sGU_ITEM_DELETE *)packet1.GetPacketData();

		res->wOpCode = GU_ITEM_MOVE_STACK_RES;
		res->bySrcPlace = req->bySrcPlace;
		res->bySrcPos = req->bySrcPos;
		res->byDestPlace = req->byDestPlace;
		res->byDestPos = req->byDestPos;
		res->hSrcItem = uniqueID;
		res->hDestItem = uniqueID2;
		res->byStackCount1 = req->byStackCount;
		res->byStackCount2 = req->byStackCount + app->db->getInt("count");
		res->wResultCode = GAME_SUCCESS;

		res1->bySrcPlace = req->bySrcPlace;
		res1->bySrcPos = req->bySrcPos;
		res1->hSrcItem = uniqueID;
		res1->wOpCode = GU_ITEM_DELETE;

		// UPDATE AND DELETE
		app->db->prepare("UPDATE items SET count=? WHERE id=?");
		app->db->setInt(1, res->byStackCount2);
		app->db->setInt(2, uniqueID2);
		app->db->execute();

		app->db->prepare("DELETE FROM items WHERE id=?");
		app->db->setInt(1, uniqueID);
		app->db->execute();
		plr->cPlayerInventory->RemoveItemFromInventory(uniqueID);

		// Send packet to client
		packet.SetPacketLen(sizeof(sGU_ITEM_MOVE_STACK_RES));
		g_pApp->Send(this->GetHandle(), &packet);
		packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE));
		g_pApp->Send(this->GetHandle(), &packet1);
	}
	else
	{
		CNtlPacket packet(sizeof(sGU_ITEM_MOVE_STACK_RES));
		sGU_ITEM_MOVE_STACK_RES * res = (sGU_ITEM_MOVE_STACK_RES *)packet.GetPacketData();
		res->wOpCode = GU_ITEM_MOVE_STACK_RES;
		res->wResultCode = GAME_FAIL;
		packet.SetPacketLen(sizeof(sGU_ITEM_MOVE_STACK_RES));
		g_pApp->Send(this->GetHandle(), &packet);
		this->gsf->printError("An error is occured in SendItemStackReq: GAME_FAIL item1ID != item2ID");
	}
	plr = NULL;
	delete plr;
}

void CClientSession::SendShopStartReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_SHOP_START_REQ * req = (sUG_SHOP_START_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_SHOP_START_RES));
	sGU_SHOP_START_RES * res = (sGU_SHOP_START_RES *)packet.GetPacketData();

	res->wOpCode = GU_SHOP_START_RES;
	res->wResultCode = GAME_SUCCESS;
	res->handle = req->handle;
	res->byType = 1;

	packet.SetPacketLen(sizeof(sGU_SHOP_START_RES));
	g_pApp->Send(this->GetHandle(), &packet); 
}
void CClientSession::SendShopBuyReq(CNtlPacket * pPacket, CGameServer * app)
{
	WORD		buy_item_result;
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_SHOP_BUY_REQ * req = (sUG_SHOP_BUY_REQ *)pPacket->GetPacketData();
	CMerchantTable* pMerchantItemTable = app->g_pTableContainer->GetMerchantTable();
	CItemTable *itemTbl = app->g_pTableContainer->GetItemTable();
	CNPCTable* pNpcTable = app->g_pTableContainer->GetNpcTable();
	for (CTable::TABLEIT itNPCSpawn = pNpcTable->Begin(); itNPCSpawn != pNpcTable->End(); ++itNPCSpawn)
	{
		sNPC_TBLDAT* pNPCtData = (sNPC_TBLDAT*)itNPCSpawn->second;
		if (pNPCtData->tblidx == g_pMobManager->FindNpc(req->handle))
		{
			sMERCHANT_TBLDAT* pMerchantData = (sMERCHANT_TBLDAT*)pMerchantItemTable->FindData(pNPCtData->amerchant_Tblidx[req->sBuyData[0].byMerchantTab]);
			if (pMerchantData->bySell_Type == MERCHANT_SELL_TYPE_ITEM)
			{
				for (RwInt32 j = 0; j < NTL_MAX_MERCHANT_COUNT; j++)
				{
					if (pMerchantData->aitem_Tblidx[j] == INVALID_TBLIDX)
					{
						buy_item_result = 501;
						break;
					}
					if (j == 0)
						j = req->sBuyData[j].byItemPos;
					for (int l = 12; l >= 0; l--)
					{
						if (req->sBuyData[l].byItemPos == j)
						{
							sITEM_TBLDAT* pItemData = (sITEM_TBLDAT*) itemTbl->FindData( pMerchantData->aitem_Tblidx[j] );
							int ItemPos = 0;

							app->db->prepare("SELECT * FROM items WHERE owner_ID = ? AND place=1 ORDER BY pos ASC");
							app->db->setInt(1, plr->GetPcProfile()->charId);
							app->db->execute();
							int k = 0;
							while(app->db->fetch())
							{
							if(app->db->getInt("pos") < NTL_MAX_ITEM_SLOT)
							ItemPos = app->db->getInt("pos") + 1;
							else
							ItemPos = app->db->getInt("pos");
							k++;
							}
							app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?, @unique_iID)");
							app->db->setInt(1, pMerchantData->aitem_Tblidx[j]);
							app->db->setInt(2, plr->GetPcProfile()->charId);
							app->db->setInt(3, ItemPos);
							app->db->setInt(4, pItemData->byRank);
							app->db->setInt(5, pItemData->byDurability);
							app->db->execute();
							app->db->execute("SELECT @unique_iID");
							app->db->fetch();

							CNtlPacket packet2(sizeof(sGU_ITEM_CREATE));
							sGU_ITEM_CREATE * res2 = (sGU_ITEM_CREATE *)packet2.GetPacketData();

							res2->bIsNew = true;
							res2->wOpCode = GU_ITEM_CREATE;
							res2->handle = app->db->getInt("@unique_iID");
							res2->sItemData.charId = this->GetavatarHandle();
							res2->sItemData.itemNo = pItemData->tblidx;
							res2->sItemData.byStackcount = req->sBuyData[0].byStack;
							res2->sItemData.itemId = app->db->getInt("@unique_iID");
							res2->sItemData.byPlace = 1;
							res2->sItemData.byPosition = ItemPos;
							res2->sItemData.byCurrentDurability = pItemData->byDurability;
							res2->sItemData.byRank = pItemData->byRank;

							packet2.SetPacketLen( sizeof(sGU_ITEM_CREATE) );
							g_pApp->Send( this->GetHandle(), &packet2 );
							this->gsf->CreateUpdateItem(plr, req->sBuyData[0].byStack, pMerchantData->aitem_Tblidx[j], false, this->GetHandle());
							break;
						}

					}
				}
			}
		}
	}

	CNtlPacket packet(sizeof(sGU_SHOP_BUY_RES));
	sGU_SHOP_BUY_RES * res = (sGU_SHOP_BUY_RES *)packet.GetPacketData();

	res->wOpCode = GU_SHOP_BUY_RES;
	res->wResultCode = plr->cPlayerInventory->GetBagStatus();
	res->handle = req->handle;

	packet.SetPacketLen(sizeof(sGU_SHOP_BUY_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
void CClientSession::SendShopEndReq(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_SHOP_END_RES));
	sGU_SHOP_END_RES * res = (sGU_SHOP_END_RES *)packet.GetPacketData();

	res->wOpCode = GU_SHOP_END_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_SHOP_END_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void	CClientSession::SendShopSellReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_SHOP_SELL_REQ * req = (sUG_SHOP_SELL_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_SHOP_SELL_RES));
	sGU_SHOP_SELL_RES * res = (sGU_SHOP_SELL_RES *)packet.GetPacketData();
	CNtlPacket packet1(sizeof(sGU_ITEM_DELETE));
	sGU_ITEM_DELETE * res1 = (sGU_ITEM_DELETE *)packet1.GetPacketData();
	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_ZENNY));
	sGU_UPDATE_CHAR_ZENNY * res2 = (sGU_UPDATE_CHAR_ZENNY *)packet2.GetPacketData();

	CItemTable *itemTbl = app->g_pTableContainer->GetItemTable();
	int zenit_amount = 0;
	for (int i = 0; (req->sSellData[i].byStack != 0); i++)
	{
		app->db->prepare("SELECT * FROM items WHERE owner_ID = ? AND place = ? AND pos = ?");
		app->db->setInt(1, plr->GetCharID());
		app->db->setInt(2, req->sSellData[i].byPlace);
		app->db->setInt(3, req->sSellData[i].byPos);
		app->db->execute();
		app->db->fetch();
		int item_id = app->db->getInt("tblidx");
		int id = app->db->getInt("id");

		res1->bySrcPlace = req->sSellData[i].byPlace;
		res1->bySrcPos = req->sSellData[i].byPos;
		res1->hSrcItem = id;
		res1->wOpCode = GU_ITEM_DELETE;
		packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE));
		sITEM_TBLDAT* pItemData = (sITEM_TBLDAT*)itemTbl->FindData(item_id);
		zenit_amount += pItemData->bySell_Price * req->sSellData[i].byStack;
		int count_less = req->sSellData[i].byStack - app->db->getInt("count");
		if (count_less <= 0)
		{
			g_pApp->Send(this->GetHandle(), &packet1);
			app->db->prepare("DELETE FROM items WHERE id = ?");
			app->db->setInt(1, id);
			app->db->execute();
			plr->cPlayerInventory->RemoveItemFromInventory(id);
		}
		else if (count_less >= 1)
		{
			app->db->prepare("UPDATE items SET count=? WHERE id=?");
			app->db->setInt(1, count_less);
			app->db->setInt(2, id);
			app->db->execute();
			this->gsf->CreateUpdateItem(plr, count_less, item_id, true, plr->GetSession());
		}
	}
	plr->GetPcProfile()->dwZenny += zenit_amount;
	res->handle = req->handle;
	res->wOpCode = GU_SHOP_SELL_RES;
	res->wResultCode = GAME_SUCCESS;
	res2->bIsNew = true;
	res2->byChangeType = 1;
	res2->dwZenny = plr->GetPcProfile()->dwZenny;
	res2->handle = this->GetavatarHandle();
	res2->wOpCode = GU_UPDATE_CHAR_ZENNY;
	app->qry->SetPlusMoney(plr->GetCharID(), zenit_amount);
	packet.SetPacketLen(sizeof(sGU_SHOP_SELL_RES));
	packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ZENNY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet1);
	g_pApp->Send(this->GetHandle(), &packet2);
	plr = NULL;
	delete plr;
}
//ROLL DICE
void	CClientSession::SendRollDice(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_DICE_ROLL_RES));
	sGU_DICE_ROLL_RES * res = (sGU_DICE_ROLL_RES *)packet.GetPacketData();

	res->wOpCode = GU_DICE_ROLL_RES;
	res->wDiceResult = (WORD)rand() % 100;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_DICE_ROLL_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	CNtlPacket packet2(sizeof(sGU_DICE_ROLLED_NFY));
	sGU_DICE_ROLLED_NFY * res2 = (sGU_DICE_ROLLED_NFY *)packet2.GetPacketData();

	res2->wDiceResult = res->wDiceResult;
	res2->wOpCode = GU_DICE_ROLLED_NFY;
	res2->hSubject = this->GetavatarHandle();

	packet2.SetPacketLen(sizeof(sGU_DICE_ROLLED_NFY));
	g_pApp->Send(this->GetHandle(), &packet2);
	app->UserBroadcastothers(&packet2, this);
}
//Scauter
void	CClientSession::SendScouterIndicatorReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_SCOUTER_INDICATOR_REQ * req = (sUG_SCOUTER_INDICATOR_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_SCOUTER_INDICATOR_RES));
	sGU_SCOUTER_INDICATOR_RES * res = (sGU_SCOUTER_INDICATOR_RES *)packet.GetPacketData();

	res->hTarget = req->hTarget;
	res->dwRetValue = 1;
	res->wOpCode = GU_SCOUTER_PREDICT_RES;
	
	packet.SetPacketLen(sizeof(sGU_SCOUTER_INDICATOR_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void	CClientSession::SendDragonBallCheckReq(CNtlPacket * pPacket, CGameServer * app) // THIS IS THE FIRST VERSION
{
	printf("--- UG_DRAGONBALL_CHECK_REQ --- \n");
	sUG_DRAGONBALL_CHECK_REQ * req = (sUG_DRAGONBALL_CHECK_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_DRAGONBALL_CHECK_RES));
	sGU_DRAGONBALL_CHECK_RES * res = (sGU_DRAGONBALL_CHECK_RES *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet2(sizeof(SpawnNPC));
	SpawnNPC * obj = (SpawnNPC *)packet2.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_AVATAR_ZONE_INFO));
	sGU_AVATAR_ZONE_INFO * zone = (sGU_AVATAR_ZONE_INFO *)packet3.GetPacketData();

	int dragonBall[7] = { 9, 9, 9, 9, 9, 9, 9 };// 7 because we need loop 0 - 6 because position start to 0 :)
	int i = 0;
	while (i <= 6)
	{
		app->db->prepare("SELECT * FROM items WHERE id = ?");
		app->db->setInt(1, req->sData[i].hItem);
		app->db->execute();
		app->db->fetch();
		dragonBall[i] = app->db->getInt("tblidx");
		i++;
	}
	i = 0;
	std::sort(dragonBall, dragonBall + 6);
	while (i <= 6)
	{
		if (req->sData[i].byPos == i && dragonBall[i] == (200001 + i));
		i++;
		/*else
		{
			res->hObject = req->hObject;
			res->wResultCode = GAME_DRAGONBALL_NOT_FOUND;
			res->wOpCode = GU_DRAGONBALL_CHECK_RES;
			packet.SetPacketLen(sizeof(sGU_DRAGONBALL_CHECK_RES));
			g_pApp->Send(this->GetHandle(), &packet);
			this->gsf->printError("An error is occured in SendDragonBallReq: GAME_DRAGONBALL_NOT_FOUND");
			i = 0;
			break;*/
		}
		
	
	if (i == 7)
	{

		zone->wOpCode = GU_AVATAR_ZONE_INFO;
		zone->zoneInfo.bIsDark = true;
		zone->zoneInfo.zoneId = 0; // 0 namek start zone

		packet3.SetPacketLen(sizeof(sGU_AVATAR_ZONE_INFO));
		g_pApp->Send(this->GetHandle(), &packet3);
		app->UserBroadcastothers(&packet3, this);

		sSPAWN_TBLDAT* pMOBTblData = (sSPAWN_TBLDAT*)app->g_pTableContainer->GetMobSpawnTable(1)->FindData(6361105);

		obj->Handle = INVALID_TBLIDX - 10; // this is wrong
		obj->wOpCode = GU_OBJECT_CREATE;
		obj->Type = OBJTYPE_NPC; // this is wrong
		obj->Tblidx = 6361105; // this is wrong
		obj->Loc[0] = 4708;
		obj->Loc[1] = -52;
		obj->Loc[2] = 4001;
		obj->StateID = CHARSTATE_DIRECT_PLAY;
		obj->curEP = 100;
		obj->curLP = 100;
		obj->maxEP = 100;
		obj->maxLP = 100;
		obj->Size = 10;

		packet2.SetPacketLen(sizeof(SpawnNPC));
		g_pApp->Send(this->GetHandle(), &packet2);
		app->UserBroadcastothers(&packet2, this);

		res->hObject = req->hObject;
		res->wResultCode = GAME_SUCCESS;
		res->wOpCode = GU_DRAGONBALL_CHECK_RES;
		packet.SetPacketLen(sizeof(sGU_DRAGONBALL_CHECK_RES));
		g_pApp->Send(this->GetHandle(), &packet);
	}
}

void	CClientSession::SendDragonBallRewardReq(CNtlPacket * pPacket, CGameServer * app) // THIS IS THE FIRST VERSION
{
	printf("--- UG_DRAGONBALL_REWARD_REQ --- \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_DRAGONBALL_REWARD_REQ * req = (sUG_DRAGONBALL_REWARD_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_DRAGONBALL_REWARD_RES));
	sGU_DRAGONBALL_REWARD_RES * res = (sGU_DRAGONBALL_REWARD_RES *)packet.GetPacketData();

	sDRAGONBALL_REWARD_TBLDAT* pDBtData = (sDRAGONBALL_REWARD_TBLDAT*)app->g_pTableContainer->GetDragonBallRewardTable()->FindData(req->rewardTblidx);

	//printf("Datacontainer = byBallType = %d, byRewardCategoryDepth = %d, rewardType = %d\ndwRewardZenny = %d, catergoryDialogue = %d\nCategoryName = %d, RewardDialog1 = %d, RewardDialog2 = %d\nrewardlinktblidx = %d, rewardname = %d, tdblidx = %d\n",
	//pDBtData->byBallType, pDBtData->byRewardCategoryDepth, pDBtData->byRewardType, pDBtData->dwRewardZenny, pDBtData->rewardCategoryDialog,pDBtData->rewardCategoryName,
	//pDBtData->rewardDialog1, pDBtData->rewardDialog2,pDBtData->rewardLinkTblidx, pDBtData->rewardName, pDBtData->tblidx);
	//printf("Reward have been found.\nReward id = %d.\n", req->rewardTblidx);
	//------------ Wishlist --------//

	CSkillTable* pSkillTable = app->g_pTableContainer->GetSkillTable();
	sSKILL_TBLDAT* pSkillData = (sSKILL_TBLDAT*)pSkillTable->FindData(pDBtData->rewardLinkTblidx);

	CItemTable* pItemTable = app->g_pTableContainer->GetItemTable();
	sITEM_TBLDAT * pItemData = (sITEM_TBLDAT*)pItemTable->FindData(pDBtData->rewardLinkTblidx);

	CNtlPacket packet3(sizeof(sGU_AVATAR_ZONE_INFO));
	sGU_AVATAR_ZONE_INFO * zone = (sGU_AVATAR_ZONE_INFO *)packet3.GetPacketData();
	
	switch (pDBtData->byRewardType)
	{
	case DRAGONBALL_REWARD_TYPE_SKILL:{
		CNtlPacket packet3(sizeof(sGU_SKILL_LEARNED_NFY));
		sGU_SKILL_LEARNED_NFY * res3 = (sGU_SKILL_LEARNED_NFY *)packet3.GetPacketData();
		//Fixed Slot Index for Shenron's Buff Skill
		res3->wOpCode = GU_SKILL_LEARNED_NFY;
		res3->skillId = pSkillData->tblidx;
		res3->bySlot = (this->gsf->GetTotalSlotSkill(plr->GetCharID()) + 1);
		app->qry->InsertNewSkill(pSkillData->tblidx, plr->GetCharID(), res3->bySlot, pSkillData->wKeep_Time, 0);
		packet3.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
		g_pApp->Send(this->GetHandle(), &packet3);
	}
		break;
	case DRAGONBALL_REWARD_TYPE_ITEM:{
		CNtlPacket packet4(sizeof(sGU_ITEM_PICK_RES));
		sGU_ITEM_PICK_RES * res4 = (sGU_ITEM_PICK_RES*)packet4.GetPacketData();
		res4->itemTblidx = pItemData->tblidx;
		res4->wOpCode = GU_ITEM_PICK_RES;
		res4->wResultCode = GAME_SUCCESS;
		this->gsf->CreateUpdateItem(plr, pItemData->byMax_Stack, pItemData->tblidx, false, this->GetHandle());
		int ItemPos = 0;

		app->db->prepare("SELECT * FROM items WHERE owner_ID = ? AND place=1 ORDER BY pos ASC");
		app->db->setInt(1, plr->GetPcProfile()->charId);
		app->db->execute();
		int k = 0;
		//Need a right loop
		while (app->db->fetch())
		{
		if (app->db->getInt("pos") < NTL_MAX_ITEM_SLOT)
		ItemPos = app->db->getInt("pos") + 1;
		else
		ItemPos = app->db->getInt("pos");
		k++;
		}
		app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?, @unique_iID)");//this basicaly a insert into...
		app->db->setInt(1, pItemData->tblidx);
		app->db->setInt(2, plr->GetPcProfile()->charId);
		app->db->setInt(3, ItemPos);
		app->db->setInt(4, pItemData->byRank);
		app->db->setInt(5, pItemData->byDurability);
		app->db->execute();
		app->db->execute("SELECT @unique_iID");
		app->db->fetch();

		CNtlPacket packet2(sizeof(sGU_ITEM_CREATE));
		sGU_ITEM_CREATE * res2 = (sGU_ITEM_CREATE *)packet2.GetPacketData();

		res2->bIsNew = true;
		res2->wOpCode = GU_ITEM_CREATE;
		res2->handle = app->db->getInt("@unique_iID");
		res2->sItemData.charId = plr->GetPcProfile()->charId;
		res2->sItemData.itemNo = pItemData->tblidx;
		res2->sItemData.byStackcount = pItemData->byMax_Stack;//1 is need to be default,you can use byMaxStack(but if you choose senzubeans the correct is receive 3(like dragon ball Saga) but give you 20
		res2->sItemData.itemId = app->db->getInt("@unique_iID");
		res2->sItemData.byPlace = 1;
		res2->sItemData.byPosition = ItemPos;
		res2->sItemData.byCurrentDurability = pItemData->byDurability;
		res2->sItemData.byRank = pItemData->byRank;

		packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
		packet4.SetPacketLen(sizeof(sGU_ITEM_PICK_RES));
		g_pApp->Send(this->GetHandle(), &packet2);
		g_pApp->Send(this->GetHandle(), &packet4);
	}
		break;
	case DRAGONBALL_REWARD_TYPE_ZENNY:{
		CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_ZENNY));
		sGU_UPDATE_CHAR_ZENNY * res5 = (sGU_UPDATE_CHAR_ZENNY *)packet5.GetPacketData();
		res5->dwZenny = plr->GetPcProfile()->dwZenny + req->rewardTblidx;//by analazying this is the ammount....				
		res5->bIsNew = true;
		res5->handle = this->GetavatarHandle();
		res5->byChangeType = 0;//never mind
		res5->wOpCode = GU_UPDATE_CHAR_ZENNY;
		packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ZENNY));
		g_pApp->Send(this->GetHandle(), &packet5);
		app->qry->SetPlusMoney(plr->GetCharID(), (res5->dwZenny -= plr->GetPcProfile()->dwZenny));
		plr->GetPcProfile()->dwZenny += res5->dwZenny;

	}
		break;
	}

	//---------------End Wish Table---------------------------------------------------------------------------//
	res->hObject = req->hObject;
	res->wOpCode = GU_DRAGONBALL_REWARD_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_DRAGONBALL_REWARD_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	zone->wOpCode = GU_AVATAR_ZONE_INFO;
	zone->zoneInfo.bIsDark = false;
	zone->zoneInfo.zoneId = 0;

	packet3.SetPacketLen(sizeof(sGU_AVATAR_ZONE_INFO));
	g_pApp->Send(this->GetHandle(), &packet3);
	app->UserBroadcastothers(&packet3, this);

	CNtlPacket packet2(sizeof(sGU_DRAGONBALL_SCHEDULE_INFO));
	sGU_DRAGONBALL_SCHEDULE_INFO * res2 = (sGU_DRAGONBALL_SCHEDULE_INFO *)packet2.GetPacketData();
	res2->bIsAlive = true;
	res2->byEventType = SCHEDULE_EVENT_TYPE_NORMAL_DRAGONBALL;
	res2->byTermType = 1;
	res2->nStartTime = timeGetTime();
	res2->nEndTime = timeGetTime() * 2;
	res2->wOpCode = GU_DRAGONBALL_SCHEDULE_INFO;

	packet2.SetPacketLen(sizeof(sGU_DRAGONBALL_SCHEDULE_INFO));
	g_pApp->Send(this->GetHandle(), &packet2);
	app->UserBroadcastothers(&packet2, this);

	CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res3 = (sGU_UPDATE_CHAR_STATE *)packet5.GetPacketData();

	res3->handle = INVALID_TBLIDX - 10;
	res3->sCharState.sCharStateBase.byStateID = CHARSTATE_DESPAWNING;
	res3->wOpCode = GU_UPDATE_CHAR_STATE;
	packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet5);
	app->UserBroadcastothers(&packet5, this);



	Sleep(10000);
	CNtlPacket packet4(sizeof(sGU_OBJECT_DESTROY));
	sGU_OBJECT_DESTROY * res4 = (sGU_OBJECT_DESTROY *)packet4.GetPacketData();
	res4->handle = INVALID_TBLIDX - 10;
	res4->wOpCode = GU_OBJECT_DESTROY;
	packet4.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
	g_pApp->Send(this->GetHandle(), &packet4);
	app->UserBroadcastothers(&packet4, this);
	plr = NULL;
	delete plr;

}
void CClientSession::SendGambleBuyReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- UG_SHOP_GAMBLE_BUY_REQ --- \n");
	sUG_SHOP_GAMBLE_BUY_REQ *req = (sUG_SHOP_GAMBLE_BUY_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_SHOP_GAMBLE_BUY_RES));
	sGU_SHOP_GAMBLE_BUY_RES * res = (sGU_SHOP_GAMBLE_BUY_RES *)packet.GetPacketData();

	res->handle = req->handle;
	res->wOpCode = GU_SHOP_GAMBLE_BUY_RES;
	res->wResultCode = GAME_SUCCESS;
	CItemMixMachineTable *mudo = app->g_pTableContainer->GetItemMixMachineTable();
	for (CTable::TABLEIT itmob = mudo->Begin(); itmob != mudo->End(); ++itmob)
	{
		sITEM_MIX_MACHINE_TBLDAT* pMusoData = (sITEM_MIX_MACHINE_TBLDAT*)itmob->second;
		if (pMusoData)
		{
			//printf("%d, %d, %d, %d, %d, %d, %d, %d, %S", pMusoData->aBuiltInRecipeTblidx[0], pMusoData->aBuiltInRecipeTblidx[1], pMusoData->aBuiltInRecipeTblidx[2], pMusoData->bValidityAble, pMusoData->byMachineType, pMusoData->byMixZennyDiscountRate, pMusoData->dynamicObjectTblidx, pMusoData->tblidx, pMusoData->wFunctionBitFlag);
			res->hItem = pMusoData->aBuiltInRecipeTblidx[0];
		}
	}
	packet.SetPacketLen(sizeof(sGU_SHOP_GAMBLE_BUY_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	this->gsf->printError("An error is occured in SendGambleBuyReq: GAME_FAIL");
}
//------------------------------------------------
// Character Skill Upgrade By luiz45
//------------------------------------------------
void CClientSession::SendCharSkillUpgrade(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	//Upgrading Process
	sUG_SKILL_UPGRADE_REQ * req = (sUG_SKILL_UPGRADE_REQ*)pPacket->GetPacketData();
	CSkillTable* pSkillTable = app->g_pTableContainer->GetSkillTable();

	app->db->prepare("SELECT * FROM skills WHERE owner_id=? AND SlotID = ? ");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySlotIndex);
	app->db->execute();
	app->db->fetch();

	int skillID = app->db->getInt("skill_id");
	sSKILL_TBLDAT* pSkillData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(skillID));

	if (pSkillData->dwNextSkillTblidx)
	{
	

		CNtlPacket packet(sizeof(sGU_SKILL_UPGRADE_RES));
		sGU_SKILL_UPGRADE_RES * res2 = (sGU_SKILL_UPGRADE_RES *)packet.GetPacketData();

		CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
		sGU_UPDATE_CHAR_SP * res3 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();

		res2->wOpCode = GU_SKILL_UPGRADE_RES;
		res2->wResultCode = GAME_SUCCESS;
		res2->skillId = pSkillData->dwNextSkillTblidx;
		res2->bySlot = req->bySlotIndex;
		packet.SetPacketLen(sizeof(sGU_SKILL_UPGRADE_RES));
		g_pApp->Send(this->GetHandle(), &packet);

		//Skill Level(ID)
		app->db->prepare("UPDATE skills SET skill_id=? WHERE owner_id=? AND skill_id=?");
		app->db->setInt(1, pSkillData->dwNextSkillTblidx);
		app->db->setInt(2, plr->GetCharID());
		app->db->setInt(3, skillID);
		app->db->execute();

		//Update player's SP
		plr->GetPcProfile()->dwSpPoint -= 1;
		app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);

		//Send a response to client to get Update SP OK
		res3->wOpCode = GU_UPDATE_CHAR_SP;
		res3->dwSpPoint = plr->GetPcProfile()->dwSpPoint;

		packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
		g_pApp->Send(this->GetHandle(), &packet2);
	}
	plr = NULL;
	delete plr;
}
//-------------------------------------------------//
//--		BANK FUNCTION BY DANEOS START		-- //
//-------------------------------------------------//
//--------------------------------------------------------------------------------------//
//		BANK START
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankStartReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("START BANK \n");
	sUG_BANK_START_REQ * req = (sUG_BANK_START_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_BANK_START_RES));
	sGU_BANK_START_RES * res = (sGU_BANK_START_RES *)packet.GetPacketData();

	res->wOpCode = GU_BANK_START_RES;
	res->wResultCode = GAME_SUCCESS;
	res->handle = req->handle;

	packet.SetPacketLen(sizeof(sGU_BANK_START_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	
}
//--------------------------------------------------------------------------------------//
//		BANK END
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankEndReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("END BANK \n");
	CNtlPacket packet(sizeof(sGU_BANK_END_RES));
	sGU_BANK_END_RES * res = (sGU_BANK_END_RES *)packet.GetPacketData();

	res->wOpCode = GU_BANK_END_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_BANK_END_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		BANK LOAD
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankLoadReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("LOAD BANK \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet2(sizeof(sGU_BANK_ITEM_INFO));
	sGU_BANK_ITEM_INFO * res2 = (sGU_BANK_ITEM_INFO *)packet2.GetPacketData();


	app->db->prepare("SELECT * FROM items WHERE owner_id = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();

	res2->wOpCode = GU_BANK_ITEM_INFO;
	res2->byBeginCount = 0;
	res2->byItemCount = app->db->rowsCount();

	while (app->db->fetch())
	{
		res2->aBankProfile->handle = app->db->getInt("id");
		res2->aBankProfile->tblidx = app->db->getInt("tblidx");
		res2->aBankProfile->byPlace = app->db->getInt("place");
		res2->aBankProfile->byPos = app->db->getInt("pos");
		res2->aBankProfile->byStackcount = app->db->getInt("count");
		res2->aBankProfile->byRank = app->db->getInt("rank");
		res2->aBankProfile->byCurDur = app->db->getInt("durability");

		packet2.AdjustPacketLen(sizeof(sNTLPACKETHEADER)+(2 * sizeof(BYTE)) + (res2->byItemCount * sizeof(sITEM_PROFILE)));
		g_pApp->Send(this->GetHandle(), &packet2);
	}

	CNtlPacket packet3(sizeof(sGU_BANK_ZENNY_INFO));
	sGU_BANK_ZENNY_INFO * res3 = (sGU_BANK_ZENNY_INFO *)packet3.GetPacketData();
	//res3->dwZenny = this->plr->GetBankMoney();
	res3->wOpCode = GU_BANK_ZENNY_INFO;
	packet3.SetPacketLen(sizeof(sGU_BANK_ZENNY_INFO));
	g_pApp->Send(this->GetHandle(), &packet3);

}
//--------------------------------------------------------------------------------------//
//		BANK BUY
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankBuyReq(CNtlPacket * pPacket, CGameServer * app)
{
	int result = 501;
	sUG_BANK_BUY_REQ * req = (sUG_BANK_BUY_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_BANK_BUY_RES));
	sGU_BANK_BUY_RES * res = (sGU_BANK_BUY_RES *)packet.GetPacketData();

	res->wOpCode = GU_BANK_BUY_RES;
	res->hNpchandle = req->hNpchandle;
	printf("%i %i \n", req->byMerchantTab, req->byPos);

	//if (this->plr->GetMoney() >= 3000){
		if (req->byMerchantTab == 0 && req->byPos == 1){
			app->db->prepare("CALL CreateItem (?,?,?,?, @unique_iID)");
			app->db->setInt(1, 19992);
			//app->db->setInt(2, this->plr->pcProfile->charId);
			app->db->setInt(3, 9);
			app->db->setInt(4, 1);
			app->db->execute();
			app->db->execute("SELECT @unique_iID");
			app->db->fetch();
			res->sData.charId = this->GetavatarHandle();
			res->sData.itemNo = 19992;
			res->sData.itemId = app->db->getInt("@unique_iID");
			res->sData.byPlace = 9;
			res->sData.byPosition = 1;
		}
		else if (req->byMerchantTab == 0 && req->byPos == 2){
			app->db->prepare("CALL CreateItem (?,?,?,?, @unique_iID)");
			app->db->setInt(1, 19993);
			//app->db->setInt(2, this->plr->pcProfile->charId);
			app->db->setInt(3, 9);
			app->db->setInt(4, 2);
			app->db->execute();
			app->db->execute("SELECT @unique_iID");
			app->db->fetch();
			res->sData.charId = this->GetavatarHandle();
			res->sData.itemNo = 19993;
			res->sData.itemId = app->db->getInt("@unique_iID");
			res->sData.byPlace = 9;
			res->sData.byPosition = 2;
		}
		result = 500;
		res->hItemhandle = app->db->getInt("@unique_iID");
		//this->plr->SetMoney(this->plr->GetMoney() - 3000);
		//app->qry->SetMinusMoney(this->plr->pcProfile->charId, 3000);
//		this->gsf->UpdateCharMoney(pPacket, this, 11, this->plr->GetMoney(), this->GetavatarHandle());
	//}
	res->wResultCode = result;

	packet.SetPacketLen(sizeof(sGU_BANK_BUY_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		BANK MONEY
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankMoneyReq(CNtlPacket * pPacket, CGameServer * app)
{
	int result = 501;
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_BANK_ZENNY_REQ * req = (sUG_BANK_ZENNY_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_BANK_ZENNY_RES));
	sGU_BANK_ZENNY_RES * res = (sGU_BANK_ZENNY_RES *)packet.GetPacketData();

	res->wOpCode = GU_BANK_ZENNY_RES;
	res->bIsSave = req->bIsSave;
	res->dwZenny = req->dwZenny;
	res->handle = req->handle;

	/*if (req->bIsSave == TRUE) {
		if (this->plr->GetMoney() >= req->dwZenny){
		this->plr->SetBankMoney(this->plr->GetBankMoney() + req->dwZenny);
		app->qry->SetBankMoney(plr->GetCharID(), this->plr->GetBankMoney());
		//REMOVE MONEY FROM INVENTORY
		this->plr->SetMoney(this->plr->GetMoney() - req->dwZenny);
		app->qry->SetMinusMoney(this->plr->pcProfile->charId, req->dwZenny);
		this->gsf->UpdateCharMoney(pPacket, this, 11, this->plr->GetMoney(), this->GetavatarHandle());

		result = 500;
		}
		}
		else if (req->bIsSave == FALSE) {
		if (this->plr->GetBankMoney() >= req->dwZenny){
		this->plr->SetBankMoney(this->plr->GetBankMoney() - req->dwZenny);
		app->qry->SetBankMoney(plr->GetCharID(), this->plr->GetBankMoney());
		//ADD MONEY TO INVENTORY
		this->plr->SetMoney(this->plr->GetMoney() + req->dwZenny);
		app->qry->SetPlusMoney(this->plr->pcProfile->charId, req->dwZenny);
		this->gsf->UpdateCharMoney(pPacket, this, 11, this->plr->GetMoney(), this->GetavatarHandle());

		result = 500;
		}
		}

		res->wResultCode = result;

		packet.SetPacketLen(sizeof(sGU_BANK_ZENNY_RES));
		g_pApp->Send(this->GetHandle(), &packet);*/
	plr = NULL;
	delete plr;
}
//--------------------------------------------------------------------------------------//
//		BANK MOVE ITEM
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankMoveReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("bank move item \n");
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_BANK_MOVE_REQ * req = (sUG_BANK_MOVE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_BANK_MOVE_RES));
	sGU_BANK_MOVE_RES * res = (sGU_BANK_MOVE_RES *)packet.GetPacketData();

	app->db->prepare("SELECT * FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySrcPlace);
	app->db->setInt(3, req->bySrcPos);
	app->db->execute();
	app->db->fetch();
	RwUInt32 uniqueID = app->db->getInt("id");

	if (app->qry->CheckIfCanMoveItemThere(plr->GetCharID(), req->byDestPlace, req->byDestPos) == false){
		res->wResultCode = GAME_MOVE_CANT_GO_THERE;
	}
	else {
		app->qry->UpdateItemPlaceAndPos(uniqueID, req->byDestPlace, req->byDestPos);
		res->wResultCode = GAME_SUCCESS;
	}

	res->wOpCode = GU_BANK_MOVE_RES;
	res->hSrcItem = uniqueID;
	res->bySrcPlace = req->bySrcPlace;
	res->bySrcPos = req->bySrcPos;
	res->hDstItem = -1;
	res->byDestPlace = req->byDestPlace;
	res->byDestPos = req->byDestPos;
	res->handle = req->handle;

	packet.SetPacketLen(sizeof(sGU_BANK_MOVE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;

}
//--------------------------------------------------------------------------------------//
//		BANK STACK ITEM
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankStackReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_BANK_MOVE_STACK_REQ * req = (sUG_BANK_MOVE_STACK_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	// GET DATA FROM MYSQL
	app->db->prepare("SELECT id,tblidx FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySrcPlace);
	app->db->setInt(3, req->bySrcPos);
	app->db->execute();
	app->db->fetch();
	RwUInt32 uniqueID = app->db->getInt("id");
	RwUInt32 Src_Item_ID = app->db->getInt("tblidx");

	app->db->prepare("SELECT id,tblidx,count FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->byDestPlace);
	app->db->setInt(3, req->byDestPos);
	app->db->execute();
	app->db->fetch();
	RwUInt32 uniqueID2 = app->db->getInt("id");

	RwUInt32 result_code;

	if (Src_Item_ID == app->db->getInt("tblidx")) {
		result_code = 500;
	}
	else {
		result_code = 805;
	}

	// UPDATE ITEMS
	CNtlPacket packet(sizeof(sGU_BANK_MOVE_STACK_RES));
	sGU_BANK_MOVE_STACK_RES * res = (sGU_BANK_MOVE_STACK_RES *)packet.GetPacketData();

	res->wOpCode = GU_BANK_MOVE_STACK_RES;
	res->bySrcPlace = req->bySrcPlace;
	res->bySrcPos = req->bySrcPos;
	res->byDestPlace = req->byDestPlace;
	res->byDestPos = req->byDestPos;
	res->hSrcItem = uniqueID;
	res->hDestItem = uniqueID2;
	res->byStackCount1 = req->byStackCount;
	res->byStackCount2 = req->byStackCount + app->db->getInt("count");
	res->handle = req->handle;
	res->wResultCode = result_code;

	// Send packet to client
	packet.SetPacketLen(sizeof(sGU_BANK_MOVE_STACK_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	// UPDATE AND DELETE
	if (result_code == 500){
		app->qry->UpdateItemsCount(uniqueID2, res->byStackCount2);
		app->qry->DeleteItemById(uniqueID);
		this->gsf->DeleteItemByUIdPlacePos(pPacket, this, uniqueID, req->bySrcPlace, req->bySrcPos);
	}
	plr = NULL;
	delete plr;

}
//--------------------------------------------------------------------------------------//
//		BANK DELETE ITEM
//--------------------------------------------------------------------------------------//
void CClientSession::SendBankDeleteReq(CNtlPacket * pPacket, CGameServer * app)
{
	// GET DELETE ITEM
	sUG_BANK_ITEM_DELETE_REQ * req = (sUG_BANK_ITEM_DELETE_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_BANK_ITEM_DELETE_RES));
	sGU_BANK_ITEM_DELETE_RES * res = (sGU_BANK_ITEM_DELETE_RES *)packet.GetPacketData();

	app->db->prepare("SELECT id FROM items WHERE owner_id=? AND place=? AND pos=?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->byPlace);
	app->db->setInt(3, req->byPos);
	app->db->execute();
	app->db->fetch();
	RwUInt32 u_itemid = app->db->getInt("id");

	res->wOpCode = GU_BANK_ITEM_DELETE_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_BANK_ITEM_DELETE_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	// DELETE ITEM
	app->qry->DeleteItemById(u_itemid);
	this->gsf->DeleteItemByUIdPlacePos(pPacket, this, u_itemid, req->byPlace, req->byPos);
	plr = NULL;
	delete plr;
}

//-------------------------------------------------
//      Quick Slot Update insert luiz45
//-------------------------------------------------
void CClientSession::SendCharUpdQuickSlot(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_QUICK_SLOT_UPDATE_REQ * req = (sUG_QUICK_SLOT_UPDATE_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_QUICK_SLOT_UPDATE_RES));
	sGU_QUICK_SLOT_UPDATE_RES * res = (sGU_QUICK_SLOT_UPDATE_RES*)packet.GetPacketData();

	printf("QUICK SLOT ID: %i", req->bySlotID);
	app->qry->InsertRemoveQuickSlot(req->tblidx, req->bySlotID, plr->GetCharID());

	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_QUICK_SLOT_UPDATE_RES;

	packet.SetPacketLen(sizeof(sGU_QUICK_SLOT_UPDATE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//-------------------------------------------------
//      Quick Slot Update Delete luiz45
//-------------------------------------------------
void CClientSession::SendCharDelQuickSlot(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_QUICK_SLOT_DEL_REQ * req = (sUG_QUICK_SLOT_DEL_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_QUICK_SLOT_DEL_NFY));
	sGU_QUICK_SLOT_DEL_NFY * response = (sGU_QUICK_SLOT_DEL_NFY*)packet.GetPacketData();
	if (req->bySlotID <= 47)
		app->qry->InsertRemoveQuickSlot(0, req->bySlotID, plr->GetCharID());

	response->bySlotID = req->bySlotID;
	response->wOpCode = GU_QUICK_SLOT_DEL_NFY;

	packet.SetPacketLen(sizeof(sGU_QUICK_SLOT_DEL_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
void CClientSession::SendPlayerLevelUpCheck(CGameServer * app, int exp)
{
	/*CExpTable *expT = app->g_pTableContainer->GetExpTable();
	for ( CTable::TABLEIT itNPCSpawn = expT->Begin(); itNPCSpawn != expT->End(); ++itNPCSpawn )
	{
	sEXP_TBLDAT *expTbl = (sEXP_TBLDAT *)itNPCSpawn->second;
	printf("%d, %d\n",expTbl->dwExp, expTbl->dwNeed_Exp);
	}*/
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_EXP));
	sGU_UPDATE_CHAR_EXP * response = (sGU_UPDATE_CHAR_EXP*)packet.GetPacketData();

	response->dwIncreasedExp = exp + rand() % plr->GetPcProfile()->byLevel * 2;
	plr->GetPcProfile()->dwCurExp += response->dwIncreasedExp;
	response->dwAcquisitionExp = exp;
	response->dwCurExp = plr->GetPcProfile()->dwCurExp;
	response->dwBonusExp = (exp - response->dwIncreasedExp);
	response->handle = plr->GetAvatarHandle();
	response->wOpCode = GU_UPDATE_CHAR_EXP;
	if (plr->GetPcProfile()->dwCurExp >= plr->GetPcProfile()->dwMaxExpInThisLevel)
	{
		CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
		sGU_UPDATE_CHAR_SP * res2 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();
		plr->GetPcProfile()->dwCurExp -= plr->GetPcProfile()->dwMaxExpInThisLevel;
		plr->GetPcProfile()->dwMaxExpInThisLevel += (plr->GetPcProfile()->dwMaxExpInThisLevel / 2);
		CNtlPacket packet1(sizeof(sGU_UPDATE_CHAR_LEVEL));
		sGU_UPDATE_CHAR_LEVEL * response1 = (sGU_UPDATE_CHAR_LEVEL*)packet1.GetPacketData();
		plr->GetPcProfile()->byLevel++;
		response1->byCurLevel = plr->GetPcProfile()->byLevel;
		response1->byPrevLevel = plr->GetPcProfile()->byLevel - 1;
		response1->dwMaxExpInThisLevel = plr->GetPcProfile()->dwMaxExpInThisLevel;
		response1->handle = plr->GetAvatarHandle();
		response1->wOpCode = GU_UPDATE_CHAR_LEVEL;
		packet1.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LEVEL));
		g_pApp->Send(this->GetHandle(), &packet1);
		plr->SetLevelUP();
		plr->cPlayerAttribute->UpdateAvatarAttributes(plr->GetAvatarHandle());
		plr->GetPcProfile()->dwSpPoint += 1;
		app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);
		app->qry->UpdatePlayerLevel(plr->GetPcProfile()->byLevel, plr->GetCharID(), plr->GetPcProfile()->dwCurExp, plr->GetPcProfile()->dwMaxExpInThisLevel);
		response->dwCurExp = plr->GetPcProfile()->dwCurExp;
		plr->SetRPBall();
		plr->SendRpBallInformation();
		res2->wOpCode = GU_UPDATE_CHAR_SP;
		res2->dwSpPoint = plr->GetPcProfile()->dwSpPoint;
		packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
		g_pApp->Send(this->GetHandle(), &packet2);
	}
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EXP));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//-------------------------------------------------
//      Envia Quest ao Player // Teleport xp rtward tlq etc...
//-------------------------------------------------
void CClientSession::SendPlayerQuestReq(CNtlPacket * pPacket, CGameServer * app)
{
	printf("--- UG_TS_CONFIRM_STEP_REQ --- \n");
	sUG_TS_CONFIRM_STEP_REQ* req = (sUG_TS_CONFIRM_STEP_REQ *)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet3(sizeof(sGU_TS_CONFIRM_STEP_RES));
	sGU_TS_CONFIRM_STEP_RES * res3 = (sGU_TS_CONFIRM_STEP_RES *)packet3.GetPacketData();

	res3->byTsType = req->byTsType;
	res3->dwParam = req->dwParam;
	res3->tcCurId = req->tcCurId;
	res3->tcNextId = req->tcNextId;
	res3->tId = req->tId;
	res3->wOpCode = GU_TS_CONFIRM_STEP_RES;
	res3->wResultCode = RESULT_SUCCESS;
	packet3.SetPacketLen(sizeof(sGU_TS_CONFIRM_STEP_RES));
	g_pApp->Send(this->GetHandle(), &packet3);
	//Save every step
	this->gsf->QuestStarted(plr->GetCharID(), req->tId, req->tcCurId, req->tcNextId, req->byTsType, req->dwEventData);
	//CObjectTable* pObjTable = app->g_pTableContainer->GetObjectTable(1);
	//sOBJECT_TBLDAT* pObjDat = reinterpret_cast<sOBJECT_TBLDAT*>(pObjTable->FindData(req->dwEventData));
	switch (req->byTsType)
	{
		//Explanation...Case 0 For QuestTrigger and Case 1 For PC Trigger
	case 0:
	{
			  if (res3->tcNextId == 254)
			  {
				  printf("Quest Aceite\n");
				  int exp = 18500;
				  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				 /* CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_EXP));
				  sGU_UPDATE_CHAR_EXP * response = (sGU_UPDATE_CHAR_EXP*)packet.GetPacketData();

				  response->dwIncreasedExp = exp + rand() % plr->GetPcProfile()->byLevel * 2;
				  plr->GetPcProfile()->dwCurExp += response->dwIncreasedExp;
				  response->dwAcquisitionExp = exp + response->dwBonusExp;
				  response->dwCurExp = plr->GetPcProfile()->dwCurExp;
				  response->dwBonusExp = 1900;
				  response->handle = plr->GetAvatarHandle();
				  response->wOpCode = GU_UPDATE_CHAR_EXP;
				  if (plr->GetPcProfile()->dwCurExp >= plr->GetPcProfile()->dwMaxExpInThisLevel)
				  {
					  CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
					  sGU_UPDATE_CHAR_SP * res2 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();
					  plr->GetPcProfile()->dwCurExp -= plr->GetPcProfile()->dwMaxExpInThisLevel;
					  plr->GetPcProfile()->dwMaxExpInThisLevel += (plr->GetPcProfile()->dwMaxExpInThisLevel * 1);
					  CNtlPacket packet1(sizeof(sGU_UPDATE_CHAR_LEVEL));
					  sGU_UPDATE_CHAR_LEVEL * response1 = (sGU_UPDATE_CHAR_LEVEL*)packet1.GetPacketData();
					  plr->GetPcProfile()->byLevel++;
					  response1->byCurLevel = plr->GetPcProfile()->byLevel;
					  response1->byPrevLevel = plr->GetPcProfile()->byLevel - 1;
					  response1->dwMaxExpInThisLevel = plr->GetPcProfile()->dwMaxExpInThisLevel;
					  response1->handle = plr->GetAvatarHandle();
					  response1->wOpCode = GU_UPDATE_CHAR_LEVEL;
					  packet1.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LEVEL));
					  g_pApp->Send(this->GetHandle(), &packet1);
					  plr->SetLevelUP();
					  //plr->cPlayerAttribute->UpdateAvatarAttributes(plr->GetAvatarHandle());
					  plr->GetPcProfile()->dwSpPoint += 1;
					  app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);
					  app->qry->UpdatePlayerLevel(plr->GetPcProfile()->byLevel, plr->GetCharID(), plr->GetPcProfile()->dwCurExp, plr->GetPcProfile()->dwMaxExpInThisLevel);
					  response->dwCurExp = plr->GetPcProfile()->dwCurExp;
					 plr->SetRPBall();
					  plr->SendRpBallInformation();
					  res2->wOpCode = GU_UPDATE_CHAR_SP;
					  res2->dwSpPoint = plr->GetPcProfile()->dwSpPoint;
					  packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
					  g_pApp->Send(this->GetHandle(), &packet2);

					  CNtlPacket packet4(sizeof(sGU_UPDATE_CHAR_STATE));
					  sGU_UPDATE_CHAR_STATE* res4 = (sGU_UPDATE_CHAR_STATE*)packet4.GetPacketData();
					  res4->handle = this->GetavatarHandle();
					  res4->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
					  res4->wOpCode = GU_UPDATE_CHAR_STATE;
					  packet4.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
					  g_pApp->Send(this->GetHandle(), &packet4);
				  }
				  packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EXP));
				  g_pApp->Send(this->GetHandle(), &packet);*/


				  res3->byTsType = req->byTsType;
				  res3->dwParam = req->dwParam;
				  res3->tcCurId = req->tcCurId;
				  res3->tcNextId = req->tcNextId;
				  res3->tId = req->tId;
				  res3->wOpCode = GU_TS_CONFIRM_STEP_RES;
				  res3->wResultCode = RESULT_SUCCESS;

			  }
	}
	case 1:
	{
			  printf("Trigger Case\n");
			  printf("res->byTsType = %d, res->dwParam = %d, res->tcCurId = %d, res->tcNextId = %d, res->tId = %d, req->byEventType = %d \n", res3->byTsType, res3->dwParam, res3->tcCurId, res3->tcNextId, res3->tId, req->byEventType);

			 // sOBJECT_TBLDAT* pObjDat = reinterpret_cast<sOBJECT_TBLDAT*>(pObjTable->FindData(req->dwEventData));
			  CWorldTable* pWorldTable = app->g_pTableContainer->GetWorldTable();
			  CDungeonTable* pDungeonTable = app->g_pTableContainer->GetDungeonTable(); 
			  if (res3->tId >= 6000)
			  {
				  //Nothing to do here...
				  //Time to find data for something we know is a correct tblidx for portal collision
				  //printf("PortalId info: \n\rContents Tblidx:%d \n\rwFunction:%d", pObjDat->contentsTblidx, pObjDat->wFunction);
				  CScriptLinkTable* pSlink = app->g_pTableContainer->GetScriptLinkTable();
				  CWorldZoneTable* pWZTable = app->g_pTableContainer->GetWorldZoneTable();
				  CWorldPathTable* pWpathT = app->g_pTableContainer->GetWorldPathTable();
				  CWorldPlayTable* pWPlay = app->g_pTableContainer->GetWorldPlayTable();
				  //sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(120000));
				 // printf("res->byTsType = %d, res->dwParam = %d, res->tcCurId = %d, res->tcNextId = %d, res->tId = %d, req->byEventType = %d \n", res3->byTsType, res3->dwParam, res3->tcCurId, res3->tcNextId, res3->tId, req->byEventType);
				  if (res3->tcCurId == 0)
				  {
					//saida TLQ
					  printf("Saida TLQ");
				  }
				  if (res3->tcCurId == 1)
				  {
					  if (res3->tId == 6005)
					  {
						  printf("Inicio da Saida TLQ 1 \n");
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 1;//app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 1;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = 4700;//app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = 0;//app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = 4500;// app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = 0;//app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = 0; //app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }
						 
						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						  CNtlPacket packet10(sizeof(SpawnMOB));
						  SpawnMOB * res10 = (SpawnMOB *)packet10.GetPacketData();
						  res10->Size = 10;
						  packet10.SetPacketLen(sizeof(SpawnMOB));
						  g_pApp->Send(this->GetHandle(), &packet10);
						  plr = NULL;
						  delete plr;
						 // this->SendNpcCreate(pPacket, app);
						 // this->SendMonsterCreate(pPacket, app);
					  }
					  //Seguinte
					  else if (res3->tId == 6009)
					  {
						  printf("Inicio da Saida TLQ 2 \n");
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 1;//app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 1;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = 4700;//app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = 0;//app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = 4500;// app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = 0;//app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = 0; //app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }

						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						  CNtlPacket packet10(sizeof(SpawnMOB));
						  SpawnMOB * res10 = (SpawnMOB *)packet10.GetPacketData();
						  res10->Size = 10;
						  packet10.SetPacketLen(sizeof(SpawnMOB));
						  g_pApp->Send(this->GetHandle(), &packet10);
						  plr = NULL;
						  delete plr;
						  // this->SendNpcCreate(pPacket, app);
						  // this->SendMonsterCreate(pPacket, app);
					  }
					  //Seguinte
					  else if (res3->tId == 6016)
					  {
						  printf("Inicio da Saida TLQ 3 \n");
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 1;//app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 1;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = 4700;//app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = 0;//app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = 4500;// app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = 0;//app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = 0; //app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }

						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						 
						  
						  plr = NULL;
						  delete plr;
						   //this->SendNpcCreate(pPacket, app);
						   //this->SendMonsterCreate(pPacket, app);
					  }
					  //Seguinte
				  }
				  else if (res3->tcCurId == 3)
				  {
					  printf("Entradas TLQ \n");
					  //printf("res->byTsType = %d, res->dwParam = %d, res->tcCurId = %d, res->tcNextId = %d, res->tId = %d\n", res3->byTsType, res3->dwParam, res3->tcCurId, res3->tcNextId, res3->tId);
					  if (res3->tId == 6000)
					  {
						  printf("Inicio Entrada TLQ 1 \n");
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 160000;// app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 160000;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = 792.04999;// app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = 19.97;// app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = -1099.18005;//app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = 0.98719;// app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0.0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = -1099.18005;// app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }
						
						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						  plr = NULL;
						  delete plr;
						 // this->SendNpcCreate(pPacket, app);
						 // this->SendMonsterCreate(pPacket, app);
					  }
					//seguinte
					 
					 // this->tlqManager->SetTLQAccessForPlayer(res3->tId, plr);
				  }
				 
				  else if (res3->tcCurId == 2)
				  {
					  printf("tcCurId 2\n");
					  if (res3->tId == 6006)
					  {
						  printf("Inicio Entrada TLQ 2 \n");
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 170000;// app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 170000;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = 388.859985;// app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = -92.120003;// app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = 142.080002;//app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = -0.588765;// app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0.0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = -0.808304;// app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }

						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						  plr = NULL;
						  delete plr;
						  // this->SendNpcCreate(pPacket, app);
						  // this->SendMonsterCreate(pPacket, app);
					  }
					  //seguinte
					  else if (res3->tId == 6012)
					  {
						  
						  printf("Inicio Entrada TLQ 3 \n");
						  CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
						  sGU_UPDATE_CHAR_STATE * res3 = (sGU_UPDATE_CHAR_STATE *)packet3.GetPacketData();
						  res3->handle = this->GetavatarHandle();
						  res3->sCharState.sCharStateBase.vCurLoc.x = -186.520004;// pWorldTbldat->vStart1Loc.x;
						  res3->sCharState.sCharStateBase.vCurLoc.y = 22.129999;//pWorldTbldat->vStart1Loc.y;
						  res3->sCharState.sCharStateBase.vCurLoc.z = -24.150000;// pWorldTbldat->vStart1Loc.z;
						  res3->sCharState.sCharStateBase.vCurDir.x = 0.994937;// pWorldTbldat->vStart1Dir.x;
						  res3->sCharState.sCharStateBase.vCurDir.z = 0.100499;// pWorldTbldat->vStart1Dir.z;
						  res3->sCharState.sCharStateDetail.sCharStateTeleporting.byTeleportType = TELEPORT_TYPE_TMQ_PORTAL;
						  res3->sCharState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
						  res3->wOpCode = GU_UPDATE_CHAR_STATE;

						  packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
						  g_pApp->Send(this->GetHandle(), &packet3);
						  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
						  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
						  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
						  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

						  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
						  app->db->setInt(1, plr->GetCharID());
						  app->db->execute();
						  app->db->fetch();

						  res->wOpCode = GU_AVATAR_WORLD_INFO;
						  res->byDojoCount = 0;
						  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
						  {
							  res->sDojoData[n].guildId = 0xffffffff;
							  res->sDojoData[n].dojoTblidx = 0xffffffff;
						  }
						  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
						  if (app->db->getInt("TutorialFlag") == 1)
						  {

							  res->worldInfo.tblidx = 400000;// app->db->getInt("WorldTable");
							  res->worldInfo.worldID = 400000;// app->db->getInt("WorldID");
							  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
							  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
							  res->vCurLoc.x = -186.520004;// app->db->getDouble("tlqlocX");
							  res->vCurLoc.y = 22.129999;// app->db->getDouble("tlqlocY");
							  res->vCurLoc.z = -24.150000;//app->db->getDouble("tlqlocZ");
							  res->vCurDir.x = 0.994937;// app->db->getDouble("tlqdirX");
							  res->vCurDir.y = 0.0;// app->db->getDouble("tlqdirY");
							  res->vCurDir.z = 0.100499;// app->db->getDouble("tlqdirZ");
							  //plr->SetWorldID(app->db->getInt("WorldID"));
							  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
						  }

						  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
						  g_pApp->Send(this->GetHandle(), &packet);
						  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
						  plr = NULL;
						  delete plr;

						   this->SendNpcTLQ3Create(pPacket, app);
						   this->SendMonsterTLQ3Create(pPacket, app);
					  }
				  }

			  }
			  // Começa entradas RP UD Dojos ETC
			  else if (req->byEventType == 255)//Is colision
			  {
				  printf("byEventType 2\n");
				  printf("res->byTsType = %d, res->dwParam = %d, res->tcCurId = %d, res->tcNextId = %d, res->tId = %d, req->byEventType = %d \n", res3->byTsType, res3->dwParam, res3->tcCurId, res3->tcNextId, res3->tId, req->byEventType);

				 // CNtlPacket packetQ(sizeof(sGU_CHAR_TELEPORT_RES));
				 // sGU_CHAR_TELEPORT_RES*as = (sGU_CHAR_TELEPORT_RES*)packetQ.GetPacketData();
				  //sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(2));
				  if (res3->tId == 601)
				  {
					  printf("Dojo Kokara 601 \n");
					  printf("Inicio Entrada Dojo Kokara \n");
					  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
					  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
					  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
					  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

					  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
					  app->db->setInt(1, plr->GetCharID());
					  app->db->execute();
					  app->db->fetch();

					  res->wOpCode = GU_AVATAR_WORLD_INFO;
					  res->byDojoCount = 0;
					  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
					  {
						  res->sDojoData[n].guildId = 0xffffffff;
						  res->sDojoData[n].dojoTblidx = 0xffffffff;
					  }
					  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
					  if (app->db->getInt("TutorialFlag") == 1)
					  {

						  res->worldInfo.tblidx = 504;// app->db->getInt("WorldTable");
						  res->worldInfo.worldID = 504;// app->db->getInt("WorldID");
						 // res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
						  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
						  res->vCurLoc.x = -62.130001;// app->db->getDouble("tlqlocX");
						  res->vCurLoc.y = 85.620003;// app->db->getDouble("tlqlocY");
						  res->vCurLoc.z = -14.160000;//app->db->getDouble("tlqlocZ");
						  res->vCurDir.x = 0.000000;// app->db->getDouble("tlqdirX");
						  res->vCurDir.y = 0.0;// app->db->getDouble("tlqdirY");
						  res->vCurDir.z = 1.000000;// app->db->getDouble("tlqdirZ");
						  //plr->SetWorldID(app->db->getInt("WorldID"));
						  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
					  }

					  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
					  g_pApp->Send(this->GetHandle(), &packet);
					  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
					  plr = NULL;
					  delete plr;
					  // this->SendNpcCreate(pPacket, app);
					  // this->SendMonsterCreate(pPacket, app);
				  }
				  //seguinte
				  else if (res3->tId == 602)
				  {
					  printf(" Saida Dojo Kokara \n");
					  printf("Saida dojo Kokara \n");
					  //printf("--- sGU_AVATAR_WORLD_INFO --- \n");
					  PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
					  CNtlPacket packet(sizeof(sGU_AVATAR_WORLD_INFO));
					  sGU_AVATAR_WORLD_INFO * res = (sGU_AVATAR_WORLD_INFO *)packet.GetPacketData();

					  app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
					  app->db->setInt(1, plr->GetCharID());
					  app->db->execute();
					  app->db->fetch();

					  res->wOpCode = GU_AVATAR_WORLD_INFO;
					  res->byDojoCount = 0;
					  for (int n = 0; n < DBO_MAX_COUNT_DOJO_IN_WORLD; n++)
					  {
						  res->sDojoData[n].guildId = 0xffffffff;
						  res->sDojoData[n].dojoTblidx = 0xffffffff;
					  }
					  //Uncomment this if lines to see the first tutorial...not working for now this method above is wrong...we need get from database - Luiz45
					  if (app->db->getInt("TutorialFlag") == 1)
					  {

						  res->worldInfo.tblidx = 1;// app->db->getInt("WorldTable");
						  res->worldInfo.worldID = 1;// app->db->getInt("WorldID");
						  res->worldInfo.hTriggerObjectOffset = 133000;//They added some new things then its 133000 instead 100000
						  res->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
						  res->vCurLoc.x = 4700;// app->db->getDouble("tlqlocX");
						  res->vCurLoc.y = 19.97;// app->db->getDouble("tlqlocY");
						  res->vCurLoc.z = -4500;//app->db->getDouble("tlqlocZ");
						  res->vCurDir.x = 0.98719;// app->db->getDouble("tlqdirX");
						  res->vCurDir.y = 0.0;// app->db->getDouble("tlqdirY");
						  res->vCurDir.z = -0;// app->db->getDouble("tlqdirZ");
						  //plr->SetWorldID(app->db->getInt("WorldID"));
						  //plr->SetWorldTblidx(app->db->getInt("WorldTable"));
					  }

					  packet.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
					  g_pApp->Send(this->GetHandle(), &packet);
					  PACKET_TRACE(GU_AVATAR_WORLD_INFO, packet);
					  plr = NULL;
					  delete plr;
					  // this->SendNpcCreate(pPacket, app);
					  // this->SendMonsterCreate(pPacket, app);
				  }
				  else if (res3->tcCurId == 2)
				  {
					  printf("tcCurId UD 2\n");
					  /*CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
					  sGU_UPDATE_CHAR_STATE * res3 = (sGU_UPDATE_CHAR_STATE *)packet3.GetPacketData();
					  res3->handle = this->GetavatarHandle();
					  res3->sCharState.sCharStateBase.vCurLoc.x = pWorldTbldat->vStart1Loc.x;
					  res3->sCharState.sCharStateBase.vCurLoc.y = pWorldTbldat->vStart1Loc.y;
					  res3->sCharState.sCharStateBase.vCurLoc.z = pWorldTbldat->vStart1Loc.z;
					  res3->sCharState.sCharStateBase.vCurDir.x = pWorldTbldat->vStart1Dir.x;
					  res3->sCharState.sCharStateBase.vCurDir.z = pWorldTbldat->vStart1Dir.z;
					  res3->sCharState.sCharStateDetail.sCharStateTeleporting.byTeleportType = TELEPORT_TYPE_DUNGEON;
					  res3->sCharState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
					  res3->wOpCode = GU_UPDATE_CHAR_STATE;

					  packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
					  g_pApp->Send(this->GetHandle(), &packet3);
					  printf("teleport sent");*/
				  }
			  }
	}
		break;
	default:
	{
			   printf("Unhandled Type, Server and Client Didn't Recognize this Trigger Type");
	}
		break;
	}
	//printf("res->byTsType = %d, res->dwParam = %d, res->tcCurId = %d, res->tcNextId = %d, res->tId = %d\n",res->byTsType, res->dwParam, res->tcCurId, res->tcNextId, res->tId); 
	plr = NULL;
	delete plr;
}
void	CClientSession::SendZennyPickUpReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_ZENNY_PICK_REQ* req = (sUG_ZENNY_PICK_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_ZENNY_PICK_RES));
	sGU_ZENNY_PICK_RES * res = (sGU_ZENNY_PICK_RES *)packet.GetPacketData();

	int amnt = app->FindZenny(req->handle);

	if (amnt > 0)
	{
		app->RemoveZenny(req->handle);
		res->bSharedInParty = false; //this->plr->isInParty();
		res->dwBonusZenny = rand() % 100;
		//res->dwOriginalZenny = amnt;
		plr->GetPcProfile()->dwZenny += (amnt + res->dwBonusZenny);
		res->dwZenny = plr->GetPcProfile()->dwZenny;
		res->dwAcquisitionZenny = amnt + res->dwBonusZenny;
		res->wResultCode = ZENNY_CHANGE_TYPE_PICK;

		app->qry->SetPlusMoney(plr->GetCharID(), res->dwAcquisitionZenny);

		CNtlPacket packet3(sizeof(sGU_OBJECT_DESTROY));
		sGU_OBJECT_DESTROY * res3 = (sGU_OBJECT_DESTROY *)packet3.GetPacketData();
		res3->handle = req->handle;
		res3->wOpCode = GU_OBJECT_DESTROY;
		packet3.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
		g_pApp->Send(this->GetHandle(), &packet3); 

		CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_ZENNY));
		sGU_UPDATE_CHAR_ZENNY * res5 = (sGU_UPDATE_CHAR_ZENNY *)packet5.GetPacketData();
		res5->dwZenny = plr->GetPcProfile()->dwZenny;//by analazying this is the ammount...				
		res5->bIsNew = true;
		res5->handle = this->GetavatarHandle();
		res5->byChangeType = 0;//never mind
		res5->wOpCode = GU_UPDATE_CHAR_ZENNY;
		packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ZENNY));
		g_pApp->Send(this->GetHandle(), &packet5);
	}
	else
	{
		res->wResultCode = GAME_FAIL;
	}
	res->wOpCode = GU_ZENNY_PICK_RES;
	packet.SetPacketLen(sizeof(sGU_ZENNY_PICK_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}

//Pick up yo shit yo
void	CClientSession::SendItemPickUpReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_ITEM_PICK_REQ* req = (sUG_ITEM_PICK_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_ITEM_PICK_RES));
	sGU_ITEM_PICK_RES * res = (sGU_ITEM_PICK_RES*)packet.GetPacketData();
	CGameServer::ITEMDROPEDFROMMOB* itemDropped = app->FindItemPickup(req->handle);
	printf("Pegar %d \n",req->handle);	

		res->itemTblidx = 11170019 + rand() % 261 + 1;
		res->wOpCode = GU_ITEM_PICK_RES;
		//printf(" Item %d\n"), res->itemTblidx;
	// define posiçao do item
		int Place = 1;// Bag vai de 1 a 5
		int Pos = rand()%14+1;	// slots livres bag 1 16 slots outras podem ir ate 32^^	
		int CharID = plr->GetCharID(); // ID personagem
		int count = 1; //numero de items
		// verifica se a posiçao esta ocupada
		if (app->qry->CheckIfCanMoveItemThere(plr->GetCharID(), Place, Pos) == false)
		{
			
			//res->wResultCode = GAME_MOVE_CANT_GO_THERE;
		}
		else //se estiver livre envia o item para a bag
		{
			app->qry->UpdateItemPlaceAndPos(res->itemTblidx, Place, Pos);
			app->db->prepare("INSERT INTO items (tblidx,owner_id,place,pos,count) VALUES (?,?,?,?,?) ");
			app->db->setInt(1, res->itemTblidx);
			app->db->setInt(2, CharID);
			app->db->setInt(3, Place);
			app->db->setInt(4, Pos);
			app->db->setInt(5, count);
			app->db->execute();
			//this->gsf->CreateUpdateItem(plr, 1, itemDropped->itemTblidx, false, this->GetHandle(), itemDropped->byGrade, itemDropped->byRank, itemDropped->byBattleAttribute);//Luiz, This needs to be changed i think. Not exactly sure why its doing what its doing.
			res->wResultCode = GAME_SUCCESS;
			//printf("Pega Item DataBase %d\n"), res->itemTblidx;
		}
		

		CNtlPacket packet3(sizeof(sGU_OBJECT_DESTROY));
		sGU_OBJECT_DESTROY * res3 = (sGU_OBJECT_DESTROY *)packet3.GetPacketData();
		res3->handle = req->handle;
		res3->wOpCode = GU_OBJECT_DESTROY;
		packet3.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
		g_pApp->Send(this->GetHandle(), &packet3);
		app->RemoveItemPickup(req->handle);
	
	res->wOpCode = GU_ITEM_PICK_RES;
	packet.SetPacketLen(sizeof(sGU_ITEM_PICK_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}



void	CClientSession::SendFreeBattleReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_FREEBATTLE_CHALLENGE_REQ* req = (sUG_FREEBATTLE_CHALLENGE_REQ *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_FREEBATTLE_CHALLENGE_RES));
	sGU_FREEBATTLE_CHALLENGE_RES * res = (sGU_FREEBATTLE_CHALLENGE_RES *)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_FREEBATTLE_ACCEPT_REQ));
	sGU_FREEBATTLE_ACCEPT_REQ * res2 = (sGU_FREEBATTLE_ACCEPT_REQ *)packet2.GetPacketData();

	PlayersMain* targeted = app->GetUserSession(req->hTarget);
	plr->SetCharIDForDuel(targeted->GetCharID());

	res->hTarget = req->hTarget;
	res->wOpCode = GU_FREEBATTLE_CHALLENGE_RES;
	res->wResultCode = GAME_SUCCESS;

	res2->hChallenger = plr->GetAvatarHandle();
	res2->wOpCode = GU_FREEBATTLE_ACCEPT_REQ;

	packet.SetPacketLen(sizeof(sGU_FREEBATTLE_CHALLENGE_RES));
	g_pApp->Send(this->GetHandle(), &packet);

	packet2.SetPacketLen(sizeof(sGU_FREEBATTLE_ACCEPT_REQ));
	g_pApp->Send(targeted->GetSession(), &packet2);
	app->UserBroadcastothers(&packet, this);
	plr = NULL;
	delete plr;
}
void	CClientSession::SendFreeBattleAccpetReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_FREEBATTLE_ACCEPT_RES* req = (sUG_FREEBATTLE_ACCEPT_RES *)pPacket->GetPacketData();

	if (req->byAccept == 1)
	{
		CNtlPacket packet2(sizeof(sGU_FREEBATTLE_START_NFY));
		sGU_FREEBATTLE_START_NFY * res2 = (sGU_FREEBATTLE_START_NFY *)packet2.GetPacketData();
		res2->hTarget = plr->GetAvatarHandle();
		res2->vRefreeLoc = plr->GetPlayerPosition();
		res2->vRefreeLoc.x += rand() % 10 + 5;
		res2->vRefreeLoc.z -= 2;
		res2->wOpCode = GU_FREEBATTLE_START_NFY;
		packet2.SetPacketLen(sizeof(sGU_FREEBATTLE_START_NFY));
		g_pApp->Send(this->GetHandle(), &packet2);
		app->UserBroadcastothers(&packet2, this);
	}
	else
	{
		CNtlPacket packet2(sizeof(sGU_FREEBATTLE_CANCEL_NFY));
		sGU_FREEBATTLE_CANCEL_NFY * res2 = (sGU_FREEBATTLE_CANCEL_NFY *)packet2.GetPacketData();
		res2->wOpCode = GU_FREEBATTLE_CANCEL_NFY;
		res2->wResultCode = GAME_FREEBATTLE_CHALLENGE_ACCEPT_DENIED;
		packet2.SetPacketLen(sizeof(sGU_FREEBATTLE_CANCEL_NFY));
		g_pApp->Send(this->GetHandle(), &packet2);
		app->UserBroadcastothers(&packet2, this);
	}
	plr = NULL;
	delete plr;
}
/////////////////////////////////////////////////////////////////////////////
//// item usage by Luiz45
/////////////////////////////////////////////////////////////////////////////
void	CClientSession::SendItemUseReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_ITEM_USE_REQ * req = (sUG_ITEM_USE_REQ*)pPacket->GetPacketData();

	//Defining Packets
	CNtlPacket packet(sizeof(sGU_ITEM_USE_RES));
	sGU_ITEM_USE_RES * res = (sGU_ITEM_USE_RES*)packet.GetPacketData();
	int item = 0;
	int CharID = plr->GetCharID();
	app->db->prepare("SELECT * FROM items WHERE owner_id = ? AND place = ? AND pos = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->byPlace);
	app->db->setInt(3, req->byPos);
	app->db->execute();
	app->db->fetch();
	item = app->db->getInt("tblidx");
	//Prepared Item Response	
	res->byPlace = req->byPlace;
	res->byPos = req->byPos;
	res->tblidxItem = item;
	res->wOpCode = GU_ITEM_USE_RES;
	res->wResultCode = 500;

	printf("tblidxItem res %d \n byPlace res %d \n  byPos res %d \n  hTarget req %d \n  byPlace req %d \n Pos req %d \n", res->tblidxItem, res->byPlace, res->byPos, req->hTarget, req->byPlace, req->byPos);

	CNtlPacket packet2(sizeof(sGU_CHAR_ACTION_ITEM));
	sGU_CHAR_ACTION_ITEM * pItemAct = (sGU_CHAR_ACTION_ITEM*)packet2.GetPacketData();

	CNtlPacket packet4(sizeof(sGU_BUFF_REGISTERED));
	sGU_BUFF_REGISTERED * pItemBuff = (sGU_BUFF_REGISTERED*)packet4.GetPacketData();
	//VehiclePackets
	CNtlPacket packet5(sizeof(sGU_VEHICLE_START_NFY));
	sGU_VEHICLE_START_NFY* pVehicleStart = (sGU_VEHICLE_START_NFY*)packet5.GetPacketData();

	CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* pVehicleAspcStateUpd = (sGU_UPDATE_CHAR_STATE*)packet6.GetPacketData();

	CNtlPacket packet7(sizeof(sGU_UPDATE_CHAR_LP_STATUS_NFY));
	sGU_UPDATE_CHAR_LP_STATUS_NFY* pVehicleAspect = (sGU_UPDATE_CHAR_LP_STATUS_NFY*)packet7.GetPacketData();
	//KidClock
	CNtlPacket packet8(sizeof(sGU_CHILD_ADULT_SWITCHED_NFY));
	sGU_CHILD_ADULT_SWITCHED_NFY* adult = (sGU_CHILD_ADULT_SWITCHED_NFY*)packet8.GetPacketData();
	//------------------------------------------------------------------------------------------------//
	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	app->db->fetch();
	int level = app->db->getInt("Level");
	if (res->tblidxItem == 11120154)
	{
		if (level >= 30)
		{

			//DST_CHAR_GROWN_DOWN = "PARAB�NS! Voc??uma crian�a!"
			//	DST_CHAR_GROWN_UP = "Voc??um Adulto!"
			//int CharID = plr->GetCharID();

			adult->bIsAdult = app->db->getBoolean("Adult");

			printf("Adulto %d \n", adult->bIsAdult);

			//pVehicleAspect->handle = this->GetavatarHandle();
			//pVehicleAspect->bEmergency = true;
			//pVehicleAspect->wOpCode = GU_UPDATE_CHAR_LP_STATUS_NFY;
			//pVehicleAspect->wResultCode = 500;
			if (adult->bIsAdult == 0) //se for crian�a converte em adulto
			{
				printf("Adulto %d \n", adult->bIsAdult);
				adult->hSubject = this->GetavatarHandle();
				adult->bIsAdult = 1;//transforma em adulto
				printf("Adulto %d \n", adult->bIsAdult);
				adult->wOpCode = GU_CHILD_ADULT_SWITCHED_NFY;//evnvia o packet
				adult->wResultCode = 500;//envia resultado
				//actualiza a base de dados
				app->db->prepare("UPDATE characters SET Adult=?  WHERE CharID=?");
				app->db->setInt(1, adult->bIsAdult);
				app->db->setInt(2, plr->GetCharID());
				app->db->execute();

				sUG_ITEM_DELETE_REQ * req1 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet1(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res1 = (sGU_ITEM_DELETE_RES *)packet1.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->byPlace);
				app->db->setInt(3, req->byPos);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res1->wOpCode = GU_ITEM_DELETE_RES;
				res1->wResultCode = GAME_SUCCESS;
				res1->byPlace = req1->bySrcPlace;
				res1->byPos = req1->bySrcPos;

				packet1.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet1);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

				res2->bySrcPlace = item_place;
				res2->bySrcPos = item_pos;
				res2->hSrcItem = u_itemid;
				res2->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet2);
				plr = NULL;
				delete plr;

			}
			else if (adult->bIsAdult == 1)	//ser for adulto converte em crian�a
			{

				adult->hSubject = this->GetavatarHandle();
				adult->bIsAdult = 0;
				printf("Crian�a %d \n", adult->bIsAdult);
				adult->wOpCode = GU_CHILD_ADULT_SWITCHED_NFY;
				adult->wResultCode = 500;

				app->db->prepare("UPDATE characters SET Adult=?  WHERE CharID=?");
				app->db->setInt(1, adult->bIsAdult);
				app->db->setInt(2, plr->GetCharID());
				app->db->execute();

				sUG_ITEM_DELETE_REQ * req3 = (sUG_ITEM_DELETE_REQ*)pPacket->GetPacketData();
				//PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
				CNtlPacket packet3(sizeof(sGU_ITEM_DELETE_RES));
				sGU_ITEM_DELETE_RES * res3 = (sGU_ITEM_DELETE_RES *)packet3.GetPacketData();

				app->db->prepare("SELECT id,place,pos FROM items WHERE owner_id=? AND place=? AND pos=?");
				app->db->setInt(1, plr->GetCharID());
				app->db->setInt(2, req->byPlace);
				app->db->setInt(3, req->byPos);
				app->db->execute();
				app->db->fetch();

				RwUInt32 u_itemid = app->db->getInt("id");
				RwUInt32 item_place = app->db->getInt("place");
				RwUInt32 item_pos = app->db->getInt("pos");

				res3->wOpCode = GU_ITEM_DELETE_RES;
				res3->wResultCode = GAME_SUCCESS;
				res3->byPlace = req3->bySrcPlace;
				res3->byPos = req3->bySrcPos;

				packet3.SetPacketLen(sizeof(sGU_ITEM_DELETE_RES));
				g_pApp->Send(this->GetHandle(), &packet3);

				// DELETE ITEM
				app->qry->DeleteItemById(u_itemid);

				CNtlPacket packet4(sizeof(sGU_ITEM_DELETE));
				sGU_ITEM_DELETE * res4 = (sGU_ITEM_DELETE *)packet4.GetPacketData();

				res4->bySrcPlace = item_place;
				res4->bySrcPos = item_pos;
				res4->hSrcItem = u_itemid;
				res4->wOpCode = GU_ITEM_DELETE;
				plr->cPlayerInventory->RemoveItemFromInventory(u_itemid);

				packet4.SetPacketLen(sizeof(sGU_ITEM_DELETE));
				g_pApp->Send(this->GetHandle(), &packet4);
				plr = NULL;
				delete plr;
			}

		}
		//verifica�ao de lvl kid clock termina
		else // se for menor que level 30 falha
		{
			res->wResultCode = GAME_ITEM_NEED_MORE_LEVEL;
		}
	}

	else if (res->tblidxItem == 11120117)
	{
		if (level <= 29)
		{

			CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_SP));
			sGU_UPDATE_CHAR_SP * res2 = (sGU_UPDATE_CHAR_SP *)packet2.GetPacketData();
			plr->GetPcProfile()->dwCurExp -= plr->GetPcProfile()->dwMaxExpInThisLevel;
			plr->GetPcProfile()->dwMaxExpInThisLevel += (plr->GetPcProfile()->dwMaxExpInThisLevel * 1);
			CNtlPacket packet1(sizeof(sGU_UPDATE_CHAR_LEVEL));
			sGU_UPDATE_CHAR_LEVEL * response1 = (sGU_UPDATE_CHAR_LEVEL*)packet1.GetPacketData();
			plr->GetPcProfile()->byLevel = 30;
			response1->byCurLevel = plr->GetPcProfile()->byLevel;
			response1->byPrevLevel = plr->GetPcProfile()->byLevel - 1;
			response1->dwMaxExpInThisLevel = plr->GetPcProfile()->dwMaxExpInThisLevel;
			response1->handle = plr->GetAvatarHandle();
			response1->wOpCode = GU_UPDATE_CHAR_LEVEL;
			packet1.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LEVEL));
			g_pApp->Send(this->GetHandle(), &packet1);
			plr->SetLevelUP();
			//plr->cPlayerAttribute->UpdateAvatarAttributes(plr->GetAvatarHandle());
			plr->GetPcProfile()->dwSpPoint = 30;
			app->qry->UpdateSPPoint(plr->GetCharID(), plr->GetPcProfile()->dwSpPoint);
			app->qry->UpdatePlayerLevel(plr->GetPcProfile()->byLevel, plr->GetCharID(), plr->GetPcProfile()->dwCurExp, plr->GetPcProfile()->dwMaxExpInThisLevel);
			//response->dwCurExp = plr->GetPcProfile()->dwCurExp;
			plr->SetRPBall();
			plr->SendRpBallInformation();
			res2->wOpCode = GU_UPDATE_CHAR_SP;
			res2->dwSpPoint = plr->GetPcProfile()->dwSpPoint;
			packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_SP));
			g_pApp->Send(this->GetHandle(), &packet2);

			CNtlPacket packet4(sizeof(sGU_UPDATE_CHAR_STATE));
			sGU_UPDATE_CHAR_STATE* res4 = (sGU_UPDATE_CHAR_STATE*)packet4.GetPacketData();
			res4->handle = this->GetavatarHandle();
			res4->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
			res4->wOpCode = GU_UPDATE_CHAR_STATE;
			packet4.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
			g_pApp->Send(this->GetHandle(), &packet4);
		}
		else
		{
			res->wResultCode = GAME_ITEM_NOT_READY_TO_BE_USED;
		}
	}
	else//se nao for kid clock id falha
	{
		res->wResultCode = GAME_ITEM_NOT_READY_TO_BE_USED;
	}

	packet.SetPacketLen(sizeof(sGU_ITEM_USE_RES));
	packet5.SetPacketLen(sizeof(sGU_VEHICLE_START_NFY));
	packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	packet7.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_STATUS_NFY));
	packet8.SetPacketLen(sizeof(sGU_CHILD_ADULT_SWITCHED_NFY));

	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet5, this);
	g_pApp->Send(this->GetHandle(), &packet6);
	g_pApp->Send(this->GetHandle(), &packet7);
	g_pApp->Send(this->GetHandle(), &packet8);


	plr = NULL;
	delete plr;
}
//---------------------------------------------------------------------//
//-----------------------Item Upgrade - Kalisto------------------------//
//---------------------------------------------------------------------//
void	CClientSession::SendItemUpgradeReq(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_ITEM_UPGRADE_REQ* req = (sUG_ITEM_UPGRADE_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_ITEM_UPGRADE_RES));
	sGU_ITEM_UPGRADE_RES * res = (sGU_ITEM_UPGRADE_RES*)packet.GetPacketData();
	
	//Get Needed Tables
	CItemTable* itemTbl = app->g_pTableContainer->GetItemTable();
	CItemOptionTable* itemOptTbl = app->g_pTableContainer->GetItemOptionTable();
	CItemUpgradeTable* iUpgradeTbl = app->g_pTableContainer->GetItemUpgradeTable();


	/*res->wOpCode = GU_ITEM_UPGRADE_RES;
	packet.SetPacketLen(sizeof(sGU_ITEM_UPGRADE_RES));
	g_pApp->Send(this->GetHandle(), &packet);*/
	plr = NULL;
	delete plr;
}
//---------------------------------------------------------------------//
//------------------Skill Transform Cancel - Luiz45--------------------//
//---------------------------------------------------------------------//
void CClientSession::SendCharSkillTransformCancel(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	//Response Skill
	sUG_TRANSFORM_CANCEL_REQ * req = (sUG_TRANSFORM_CANCEL_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_TRANSFORM_CANCEL_RES));
	sGU_TRANSFORM_CANCEL_RES * res = (sGU_TRANSFORM_CANCEL_RES*)packet.GetPacketData();
	res->wOpCode = GU_TRANSFORM_CANCEL_RES;
	res->wResultCode = GAME_SUCCESS;

	//Update Char State
	CNtlPacket packet2(sizeof(sGU_UPDATE_CHAR_ASPECT_STATE));
	sGU_UPDATE_CHAR_ASPECT_STATE * myPlayerState = (sGU_UPDATE_CHAR_ASPECT_STATE*)packet2.GetPacketData();
	myPlayerState->handle = plr->GetAvatarHandle();
	myPlayerState->wOpCode = GU_UPDATE_CHAR_ASPECT_STATE;
	myPlayerState->aspectState.sAspectStateBase.byAspectStateId = 255;//Don't see any Const then i send 0 because i'm not going transform ^^

	//Packets Sending
	packet.SetPacketLen(sizeof(sGU_TRANSFORM_CANCEL_RES));
	packet2.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ASPECT_STATE));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);

	//Sending to others
	app->UserBroadcastothers(&packet, this);
	app->UserBroadcastothers(&packet2, this);
	plr = NULL;
	delete plr;
}


void CClientSession::SendSocialSkillRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_SOCIAL_ACTION * req = (sUG_SOCIAL_ACTION*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_SOCIAL_ACTION));
	sGU_SOCIAL_ACTION* res = (sGU_SOCIAL_ACTION*)packet.GetPacketData();

	res->hSubject = this->GetavatarHandle();
	res->socialActionId = req->socialActionId;
	res->wOpCode = GU_SOCIAL_ACTION;

	packet.SetPacketLen(sizeof(sGU_SOCIAL_ACTION));
	//g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
	//this->gsf->printOk("Sended");
}
DWORD WINAPI	SendRpChargethread(LPVOID arg)
{
	CClientSession* session = (CClientSession*)arg;
	bool isRpBall = false;
	while (42)
	{
		CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_RP));
		sGU_UPDATE_CHAR_RP * res3 = (sGU_UPDATE_CHAR_RP *)packet3.GetPacketData();

		session->cPlayersMain->GetPcProfile()->wCurRP += 1;
		if (session->cPlayersMain->GetRpBallCounter() >= 1)
		{
			res3->wMaxRP = (session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP / session->cPlayersMain->GetRpBallCounter());
			isRpBall = true;
			if (session->cPlayersMain->GetPcProfile()->wCurRP > (session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP / session->cPlayersMain->GetRpBallCounter()))
				session->cPlayersMain->GetPcProfile()->wCurRP = session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP / session->cPlayersMain->GetRpBallCounter();
		}
		else
		{
			res3->wMaxRP = session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP;
			isRpBall = false;
			if (session->cPlayersMain->GetPcProfile()->wCurRP > session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP)
				session->cPlayersMain->GetPcProfile()->wCurRP = session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP;
			res3->bHitDelay = false;
		}
		if (isRpBall == true)
		{
			if (session->cPlayersMain->GetPcProfile()->wCurRP >= (session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP / session->cPlayersMain->GetRpBallCounter()))
			{
				if (session->cPlayersMain->GetRpBallFull() < 8 && session->cPlayersMain->GetRpBallFull() < session->cPlayersMain->GetRpBallCounter())
				{
					res3->bHitDelay = true;
					session->cPlayersMain->GetPcProfile()->wCurRP = 0;
					session->cPlayersMain->SendRpBallUpdate(0); // 0 for increase ! 1 for decrease !
				}
				else
				{
					res3->bHitDelay = false;
					session->cPlayersMain->GetPcProfile()->wCurRP = (session->cPlayersMain->GetPcProfile()->avatarAttribute.wBaseMaxRP / session->cPlayersMain->GetRpBallCounter());
				}
			}

		}
		res3->handle = session->GetavatarHandle();
		res3->wCurRP = session->cPlayersMain->GetPcProfile()->wCurRP;

		res3->wOpCode = GU_UPDATE_CHAR_RP;
		packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP));
		g_pApp->Send(session->cPlayersMain->GetSession(), &packet3);
		Sleep(5);
	}
}
void CClientSession::SendRpCharge(CNtlPacket *pPacket, CGameServer * app)
{
	sUG_CHAR_CHARGE * req = (sUG_CHAR_CHARGE*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	if (req->bCharge)
	{
		CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
		sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
		CNtlPacket packet2(sizeof(sGU_AVATAR_RP_INCREASE_START_NFY));
		sGU_AVATAR_RP_INCREASE_START_NFY * res2 = (sGU_AVATAR_RP_INCREASE_START_NFY *)packet2.GetPacketData();
		CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_MAX_RP));
		sGU_UPDATE_CHAR_MAX_RP * res3 = (sGU_UPDATE_CHAR_MAX_RP *)packet3.GetPacketData();

		res->handle = this->GetavatarHandle();
		memcpy(&res->sCharState, plr->GetCharState(), sizeof(sCHARSTATE));
		res->sCharState.sCharStateBase.byStateID = CHARSTATE_CHARGING;
		res->wOpCode = GU_UPDATE_CHAR_STATE;
		res2->wOpCode = GU_AVATAR_RP_INCREASE_START_NFY;
		res3->hSubject = this->GetavatarHandle();
		res3->wMaxRp = plr->GetPcProfile()->avatarAttribute.wBaseMaxRP;
		res3->wOpCode = GU_UPDATE_CHAR_MAX_RP;
		packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
		g_pApp->Send(this->GetHandle(), &packet);
		packet2.SetPacketLen(sizeof(sGU_AVATAR_RP_INCREASE_START_NFY));
		g_pApp->Send(this->GetHandle(), &packet2);
		packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_MAX_RP));
		g_pApp->Send(this->GetHandle(), &packet3);

		plr->SetChargingID(app->ThreadRequest());
		DWORD threadID = plr->GetChargingID();
		plr->SetChargindThread(CreateThread(NULL, 0, SendRpChargethread, (LPVOID)this, 0, &threadID));//Dont know why i need cast...when atidote done he didnt casted
		if (plr->GetChargingThread() == NULL)
			printf("Can't create thread charging\n");
		app->UserBroadcastothers(&packet, this);
	}
	if (req->bCharge == false)
	{
		if (TerminateThread(plr->GetChargingThread(), 1) == 0)
			printf("Can't kill thread\n");
		else
			CloseHandle(plr->GetChargingThread());
		CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
		sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
		CNtlPacket packet2(sizeof(sGU_AVATAR_RP_INCREASE_STOP_NFY));
		sGU_AVATAR_RP_INCREASE_STOP_NFY * res2 = (sGU_AVATAR_RP_INCREASE_STOP_NFY *)packet2.GetPacketData();

		res->handle = this->GetavatarHandle();
		memcpy(&res->sCharState, plr->GetCharState(), sizeof(sCHARSTATE));
		res->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
		res->wOpCode = GU_UPDATE_CHAR_STATE;
		res2->wOpCode = GU_AVATAR_RP_INCREASE_STOP_NFY;
		packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
		g_pApp->Send(this->GetHandle(), &packet);
		packet2.SetPacketLen(sizeof(sGU_AVATAR_RP_INCREASE_STOP_NFY));
		g_pApp->Send(this->GetHandle(), &packet2);
		app->UserBroadcastothers(&packet, this);
		CNtlPacket packet5(sizeof(sGU_AVATAR_RP_DECREASE_START_NFY));
		sGU_AVATAR_RP_DECREASE_START_NFY * res5 = (sGU_AVATAR_RP_DECREASE_START_NFY *)packet5.GetPacketData();
		res5->wOpCode = GU_AVATAR_RP_DECREASE_START_NFY;
		packet5.SetPacketLen(sizeof(sGU_AVATAR_RP_DECREASE_START_NFY));
		g_pApp->Send(this->GetHandle(), &packet5);
	}
	plr = NULL;
	delete plr;
}
//-----------------------------------------------------------------//
//-------------------Skill/Item BUFF Drop--------------------------//
//-----------------------------------------------------------------//
void CClientSession::SendCharSkillBuffDrop(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_BUFF_DROP_REQ * req = (sUG_BUFF_DROP_REQ*)pPacket->GetPacketData();
	//Get Skill to Remove
	//CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	//sSKILL_RESULT * pSkillData = (sSKILL_RESULT*)pPacket->GetPacketData();

	//Response Prepare
	CNtlPacket packet(sizeof(sGU_BUFF_DROP_RES));
	sGU_BUFF_DROP_RES * res = (sGU_BUFF_DROP_RES*)packet.GetPacketData();
	res->wOpCode = GU_BUFF_DROP_RES;
	res->wResultCode = GAME_SUCCESS;

	//Dropp Event Prepare
	CNtlPacket packet2(sizeof(sGU_BUFF_DROPPED));
	sGU_BUFF_DROPPED * pBuffDrop = (sGU_BUFF_DROPPED*)packet2.GetPacketData();
	pBuffDrop->hHandle = this->GetavatarHandle();
	pBuffDrop->bySourceType = DBO_OBJECT_SOURCE_SKILL;//Need be rechecked because this can be a type DBO_OBJECT_SOURCE_ITEM
	pBuffDrop->wOpCode = GU_BUFF_DROPPED;
	pBuffDrop->tblidx = req->tblidx;

	//First Drop,Second Resp to client
	packet2.SetPacketLen(sizeof(sGU_BUFF_DROPPED));
	packet.SetPacketLen(sizeof(sGU_BUFF_DROP_RES));
	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet2, this);
	app->UserBroadcastothers(&packet, this);
}
//-----------------------------------------------------------//
//-----------HTB SKILL REQ/RES  Luiz45  ---------------------//
//-----------------------------------------------------------//
void CClientSession::SendHTBStartReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_HTB_START_REQ * req = (sUG_HTB_START_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	app->db->prepare("SELECT * FROM skills WHERE owner_id = ? AND SlotID= ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->setInt(2, req->bySkillSlot);
	app->db->execute();
	app->db->fetch();

	CNtlPacket packet(sizeof(sGU_HTB_START_RES));
	sGU_HTB_START_RES * resp = (sGU_HTB_START_RES*)packet.GetPacketData();

	int skillTbl = app->db->getInt("skill_id");
	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	sHTB_SET_TBLDAT *pHTBSetTblData = reinterpret_cast<sHTB_SET_TBLDAT*>(pSkillTable->FindData(skillTbl));

	resp->wOpCode = GU_HTB_START_RES;
	resp->wResultCode = GAME_SUCCESS;
	resp->bySkillSlot = req->bySkillSlot;

	packet.SetPacketLen(sizeof(sGU_HTB_START_RES));

	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
	//Initiate our HTB Skill
	SendCharUpdateHTBState(skillTbl, app);
	plr = NULL;
	delete plr;
}
//-----------------------------------------------------------//
//-----------HTB States/Steps   Luiz45  ---------------------//
//-----------------------------------------------------------//
void CClientSession::SendCharUpdateHTBState(int SkillID, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * respA = (sGU_UPDATE_CHAR_STATE*)packet.GetPacketData();

	//Setting Our Location
	respA->wOpCode = GU_UPDATE_CHAR_STATE;
	respA->handle = this->GetavatarHandle();
	respA->sCharState.sCharStateBase.byStateID = CHARSTATE_HTB;
	respA->sCharState.sCharStateBase.vCurLoc.x = plr->GetCharState()->sCharStateBase.vCurLoc.x;
	respA->sCharState.sCharStateBase.vCurLoc.y = plr->GetCharState()->sCharStateBase.vCurLoc.y;
	respA->sCharState.sCharStateBase.vCurLoc.z = plr->GetCharState()->sCharStateBase.vCurLoc.z;
	respA->sCharState.sCharStateBase.vCurDir.x = plr->GetCharState()->sCharStateBase.vCurDir.x;
	respA->sCharState.sCharStateBase.vCurDir.y = plr->GetCharState()->sCharStateBase.vCurDir.y;
	respA->sCharState.sCharStateBase.vCurDir.z = plr->GetCharState()->sCharStateBase.vCurDir.z;

	CHTBSetTable *pHTBSetTbl = app->g_pTableContainer->GetHTBSetTable();
	sHTB_SET_TBLDAT *pHTBSetTblData = reinterpret_cast<sHTB_SET_TBLDAT*>(pHTBSetTbl->FindData(SkillID));

	respA->sCharState.sCharStateDetail.sCharStateHTB.hTarget = m_uiTargetSerialId;
	respA->sCharState.sCharStateDetail.sCharStateHTB.byStepCount = pHTBSetTblData->bySetCount;
	respA->sCharState.sCharStateDetail.sCharStateHTB.byCurStep = 0;
	respA->sCharState.sCharStateDetail.sCharStateHTB.byResultCount = 0;
	respA->sCharState.sCharStateDetail.sCharStateHTB.HTBId = pHTBSetTblData->tblidx;
	CSkillTable *pSkillTbl = app->g_pTableContainer->GetSkillTable();
	sSKILL_TBLDAT *pSkillTblData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTbl->FindData(SkillID));

	//Extract from Client Code
	RwInt8 byResultCount = 0;
	for (RwInt32 i = 0; i < pHTBSetTblData->bySetCount; ++i)
	{
		if (pHTBSetTblData->aHTBAction[i].skillTblidx != INVALID_TBLIDX)
		{
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].byStep = i;
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.byAttackResult = BATTLE_ATTACK_RESULT_HIT;
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.effectResult1.fResultValue = pSkillTblData->fSkill_Effect_Value[0];
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.effectResult2.fResultValue = pSkillTblData->fSkill_Effect_Value[1];
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.vShift.x = 0.0f;
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.vShift.y = 0.0f;
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.vShift.z = 0.0f;
			respA->sCharState.sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.byBlockedAction = 255;

			plr->GetCharState()->sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.effectResult1.fResultValue = pSkillTblData->fSkill_Effect_Value[0];
			plr->GetCharState()->sCharStateDetail.sCharStateHTB.aHTBSkillResult[byResultCount].sSkillResult.effectResult2.fResultValue = pSkillTblData->fSkill_Effect_Value[1];
			respA->sCharState.sCharStateDetail.sCharStateHTB.byResultCount++;

			byResultCount++;
		}
	}
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
	plr = NULL;
	delete plr;
}
//-----------------------------------------------------------//
//SendBagState Only Display RP Choice for PVP Luiz45  -------//
//-----------------------------------------------------------//
void CClientSession::SendHTBSendbagState(CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	//Extract from client codes
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * respA = (sGU_UPDATE_CHAR_STATE*)packet.GetPacketData();
	respA->wOpCode = GU_UPDATE_CHAR_STATE;
	respA->handle = this->GetavatarHandle();
	respA->sCharState.sCharStateBase.byStateID = CHARSTATE_SANDBAG;
	respA->sCharState.sCharStateBase.vCurLoc.x = plr->GetCharState()->sCharStateBase.vCurLoc.x;
	respA->sCharState.sCharStateBase.vCurLoc.y = plr->GetCharState()->sCharStateBase.vCurLoc.y;
	respA->sCharState.sCharStateBase.vCurLoc.z = plr->GetCharState()->sCharStateBase.vCurLoc.z;
	respA->sCharState.sCharStateBase.vCurDir.x = plr->GetCharState()->sCharStateBase.vCurDir.x;
	respA->sCharState.sCharStateBase.vCurDir.y = plr->GetCharState()->sCharStateBase.vCurDir.y;
	respA->sCharState.sCharStateBase.vCurDir.z = plr->GetCharState()->sCharStateBase.vCurDir.z;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
	plr = NULL;
	delete plr;
}
//-----------------------------------------------------------//
//SendHTBRpBall Player vs Player HTB Choice   Luiz45  -------//
//-----------------------------------------------------------//
void CClientSession::SendHTBRpBall(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_HTB_RP_BALL_USE_REQ * req = (sUG_HTB_RP_BALL_USE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_HTB_RP_BALL_USE_RES));
	sGU_HTB_RP_BALL_USE_RES * res1 = (sGU_HTB_RP_BALL_USE_RES*)packet.GetPacketData();

	res1->byRpBallCount = req->byRpBallCount;
	res1->wOpCode = GU_HTB_RP_BALL_USE_RES;
	res1->wResultCode = GAME_SUCCESS;

	CNtlPacket packet2(sizeof(sGU_HTB_RP_BALL_USED_NFY));
	sGU_HTB_RP_BALL_USED_NFY * res2 = (sGU_HTB_RP_BALL_USED_NFY*)packet2.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_HTB_RP_BALL_RESULT_DECIDED_NFY));
	sGU_HTB_RP_BALL_RESULT_DECIDED_NFY * res3 = (sGU_HTB_RP_BALL_RESULT_DECIDED_NFY*)packet3.GetPacketData();

	res2->byRpBallCount = req->byRpBallCount;
	res2->hSubject = this->GetavatarHandle();
	res2->wOpCode = GU_HTB_RP_BALL_USED_NFY;

	res3->hAttacker = this->GetavatarHandle();
	res3->hWinner = this->GetavatarHandle();
	res3->byTargetRpBallUsed = req->byRpBallCount;
	res3->wOpCode = GU_HTB_RP_BALL_RESULT_DECIDED_NFY;

	packet2.SetPacketLen(sizeof(sGU_HTB_RP_BALL_USED_NFY));
	packet3.SetPacketLen(sizeof(sGU_HTB_RP_BALL_RESULT_DECIDED_NFY));
	packet.SetPacketLen(sizeof(sGU_HTB_RP_BALL_USE_RES));
	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet3);
	g_pApp->Send(this->GetHandle(), &packet);

	app->UserBroadcastothers(&packet2, this);
	app->UserBroadcastothers(&packet3, this);
	app->UserBroadcastothers(&packet, this);

}
//-----------------------------------------------------------//
//-----------Advance Steps HTB? Luiz45  ---------------------//
//-----------------------------------------------------------//
void CClientSession::SendHTBFoward(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_HTB_FORWARD_RES));
	sGU_HTB_FORWARD_RES * res = (sGU_HTB_FORWARD_RES*)packet.GetPacketData();

	float newLP = 0;
	int stepNow = plr->GetCharState()->sCharStateDetail.sCharStateHTB.byStepCount;

	if (IsMonsterInsideList(m_uiTargetSerialId) == true)
	{
		/*MobActivity::CreatureData *lol = app->mob->GetMobByHandle(m_uiTargetSerialId);
		if (lol != NULL)
		{
		lol->FightMode = true;
		newLP = (float)lol->CurLP;
		newLP -= plr->GetCharState()->sCharStateDetail.sCharStateHTB.aHTBSkillResult[stepNow].sSkillResult.effectResult1.DD_DOT_fDamage + 100;
		printf("LP: %f, damage: %f\n", newLP, plr->GetCharState()->sCharStateDetail.sCharStateHTB.aHTBSkillResult[stepNow].sSkillResult.effectResult1.DD_DOT_fDamage + 100);
		if (newLP <= 0 || (newLP > lol->MaxLP))
		{
		lol->IsDead = true;
		CClientSession::SendMobLoot(&packet, app, m_uiTargetSerialId);
		this->gsf->printOk("DIE MOTHER FUCKER");
		SendCharUpdateFaintingState(&packet, app, this->GetavatarHandle(), m_uiTargetSerialId);
		}
		else if (newLP > 0 && lol->IsDead == false)
		{
		SendCharUpdateLp(&packet, app, newLP, m_uiTargetSerialId);
		}
		}*/
	}

	plr->GetCharState()->sCharStateDetail.sCharStateHTB.byStepCount++;
	res->wOpCode = GU_HTB_FORWARD_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_HTB_FORWARD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
	plr = NULL;
	delete plr;
}
//-----------------------------------------------------------//
//-----------GMT UPDATES        Luiz45  ---------------------//
//-----------------------------------------------------------//
void CClientSession::SendGmtUpdateReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_GMT_UPDATE_REQ * req = (sUG_GMT_UPDATE_REQ*)pPacket->GetPacketData();
	sGAME_MANIA_TIME * gmMania = (sGAME_MANIA_TIME*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_GMT_UPDATE_RES));
	sGU_GMT_UPDATE_RES * res = (sGU_GMT_UPDATE_RES*)packet.GetPacketData();
	res->wOpCode = GU_GMT_UPDATE_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_GMT_UPDATE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
}

void CClientSession::SendFogOfWarRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_WAR_FOG_UPDATE_REQ * req = (sUG_WAR_FOG_UPDATE_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_WAR_FOG_UPDATE_RES));
	sGU_WAR_FOG_UPDATE_RES * res = (sGU_WAR_FOG_UPDATE_RES*)packet.GetPacketData();
	res->handle = req->hObject;
	res->wOpCode = GU_WAR_FOG_UPDATE_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_WAR_FOG_UPDATE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	//CObjectTable* myObjTbl = app->g_pTableContainer->GetObjectTable(1);
	//sOBJECT_TBLDAT* pOBJECT_TBLDAT = reinterpret_cast<sOBJECT_TBLDAT*>(myObjTbl->FindData(res->handle));
	//printf("Tblidx for Warfog %d",pOBJECT_TBLDAT->tblidx);
	//printf("Name for Warfog %s", pOBJECT_TBLDAT->dwName);

	app->db->setInt(1, plr->GetCharID());
	//if (req->hObject - 100006 >=1)
	//	app->db->setInt(2, req->hObject - 100006);
	//else
	//app->db->setInt(2, req->hObject);
	app->db->execute();
	plr = NULL;
	delete plr;
}

void CClientSession::SendRideOnBusRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_RIDE_ON_BUS_REQ * req = (sUG_RIDE_ON_BUS_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sUG_RIDE_ON_BUS_REQ));
	sGU_RIDE_ON_BUS_RES * res = (sGU_RIDE_ON_BUS_RES *)packet.GetPacketData();

	res->hTarget = req->hTarget;
	res->wOpCode = GU_RIDE_ON_BUS_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_RIDE_ON_BUS_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}

void CClientSession::SendRideOffBusRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_RIDE_OFF_BUS_REQ * req = (sUG_RIDE_OFF_BUS_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sUG_RIDE_OFF_BUS_REQ));
	sGU_RIDE_OFF_BUS_RES * res = (sGU_RIDE_OFF_BUS_RES *)packet.GetPacketData();

	res->wOpCode = GU_RIDE_OFF_BUS_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_RIDE_OFF_BUS_RES));;
	g_pApp->Send(this->GetHandle(), &packet);
}

void CClientSession::SendCharDashKeyBoard(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_CHAR_DASH_KEYBOARD * req = (sUG_CHAR_DASH_KEYBOARD*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_EP));
	sGU_UPDATE_CHAR_EP * res = (sGU_UPDATE_CHAR_EP*)packet.GetPacketData();

	//Response
	plr->GetPcProfile()->wCurEP = (plr->GetPcProfile()->wCurEP - 50);
	plr->GetCharState()->sCharStateBase.vCurDir.x = req->vCurDir.x;
	plr->GetCharState()->sCharStateBase.vCurDir.z = req->vCurDir.z;
	plr->GetCharState()->sCharStateBase.vCurDir.y = req->vCurLoc.y;
	plr->GetCharState()->sCharStateBase.vCurLoc.x = req->vCurLoc.x;
	plr->GetCharState()->sCharStateBase.vCurLoc.z = req->vCurLoc.z;
	plr->GetCharState()->sCharStateBase.vCurLoc.y = req->vCurLoc.y;
	res->handle = this->GetavatarHandle();
	res->wOpCode = GU_UPDATE_CHAR_EP;
	res->wCurEP = plr->GetPcProfile()->wCurEP;//Or some number to get right xD
	res->wMaxEP = plr->GetPcProfile()->avatarAttribute.wBaseMaxEP;

	//this->plr->SavePlayerData();//Always save after update ? or no?

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EP));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;

}
void CClientSession::SendCharDashMouse(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_CHAR_DASH_MOUSE * req = (sUG_CHAR_DASH_MOUSE*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_EP));
	sGU_UPDATE_CHAR_EP * res = (sGU_UPDATE_CHAR_EP*)packet.GetPacketData();

	//Response
	plr->GetPcProfile()->wCurEP = (plr->GetPcProfile()->wCurEP - 50);
	plr->GetCharState()->sCharStateBase.vCurLoc.x = req->vDestLoc.x;
	plr->GetCharState()->sCharStateBase.vCurLoc.z = req->vDestLoc.z;
	plr->GetCharState()->sCharStateBase.vCurLoc.y = req->vDestLoc.y;
	res->handle = this->GetavatarHandle();
	res->wOpCode = GU_UPDATE_CHAR_EP;
	res->wCurEP = plr->GetPcProfile()->wCurEP;//Or some number to get right xD
	res->wMaxEP = plr->GetPcProfile()->avatarAttribute.wBaseMaxEP;

	//this->plr->SavePlayerData();//Always save after update ? or no?

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EP));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
void CClientSession::SendBusLocation(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_BUS_LOCATION_NFY));
	sGU_BUS_LOCATION_NFY * res = (sGU_BUS_LOCATION_NFY *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNPCTable* busNpc = app->g_pTableContainer->GetNpcTable();
	CSpawnTable* pSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable(plr->GetWorldID());
	CObjectTable* objTbl = app->g_pTableContainer->GetObjectTable(plr->GetWorldID());
	/*for (CTable::TABLEIT npcTbl = busNpc->Begin(); npcTbl != busNpc->End(); ++npcTbl)
	{
	sNPC_TBLDAT* pBusTblDat = (sNPC_TBLDAT*)npcTbl->second;
	if (pBusTblDat)
	{
	if ((pBusTblDat->byJob == NPC_JOB_BUS) && ((strcmp(pBusTblDat->szModel, "N_BUS_A1") == 0) ||
	(strcmp(pBusTblDat->szModel, "N_BUS_C1") == 0) || (strcmp(pBusTblDat->szModel, "N_BUS_B1") == 0)))
	{

	res->busTblidx = pBusTblDat->tblidx;
	res->vCurDir.x = this->plr->GetDirection().x;
	res->vCurDir.z = this->plr->GetDirection().z;
	res->vCurLoc.x = this->plr->GetPosition().x;
	res->vCurLoc.z = this->plr->GetPosition().z;
	res->hSubject = AcquireSerialId();
	res->wOpCode = GU_BUS_LOCATION_NFY;
	packet.SetPacketLen(sizeof(sGU_BUS_LOCATION_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	}
	}
	}*/
	res->busTblidx;
	res->hSubject;
	res->vCurDir;
	res->vCurLoc;
	res->wOpCode;
}
//----------------------------------//
//--ObjectVisit Luiz45
//----------------------------------//
void CClientSession::SendObjectVisitQuest(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_QUEST_OBJECT_VISIT_REQ * req = (sUG_QUEST_OBJECT_VISIT_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_QUEST_OBJECT_VISIT_RES));
	sGU_QUEST_OBJECT_VISIT_RES* res = (sGU_QUEST_OBJECT_VISIT_RES*)packet.GetPacketData();

	res->byObjType = req->byObjType;
	res->objectTblidx = req->objectTblidx;
	res->qId = req->qId;
	res->worldId = req->worldId;
	res->wOpCode = GU_QUEST_OBJECT_VISIT_RES;
	res->wResultCode = GAME_SUCCESS;


	packet.SetPacketLen(sizeof(sGU_QUEST_OBJECT_VISIT_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--TSUpdateState Luiz45
//----------------------------------//
void CClientSession::SendTSUpdateState(CNtlPacket * pPacket, CGameServer * app)
{
	printf("Sending TS_UPDATE_STATE");
	sUG_TS_UPDATE_STATE* req = (sUG_TS_UPDATE_STATE*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_TS_UPDATE_STATE));
	sGU_TS_UPDATE_STATE* res = (sGU_TS_UPDATE_STATE*)packet.GetPacketData();
	res->byTsType = req->byTsType;
	res->byType = req->byType;
	res->wTSState = req->wTSState;
	res->tId = req->tId;
	res->dwParam = req->dwParam;
	res->wOpCode = GU_TS_UPDATE_STATE;
	packet.SetPacketLen(sizeof(sGU_TS_UPDATE_STATE));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--Sending TimeQuestList Luiz45
//----------------------------------//
void CClientSession::SendTimeQuestList(CNtlPacket * pPacket, CGameServer * app)
{
	printf("Sending Time Quest List\n");
	/*PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TIMEQUEST_ROOM_LIST_RES));
	sGU_TIMEQUEST_ROOM_LIST_RES* res = (sGU_TIMEQUEST_ROOM_LIST_RES*)packet.GetPacketData();
	CTimeQuestTable* pTmqTable = app->g_pTableContainer->GetTimeQuestTable();
	res->sTMQInfo.byDifficult = this->gsf->GetTmqLevel(plr);
	res->sTMQInfo.tmqTblidx = this->gsf->GetTmq(plr);
	res->sTMQInfo.wWaitIndividualCount = 1;
	res->sTMQInfo.wWaitPartyCount = 1;
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TIMEQUEST_ROOM_LIST_RES;
	packet.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_LIST_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;*/
}
//----------------------------------//
//--Leaving Out TMQ Luiz45
//----------------------------------//
void CClientSession::LeaveTimeQuestRoom(CNtlPacket * pPacket, CGameServer * app)
{
	printf("Leaving Out TMQ Room\n");
	/*CNtlPacket packet2(sizeof(sGU_TIMEQUEST_ROOM_LEAVE_NFY));
	sGU_TIMEQUEST_ROOM_LEAVE_NFY* res2 = (sGU_TIMEQUEST_ROOM_LEAVE_NFY*)packet2.GetPacketData();

	CNtlPacket packet(sizeof(sGU_TIMEQUEST_ROOM_LEAVE_RES));
	sGU_TIMEQUEST_ROOM_LEAVE_RES* res = (sGU_TIMEQUEST_ROOM_LEAVE_RES*)packet.GetPacketData();

	res->wOpCode = GU_TIMEQUEST_ROOM_LEAVE_RES;
	res2->wOpCode = GU_TIMEQUEST_ROOM_LEAVE_NFY;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_LEAVE_RES));
	packet2.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_LEAVE_NFY));

	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
	app->UserBroadcastothers(&packet2, this);*/
}
//----------------------------------//
//--Join TMQ Luiz45
//----------------------------------//
//void CClientSession::JoinTimeQuestRoom(CNtlPacket * pPacket, CGameServer * app)
//{
//	printf("Joining...\n");
//	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
//
//	sUG_TIMEQUEST_ROOM_JOIN_REQ* req = (sUG_TIMEQUEST_ROOM_JOIN_REQ*)pPacket->GetPacketData();
//
//	//Packets definition
//	CNtlPacket packet(sizeof(sGU_TIMEQUEST_ROOM_JOIN_RES));
//	sGU_TIMEQUEST_ROOM_JOIN_RES* res = (sGU_TIMEQUEST_ROOM_JOIN_RES*)packet.GetPacketData();
//
//	CNtlPacket packet2(sizeof(sGU_TIMEQUEST_ROOM_JOIN_NFY));
//	sGU_TIMEQUEST_ROOM_JOIN_NFY* res2 = (sGU_TIMEQUEST_ROOM_JOIN_NFY*)packet2.GetPacketData();
//
//	CNtlPacket packet3(sizeof(sGU_TIMEQUEST_ROOM_SELECTION_NFY));
//	sGU_TIMEQUEST_ROOM_SELECTION_NFY* res3 = (sGU_TIMEQUEST_ROOM_SELECTION_NFY*)packet3.GetPacketData();
//
//	//Preparing Data
//	res->hTroubleMember = this->GetavatarHandle();
//	res->sJoinInfo.tmqTblidx = this->gsf->GetTmq(plr);
//	res->sJoinInfo.byDifficult = this->gsf->GetTmqLevel(plr);
//	res->sJoinInfo.byRoomState = TIMEQUEST_ROOM_STATE_REGISTRAITION;
//	res->sJoinInfo.dwRemainTime = 60000;
//	res->sJoinInfo.byTimeQuestMode = req->byTimeQuestMode;
//	res->wOpCode = GU_TIMEQUEST_ROOM_JOIN_RES;
//	res->wResultCode = GAME_SUCCESS;
//
//	res2->sJoinInfo.tmqTblidx = res->sJoinInfo.tmqTblidx;
//	res2->sJoinInfo.byDifficult = this->gsf->GetTmqLevel(plr);
//	res2->sJoinInfo.byRoomState = TIMEQUEST_ROOM_STATE_WAITENTRY;
//	res2->sJoinInfo.byTimeQuestMode = req->byTimeQuestMode;
//	res2->sJoinInfo.dwRemainTime = 60000;
//	res2->wOpCode = GU_TIMEQUEST_ROOM_JOIN_NFY;
//
//	res3->bIsSecondWinner = true;
//	res3->uSelectionInfo.sEntryInfo.bHaveItem = true;
//	res3->uSelectionInfo.sEntryInfo.dwReaminEntryTime = 60000;
//	res3->uSelectionInfo.sNextTmqInfo.tmqTblidx = this->gsf->GetTmq(plr);
//	res3->wOpCode = GU_TIMEQUEST_ROOM_SELECTION_NFY;
//
//	//Finishing
//	packet.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_JOIN_RES));
//	packet2.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_JOIN_NFY));
//	packet3.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_SELECTION_NFY));
//	g_pApp->Send(this->GetHandle(), &packet);
//	g_pApp->Send(this->GetHandle(), &packet3);
//	g_pApp->Send(this->GetHandle(), &packet2);
//	app->UserBroadcastothers(&packet3, this);
//	app->UserBroadcastothers(&packet2, this);
//	plr = NULL;
//	delete plr;
//}
//----------------------------------//
//--Teleport to TMQ Luiz45
//----------------------------------//
//void CClientSession::SendTimeQuestTeleport(CNtlPacket * pPacket, CGameServer * app)
//{
//	sUG_TIMEQUEST_ROOM_TELEPORT_REQ* req = (sUG_TIMEQUEST_ROOM_TELEPORT_REQ*)pPacket->GetPacketData();
//
//	CNtlPacket packet(sizeof(sGU_TIMEQUEST_ROOM_TELEPORT_RES));
//	sGU_TIMEQUEST_ROOM_TELEPORT_RES* res = (sGU_TIMEQUEST_ROOM_TELEPORT_RES*)packet.GetPacketData();
//
//	res->wOpCode = GU_TIMEQUEST_ROOM_TELEPORT_RES;
//	res->wResultCode = GAME_SUCCESS;
//
//	packet.SetPacketLen(sizeof(sGU_TIMEQUEST_ROOM_TELEPORT_RES));
//	g_pApp->Send(this->GetHandle(), &packet);
//}
//----------------------------------//
//--Party Dungeon Init/Cancel Luiz45
//----------------------------------//
void CClientSession::SendInitPartyDungeon(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_PARTY_DUNGEON_INIT_REQ* req = (sUG_PARTY_DUNGEON_INIT_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PARTY_DUNGEON_INIT_RES));
	sGU_PARTY_DUNGEON_INIT_RES* res = (sGU_PARTY_DUNGEON_INIT_RES*)packet.GetPacketData();

	res->wOpCode = GU_PARTY_DUNGEON_INIT_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_PARTY_DUNGEON_INIT_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--Party Dungeon Difficult Luiz45
//----------------------------------//
void CClientSession::SendPartyChangeDiff(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_PARTY_DIFF_CHANGE_REQ* req = (sUG_PARTY_DIFF_CHANGE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PARTY_DUNGEON_DIFF_RES));
	sGU_PARTY_DUNGEON_DIFF_RES* res = (sGU_PARTY_DUNGEON_DIFF_RES*)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_PARTY_DUNGEON_DIFF_NFY));
	sGU_PARTY_DUNGEON_DIFF_NFY* res2 = (sGU_PARTY_DUNGEON_DIFF_NFY*)packet2.GetPacketData();

	res->eDiff = req->eDiff;
	res->wOpCode = GU_PARTY_DUNGEON_DIFF_RES;
	res->wResultCode = GAME_SUCCESS;

	res2->eDiff = req->eDiff;
	res2->wOpCode = GU_PARTY_DUNGEON_DIFF_NFY;

	packet.SetPacketLen(sizeof(sGU_PARTY_DUNGEON_DIFF_RES));
	packet2.SetPacketLen(sizeof(sGU_PARTY_DUNGEON_DIFF_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
}
//----------------------------------//
//--Party Changing Zenny Method Luiz45
//----------------------------------//
void CClientSession::SendPartyChangeZenny(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ* req = (sUG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PARTY_CHANGE_ZENNY_LOOTING_METHOD_RES));
	sGU_PARTY_CHANGE_ZENNY_LOOTING_METHOD_RES* res = (sGU_PARTY_CHANGE_ZENNY_LOOTING_METHOD_RES*)packet.GetPacketData();

	res->byNewLootingMethod = req->byLootingMethod;
	res->wOpCode = GU_PARTY_CHANGE_ZENNY_LOOTING_METHOD_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_PARTY_CHANGE_ZENNY_LOOTING_METHOD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--Party Changing Items Method Luiz45
//----------------------------------//
void CClientSession::SendPartyChangeItem(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ* req = (sUG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PARTY_CHANGE_ITEM_LOOTING_METHOD_RES));
	sGU_PARTY_CHANGE_ITEM_LOOTING_METHOD_RES* res = (sGU_PARTY_CHANGE_ITEM_LOOTING_METHOD_RES*)packet.GetPacketData();

	res->byNewLootingMethod = req->byLootingMethod;
	res->wOpCode = GU_PARTY_CHANGE_ITEM_LOOTING_METHOD_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_PARTY_CHANGE_ITEM_LOOTING_METHOD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--NetPY ShopItem Method 
//----------------------------------//
void CClientSession::SendNetPyStart(CNtlPacket * pPacket, CGameServer * app)
{
	

	CNtlPacket packet1(sizeof(sGU_SHOP_NETPYITEM_START_RES));
	sGU_SHOP_NETPYITEM_START_RES* res1 = (sGU_SHOP_NETPYITEM_START_RES*)packet1.GetPacketData();

	res1->byType = 0;
	res1->wOpCode = GU_SHOP_NETPYITEM_START_RES;
	res1->wResultCode = GAME_SUCCESS;
	packet1.SetPacketLen(sizeof(sGU_SHOP_NETPYITEM_START_RES));
	g_pApp->Send(this->GetHandle(), &packet1);

	
	

	
}
void CClientSession::SendNetPyBuy(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_SHOP_NETPYITEM_BUY_RES));
	sGU_SHOP_NETPYITEM_BUY_RES* res = (sGU_SHOP_NETPYITEM_BUY_RES*)packet.GetPacketData();

	res->wOpCode = GU_SHOP_NETPYITEM_BUY_RES;
	res->wResultCode = GAME_SUCCESS;


	packet.SetPacketLen(sizeof(sGU_SHOP_NETPYITEM_BUY_RES));
	g_pApp->Send(this->GetHandle(), &packet);

}
void CClientSession::SendNetPyEnd(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_SHOP_NETPYITEM_END_RES));
	sGU_SHOP_NETPYITEM_END_RES* res = (sGU_SHOP_NETPYITEM_END_RES*)packet.GetPacketData();

	
	res->wOpCode = GU_SHOP_NETPYITEM_END_RES;
	res->wResultCode = GAME_SUCCESS;


	packet.SetPacketLen(sizeof(sGU_SHOP_NETPYITEM_END_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--SendWarFogInfo Method Luiz45
//----------------------------------//
void CClientSession::SendAvatarWarFogInfo(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_WAR_FOG_INFO));
	sGU_WAR_FOG_INFO* res = (sGU_WAR_FOG_INFO*)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	app->db->prepare("SELECT * FROM warfoginfo WHERE owner_id = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();
	int iPosition = 0;
	CObjectTable* myObjTbl = app->g_pTableContainer->GetObjectTable(1);
	CWorldMapTable* pas = app->g_pTableContainer->GetWorldMapTable();
	while (app->db->fetch())
	{

		sOBJECT_TBLDAT* pOBJECT_TBLDAT = reinterpret_cast<sOBJECT_TBLDAT*>(myObjTbl->FindData(app->db->getInt("hObject")));
		if (pOBJECT_TBLDAT)
		{
			RwUInt32 uiIndex = pOBJECT_TBLDAT->contentsTblidx;
			RwUInt32 uiArrayPos = uiIndex / 8;
			RwUInt8 byCurBit = (RwUInt8)(uiIndex % 8);

			res->abyWarFogInfo[uiArrayPos] |= 0x01ui8 << byCurBit;
		}
		iPosition++;
	}
	res->wOpCode = GU_WAR_FOG_INFO;

	packet.SetPacketLen(sizeof(sGU_WAR_FOG_INFO));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//----------------------------------//
//--HoiPoi Job Method Luiz45
//----------------------------------//
void CClientSession::SendHoiPoiJob(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	sUG_HOIPOIMIX_JOB_SET_REQ* req = (sUG_HOIPOIMIX_JOB_SET_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_HOIPOIMIX_JOB_SET_RES));
	sGU_HOIPOIMIX_JOB_SET_RES* res = (sGU_HOIPOIMIX_JOB_SET_RES*)packet.GetPacketData();
	app->db->prepare("SELECT * FROM warfoginfo WHERE owner_id = ?");
	app->db->setInt(1, plr->GetCharID());
	app->db->execute();

	res->byRecipeType = req->byRecipeType;
	res->hNpchandle = req->hNpchandle;
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_HOIPOIMIX_JOB_SET_RES;

	packet.SetPacketLen(sizeof(sGU_HOIPOIMIX_JOB_SET_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	plr = NULL;
	delete plr;
}
//----------------------------------//
//--HoiPoi Job Reset Method Luiz45
//----------------------------------//
void CClientSession::SendHoiPoiJobReset(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_HOIPOIMIX_JOB_RESET_REQ* req = (sUG_HOIPOIMIX_JOB_RESET_REQ*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sGU_HOIPOIMIX_JOB_SET_RES));
	sGU_HOIPOIMIX_JOB_RESET_RES* res = (sGU_HOIPOIMIX_JOB_RESET_RES*)packet.GetPacketData();

	res->byRecipeType = req->byRecipeType;
	res->hNpchandle = req->hNpchandle;
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_HOIPOIMIX_JOB_RESET_RES;

	packet.SetPacketLen(sizeof(sGU_HOIPOIMIX_JOB_RESET_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--Tutorial play Method Luiz45
//----------------------------------//
void CClientSession::SendDirectPlay(CNtlPacket *pPacket, CGameServer * app)
{
	sUG_CHAR_DIRECT_PLAY_ACK* req = (sUG_CHAR_DIRECT_PLAY_ACK*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_DIRECT_PLAY));
	sGU_CHAR_DIRECT_PLAY* res = (sGU_CHAR_DIRECT_PLAY*)packet.GetPacketData();

	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CDirectionLinkTable* pLinkTbl = app->g_pTableContainer->GetDirectionLinkTable();
	sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1000));

	if (plr->IsInTutorial())
	{
		res->bSynchronize = true;
		res->byPlayMode = 1;
		res->directTblidx = 1000;
		res->hSubject = this->GetavatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->GetHandle(), &packet);

		Sleep(7000);
		CNtlPacket packetEnd(sizeof(sGU_CHAR_DIRECT_PLAY_END));
		sGU_CHAR_DIRECT_PLAY_END* res4 = (sGU_CHAR_DIRECT_PLAY_END*)packetEnd.GetPacketData();
		res4->wOpCode = GU_CHAR_DIRECT_PLAY_END;
		packetEnd.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY_END));
		g_pApp->Send(this->GetHandle(), &packetEnd);
	}
	else
	{
		//this->tlqManager->SendDirectPlayAckForTLQ();
	}
	plr = NULL;
	delete plr;
}
//----------------------------------//
//--Tutorial play Quit Method Luiz45
//----------------------------------//
void CClientSession::SendTutorialPlayQuit(CNtlPacket *pPacket, CGameServer * app)
{
	sUG_TUTORIAL_PLAY_QUIT_REQ* req = (sUG_TUTORIAL_PLAY_QUIT_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_TUTORIAL_PLAY_QUIT_RES));
	sGU_TUTORIAL_PLAY_QUIT_RES* res = (sGU_TUTORIAL_PLAY_QUIT_RES*)packet.GetPacketData();

	res->wOpCode = GU_TUTORIAL_PLAY_QUIT_RES;
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_TUTORIAL_PLAY_QUIT_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//----------------------------------//
//--PrivateShop Create Method Fix Marco Tw
//----------------------------------//
void CClientSession::SendPrivateShopCreate(CNtlPacket *pPacket, CGameServer * app)
{
	sUG_PRIVATESHOP_CREATE_REQ* req = (sUG_PRIVATESHOP_CREATE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PRIVATESHOP_CREATE_RES));
	sGU_PRIVATESHOP_CREATE_RES* res1 = (sGU_PRIVATESHOP_CREATE_RES*)packet.GetPacketData();

	CNtlPacket packet2(sizeof(sGU_PRIVATESHOP_CREATE_NFY));
	sGU_PRIVATESHOP_CREATE_NFY* res2 = (sGU_PRIVATESHOP_CREATE_NFY*)packet2.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* res3 = (sGU_UPDATE_CHAR_STATE*)packet3.GetPacketData();

	res1->sPrivateShopData.hOwner = this->GetavatarHandle();
	res1->wOpCode = GU_PRIVATESHOP_CREATE_RES;
	res1->wResultCode = GAME_SUCCESS;

	res2->hOwner = this->GetavatarHandle();
	res2->wOpCode = GU_PRIVATESHOP_CREATE_NFY;

	res3->handle = this->GetavatarHandle();
	res3->sCharState.sCharStateBase.byStateID = CHARSTATE_PRIVATESHOP;
	res3->wOpCode = GU_UPDATE_CHAR_STATE;

	packet.SetPacketLen(sizeof(sGU_PRIVATESHOP_CREATE_RES));
	packet2.SetPacketLen(sizeof(sGU_PRIVATESHOP_CREATE_NFY));
	packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(this->GetHandle(), &packet2);
	g_pApp->Send(this->GetHandle(), &packet3);
}
//----------------------------------//
//--PrivateShop Exit Method Fix Marco TW
//----------------------------------//
void CClientSession::SendPrivateShopExit(CNtlPacket *pPacket, CGameServer * app)
{
	sUG_PRIVATESHOP_EXIT_REQ* req = (sUG_PRIVATESHOP_EXIT_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_PRIVATESHOP_EXIT_RES));
	sGU_PRIVATESHOP_EXIT_RES* res1 = (sGU_PRIVATESHOP_EXIT_RES*)packet.GetPacketData();

	CNtlPacket packet3(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* res3 = (sGU_UPDATE_CHAR_STATE*)packet3.GetPacketData();

	res1->wOpCode = GU_PRIVATESHOP_EXIT_RES;
	res1->wResultCode = GAME_SUCCESS;

	res3->handle = this->GetavatarHandle();
	res3->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	res3->wOpCode = GU_UPDATE_CHAR_STATE;

	packet.SetPacketLen(sizeof(sGU_PRIVATESHOP_EXIT_RES));
	g_pApp->Send(this->GetHandle(), &packet);
	packet3.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	g_pApp->Send(this->GetHandle(), &packet3);
}
//---------------------------------------------
//---Send Notice Luiz45 -- Improved by Kalisto
//---------------------------------------------
void CClientSession::SendServerAnnouncement(wstring wsMsg, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	sGU_SYSTEM_DISPLAY_TEXT* sNotice = (sGU_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();

	WCHAR wcsMsg[BUDOKAI_MAX_NOTICE_LENGTH + 1];
	char ch[256];
	char DefChar = '\0';
	WideCharToMultiByte(CP_ACP, 0, wsMsg.c_str(), -1, ch, 256, &DefChar, NULL);
	mbstowcs(wcsMsg, ch, 255);

	sNotice->wOpCode = GU_SYSTEM_DISPLAY_TEXT;
	sNotice->byDisplayType = 1;
	wcscpy_s(sNotice->awchMessage, BUDOKAI_MAX_NOTICE_LENGTH + 1, wcsMsg);
	wcscpy_s(sNotice->awGMChar, NTL_MAX_SIZE_CHAR_NAME_UNICODE, L"Marco Admin");
	sNotice->wMessageLengthInUnicode = (WORD)wcslen(wcsMsg);
	packet.SetPacketLen(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	app->UserBroadcast(&packet);
}
//---------------------------------------------
//---Send Notice by Kalisto
//---------------------------------------------
void CClientSession::SendServerBroadcast(wstring wsMsg, CGameServer * app)
{

	CNtlPacket packet(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	sGU_SYSTEM_DISPLAY_TEXT* sNotice = (sGU_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();

	WCHAR wcsMsg[BUDOKAI_MAX_NOTICE_LENGTH + 1];
	char ch[256];
	char DefChar = '\0';
	WideCharToMultiByte(CP_ACP, 0, wsMsg.c_str(), -1, ch, 256, &DefChar, NULL);
	mbstowcs(wcsMsg, ch, 255);

	sNotice->wOpCode = GU_SYSTEM_DISPLAY_TEXT;
	sNotice->byDisplayType = 3;
	wcscpy_s(sNotice->awchMessage, BUDOKAI_MAX_NOTICE_LENGTH + 1, wcsMsg);
	wcscpy_s(sNotice->awGMChar, NTL_MAX_SIZE_CHAR_NAME_UNICODE, L"Marco Admin");
	sNotice->wMessageLengthInUnicode = (WORD)wcslen(wcsMsg);
	packet.SetPacketLen(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	app->UserBroadcast(&packet);

}
void CClientSession::CreateItemById(uint32_t tblidx, int playerId)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* pSession = g_pPlayerManager->GetPlayerByID(playerId);

	CItemTable* pMyItemTbl = app->g_pTableContainer->GetItemTable();
	sITEM_TBLDAT* item = reinterpret_cast<sITEM_TBLDAT*>(pMyItemTbl->FindData(tblidx));
	if (item != NULL)
	{
		GsFunctionsClass* gsFunction = new GsFunctionsClass();//I dont believe that leave this way because the other way is wrong
		gsFunction->CreateUpdateItem(pSession, 1, tblidx, false, pSession->GetSession());
		gsFunction = NULL;
		delete gsFunction;
	}
	else
		cout << "No Such ItemID" << endl;
}
void  CClientSession::CreateNPCById(unsigned int uiNpcId)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* pSession = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	int playerId = 0;
	playerId = pSession->GetCharID();
	sVECTOR3 curpos = pSession->GetPlayerPosition();
	CNPCTable* pMyNpcTable = app->g_pTableContainer->GetNpcTable();
	sNPC_TBLDAT* npc = reinterpret_cast<sNPC_TBLDAT*>(pMyNpcTable->FindData(uiNpcId));
	if (npc != NULL)
	{
		CNtlPacket packet(sizeof(SpawnNPC));
		SpawnNPC * res = (SpawnNPC *)packet.GetPacketData();

		res->wOpCode = GU_OBJECT_CREATE;
		res->Type = OBJTYPE_NPC;
		res->Handle = AcquireSerialId(); //app->mob->AcquireMOBSerialId(); //this will get your Player Handle, need change "AcquireSerialId" because here is used to generate a Handler for the players!#Issue 6 Luiz45
		res->Loc[0] = curpos.x;
		res->Loc[1] = curpos.y;
		res->Loc[2] = curpos.z;
		res->Dir[0] = curpos.x;
		res->Dir[1] = curpos.y;
		res->Dir[2] = curpos.z;
		res->StateID = CHARSTATE_SPAWNING;
		//res->sObjectInfo.mobState.sCharStateBase.bFightMode = false;
		res->Tblidx = uiNpcId;
		//res->curEP = npc->wBasic_EP;
		//res->maxEP = npc->wBasic_EP;
		////res->curLP = npc->wBasic_LP;
		//res->maxLP = npc->wBasic_LP;
		//res->Level = npc->byLevel;
		res->Size = 10;
		//res->sObjectInfo.mobBrief.fLastRunningSpeed = mob->fRun_Speed;
		//res->sObjectInfo.mobBrief.fLastWalkingSpeed = mob->fWalk_Speed;
		pSession->myCCSession->InsertIntoMyMonsterList(res->Handle, curpos, uiNpcId);

		packet.SetPacketLen(sizeof(SpawnNPC));
		g_pApp->Send(pSession->GetSession(), &packet);
		app->UserBroadcastothers(&packet, this);
	}
	else

		printf("NPC not exist/n");

}
void  CClientSession::CreateMonsterById(unsigned int uiMobId)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* pSession = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	int playerId = 0;
	playerId = pSession->GetCharID();
	
	sVECTOR3 curpos = pSession->GetPlayerPosition();
	CMobTable* pMyMobTable = app->g_pTableContainer->GetMobTable();
	sMOB_TBLDAT* mob = reinterpret_cast<sMOB_TBLDAT*>(pMyMobTable->FindData(uiMobId));
	if (mob != NULL)
	{
		CNtlPacket packet(sizeof(SpawnMOB));
		SpawnMOB * res = (SpawnMOB *)packet.GetPacketData();

		res->wOpCode = GU_OBJECT_CREATE;
		res->Type = OBJTYPE_MOB;
		res->Handle = AcquireSerialId(); app->mob->AcquireMOBSerialId(); //this will get your Player Handle, need change "AcquireSerialId" because here is used to generate a Handler for the players!#Issue 6 Luiz45
		res->Loc[0] = curpos.x;
		res->Loc[1] = curpos.y;
		res->Loc[2] = curpos.z; 
		res->Dir[0] = curpos.x;
		res->Dir[1] = curpos.y;
		res->Dir[2] = curpos.z;
		res->StateID = CHARSTATE_SPAWNING;
		//res->sObjectInfo.mobState.sCharStateBase.bFightMode = false;
		res->Tblidx = uiMobId;
		res->curEP = mob->wBasic_EP;
		res->maxEP = mob->wBasic_EP;
		res->curLP = mob->wBasic_LP;
		res->maxLP = mob->wBasic_LP;
		res->Level = mob->byLevel;
		res->Size = 10;
		//res->sObjectInfo.mobBrief.fLastRunningSpeed = mob->fRun_Speed;
		//res->sObjectInfo.mobBrief.fLastWalkingSpeed = mob->fWalk_Speed;
		pSession->myCCSession->InsertIntoMyMonsterList(res->Handle, curpos, uiMobId);

		packet.SetPacketLen(sizeof(SpawnMOB));
		g_pApp->Send(pSession->GetSession(), &packet);
		app->UserBroadcastothers(&packet, this);
	}

	else
			cout << "Mob not Found\n" << endl;
	
}


void CClientSession::AddSkillById(uint32_t tblidx)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* pSession = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	int playerId = 0;
	playerId = pSession->GetCharID();
	CSkillTable* pSkillTable = app->g_pTableContainer->GetSkillTable();
	int iSkillCount = pSession->myCCSession->gsf->GetTotalSlotSkill(playerId);//you are still using "this" but "this" is null this is why you get the error #Issue 6

	sSKILL_TBLDAT* pSkillData = (sSKILL_TBLDAT*)pSkillTable->FindData(tblidx);
	if (pSkillData != NULL)
	{
		if (app->qry->CheckIfSkillAlreadyLearned(pSkillData->tblidx, playerId) == false)
		{
			CNtlPacket packet2(sizeof(sGU_SKILL_LEARNED_NFY));
			sGU_SKILL_LEARNED_NFY * res2 = (sGU_SKILL_LEARNED_NFY *)packet2.GetPacketData();
			iSkillCount++;//don't send this directly if you learned SSJ first...because we always are sending 1...nevermind :P
			res2->wOpCode = GU_SKILL_LEARNED_NFY;
			res2->skillId = pSkillData->tblidx;
			res2->bySlot = iSkillCount;

			app->qry->InsertNewSkill(pSkillData->tblidx, playerId, iSkillCount, pSkillData->wKeep_Time, pSkillData->wNext_Skill_Train_Exp);
			packet2.SetPacketLen(sizeof(sGU_SKILL_LEARNED_NFY));
			g_pApp->Send(pSession->GetSession(), &packet2);

			CNtlPacket packet(sizeof(sGU_SKILL_LEARN_RES));
			sGU_SKILL_LEARN_RES * res = (sGU_SKILL_LEARN_RES *)packet.GetPacketData();

			res->wOpCode = GU_SKILL_LEARN_RES;
			res->wResultCode = 500;

			packet.SetPacketLen(sizeof(sGU_SKILL_LEARN_RES));
			g_pApp->Send(pSession->GetSession(), &packet);

		}
		else
			cout << "Player already knows skill" << endl;
	}
	else
		cout << "No skill with that ID exists" << endl;
}
//Trade Start Ress - Luiz45
void CClientSession::SendTradeStartRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_START_REQ* req = (sUG_TRADE_START_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_OK_REQ));
	CNtlPacket packet2(sizeof(sGU_TRADE_START_RES));

	sGU_TRADE_OK_REQ* res = (sGU_TRADE_OK_REQ*)packet.GetPacketData();
	sGU_TRADE_START_RES* res1 = (sGU_TRADE_START_RES*)packet2.GetPacketData();

	res->handle = plr->GetAvatarHandle();
	res->wOpCode = GU_TRADE_OK_REQ;

	res1->byEmptyInven = 12;//Here you can put how many free slots are available to trade i've placed the MAX
	res1->handle = plr->GetAvatarHandle();
	res1->hTarget = req->hTarget;
	res1->wResultCode = GAME_TRADE_REPLY_WAIT_OVER;
	res1->wOpCode = GU_TRADE_START_RES;

	packet.SetPacketLen(sizeof(sGU_TRADE_OK_REQ));
	packet2.SetPacketLen(sizeof(sGU_TRADE_START_RES));

	//Getting User Session to trade
	PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);

	if (PlayerTrade->GetPlayerIsInTrade())
	{
		res1->wResultCode = GAME_TRADE_ALREADY_OPEN;
		g_pApp->Send(this->GetHandle(), &packet2);
	}
	else{
		g_pApp->Send(this->GetHandle(), &packet2);
		g_pApp->Send(PlayerTrade->GetSession(), &packet);
	}
	plr = NULL;
	delete plr;
}
//Trade Finish Res - Luiz45
void CClientSession::SendTradeFinish(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_END_REQ* req = (sUG_TRADE_END_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_END_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_END_NFY));
	sGU_TRADE_END_RES* res = (sGU_TRADE_END_RES*)packet.GetPacketData();
	sGU_TRADE_END_NFY* res2 = (sGU_TRADE_END_NFY*)packet2.GetPacketData();

	if (req->bIsSet = true)
	{
		plr->SetTradeOK(true);
		PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);
		res->bIsSet = true;
		res->hTarget = plr->GetAvatarHandle();
		res->dwPacketCount = req->dwPacketCount;
		res->wResultCode = GAME_SUCCESS;
		res->wOpCode = GU_TRADE_END_RES;

		res2->bIsSet = true;
		res2->hTarget = PlayerTrade->GetAvatarHandle();
		res2->wOpCode = GU_TRADE_END_NFY;
		res2->wResultCode = GAME_SUCCESS;

		packet2.SetPacketLen(sizeof(sGU_TRADE_END_NFY));
		packet.SetPacketLen(sizeof(sGU_TRADE_END_RES));
		g_pApp->Send(this->GetHandle(), &packet);
		g_pApp->Send(PlayerTrade->GetSession(), &packet2);

		if (PlayerTrade->GetTradeOK() == true)
		{
			plr->SetTradeOK(false);
			plr->SetPlayerIsInTrade(false);
			PlayerTrade->SetTradeOK(false);
			PlayerTrade->SetPlayerIsInTrade(false);
			//Trading items guys !! =D need fix delete when you wanna trade the max Amount for some item
			if (plr->cPlayerInventory->GetTotalItemsTrade() != 0)
			{
				for (int i = 0; i < plr->cPlayerInventory->GetTotalItemsTrade(); i++)
				{
					this->gsf->CreateUpdateItem(PlayerTrade, plr->cPlayerInventory->GetItemsToTrade()[i].byStackcount, plr->cPlayerInventory->GetItemsToTrade()[i].itemNo, false, PlayerTrade->GetSession());
					this->gsf->CreateUpdateItem(plr, plr->cPlayerInventory->GetItemsToTrade()[i].byStackcount, plr->cPlayerInventory->GetItemsToTrade()[i].itemNo, true, this->GetHandle(), plr->cPlayerInventory->GetItemsToTrade()[i].itemId);
					PlayerTrade->cPlayerInventory->AddItemToInventory(plr->cPlayerInventory->GetItemsToTrade()[i]);//Add in Player Inventory
					plr->cPlayerInventory->RemoveItemFromInventory(plr->cPlayerInventory->GetItemsToTrade()[i].itemId);//Remove for User Inventory
					plr->cPlayerInventory->RemoveUpdateTrade(plr->cPlayerInventory->GetItemsToTrade()[i].itemId, true);//Remove from the list
				}
			}
			if (PlayerTrade->cPlayerInventory->GetTotalItemsTrade() != 0)
			{
				for (int i = 0; i < PlayerTrade->cPlayerInventory->GetTotalItemsTrade(); i++)
				{
					this->gsf->CreateUpdateItem(plr, PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].byStackcount, PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].itemNo, false, this->GetHandle());
					this->gsf->CreateUpdateItem(PlayerTrade, PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].byStackcount, PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].itemNo, true, PlayerTrade->GetSession(), PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].itemId);
					plr->cPlayerInventory->AddItemToInventory(PlayerTrade->cPlayerInventory->GetItemsToTrade()[i]);//Add in Player Inventory
					PlayerTrade->cPlayerInventory->RemoveItemFromInventory(PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].itemId);//Remove for User Inventory
					PlayerTrade->cPlayerInventory->RemoveUpdateTrade(PlayerTrade->cPlayerInventory->GetItemsToTrade()[i].itemId, true);//Remove from the list
				}
			}
			if (plr->cPlayerInventory->GetZennyToTrade() != 0)
			{
				this->gsf->UpdateCharMoney(NULL, PlayerTrade->myCCSession, ZENNY_CHANGE_TYPE_TRADE, (PlayerTrade->GetPcProfile()->dwZenny += plr->cPlayerInventory->GetZennyToTrade()), PlayerTrade->GetAvatarHandle());
				this->gsf->UpdateCharMoney(NULL, this, ZENNY_CHANGE_TYPE_TRADE, (plr->GetPcProfile()->dwZenny -= plr->cPlayerInventory->GetZennyToTrade()), plr->GetAvatarHandle());
				plr->cPlayerInventory->SetAmountZennyToTrade(0);
				app->qry->SetBankMoney(PlayerTrade->GetCharID(), PlayerTrade->GetPcProfile()->dwZenny);
				app->qry->SetBankMoney(plr->GetCharID(), plr->GetPcProfile()->dwZenny);
			}
			if (PlayerTrade->cPlayerInventory->GetZennyToTrade() != 0)
			{
				this->gsf->UpdateCharMoney(NULL, this, ZENNY_CHANGE_TYPE_TRADE, (plr->GetPcProfile()->dwZenny += PlayerTrade->cPlayerInventory->GetZennyToTrade()), plr->GetAvatarHandle());
				this->gsf->UpdateCharMoney(NULL, PlayerTrade->myCCSession, ZENNY_CHANGE_TYPE_TRADE, (PlayerTrade->GetPcProfile()->dwZenny -= PlayerTrade->cPlayerInventory->GetZennyToTrade()), PlayerTrade->GetAvatarHandle());
				PlayerTrade->cPlayerInventory->SetAmountZennyToTrade(0);
				app->qry->SetBankMoney(PlayerTrade->GetCharID(), PlayerTrade->GetPcProfile()->dwZenny);
				app->qry->SetBankMoney(plr->GetCharID(), plr->GetPcProfile()->dwZenny);
			}
		}
	}
	else
	{
		res->bIsSet = false;
		res->hTarget = plr->GetAvatarHandle();
		res->dwPacketCount = req->dwPacketCount;
		res->wResultCode = GAME_SUCCESS;
		res->wOpCode = GU_TRADE_END_RES;

		packet.SetPacketLen(sizeof(sGU_TRADE_END_RES));
		g_pApp->Send(this->GetHandle(), &packet);
	}
	plr = NULL;
	delete plr;
}
//Trade Add Res - Luiz45
void CClientSession::SendTradeAddRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_ADD_REQ* req = (sUG_TRADE_ADD_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_ADD_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_ADD_NFY));
	sGU_TRADE_ADD_RES* res = (sGU_TRADE_ADD_RES*)packet.GetPacketData();
	sGU_TRADE_ADD_NFY* res2 = (sGU_TRADE_ADD_NFY*)packet2.GetPacketData();

	PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);

	res->byCount = req->byCount;
	res->hItem = req->hItem;
	res->wOpCode = GU_TRADE_ADD_RES;
	res->wResultCode = GAME_SUCCESS;

	res2->byCount = req->byCount;
	res2->hItem = req->hItem;
	res2->wOpCode = GU_TRADE_ADD_NFY;
	for (int i = 0; i < plr->cPlayerInventory->GetTotalItemsCount(); i++)
	{
		if (plr->cPlayerInventory->GetInventory()[i].handle == req->hItem)
		{
			res2->sItem.bNeedToIdentify = false;
			res2->sItem.itemId = plr->cPlayerInventory->GetInventory()[i].handle;
			res2->sItem.itemNo = plr->cPlayerInventory->GetInventory()[i].tblidx;
			res2->sItem.byStackcount = req->byCount;
			res2->sItem.byRank = plr->cPlayerInventory->GetInventory()[i].byRank;
			res2->sItem.byCurrentDurability = plr->cPlayerInventory->GetInventory()[i].byCurDur;
			res2->sItem.byGrade = plr->cPlayerInventory->GetInventory()[i].byGrade;
			res2->sItem.byPlace = plr->cPlayerInventory->GetInventory()[i].byPlace;
			res2->sItem.byPosition = plr->cPlayerInventory->GetInventory()[i].byPos;
			plr->cPlayerInventory->AddItemToTrade(res2->sItem);
		}
	}
	packet.SetPacketLen(sizeof(sGU_TRADE_ADD_RES));
	packet2.SetPacketLen(sizeof(sGU_TRADE_ADD_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(PlayerTrade->GetSession(), &packet2);
	plr = NULL;
	delete plr;
}
//Trade Dell - Luiz45
void CClientSession::SendTradeDelRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_DEL_REQ* req = (sUG_TRADE_DEL_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_DEL_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_DEL_NFY));
	sGU_TRADE_DEL_RES* res = (sGU_TRADE_DEL_RES*)packet.GetPacketData();
	sGU_TRADE_DEL_NFY* res2 = (sGU_TRADE_DEL_NFY*)packet2.GetPacketData();

	res->hItem = req->hItem;
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TRADE_DEL_RES;

	res2->hItem = req->hItem;
	res2->wOpCode = GU_TRADE_DEL_NFY;

	plr->cPlayerInventory->RemoveUpdateTrade(req->hItem, true, NULL);

	PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);

	packet.SetPacketLen(sizeof(sGU_TRADE_DEL_RES));
	packet2.SetPacketLen(sizeof(sGU_TRADE_DEL_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(PlayerTrade->GetSession(), &packet2);
	plr = NULL;
}
//Trade Cancel - Luiz45
void CClientSession::SendTradeCancelRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_CANCEL_REQ* req = (sUG_TRADE_CANCEL_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_CANCEL_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_CANCEL_NFY));
	sGU_TRADE_CANCEL_RES* res = (sGU_TRADE_CANCEL_RES*)packet.GetPacketData();
	sGU_TRADE_CANCEL_NFY* res2 = (sGU_TRADE_CANCEL_NFY*)packet2.GetPacketData();

	res->hTarget = plr->GetAvatarHandle();
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TRADE_CANCEL_RES;

	res2->hTarget = req->hTarget;
	res2->wResultCode = GAME_SUCCESS;
	res2->wOpCode = GU_TRADE_CANCEL_NFY;

	packet.SetPacketLen(sizeof(sGU_TRADE_CANCEL_RES));
	packet2.SetPacketLen(sizeof(sGU_TRADE_CANCEL_NFY));

	PlayersMain* PlayerTarget = app->GetUserSession(req->hTarget);

	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(PlayerTarget->GetSession(), &packet2);
	plr = NULL;
	delete plr;
}
//Trade Deny - Luiz45
void CClientSession::SendTradeDenyRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_DENY_REQ* req = (sUG_TRADE_DENY_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_TRADE_DENY_RES));
	sGU_TRADE_DENY_RES* res = (sGU_TRADE_DENY_RES*)packet.GetPacketData();
	res->bIsDeny = req->bIsDeny;
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TRADE_DENY_RES;
	packet.SetPacketLen(sizeof(sGU_TRADE_DENY_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//Trade OK - Luiz45
void CClientSession::SendTradeOkRes(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_OK_RES* req = (sUG_TRADE_OK_RES*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_START_NFY));
	CNtlPacket packet2(sizeof(sGU_TRADE_START_RES));
	sGU_TRADE_START_RES* res1 = (sGU_TRADE_START_RES*)packet2.GetPacketData();
	sGU_TRADE_START_NFY* res = (sGU_TRADE_START_NFY*)packet.GetPacketData();

	if (req->byOK == true)
	{
		PlayersMain* PlayerTrade = app->GetUserSession(req->handle);
		//Lets tell to the user that guy is in trade
		plr->SetPlayerIsInTrade(true);
		PlayerTrade->SetPlayerIsInTrade(true);
		res->byEmptyInven = 12;//Here you can put how many free slots are available to trade i've placed the MAX
		res->hTarget = plr->GetAvatarHandle();
		res->handle = req->handle;
		res->wOpCode = GU_TRADE_START_NFY;
		res->wResultCode = GAME_SUCCESS;

		res1->byEmptyInven = 12;//Here you can put how many free slots are available to trade i've placed the MAX
		res1->handle = plr->GetAvatarHandle();
		res1->hTarget = req->handle;
		res1->wResultCode = GAME_SUCCESS;
		res1->wOpCode = GU_TRADE_START_RES;

		packet.SetPacketLen(sizeof(sGU_TRADE_START_NFY));
		packet2.SetPacketLen(sizeof(sGU_TRADE_START_RES));
		g_pApp->Send(this->GetHandle(), &packet2);
		app->UserBroadcastothers(&packet, this);
	}
	else
	{
		res1->wResultCode = GAME_TRADE_DENY;
		res1->wOpCode = GU_TRADE_START_RES;
		packet2.SetPacketLen(sizeof(sGU_TRADE_START_RES));
		//Getting User Session to Cancel the trade
		PlayersMain* PlayerTrade = app->GetUserSession(req->handle);

		g_pApp->Send(this->GetHandle(), &packet2);
		g_pApp->Send(PlayerTrade->GetSession(), &packet2);
	}
	plr = NULL;
	delete plr;
}
//Trade Change Item Amount - Luiz45
void CClientSession::SendTradeItemModify(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_MODIFY_REQ* req = (sUG_TRADE_MODIFY_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_MODIFY_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_MODIFY_NFY));
	sGU_TRADE_MODIFY_RES* res = (sGU_TRADE_MODIFY_RES*)packet.GetPacketData();
	sGU_TRADE_MODIFY_NFY* res2 = (sGU_TRADE_MODIFY_NFY*)packet2.GetPacketData();

	PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);
	res->byCount = req->byCount;
	res->hItem = req->hItem;
	res->hTarget = plr->GetAvatarHandle();
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TRADE_MODIFY_RES;

	res2->byCount = req->byCount;
	res2->hItem = req->hItem;
	res2->hTarget = PlayerTrade->GetAvatarHandle();
	res2->wOpCode = GU_TRADE_MODIFY_NFY;

	plr->cPlayerInventory->RemoveUpdateTrade(req->hItem, false, req->byCount);//Add in our List

	packet.SetPacketLen(sizeof(sGU_TRADE_MODIFY_RES));
	packet2.SetPacketLen(sizeof(sGU_TRADE_MODIFY_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(PlayerTrade->GetSession(), &packet2);
	plr = NULL;
	delete plr;
}
//Trade Change Zenny Amount - Luiz45
void CClientSession::SendTradeZennyModify(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_TRADE_ZENNY_UPDATE_REQ* req = (sUG_TRADE_ZENNY_UPDATE_REQ*)pPacket->GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(sGU_TRADE_ZENNY_UPDATE_RES));
	CNtlPacket packet2(sizeof(sGU_TRADE_ZENNY_UPDATE_NFY));
	sGU_TRADE_ZENNY_UPDATE_RES* res = (sGU_TRADE_ZENNY_UPDATE_RES*)packet.GetPacketData();
	sGU_TRADE_ZENNY_UPDATE_NFY* res2 = (sGU_TRADE_ZENNY_UPDATE_NFY*)packet2.GetPacketData();
	PlayersMain* PlayerTrade = app->GetUserSession(req->hTarget);

	res->dwZenny = req->dwZenny;
	res->hTarget = plr->GetAvatarHandle();
	res->wResultCode = GAME_SUCCESS;
	res->wOpCode = GU_TRADE_ZENNY_UPDATE_RES;

	res2->dwZenny = req->dwZenny;
	res2->hTarget = req->hTarget;
	res2->wOpCode = GU_TRADE_ZENNY_UPDATE_NFY;

	plr->cPlayerInventory->SetAmountZennyToTrade(req->dwZenny);

	packet.SetPacketLen(sizeof(sGU_TRADE_ZENNY_UPDATE_RES));
	packet2.SetPacketLen(sizeof(sGU_TRADE_ZENNY_UPDATE_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
	g_pApp->Send(PlayerTrade->GetSession(), &packet2);
	plr = NULL;
	delete plr;
}

//--------------------------------------------------------------------------------------//
//		Server Change Request
//--------------------------------------------------------------------------------------//
void CClientSession::SendServerChangeReq(CNtlPacket * pPacket, CGameServer * app)
{
	sUG_CHAR_SERVER_CHANGE_REQ* req = (sUG_CHAR_SERVER_CHANGE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sGU_CHAR_SERVER_CHANGE_RES));
	sGU_CHAR_SERVER_CHANGE_RES * res = (sGU_CHAR_SERVER_CHANGE_RES*)packet.GetPacketData();

	res->wOpCode = GU_CHAR_SERVER_CHANGE_RES;
	strcpy_s(res->serverInfo.szCharacterServerIP, NTL_MAX_LENGTH_OF_IP, app->GetConfigFileExternalIP());
	res->serverInfo.wCharacterServerPortForClient = 20300;
	res->serverInfo.dwLoad = 1;
	strcpy_s((char*)res->achAuthKey, NTL_MAX_SIZE_AUTH_KEY, "Dbo");
	res->wResultCode = GAME_SUCCESS;

	packet.SetPacketLen(sizeof(sGU_CHAR_SERVER_CHANGE_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
}
//--------------------------------------------------------------------------------------//
//		Test DirectPlay Scripts -- Admin Function
//--------------------------------------------------------------------------------------//

void	CClientSession::SendTestDirectPlay(uint32_t tblidx, int playerId, bool sync)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = g_pPlayerManager->GetPlayerByID(playerId);
	CNtlPacket packetAck(sizeof(sGU_CHAR_DIRECT_PLAY));
	sGU_CHAR_DIRECT_PLAY* res = (sGU_CHAR_DIRECT_PLAY*)packetAck.GetPacketData();

	CDirectionLinkTable* pLinkTbl = app->g_pTableContainer->GetDirectionLinkTable();
	sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(tblidx));
	if (pLinkTblData->tblidx != INVALID_TBLIDX)
	{
		res->bSynchronize = sync;
		res->byPlayMode = 1;
		res->directTblidx = pLinkTblData->tblidx;
		res->hSubject = plr->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packetAck.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->SendTo(plr->myCCSession, &packetAck);
	}

}
//Air Jump - Luiz45
void CClientSession::SendAirJump(CNtlPacket* pPacket, CGameServer* app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	sUG_CHAR_AIR_JUMP* req = (sUG_CHAR_AIR_JUMP*)pPacket->GetPacketData();
	UpdateCharState(this->GetHandle(), CHARSTATE_AIR_JUMP);
	
	

}
//Air Dash - Luiz45
void CClientSession::SendAirDash(CNtlPacket* pPacket, CGameServer* app)
{
	sUG_CHAR_AIR_DASH* req = (sUG_CHAR_AIR_DASH*)pPacket->GetPacketData();	
	UpdateCharState(this->GetHandle(), CHARSTATE_AIR_DASH_ACCEL);
}
//--Cash ShopItem Method Marco
void CClientSession::SendCashItemStart(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_START_RES));
	sGU_CASHITEM_START_RES* res = (sGU_CASHITEM_START_RES*)packet.GetPacketData();


	res->wOpCode = GU_CASHITEM_START_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_CASHITEM_START_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemEnd(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_END_RES));
	sGU_CASHITEM_END_RES* res = (sGU_CASHITEM_END_RES*)packet.GetPacketData();

	//res->byType = 0;
	res->wOpCode = GU_CASHITEM_END_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_CASHITEM_END_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemMove(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_MOVE_RES));
	sGU_CASHITEM_MOVE_RES* res = (sGU_CASHITEM_MOVE_RES*)packet.GetPacketData();

	//res->byType = 0;
	res->wOpCode = GU_CASHITEM_MOVE_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_CASHITEM_MOVE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemUse(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_USE_RES));
	sGU_CASHITEM_USE_RES* res = (sGU_CASHITEM_USE_RES*)packet.GetPacketData();

	//res->byType = 0;
	res->wOpCode = GU_CASHITEM_USE_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_CASHITEM_USE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemHlsStart(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_HLSHOP_START_RES));
	sGU_CASHITEM_HLSHOP_START_RES* res = (sGU_CASHITEM_HLSHOP_START_RES*)packet.GetPacketData();

	//res->dwRemainAmount = 999999;//cash point
	res->wOpCode = GU_CASHITEM_HLSHOP_START_RES;
	res->wResultCode = GAME_SUCCESS;
	packet.SetPacketLen(sizeof(sGU_CASHITEM_HLSHOP_START_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemHlsEnd(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_CASHITEM_HLSHOP_END_RES));
	sGU_CASHITEM_HLSHOP_END_RES* res = (sGU_CASHITEM_HLSHOP_END_RES*)packet.GetPacketData();

	res->wOpCode = GU_CASHITEM_HLSHOP_END_RES;
	res->wResultCode = GAME_SUCCESS;


	packet.SetPacketLen(sizeof(sGU_CASHITEM_HLSHOP_END_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SendCashItemBuy(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
	
	/*printf("Each rate control %d\n", mob->byDropEachRateControl);
	printf("Drop Eeach Item %d\n", mob->byDropEItemRateControl);
	printf("Drop Legendary Item %d\n", mob->byDropLItemRateControl);
	printf("Drop Normal Item %d\n", mob->byDropNItemRateControl);
	printf("Drop Superior Item %d\n", mob->byDropSItemRateControl);
	printf("Drop Type %d\n", mob->byDropTypeRateControl);
	printf("Drop each tblidx %d\n", mob->dropEachTblidx);
	printf("Drop Quest tblidx %d\n", mob->dropQuestTblidx);
	printf("Drop Type TBLIDX %d\n", mob->dropTypeTblidx);
	printf("Drop Item TBLIDX %d\n", mob->drop_Item_Tblidx);*/
	CBasicDropTable *bDrop = app->g_pTableContainer->GetBasicDropTable();
	CEachDropTable * edrop = app->g_pTableContainer->GetEachDropTable();
	CNormalDropTable * ndrop = app->g_pTableContainer->GetNormalDropTable();
	CSuperiorDropTable * sdrop = app->g_pTableContainer->GetSuperiorDropTable();
	CLegendaryDropTable * ldrop = app->g_pTableContainer->GetLegendaryDropTable();
	CExcellentDropTable * exdrop = app->g_pTableContainer->GetExcellentDropTable();
	CItemTable * iTable = app->g_pTableContainer->GetItemTable();
	CTypeDropTable * tdrop = app->g_pTableContainer->GetTypeDropTable();
	//sVECTOR3 playerPos = plr->GetPlayerPosition();	
	//Drop Each Table
	int mobid = 0;
	sMOB_TBLDAT* mob = (sMOB_TBLDAT*)app->g_pTableContainer->GetMobTable()->FindData(mobid);

//		sEACH_DROP_TBLDAT* eDropDat = (sEACH_DROP_TBLDAT*)(edrop->FindData(mob->dropEachTblidx));	
		
	//Main Drop Tables
	
		sBASIC_DROP_TBLDAT* DropDat = (sBASIC_DROP_TBLDAT*)(bDrop->FindData(mob->drop_Item_Tblidx));	

		for (int i = 0; i < NTL_MAX_DROP_TABLE_SELECT; i++)
		{
		//	sNORMAL_DROP_TBLDAT* nDropDat = (sNORMAL_DROP_TBLDAT*)(ndrop->FindData(DropDat->aNoramalDropTblidx[i]));


			//sSUPERIOR_DROP_TBLDAT* sDropDat = (sSUPERIOR_DROP_TBLDAT*)(sdrop->FindData(DropDat->aSuperiorDropTblidx[i]));


			//sEXCELLENT_DROP_TBLDAT* sDropDat = (sEXCELLENT_DROP_TBLDAT*)(sdrop->FindData(DropDat->aExcellentDropTblidx[i]));

			sLEGENDARY_DROP_TBLDAT* sDropDat = (sLEGENDARY_DROP_TBLDAT*)(sdrop->FindData(DropDat->aLegendaryDropTblidx[i]));
			

			CNtlPacket packet(sizeof(Drop));
			Drop * res = (Drop *)packet.GetPacketData();


			printf("item drop \n");
			//Randomizing numbers for  see if the player get a Fucking item 
			std::random_device rd4;
			std::mt19937_64 mt4(rd4());
			std::uniform_int_distribution<int> distribution4(10, 15);
			//sITEM_TBLDAT* pLegendary = (sITEM_TBLDAT*)app->g_pTableContainer->GetItemTable()->FindData(sDropDat->aItem_Tblidx[z]);
			res->Handle = AcquireItemSerialId() + 1;
			res->Type = OBJTYPE_DROPITEM;
			res->Tblidx = DropDat->aLegendaryDropTblidx[i];
			res->Grade = 0;
			res->Rank = ITEM_RANK_LEGENDARY;
			res->IsNew = false;
			//res->bNeedToIdentify = false;
			res->Loc[0] = plr->GetPlayerPosition().x;
			res->Loc[1] = plr->GetPlayerPosition().y;
			res->Loc[2] = plr->GetPlayerPosition().z;
			//res->Size = 5;
			res->wOpCode = GU_OBJECT_CREATE;
			printf("Item Created %d \n Local X %d \n Local Y %d \n Local Z %d \n", res->Tblidx, res->Loc[0], res->Tblidx, res->Loc[1], res->Tblidx, res->Loc[2]);

			//		app->AddNewItemDrop(res2->handle, sDropDat->aItem_Tblidx[z], res2->sObjectInfo.itemBrief.byGrade, res2->sObjectInfo.itemBrief.byRank);
			if (res->Tblidx == 11170058)
			{
				printf("item Error \n");
				res->Handle = AcquireItemSerialId() + 1;
				res->Type = OBJTYPE_DROPITEM;
				res->Tblidx = 11170019;
				res->Grade = 0;
				res->Rank = ITEM_RANK_LEGENDARY;
				res->IsNew = false;
				//res->bNeedToIdentify = false;
				res->Loc[0] = plr->GetPlayerPosition().x;
				res->Loc[1] = plr->GetPlayerPosition().y;
				res->Loc[2] = plr->GetPlayerPosition().z;

				res->wOpCode = GU_OBJECT_CREATE;
			}





			packet.SetPacketLen(sizeof(Drop));
			g_pApp->Send(this->GetHandle(), &packet);
			app->UserBroadcastothers(&packet, this);
		}

				
			
		

	
}
void CClientSession::SendDragonballsEvent(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_DRAGONBALL_SCHEDULE_INFO));
	sGU_DRAGONBALL_SCHEDULE_INFO* res = (sGU_DRAGONBALL_SCHEDULE_INFO*)packet.GetPacketData();

	res->bIsAlive = 1;
	res->byEventType = 2;
	res->byTermType = 5;
	res->dwMainTerm = 4;
	res->dwSubTerm = 3;
	res->nEndTime = HOUR;
	res->nStartTime = HOUR;
	res->wOpCode = GU_DRAGONBALL_SCHEDULE_INFO;
	

	packet.SetPacketLen(sizeof(sGU_DRAGONBALL_SCHEDULE_INFO));
	g_pApp->Send(this->GetHandle(), &packet);

	
}
void CClientSession::SendBudokaiState(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_BUDOKAI_STATE_INFO_NFY));
	sGU_BUDOKAI_STATE_INFO_NFY* res = (sGU_BUDOKAI_STATE_INFO_NFY*)packet.GetPacketData();

	res->aMatchStateInfo->byState = 1;
	res->bRankbattleAllow = 1;
	//res1->sJoinInfo = 1;
	//res1->sStateInfo = 1;
	res->tmDefaultOpenTime = 1;
	res->wOpCode = GU_BUDOKAI_STATE_INFO_NFY;


	packet.SetPacketLen(sizeof(sGU_BUDOKAI_STATE_INFO_NFY));
	g_pApp->Send(this->GetHandle(), &packet);
}
//Update the Netplay poit need some logic for incressing evry 15 min
void CClientSession::SendUpdateToken(CNtlPacket * pPacket, CGameServer * app)
{
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_NETP));
	sGU_UPDATE_CHAR_NETP* res = (sGU_UPDATE_CHAR_NETP*)packet.GetPacketData();
	int time = 20;
	res->dwAccumulationNetP = 1;// poit Accumulation in corrent session
	res->dwBonusNetP = 1;//point to incress on res->netP evry 15 min
	res->netP = 1;//corrent poit
	res->timeNextGainTime = time;//Time for next gain
	res->wOpCode = GU_UPDATE_CHAR_NETP;
	res->wResultCode = GAME_SUCCESS;


	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_NETP));
	g_pApp->Send(this->GetHandle(), &packet);
}
void CClientSession::SenGiftShop(CNtlPacket * pPacket, CGameServer * app)
{
	PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

	CNtlPacket packet(sizeof(Drop));
	Drop * res = (Drop *)packet.GetPacketData();


		printf("item drop \n");
		//Randomizing numbers for  see if the player get a Fucking item 
		std::random_device rd4;
		std::mt19937_64 mt4(rd4());
		std::uniform_int_distribution<int> distribution4(10, 15);
		//sITEM_TBLDAT* pLegendary = (sITEM_TBLDAT*)app->g_pTableContainer->GetItemTable()->FindData(sDropDat->aItem_Tblidx[z]);
		res->Handle = AcquireItemSerialId() + 1;
		res->Type = OBJTYPE_DROPITEM;
		res->Tblidx = 11170019 + rand() % 261 + 1;
		res->Grade = 0;
		res->Rank = ITEM_RANK_LEGENDARY;
		res->IsNew = false;
		//res->bNeedToIdentify = false;
		res->Loc[0] = plr->GetPlayerPosition().x;
		res->Loc[1] = plr->GetPlayerPosition().y;
		res->Loc[2] = plr->GetPlayerPosition().z;
		//res->Size = 5;
		res->wOpCode = GU_OBJECT_CREATE;
				printf("Item Created %d \n Local X %d \n Local Y %d \n Local Z %d \n", res->Tblidx, res->Loc[0], res->Tblidx, res->Loc[1], res->Tblidx, res->Loc[2]);

				//		app->AddNewItemDrop(res2->handle, sDropDat->aItem_Tblidx[z], res2->sObjectInfo.itemBrief.byGrade, res2->sObjectInfo.itemBrief.byRank);
				if (res->Tblidx == 11170058)
				{
					printf("item Error \n");
					res->Handle = AcquireItemSerialId() + 1;
					res->Type = OBJTYPE_DROPITEM;
					res->Tblidx = 11170019;
					res->Grade = 0;
					res->Rank = ITEM_RANK_LEGENDARY;
					res->IsNew = false;
					//res->bNeedToIdentify = false;
					res->Loc[0] = plr->GetPlayerPosition().x;
					res->Loc[1] = plr->GetPlayerPosition().y;
					res->Loc[2] = plr->GetPlayerPosition().z;
					
					res->wOpCode = GU_OBJECT_CREATE;
				}


			
		
	
	packet.SetPacketLen(sizeof(Drop));
	g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
}

//Helper Functions
void CClientSession::UpdateCharState(HOBJECT avHandle, eCHARSTATE state)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE* res = (sGU_UPDATE_CHAR_STATE*)packet.GetPacketData();
	res->handle = avHandle;
	res->sCharState.sCharStateBase.byStateID = state;
	res->wOpCode = GU_UPDATE_CHAR_STATE;
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	app->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
}
