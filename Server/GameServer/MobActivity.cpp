#include "stdafx.h"
#include "GameServer.h"

RwUInt32		MobActivity::AcquireMOBSerialId(void)
{
	if(m_uiSerialId++)
	{
		if(m_uiSerialId == 0xffffffff)
			m_uiSerialId = 0;
	}

	return m_uiSerialId;
}
///// CREATE MONSTER LIST ///
void			MobActivity::CreatureData::ResetMob()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	this->curPos = this->Spawn_Loc;
	this->FightMode = false;
	this->isAggro = false;
	this->target = 0;
	this->CurLP = this->MaxLP;
	this->CurEP = this->MaxEP;
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
	sGU_UPDATE_CHAR_LP_EP * res = (sGU_UPDATE_CHAR_LP_EP *)packet.GetPacketData();

	res->handle = this->MonsterSpawnID;
	res->wCurEP = this->CurEP;
	res->wCurLP = this->CurLP;
	res->wMaxEP = this->MaxEP;
	res->wMaxLP = this->MaxLP;
	res->wOpCode = GU_UPDATE_CHAR_LP_EP;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
	app->UserBroadcast(&packet);
}
void			MobActivity::CreatureData::MoveToSpawn()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->MonsterSpawnID;
	res->sCharState.sCharStateBase.bFightMode = false;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
	res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 1;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = this->Spawn_Loc.x;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = this->Spawn_Loc.y;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = this->Spawn_Loc.z;
	res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;
	packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
	app->UserBroadcast(&packet);
}
void			MobActivity::CreatureData::MoveToPlayer(PlayersMain *plr)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
	
	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->MonsterSpawnID;
	res->sCharState.sCharStateBase.bFightMode = true;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
	res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 1;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = plr->GetPlayerPosition().x - 1;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = plr->GetPlayerPosition().y;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = plr->GetPlayerPosition().z - 1;
	res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;
	packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
	app->UserBroadcast(&packet);
	this->curPos.x = plr->GetPlayerPosition().x - 1;
	this->curPos.y = plr->GetPlayerPosition().y;
	this->curPos.z = plr->GetPlayerPosition().z - 1;
}
void	MobActivity::CreatureData::MoveToRand()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
       
	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->MonsterSpawnID;
	res->sCharState.sCharStateBase.bFightMode = true;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
	res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 1;
	if (rand() % 10 >= 5)
	{
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = this->curPos.x + rand() % 3;
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = this->curPos.y;
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = this->curPos.z + rand() % 3;
	}
	else
	{
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = this->curPos.x - rand() % 3;
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = this->curPos.y;
		res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = this->curPos.z - rand() % 3;
	}
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].x = this->Spawn_Loc.x;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].y = this->Spawn_Loc.y;
	res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].z = this->Spawn_Loc.z;
	res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = true;
	packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
	app->UserBroadcast(&packet);
}
DWORD WINAPI	Aggro(LPVOID arg)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	MobActivity::CreatureData* mob = (MobActivity::CreatureData*)arg;
	PlayersMain* plr = new PlayersMain(NULL,NULL,NULL);//Just to Bypass the while check
	bool haveAttack = false;
	if (mob)
	{
		while (mob && plr)
		{
			for (CGameServer::USERIT it = app->m_userList.begin(); it != app->m_userList.end(); it++)
			{
				try
				{
					plr = app->GetUserSession(it->second->cPlayersMain->GetAvatarHandle());//Here we overwrite our object
				}
				catch (exception e)
				{
					printf("Avatar Handle is invalid \n\r Error:%s", e.what());
				}
				if (plr)
				{
					sVECTOR3 myCurPos;
					myCurPos.x = mob->curPos.x;
					myCurPos.y = mob->curPos.y;
					myCurPos.z = mob->curPos.z;
					sVECTOR3 SpawnPos;
					SpawnPos.x = mob->Spawn_Loc.x;
					SpawnPos.y = mob->Spawn_Loc.y;
					SpawnPos.z = mob->Spawn_Loc.z;
					float distance = app->mob->Distance(myCurPos, plr->GetPlayerPosition());
					float distanceToSpawn = app->mob->Distance(myCurPos, SpawnPos);
					if (mob->IsDead == false && plr->GetPlayerDead() == false)
					{
						if (distance < mob->Basic_aggro_point && distance > mob->Attack_range && mob->isAggro == false)
						{
							mob->isAggro = true;
							mob->target = plr->GetAvatarHandle();
							mob->MoveToPlayer(plr);
						haveAttack = false;
						}
						else if (mob->isAggro == true && plr->GetAvatarHandle() == mob->target && distance > mob->Attack_range && distance < mob->Basic_aggro_point)
							mob->MoveToPlayer(plr);
						else if (distance <= mob->Attack_range && mob->isAggro == true && plr->GetAvatarHandle() == mob->target)
						{
							if (mob->chainAttackCount >= mob->MaxchainAttackCount)
								mob->chainAttackCount = 0;
							haveAttack = true;
							mob->Attack(plr, app);
						}
						else if (distanceToSpawn > mob->Basic_aggro_point + 10 && mob->isAggro == true && plr->GetAvatarHandle() == mob->target)
						{
							haveAttack = false;
							mob->ResetMob();
							mob->MoveToSpawn();
						}
						else if (mob->isAggro == false)
							if (rand() % 15 > 12)
								if(timeGetTime() - mob->last_mobMove >= MONSTER_MOVE_UPDATE_TICK)
								{
									mob->last_mobMove = timeGetTime();
									mob->MoveToRand();
								}
					}
					if (mob->IsDead == false && plr->GetPlayerDead() == true)
					{
						if (mob->isAggro == true && plr->GetAvatarHandle() == mob->target)
						{
							haveAttack = false;
							mob->ResetMob();
							mob->MoveToSpawn();
						}
					}
				}
			}
			Sleep(10000);
		}
	}
	return 0;
}
void		MobActivity::CreatureData::KillThreadAggro()
{
	if (TerminateThread(this->hThreadAggro, 1) == 0)
		if (this->hThreadAggro == NULL)
			printf("Can't kill thread mob aggro because already killed\n");
	if (this->hThreadAggro)
		CloseHandle(this->hThreadAggro);
}
void		MobActivity::CreatureData::RunThreadAggro()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	this->dwThreadIdAggro = app->ThreadRequest();
//	this->hThreadAggro = CreateThread(NULL, 0, Aggro, (LPVOID)this, 0, &this->dwThreadIdAggro);
	if (this->hThreadAggro == NULL)
        printf("Can't create thread Regen\n");
}
void		MobActivity::CreatureData::Attack(PlayersMain *plr, CGameServer *app)
{
	this->FightMode = true;
	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_ACTION_ATTACK;
	res->hSubject = this->MonsterSpawnID;
	res->hTarget = plr->GetAvatarHandle();
	res->dwLpEpEventId = 255;
	res->byBlockedAction = 255;
	float formula = 0;
	if (this->Level <= 5)
		formula = rand() % 25 + 5;
	else
		formula = (this->Str * this->Level) * .08;
	res->wAttackResultValue = formula;
	res->fReflectedDamage = 0;
	res->vShift = plr->GetPlayerPosition();
	this->chainAttackCount += 1;
	res->byAttackSequence = this->chainAttackCount;
	res->bChainAttack = true;
	res->byAttackResult = BATTLE_ATTACK_RESULT_HIT;
	packet.SetPacketLen( sizeof(sGU_CHAR_ACTION_ATTACK) );
	app->UserBroadcast(&packet);
	plr->SetPlayerFight(true);
	plr->SetPlayerDamage(res->wAttackResultValue);
}
///// CREATE MONSTER LIST END ///
float		MobActivity::Distance(const sVECTOR3 mycurPos, const CNtlVector othercurPos)
{
	float first = (mycurPos.x - othercurPos.x) * (mycurPos.x - othercurPos.x);
	float second = (mycurPos.y - othercurPos.y) * (mycurPos.y - othercurPos.y);
	float third = (mycurPos.z - othercurPos.z) * (mycurPos.z - othercurPos.z);
    float distance = sqrt(first + second + third);
    return distance;
}
bool		MobActivity::CreatureRangeCheck(sVECTOR3 mycurPos, CNtlVector othercurPos)
{
	float distance = Distance(mycurPos, othercurPos);
	if (distance < DISTANCE_TO_SPAWN)
	{
		return true;
	}
	return false;
}
MobActivity::CreatureData*	MobActivity::GetMobByHandle(RwUInt32 Target)
{
	CreatureData * creaturelist;

	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if (creaturelist)
		{
			if (creaturelist->MonsterSpawnID == Target)
			{
				return creaturelist;
				break;
			}
		}
	}
	return NULL;
}

