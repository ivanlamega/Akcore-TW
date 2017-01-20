#include "stdafx.h"
#include "GameServer.h"

bool	GsFunctionsClass::DeleteItemByUIdPlacePos(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 UniqueID, RwUInt32 Place, RwUInt32 Pos)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CNtlPacket packet(sizeof(sGU_ITEM_DELETE));
	sGU_ITEM_DELETE * res = (sGU_ITEM_DELETE *)packet.GetPacketData();

	res->wOpCode = GU_ITEM_DELETE;
	res->hSrcItem = UniqueID;
	res->bySrcPlace = Place;
	res->bySrcPos = Pos;

	packet.SetPacketLen(sizeof(sGU_ITEM_DELETE));
	g_pApp->Send( pSession->GetHandle() , &packet );

	return true;
}
/*
 * TODO: Get the right Effect for each thing
 */
 int     GsFunctionsClass::GetBattleResultEffect(int RpEffect)
 {
 	switch (RpEffect)
 	{
 	case DBO_RP_BONUS_TYPE_KNOCKDOWN:
 		return BATTLE_ATTACK_RESULT_KNOCKDOWN;
 	
 	case DBO_RP_BONUS_TYPE_RESULT_PLUS:
 		return BATTLE_ATTACK_RESULT_CRITICAL_HIT;
 
 	case DBO_RP_BONUS_TYPE_EP_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_KEEP_TIME_PLUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_GUARD_CRASH:
 		return BATTLE_ATTACK_RESULT_CRITICAL_HIT;
 
 	case DBO_RP_BONUS_TYPE_CASTING_TIME_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_COOL_TIME_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	default:
 		return BATTLE_ATTACK_RESULT_HIT;
 	}
 }
bool	GsFunctionsClass::UpdateCharMoney(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 ChangeType, RwUInt32 MoneyAmount, RwUInt32 AvatarHandle)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_ZENNY));
	sGU_UPDATE_CHAR_ZENNY * res = (sGU_UPDATE_CHAR_ZENNY *)packet.GetPacketData();

	res->byChangeType = ChangeType;
	res->dwZenny = MoneyAmount;
	res->handle = AvatarHandle;
	res->wOpCode = GU_UPDATE_CHAR_ZENNY;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ZENNY));
	g_pApp->Send( pSession->GetHandle() , &packet );

	return true;
}
DWORD	GsFunctionsClass::CalculePowerLevelPlayer(PlayersMain *plr)
{
	if (plr)
		return Dbo_CalculatePowerLevel(plr->GetPcProfile()->avatarAttribute.byLastStr, plr->GetPcProfile()->avatarAttribute.byLastCon, plr->GetPcProfile()->avatarAttribute.byLastFoc, plr->GetPcProfile()->avatarAttribute.byLastDex,
		plr->GetPcProfile()->avatarAttribute.byLastSol, plr->GetPcProfile()->avatarAttribute.byLastEng, plr->GetPcProfile()->avatarAttribute.wLastPhysicalOffence, plr->GetPcProfile()->avatarAttribute.wLastPhysicalDefence,
		plr->GetPcProfile()->avatarAttribute.wLastEnergyOffence, plr->GetPcProfile()->avatarAttribute.wLastEnergyDefence, plr->GetPcProfile()->avatarAttribute.wLastAttackRate, plr->GetPcProfile()->avatarAttribute.wLastDodgeRate, plr->GetPcProfile()->avatarAttribute.wLastCurseSuccessRate,
		plr->GetPcProfile()->avatarAttribute.wLastCurseToleranceRate, plr->GetPcProfile()->avatarAttribute.wLastPhysicalCriticalRate, plr->GetPcProfile()->avatarAttribute.wLastEnergyCriticalRate, plr->GetPcProfile()->avatarAttribute.wLastAttackSpeedRate, plr->GetPcProfile()->avatarAttribute.wBaseMaxLP,
		plr->GetPcProfile()->avatarAttribute.wBaseMaxEP, plr->GetPcProfile()->dwCurLP, plr->GetPcProfile()->wCurEP, plr->GetRpBallCounter(), plr->GetPcProfile()->byLevel, 0);
	return 1;
}
DWORD	GsFunctionsClass::CalculePowerLevel(sMOB_TBLDAT* pMOBtData)
{
	if (pMOBtData)
		return Dbo_CalculatePowerLevel(pMOBtData->byStr, pMOBtData->byCon, pMOBtData->byFoc,pMOBtData->byDex, pMOBtData->bySol,
			pMOBtData->byEng, pMOBtData->wBasic_Offence, pMOBtData->wBasic_Physical_Defence, pMOBtData->wBasic_Energy_Defence, pMOBtData->wBasic_Energy_Defence,
			pMOBtData->wAttack_Rate, pMOBtData->wDodge_Rate, pMOBtData->wCurse_Success_Rate, pMOBtData->wCurse_Tolerance_Rate, 5, 
			5, pMOBtData->wAttack_Speed_Rate,pMOBtData->wBasic_LP,pMOBtData->wBasic_EP, pMOBtData->wBasic_LP, pMOBtData->wBasic_EP,1,pMOBtData->byLevel, pMOBtData->byGrade);
	return 1;
}
int     GsFunctionsClass::GetTotalSlotSkill(int charID)
 {
 	CGameServer * app = (CGameServer*)NtlSfxGetApp();
 	app->db->prepare("SELECT * FROM skills WHERE owner_id = ?");
 	app->db->setInt(1, charID);
 	app->db->execute();
 	return app->db->rowsCount();
 }