void		MobActivity::MonsterRandomWalk(CNtlPacket * pPacket)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CreatureData * creaturelist;
	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(creaturelist->IsDead == false)
		{
			if (creaturelist->FightMode == false && (rand() % 30) >= 20)
			{
				CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
				sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
				res->wOpCode = GU_UPDATE_CHAR_STATE;
				res->handle = creaturelist->MonsterSpawnID;
				res->sCharState.sCharStateBase.bFightMode = false;
				res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
				res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 2;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = creaturelist->Spawn_Loc.x + rand() %15;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = creaturelist->Spawn_Loc.y;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = creaturelist->Spawn_Loc.z + rand() %25;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].x = creaturelist->Spawn_Loc.x + rand() %15;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].y = creaturelist->Spawn_Loc.y;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].z = creaturelist->Spawn_Loc.z + rand() %25;

				res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;

				packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
				app->UserBroadcast(&packet);
			}
		}else if(creaturelist->IsDead == true && timeGetTime() >= creaturelist->KilledTime + creaturelist->Spawn_Cool_Time ) 
		{
			printf("spawn time %i cur time %i killed time %i \n", creaturelist->Spawn_Cool_Time, timeGetTime(), creaturelist->KilledTime);
			creaturelist->IsDead = false;
			
			//Spawn monster
			CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
			sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();

			res->wOpCode = GU_OBJECT_CREATE;
			res->sObjectInfo.objType = OBJTYPE_MOB;
			res->handle = creaturelist->MonsterSpawnID;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = creaturelist->Spawn_Loc.x;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = creaturelist->Spawn_Loc.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = creaturelist->Spawn_Loc.z;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = creaturelist->Spawn_Dir.x;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = creaturelist->Spawn_Dir.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = creaturelist->Spawn_Dir.z;
			res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
			res->sObjectInfo.mobState.sCharStateBase.bFightMode = false;
			res->sObjectInfo.mobBrief.tblidx = creaturelist->MonsterID;
			res->sObjectInfo.mobBrief.wCurEP = creaturelist->CurEP;
			res->sObjectInfo.mobBrief.wMaxEP = creaturelist->MaxEP;
			res->sObjectInfo.mobBrief.wCurLP = creaturelist->CurLP;
			res->sObjectInfo.mobBrief.wMaxLP = creaturelist->MaxLP;
			res->sObjectInfo.mobBrief.fLastRunningSpeed =  creaturelist->Run_Speed;
			res->sObjectInfo.mobBrief.fLastWalkingSpeed = creaturelist->Walk_Speed;

			packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
			app->UserBroadcast(&packet);

			//Insert into all players list
			app->InsertMonsterIntoAllMyMonsterLists(pPacket, creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID);
		}
	}
}
bool		MobActivity::UpdateDeathStatus(RwUInt32 MobID, bool death_status)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CreatureData * creaturelist;
	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(creaturelist != NULL)
		{
			if ( creaturelist->MonsterSpawnID == MobID )
			{
				if(death_status == true)
				{
					creaturelist->isSpawned = false;
					creaturelist->isAggro = false;
					creaturelist->target = 0;
					creaturelist->KilledTime = timeGetTime();
					creaturelist->IsDead = death_status;
					creaturelist->CurLP = creaturelist->MaxLP;
					creaturelist->CurEP = creaturelist->MaxEP;
					//Delete from all Monster lists
					app->RemoveMonsterFromAllMyMonsterLists(creaturelist->MonsterSpawnID);
				}else{
					creaturelist->IsDead = death_status;
				}
				return true;
			}
		}
	}
	return true;
}