void	GsFunctionsClass::printError(const char* err)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf("%s\n", err);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

void	GsFunctionsClass::printOk(const char* ok)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    printf("%s\n", ok);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

void	GsFunctionsClass::printDebug(const char* dbg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
    printf("%s\n", dbg);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}
void	GsFunctionsClass::DebugSkillType(BYTE skillActType)
{
	switch (skillActType)
	{
	case SKILL_ACTIVE_TYPE_DD:
		printf("SKILL_ACTIVE_TYPE_DD\n");
		break;
	case SKILL_ACTIVE_TYPE_BB:
		printf("SKILL_ACTIVE_TYPE_BB\n");
		break;
	case SKILL_ACTIVE_TYPE_CB:
		printf("SKILL_ACTIVE_TYPE_CB\n");
		break;
	case SKILL_ACTIVE_TYPE_DB:
		printf("SKILL_ACTIVE_TYPE_DB\n");
		break;
	case SKILL_ACTIVE_TYPE_DC:
		printf("SKILL_ACTIVE_TYPE_DC\n");
		break;
	case SKILL_ACTIVE_TYPE_DH:
		printf("SKILL_ACTIVE_TYPE_DH\n");
		break;
	case SKILL_ACTIVE_TYPE_DOT:
		printf("SKILL_ACTIVE_TYPE_DOT\n");
		break;
	}
}
void		GsFunctionsClass::SendItemEffect(CClientSession * pSession, TBLIDX effectIdx, TBLIDX itemUseIdx)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(pSession->GetavatarHandle());
	CNtlPacket packet3(sizeof(sGU_EFFECT_AFFECTED));
	sGU_EFFECT_AFFECTED * pEffectApply = (sGU_EFFECT_AFFECTED*)packet3.GetPacketData();
	sUSE_ITEM_TBLDAT * itemUseTbl = reinterpret_cast<sUSE_ITEM_TBLDAT*>(app->g_pTableContainer->GetUseItemTable()->FindData(itemUseIdx));
	sSYSTEM_EFFECT_TBLDAT * pEffectTbl = reinterpret_cast<sSYSTEM_EFFECT_TBLDAT*>(app->g_pTableContainer->GetSystemEffectTable()->FindData(effectIdx));
	//Prepare Effect Response
	pEffectApply->effectTblidx = pEffectTbl->tblidx;
	pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
	pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
	pEffectApply->handle = pSession->GetavatarHandle();
	pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
	pEffectApply->wResultCode = GAME_SUCCESS;

	packet3.SetPacketLen(sizeof(sGU_EFFECT_AFFECTED));
	//Validation by Effect Code for better read
	switch (pEffectTbl->effectCode)
	{
		case ACTIVE_DIRECT_HEAL:{
			plr->GetPcProfile()->dwCurLP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
		}
			break;
		case ACTIVE_EP_UP:{
			plr->GetPcProfile()->wCurEP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
		}
			break;
		case ACTIVE_TELEPORT_BIND:{
			//Need Write
		}
			break;
		case ACTIVE_EP_OVER_TIME:{
			DWORD totalTime = itemUseTbl->dwKeepTimeInMilliSecs;
			while (totalTime != 1000)
			{
				plr->GetPcProfile()->wCurEP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->effectTblidx = pEffectTbl->tblidx;
				pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
				pEffectApply->handle = pSession->GetavatarHandle();
				pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
				pEffectApply->wResultCode = GAME_SUCCESS;
				g_pApp->Send(pSession->GetHandle(), &packet3);
				app->UserBroadcastothers(&packet3, pSession);
				totalTime -= 1000;
				Sleep(1000);
			}
		}
			break;
		case ACTIVE_RESCUE:{
			//Need Write
		}
			break;
		case ACTIVE_HEAL_OVER_TIME:{
			DWORD totalTime = itemUseTbl->dwKeepTimeInMilliSecs;
			while (totalTime != 1000)
			{
				plr->GetPcProfile()->dwCurLP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->effectTblidx = pEffectTbl->tblidx;
				pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
				pEffectApply->handle = pSession->GetavatarHandle();
				pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
				pEffectApply->wResultCode = GAME_SUCCESS;
				g_pApp->Send(pSession->GetHandle(), &packet3);
				app->UserBroadcastothers(&packet3, pSession);
				totalTime -= 1000;
				Sleep(1000);
			}

		}
			break;
	}
}
//----------------------------------------------------------------------------------//
// NewQuest used only if the db return 0(to see if the char already have this quest
//----------------------------------------------------------------------------------//
void GsFunctionsClass::NewQuest(RwUInt32 CharID, NTL_TS_T_ID tIdQuest, BYTE tsType, DWORD dwParam)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	std::string sql = "INSERT INTO charquestlist (charId,questID,isCompleted,currentStep,type,dwEventData) VALUES(?,?,?,?,?,?)";
	app->db->prepare(sql);
	app->db->setInt(1, CharID);
	app->db->setInt(2, tIdQuest);	
	app->db->setInt(3, 0);
	app->db->setInt(4, 0);
	app->db->setInt(5, tsType);
	app->db->setInt(6, dwParam);
	app->db->execute();
}
//----------------------------------------------------------------------------------//
// QuestStarted Check if the current quest Already started else call NewQuest
//----------------------------------------------------------------------------------//
void GsFunctionsClass::QuestStarted(RwUInt32 CharID, NTL_TS_T_ID tIdQuest, NTL_TS_TC_ID tCurrentStep, NTL_TS_TC_ID tNextAct, BYTE tsType, DWORD dwParam)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	app->db->prepare("SELECT * FROM charquestlist WHERE charId = ? AND questID = ?");
	app->db->setInt(1,CharID);
	app->db->setInt(2, tIdQuest);
	app->db->execute();
	if (app->db->rowsCount() != 0)
	{
		//Is Completed
		if (tNextAct>=100 )
		{
			app->db->prepare("UPDATE charquestlist SET currentStep = ?,nextStep = ?,isCompleted = ? WHERE charId = ? AND questID = ?");
			app->db->setInt(1, tCurrentStep);
			app->db->setInt(2, tNextAct);
			app->db->setInt(3, 1);
			app->db->setInt(4, CharID);
			app->db->setInt(5, tIdQuest);
			app->db->execute();
		}
		else
		{
			app->db->prepare("UPDATE charquestlist SET currentStep = ?,nextStep = ? WHERE charId = ? AND questID = ?");
			app->db->setInt(1, tCurrentStep);			
			app->db->setInt(2, tNextAct);
			app->db->setInt(3, CharID);
			app->db->setInt(4, tIdQuest);
			app->db->execute();
		}
	}		
	else
	{
		NewQuest(CharID, tIdQuest,tsType,dwParam);
	}	
}
//----------------------------------------------------------------------------------//
// Time Quest-GetTmqTblidx By PlayerLevel TODO GET BY MIN AND MAX MEMBER LEVEL?
//----------------------------------------------------------------------------------//
TBLIDX GsFunctionsClass::GetTmq(PlayersMain *plr)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CTimeQuestTable* pTmqTable = app->g_pTableContainer->GetTimeQuestTable();
	int iCounter = 1;//0 Is Invalid Tblidx
	while (true)
	{
		iCounter++;
		sTIMEQUEST_TBLDAT* pTmqTblData = reinterpret_cast<sTIMEQUEST_TBLDAT*>(pTmqTable->FindData(iCounter));
		if (pTmqTblData)
		{
				break;
		}		
	}	
	return iCounter;
}
//----------------------------------------------------------------------------------//
// Time Quest-GetNearDifficult By PlayerLevel(Extracted from client code) 
// don't know if need but in all cases here is
//----------------------------------------------------------------------------------//
RwUInt8	GsFunctionsClass::GetTmqLevel(PlayersMain *plr)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	TBLIDX idTmq = GsFunctionsClass::GetTmq(plr);
	sTIMEQUEST_TBLDAT* pTIMEQUEST_TBLDAT = reinterpret_cast<sTIMEQUEST_TBLDAT*>(app->g_pTableContainer->GetTimeQuestTable()->FindData(idTmq));
	if (!pTIMEQUEST_TBLDAT)
	{
		printf("Not exist tmq table of index : %i ",idTmq);
		return INVALID_TIMEQUEST_DIFFICULTY;
	}

	sTIMEQUEST_DATASET* pDATASET_EASY = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_EASY];
	sTIMEQUEST_DATASET* pDATASET_NORMAL = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_NORMAL];
	sTIMEQUEST_DATASET* pDATASET_HARD = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_HARD];
	struct sLeastLevel
	{
		RwUInt8 byLeasyLevel;
		RwUInt8 byDifficult;
	};
	std::map<RwUInt8, RwUInt8> mapDifficult;

	mapDifficult[pDATASET_EASY->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_EASY;
	mapDifficult[pDATASET_EASY->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_EASY;
	mapDifficult[pDATASET_NORMAL->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_NORMAL;
	mapDifficult[pDATASET_NORMAL->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_NORMAL;
	mapDifficult[pDATASET_HARD->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_HARD;
	mapDifficult[pDATASET_HARD->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_HARD;

	RwUInt8 byLevelGap;
	sLeastLevel leastDifficult;
	RwUInt8 byAvatarLevel = plr->GetPcProfile()->byLevel;
	std::map<RwUInt8, RwUInt8>::iterator it = mapDifficult.begin();

	leastDifficult.byLeasyLevel = 255;
	leastDifficult.byDifficult = INVALID_TIMEQUEST_DIFFICULTY;

	for (; it != mapDifficult.end(); ++it)
	{
		byLevelGap = (RwUInt8)(abs(it->first - byAvatarLevel));
		if (byLevelGap < leastDifficult.byLeasyLevel)
		{
			leastDifficult.byLeasyLevel = byLevelGap;
			leastDifficult.byDifficult = it->second;
		}
	}

	return leastDifficult.byDifficult;
}
//-----------------------------------------------------------
//CreateUpdateItem: this functions gonna do all the things like:
//-Check if need Update our count of items
//-If need create another item because he get the max stackCount
//-If we are using and when get at zero we delete it
//-Need Clean this whole code
//Luiz45
//-----------------------------------------------------------
void	GsFunctionsClass::CreateUpdateItem(PlayersMain *plr, int stackCount, TBLIDX itemID,bool deleteItem,
										   HSESSION ClientSession,int byGrade,int byRank,int byBattleAttrib,
										   int iUniqueHandle,int place,int pos)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CItemTable *itemTbl = app->g_pTableContainer->GetItemTable();
	MySQLConnWrapper *db2 = new MySQLConnWrapper;
	int iHandle = 0;
	db2->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db2->connect();
	db2->switchDb(app->GetConfigFileDatabase());
	if (deleteItem == true)
	{
		CNtlPacket packet2(sizeof(sGU_ITEM_DELETE));
		sGU_ITEM_DELETE * res2 = (sGU_ITEM_DELETE *)packet2.GetPacketData();

		CNtlPacket packet3(sizeof(sGU_ITEM_STACK_UPDATE));
		sGU_ITEM_STACK_UPDATE * res3 = (sGU_ITEM_STACK_UPDATE *)packet3.GetPacketData();

		int iCounter = 0;
		if (pos == 0)
		{
			for (int i = 0; i < plr->cPlayerInventory->GetTotalItemsCount();i++)
			{
				//If is to remove by Handle
				if (plr->cPlayerInventory->GetInventory()[i].handle == iUniqueHandle)
				{
					pos = plr->cPlayerInventory->GetInventory()[i].byPos;
					iCounter = plr->cPlayerInventory->GetInventory()[i].byStackcount;
					iHandle = iUniqueHandle;
				}
				//If is to update only the stackCount
				else if ((plr->cPlayerInventory->GetInventory()[i].byPlace == place) && (plr->cPlayerInventory->GetInventory()[i].byPos == pos))
				{
					iHandle = plr->cPlayerInventory->GetInventory()[i].handle;
					stackCount = 1;
					plr->cPlayerInventory->GetInventory()[i].byStackcount - 1;//Just update the counter in our "Real" inventory
				}
			}
		}
		if ((iCounter-stackCount)==0)
		{
			app->db->prepare("DELETE FROM items WHERE owner_id = ? AND id = ?");
			app->db->setInt(1, plr->GetPcProfile()->charId);
			app->db->setInt(2, iHandle);
			app->db->execute();

			res2->bySrcPlace = place;
			res2->bySrcPos = pos;
			res2->hSrcItem = iHandle;
			res2->wOpCode = GU_ITEM_DELETE;
			packet2.SetPacketLen(sizeof(sGU_ITEM_DELETE));
			g_pApp->Send(ClientSession, &packet2);
			plr->cPlayerInventory->RemoveItemFromInventory(iHandle);//Update our "Real" Inventory
		}		
		else if ((iCounter - stackCount)!=0)
		{
			app->db->prepare("UPDATE items SET count = ? WHERE owner_id = ? AND id = ?");
			app->db->setInt(1, (iCounter - stackCount));
			app->db->setInt(2, plr->GetPcProfile()->charId);
			app->db->setInt(3, iHandle);
			app->db->execute();

			res3->bIsNew = false;
			res3->byStack = (iCounter - stackCount);
			res3->hItemHandle = iHandle;
			res3->wOpCode = GU_ITEM_STACK_UPDATE;
			packet3.SetPacketLen(sizeof(sGU_ITEM_STACK_UPDATE));
			g_pApp->Send(ClientSession, &packet3);
		}
	}
	else
	{
		sITEM_TBLDAT* pItemData = (sITEM_TBLDAT*)itemTbl->FindData(itemID);
		CNtlPacket packet2(sizeof(sGU_ITEM_CREATE));
		sGU_ITEM_CREATE * res2 = (sGU_ITEM_CREATE *)packet2.GetPacketData();

		CNtlPacket packet3(sizeof(sGU_ITEM_STACK_UPDATE));
		sGU_ITEM_STACK_UPDATE * res3 = (sGU_ITEM_STACK_UPDATE *)packet3.GetPacketData();

		int ItemPos = 0;
		bool bUpdateItem = false;
		bool bExit = false;
		place = (plr->cPlayerInventory->CheckAvailableBag());		
		for (int i = 0; i < plr->cPlayerInventory->GetTotalItemsCount(); i++)
		{
			if (plr->cPlayerInventory->GetInventory()[i].tblidx == pItemData->tblidx)
			{
				while (stackCount != 0)
				{
					//if (pItemCount <= pItemData->byMax_Stack) max stack is only for buy
					if (stackCount <= 99)
					{
						bUpdateItem = true;
						app->db->prepare("UPDATE items SET count = ? WHERE owner_id = ? AND id = ?");
						app->db->setInt(1, (plr->cPlayerInventory->GetInventory()[i].byStackcount + stackCount));
						app->db->setInt(2, plr->GetPcProfile()->charId);
						app->db->setInt(3, plr->cPlayerInventory->GetInventory()[i].handle);
						app->db->execute();
						res3->byStack = (plr->cPlayerInventory->GetInventory()[i].byStackcount + stackCount);
						res3->hItemHandle = plr->cPlayerInventory->GetInventory()[i].handle;
						res3->wOpCode = GU_ITEM_STACK_UPDATE;
						packet3.SetPacketLen(sizeof(sGU_ITEM_STACK_UPDATE));
						g_pApp->Send(ClientSession, &packet3);
						(plr->cPlayerInventory->GetInventory()[i].byStackcount+stackCount);
						bExit = true;
						plr->cPlayerInventory->SetBagStatus(GAME_SUCCESS);
						break;
					}
					else if (plr->cPlayerInventory->CheckAvailableBag()!=99)
					{
						res2->bIsNew = true;
						app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?,?,?, @unique_iID)");
						app->db->setInt(1, itemID);
						app->db->setInt(2, plr->GetPcProfile()->charId);
						int byPos = plr->cPlayerInventory->CheckAvailableSlot(place);
						app->db->setInt(3, place);
						app->db->setInt(4, byPos);
						app->db->setInt(5, byRank);
						app->db->setInt(6, byGrade);
						app->db->setInt(7, pItemData->byDurability);
						app->db->execute();
						app->db->execute("SELECT @unique_iID");
						app->db->fetch();
						iHandle = app->db->getInt("@unique_iID");
						app->qry->UpdateItemsCount(iHandle, stackCount);
						res2->wOpCode = GU_ITEM_CREATE;
						res2->handle = iHandle;
						res2->sItemData.charId = plr->GetAvatarHandle();
						res2->sItemData.itemNo = pItemData->tblidx;
						res2->sItemData.byStackcount = stackCount;
						res2->sItemData.itemId = iHandle;
						res2->sItemData.byPlace = place;
						res2->sItemData.byPosition = byPos;
						res2->sItemData.byCurrentDurability = pItemData->byDurability;
						res2->sItemData.byRank = byRank;
						res2->sItemData.byGrade = byGrade;
						res2->sItemData.byBattleAttribute = byBattleAttrib;
						plr->cPlayerInventory->AddItemToInventory(res2->sItemData);
						packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
						g_pApp->Send(ClientSession, &packet2);
						bExit = true;
						plr->cPlayerInventory->SetBagStatus(GAME_SUCCESS);
						break;
					}					
					stackCount--;
				}
			}
			else if (plr->cPlayerInventory->CheckAvailableBag() != 99)
			{
				res2->bIsNew = true;
				app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?,?,?, @unique_iID)");
				app->db->setInt(1, itemID);
				app->db->setInt(2, plr->GetPcProfile()->charId);
				int byPos = plr->cPlayerInventory->CheckAvailableSlot(place);
				app->db->setInt(3, place);
				app->db->setInt(4, byPos);
				app->db->setInt(5, byRank);
				app->db->setInt(6, byGrade);
				app->db->setInt(7, pItemData->byDurability);
				app->db->execute();
				app->db->execute("SELECT @unique_iID");
				app->db->fetch();
				iHandle = app->db->getInt("@unique_iID");
				app->qry->UpdateItemsCount(iHandle, stackCount);
				res2->wOpCode = GU_ITEM_CREATE;
				res2->handle = iHandle;
				res2->sItemData.charId = plr->GetAvatarHandle();
				res2->sItemData.itemNo = pItemData->tblidx;
				res2->sItemData.byStackcount = stackCount;
				res2->sItemData.itemId = iHandle;
				res2->sItemData.byPlace = place;
				res2->sItemData.byPosition = byPos;
				res2->sItemData.byCurrentDurability = pItemData->byDurability;
				res2->sItemData.byRank = byRank;
				res2->sItemData.byGrade = byGrade;
				sITEM_SUMMARY* pALSKo = reinterpret_cast<sITEM_SUMMARY*>(itemTbl->FindData(pItemData->tblidx));
				sITEM_BRIEF* pasl = reinterpret_cast<sITEM_BRIEF*>(itemTbl->FindData(pItemData->tblidx));
				plr->cPlayerInventory->AddItemToInventory(res2->sItemData);
				packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
				g_pApp->Send(ClientSession, &packet2);
				bExit = true;
				plr->cPlayerInventory->SetBagStatus(GAME_SUCCESS);
				break;
			}
			else
				continue;
		}
		if (!bExit)
		{
			printf("All bags for that player are full, sending a notify to the client...\n");
			CNtlPacket packet4(sizeof(sGU_ITEM_PICK_RES));
			sGU_ITEM_PICK_RES* res4 = (sGU_ITEM_PICK_RES*)packet4.GetPacketData();
			res4->bByPartyHunting = false;
			res4->itemTblidx = 0;
			res4->wResultCode = GAME_ITEM_INVEN_FULL;
			res4->wOpCode = GU_ITEM_PICK_RES;

			packet4.SetPacketLen(sizeof(sGU_ITEM_PICK_RES));
			g_pApp->Send(ClientSession, &packet4);
			plr->cPlayerInventory->SetBagStatus(GAME_ITEM_INVEN_FULL);
			return;
		}
		/*app->db->prepare("SELECT * FROM items WHERE owner_id = ? AND place=? ORDER BY pos ASC");
		app->db->setInt(1, plr->GetPcProfile()->charId);
		app->db->setInt(2, place);
		app->db->execute();
		int k = 0;		
		int pLastStack = stackCount;
		int iMaxSlotSearch = NTL_MAX_ITEM_SLOT;
		bool bHaveFreeSlot = false;
		bool bAnyItemInBag;
		//Check if have any item in the bag
		if (app->db->rowsCount() != 0)
			bAnyItemInBag = true;
		else
			bAnyItemInBag = false;

		//Free Slots
		while (NTL_MAX_ITEM_SLOT != 0)
		{
			iMaxSlotSearch--;
			db2->prepare("SELECT id FROM items WHERE owner_id = ? AND place = ? AND pos = ? ");
			db2->setInt(1, plr->GetPcProfile()->charId);
			db2->setInt(2, place);
			db2->setInt(3, iMaxSlotSearch);
			db2->execute();
			if (db2->rowsCount() != 0)
				continue;			
			else
			{
					ItemPos = iMaxSlotSearch;
					bHaveFreeSlot = ((-1) * 1 == iMaxSlotSearch ? false : true);
					delete db2;
					break;
			}
		}		
		while (app->db->fetch())
		{	
			if (app->db->getInt("tblidx") == pItemData->tblidx)
			{
				int pItemCount = app->db->getInt("count");
				iHandle = app->db->getInt("id");				
				res2->bIsNew = false;
				while (stackCount != 0)
				{					
					//if (pItemCount <= pItemData->byMax_Stack) max stack is only for buy
					if (stackCount <= 99)
					{
						app->db->prepare("UPDATE items SET count = ? WHERE owner_id = ? AND id = ?");
						app->db->setInt(1, (pItemCount+stackCount));
						app->db->setInt(2, plr->GetPcProfile()->charId);
						app->db->setInt(3, iHandle);
						app->db->execute();									
						res3->byStack = (pItemCount + stackCount);
						res3->hItemHandle = iHandle;
						res3->wOpCode = GU_ITEM_STACK_UPDATE;
						packet3.SetPacketLen(sizeof(sGU_ITEM_STACK_UPDATE));
						g_pApp->Send(ClientSession, &packet3);
						break;
					}
					else if (bHaveFreeSlot == true)
					{
						res2->bIsNew = true;
						app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?,?, @unique_iID)");
						app->db->setInt(1, itemID);
						app->db->setInt(2, plr->GetPcProfile()->charId);
						int byPos   = plr->cPlayerInventory->CheckAvailableSlot(place);
						app->db->setInt(3, place);
						app->db->setInt(4, byPos);
						app->db->setInt(5, pItemData->byRank);
						app->db->setInt(6, pItemData->byDurability);
						app->db->execute();
						app->db->execute("SELECT @unique_iID");
						app->db->fetch();
						iHandle = app->db->getInt("@unique_iID");
						app->qry->UpdateItemsCount(iHandle, stackCount);
						res2->wOpCode = GU_ITEM_CREATE;
						res2->handle = iHandle;
						res2->sItemData.charId = plr->GetAvatarHandle();
						res2->sItemData.itemNo = pItemData->tblidx;
						res2->sItemData.byStackcount = stackCount;
						res2->sItemData.itemId = iHandle;
						res2->sItemData.byPlace = place;
						res2->sItemData.byPosition = byPos;
						res2->sItemData.byCurrentDurability = pItemData->byDurability;
						res2->sItemData.byRank = pItemData->byRank;
						plr->cPlayerInventory->AddItemToInventory(res2->sItemData);
						packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
						g_pApp->Send(ClientSession, &packet2);
						break;
					}
					stackCount--;
				}						
			}
			else if (bHaveFreeSlot == true)
			{
				res2->bIsNew = true;
				app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?,?, @unique_iID)");
				app->db->setInt(1, itemID);
				app->db->setInt(2, plr->GetPcProfile()->charId);
				int byPos = plr->cPlayerInventory->CheckAvailableSlot(place);
				app->db->setInt(3, place);
				app->db->setInt(4, byPos);
				app->db->setInt(5, pItemData->byRank);
				app->db->setInt(6, pItemData->byDurability);
				app->db->execute();
				app->db->execute("SELECT @unique_iID");
				app->db->fetch();
				iHandle = app->db->getInt("@unique_iID");
				app->qry->UpdateItemsCount(iHandle, stackCount);
				res2->wOpCode = GU_ITEM_CREATE;
				res2->handle = iHandle;
				res2->sItemData.charId = plr->GetAvatarHandle();
				res2->sItemData.itemNo = pItemData->tblidx;
				res2->sItemData.byStackcount = stackCount;
				res2->sItemData.itemId = iHandle;
				res2->sItemData.byPlace = place;
				res2->sItemData.byPosition = byPos;
				res2->sItemData.byCurrentDurability = pItemData->byDurability;
				res2->sItemData.byRank = pItemData->byRank;
				plr->cPlayerInventory->AddItemToInventory(res2->sItemData);
				packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
				g_pApp->Send(ClientSession, &packet2);

				break;
			}
			else
				continue;
		}
		//Only Added to see if already have any item in our Bag's
		if (!bAnyItemInBag)
		{
			res2->bIsNew = true;
			app->db->prepare("CALL BuyItemFromShop (?,?,?,?,?,?, @unique_iID)");
			app->db->setInt(1, itemID);
			app->db->setInt(2, plr->GetPcProfile()->charId);
			int byPos = plr->cPlayerInventory->CheckAvailableSlot(place);
			app->db->setInt(3, place);
			app->db->setInt(4, byPos);
			app->db->setInt(5, pItemData->byRank);
			app->db->setInt(6, pItemData->byDurability);
			app->db->execute();
			app->db->execute("SELECT @unique_iID");
			app->db->fetch();
			iHandle = app->db->getInt("@unique_iID");
			app->qry->UpdateItemsCount(iHandle, stackCount);
			res2->wOpCode = GU_ITEM_CREATE;
			res2->handle = iHandle;
			res2->sItemData.charId = plr->GetAvatarHandle();
			res2->sItemData.itemNo = pItemData->tblidx;
			res2->sItemData.byStackcount = stackCount;
			res2->sItemData.itemId = iHandle;
			res2->sItemData.byPlace = place;
			res2->sItemData.byPosition = byPos;
			res2->sItemData.byCurrentDurability = pItemData->byDurability;
			res2->sItemData.byRank = pItemData->byRank;

			packet2.SetPacketLen(sizeof(sGU_ITEM_CREATE));
			g_pApp->Send(ClientSession, &packet2);
		}*/

	}
}

//This command parses everything in the consoleCommands enum
//based on the input provided from the console itself
consoleCommands	GsFunctionsClass::parseConsoleEnums(std::string commands)
{
	std::transform(commands.begin(), commands.end(), commands.begin(), ::tolower);
	if (commands == "announce") return eANNOUNCE;
	if (commands == "broadcast") return eEMERGENCY;
	if (commands == "additem") return eADDITEM;
	if (commands == "addmob") return eSPAWNMOB;
	if (commands == "addnpc") return eSPAWNNPC;
	if (commands == "addskill") return eADDSKILL;
	if (commands == "scripttest") return eSCRIPTEXE;
	if (commands == "help") return eHELP;
}

//this function will clear the console
void GsFunctionsClass::commandFinished()
{
	printf("Dbo Tw server Emulator> ");
	fflush(stdout);
}

//This function handles commands at the console level
int	GsFunctionsClass::consoleCommandHandler(std::string command)
{
	CClientSession client;
					
	switch(parseConsoleEnums(command))
	{
		case eANNOUNCE:
			{
				CGameServer * app2 = (CGameServer*)NtlSfxGetApp();
				wstring sMsg = L"";
				cout << "Please enter a Msg: ";
				getline(wcin, sMsg);
				if (!sMsg.empty())
				{
					wcout << L"Message is " << sMsg << endl;
					client.SendServerAnnouncement(sMsg, app2);
					commandFinished();
					//std::cout << "MSG Sent!" << std::endl;
				}
				sMsg = L"";
				app2 = NULL;
				return 1;
			}
		case eEMERGENCY:
			{
				CGameServer * app2 = (CGameServer*)NtlSfxGetApp();
				wstring sMsg = L"";
				cout << "Please enter a Msg: ";
				getline(wcin, sMsg);
				if (!sMsg.empty())
				{
					wcout << L"Message is " << sMsg << endl;
					client.SendServerBroadcast(sMsg, app2);
					commandFinished();
					//std::cout << "MSG Sent!" << std::endl;
				}
				sMsg = L"";
				app2 = NULL;
				return 1;
			}
			return 1;
		case eADDITEM:
			{
				CGameServer *app = (CGameServer*)NtlSfxGetApp();
				app->db = new MySQLConnWrapper;
				app->db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
				app->db->connect();
				app->db->switchDb(app->GetConfigFileDatabase());
				string sPlayer = "test";
				uint32_t tblidx = 0;
				//cout << "Please enter recipents name: ";
				//getline(cin, sPlayer);
				app->db->prepare("SELECT * from characters where CharName = ?");
				app->db->setString(1, sPlayer);
				app->db->execute();
				if(app->db->fetch() != NULL)
				{
					int playerId = app->db->getInt("CharID");
					cout << "Please enter an item id: ";
					cin >> tblidx;
					
					cout << "Intended Recipent : " << sPlayer << endl << "Item ID: " << tblidx  << endl << "PlayerID: "<< playerId << endl;

					app->pSession->CreateItemById(tblidx, playerId);
					commandFinished();
				}


				//clear these pointers before returning.
				app = NULL;
				sPlayer = "";
				tblidx = 0;
			
			}
			return 1;
		case eSPAWNNPC:
		{	CGameServer *app = (CGameServer*)NtlSfxGetApp();
			app->db = new MySQLConnWrapper;
			app->db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
			app->db->connect();
			app->db->switchDb(app->GetConfigFileDatabase());
			string sPlayer = "test";
			uint32_t tblidx = 0;
			//cout << "Please enter players name to spawn near: ";
			//getline(cin, sPlayer);
			app->db->prepare("SELECT * from characters where CharName = ?");
			app->db->setString(1, sPlayer);
			app->db->execute();
			if (app->db->fetch() != NULL)
			{
				int playerId = app->db->getInt("CharID");

				cout << "Please enter an Mob id: ";
				cin >> tblidx;

				cout << "Spawn at : " << sPlayer  << "'s Position." << endl << "Mob ID: " << tblidx << endl << "PlayerID: " << playerId << endl;
				app->pSession->CreateNPCById(tblidx, playerId);
				commandFinished();
			}
			//clear these pointers before returning.
			app = NULL;
			sPlayer = "";
			tblidx = 0;

		}
			return 1;
		case eSPAWNMOB:
		{	CGameServer *app = (CGameServer*)NtlSfxGetApp();
		app->db = new MySQLConnWrapper;
		app->db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
		app->db->connect();
		app->db->switchDb(app->GetConfigFileDatabase());
		string sPlayer = "test";
		uint32_t tblidx = 0;
		//cout << "Please enter players name to spawn near: ";
		//getline(cin, sPlayer);
		app->db->prepare("SELECT * from characters where CharName = ?");
		app->db->setString(1, sPlayer);
		app->db->execute();
		if (app->db->fetch() != NULL)
		{
			int playerId = app->db->getInt("CharID");

			cout << "Please enter an Mob id: ";
			cin >> tblidx;

			cout << "Spawn at : " << sPlayer << "'s Position." << endl << "Mob ID: " << tblidx << endl << "PlayerID: " << playerId << endl;
			app->pSession->CreateMonsterById(tblidx, playerId);
			commandFinished();
		}
		//clear these pointers before returning.
		app = NULL;
		sPlayer = "";
		tblidx = 0;

		}
			return 1;
		case eADDSKILL:
		{	CGameServer *app = (CGameServer*)NtlSfxGetApp();
		app->db = new MySQLConnWrapper;
		app->db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
		app->db->connect();
		app->db->switchDb(app->GetConfigFileDatabase());
		string sPlayer = "";
		uint32_t tblidx = 0;
		cout << "Please enter recipents name: ";
		getline(cin, sPlayer);
		app->db->prepare("SELECT * from characters where CharName = ?");
		app->db->setString(1, sPlayer);
		app->db->execute();
			if (app->db->fetch() != NULL)
			{
				int playerId = app->db->getInt("CharID");

				cout << "Please enter an Skill id: ";
				cin >> tblidx;

				cout << "Recipients Name: " << sPlayer << endl << "Skill ID: " << tblidx << endl << "PlayerID: " << playerId << endl;
				app->pSession->AddSkillById(tblidx, playerId);
				commandFinished();
			}
			//clear these pointers before returning.
			app = NULL;
			sPlayer = "";
			tblidx = 0;

		}
			return 1;
		case eHELP:
			cout << "We only have a few commands working for right now." << endl;
			cout << "You must do input one at a time for now." << endl;
			cout << "Here is the list." << endl;
			cout << "announce: announces the text across the top of the screen" << endl;
			cout << "broadcast: same thing as announce but with Tori" << endl;
			cout << "addItem: Adds an item to the named player character" << endl;
			cout << "addSkill: Adds an skill to the named player character" << endl;
			cout << "addMob: Adds a mob near the named player character" << endl;
			cout << "help: Displays this help output." << endl;
			commandFinished();
			return 1;
		case eSCRIPTEXE:
		{
			CGameServer *app = (CGameServer*)NtlSfxGetApp();
			app->db = new MySQLConnWrapper;
			app->db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
			app->db->connect();
			app->db->switchDb(app->GetConfigFileDatabase());
			string sPlayer = "";
			uint32_t tblidx = 0;
			bool sync = false;
			cout << "Please enter player name: ";
			getline(cin, sPlayer);
			app->db->prepare("SELECT * from characters where CharName = ?");
			app->db->setString(1, sPlayer);
			app->db->execute();
			if (app->db->fetch() != NULL)
			{
				int playerId = app->db->getInt("CharID");
				cout << "Please enter script tbildx(directPlay): ";
				cin >> tblidx;
				cout << endl;
				cout << "Synchronize?";
				cin >> sync;
				app->pSession->SendTestDirectPlay(tblidx, playerId, sync);
			}
			commandFinished();
			return 1;
		}
		default:
			cout << "Command not found" << endl;
			commandFinished();
			return 0;
	}
	return 0;
}