void		MobActivity::SpawnNpcAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	sVECTOR3 curpos = pSession->cPlayersMain->GetPlayerPosition();
	CSpawnTable* pNPCSpawnTbl;
	try{
		pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable(1);
	}
	catch (exception e)
	{
		printf("Invaild pSession \n\r Error:%s", e.what());
		return;
	}
	for (CTable::TABLEIT itNPCSpawn = pNPCSpawnTbl->Begin(); itNPCSpawn != pNPCSpawnTbl->End(); ++itNPCSpawn)
	{
		sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*)itNPCSpawn->second;
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(pNPCSpwnTblData->mob_Tblidx);

		if (pNPCTblData)
		{
			CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
			sGU_OBJECT_CREATE * sPacket = (sGU_OBJECT_CREATE *)packet.GetPacketData();
			sPacket->wOpCode = GU_OBJECT_CREATE;
			sPacket->sObjectInfo.objType = OBJTYPE_NPC;
			sPacket->handle = pNPCTblData->tblidx;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.x = pNPCSpwnTblData->vSpawn_Loc.x;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.y = pNPCSpwnTblData->vSpawn_Loc.y;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.z = pNPCSpwnTblData->vSpawn_Loc.z;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.x = pNPCSpwnTblData->vSpawn_Dir.x;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.y = pNPCSpwnTblData->vSpawn_Dir.y;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.z = pNPCSpwnTblData->vSpawn_Dir.z;
			sPacket->sObjectInfo.npcState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
			sPacket->sObjectInfo.npcBrief.wCurEP = pNPCTblData->wBasic_EP;
			sPacket->sObjectInfo.npcBrief.wCurLP = pNPCTblData->wBasic_LP;
			sPacket->sObjectInfo.npcBrief.wMaxEP = pNPCTblData->wBasic_EP;
			sPacket->sObjectInfo.npcBrief.wMaxLP = pNPCTblData->wBasic_LP;
			sPacket->sObjectInfo.npcBrief.tblidx = pNPCSpwnTblData->mob_Tblidx;

			pSession->InsertIntoMyNpcList(pNPCSpwnTblData->mob_Tblidx, pNPCSpwnTblData->vSpawn_Loc);

			packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
			g_pApp->Send(pSession->GetHandle(), &packet);

		}
	}
}
//void		MobActivity::SpawnMonsterAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
//{
//	CGameServer * app = (CGameServer*) NtlSfxGetApp();
//	sVECTOR3 curpos = pSession->cPlayersMain->GetPlayerPosition();
//	CreatureData * creaturelist;
//
//	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
//	{
//		creaturelist = (*it);
//		if(creaturelist->IsDead == false)
//		{
//			if(app->mob->CreatureRangeCheck(curpos, creaturelist->Spawn_Loc) == true)
//			{
//				CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
//				sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();
//
//				res->wOpCode = GU_OBJECT_CREATE;
//				res->sObjectInfo.objType = OBJTYPE_MOB;
//				res->handle = creaturelist->MonsterSpawnID;
//				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = creaturelist->Spawn_Loc.x;
//				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = creaturelist->Spawn_Loc.y;
//				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = creaturelist->Spawn_Loc.z;
//				res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = creaturelist->Spawn_Dir.x + rand() % 360;
//				res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = creaturelist->Spawn_Dir.y;
//				res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = creaturelist->Spawn_Dir.z;
//				res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
//				res->sObjectInfo.mobState.sCharStateBase.bFightMode = creaturelist->FightMode;
//				res->sObjectInfo.mobBrief.tblidx = creaturelist->MonsterID;
//				res->sObjectInfo.mobBrief.wCurEP = creaturelist->CurEP;
//				res->sObjectInfo.mobBrief.wMaxEP = creaturelist->MaxEP;
//				res->sObjectInfo.mobBrief.wCurLP = creaturelist->CurLP;
//				res->sObjectInfo.mobBrief.wMaxLP = creaturelist->MaxLP;
//				res->sObjectInfo.mobBrief.fLastRunningSpeed =  creaturelist->Run_Speed;
//				res->sObjectInfo.mobBrief.fLastWalkingSpeed = creaturelist->Walk_Speed;
//				creaturelist->isAggro = false;
//				creaturelist->isSpawned = true;
//				pSession->InsertIntoMyMonsterList(creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID);
//
//				packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
//				g_pApp->Send( pSession->GetHandle(), &packet );
//			
//			}
//		}
//	}
//}

bool		MobActivity::RunSpawnCheck(CNtlPacket * pPacket, sVECTOR3 curPos, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();

#pragma region npccheck

	CSpawnTable* pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable(pSession->cPlayersMain->GetWorldID());
	for (CTable::TABLEIT itNPCSpawn = pNPCSpawnTbl->Begin(); itNPCSpawn != pNPCSpawnTbl->End(); ++itNPCSpawn)
	{
		sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*)itNPCSpawn->second;
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(pNPCSpwnTblData->mob_Tblidx);

		if (pSession->IsNpcInsideList(pNPCSpwnTblData->mob_Tblidx) == true)
		{
			if (pNPCTblData && app->mob->CreatureRangeCheck(curPos, pNPCSpwnTblData->vSpawn_Loc) == true){
				CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
				sGU_OBJECT_CREATE * sPacket = (sGU_OBJECT_CREATE *)packet.GetPacketData();

				sPacket->wOpCode = GU_OBJECT_CREATE;
				sPacket->sObjectInfo.objType = OBJTYPE_NPC;
				sPacket->handle = pNPCTblData->tblidx;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.x = pNPCSpwnTblData->vSpawn_Loc.x;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.y = pNPCSpwnTblData->vSpawn_Loc.y;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.z = pNPCSpwnTblData->vSpawn_Loc.z;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.x = pNPCSpwnTblData->vSpawn_Dir.x;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.y = pNPCSpwnTblData->vSpawn_Dir.y;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.z = pNPCSpwnTblData->vSpawn_Dir.z;
				sPacket->sObjectInfo.npcState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
				sPacket->sObjectInfo.npcBrief.wCurEP = pNPCTblData->wBasic_EP;
				sPacket->sObjectInfo.npcBrief.wCurLP = pNPCTblData->wBasic_LP;
				sPacket->sObjectInfo.npcBrief.wMaxEP = pNPCTblData->wBasic_EP;
				sPacket->sObjectInfo.npcBrief.wMaxLP = pNPCTblData->wBasic_LP;
				sPacket->sObjectInfo.npcBrief.tblidx = pNPCSpwnTblData->mob_Tblidx;

				pSession->InsertIntoMyNpcList(pNPCSpwnTblData->mob_Tblidx, pNPCSpwnTblData->vSpawn_Loc);

				packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
				g_pApp->Send(pSession->GetHandle(), &packet);

			}
		}
	}
	return true;

#pragma endregion Npccheckend
}