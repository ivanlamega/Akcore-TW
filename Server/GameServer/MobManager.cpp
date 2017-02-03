//////////////////////////////////////////////////////////////////////
//
// Mob Manager Functions
//
// This will handle all things related to the MobList map
// and a thread for the mobs as well
// NPCs are included in this section as well.
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "GameServer.h"
#include <map>

CMobManager* g_pMobManager = NULL;
sVECTOR3 range{ 50.0f, 50.0f, 50.0f };

CMobManager::CMobManager()
{
	m_bRun = true;
}

CMobManager::~CMobManager()
{
	Release();
}

void CMobManager::Init()
{
	CreateMobThread();
	CreateNpcList();
	CreateMonsterList();
	
}

void CMobManager::Release()
{

}


void CMobManager::Run()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	while (m_bRun)
	{
		Sleep(100);
	}
}

void CMobManager::CreateMobThread()
{

	pThread = CNtlThreadFactory::CreateThread(this, "CMMThread");
	pThread->Start();

}

RwUInt32		CMobManager::CreateUniqueId(void)
{
	if (m_uiMobId++)
	{
		if (m_uiMobId == 0xffffffff)
			m_uiMobId = 0;
	}

	return m_uiMobId;
}


void CMobManager::CreateNpcList()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CSpawnTable* pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable(1);
	for (CTable::TABLEIT itNPCSpawn = pNPCSpawnTbl->Begin(); itNPCSpawn != pNPCSpawnTbl->End(); ++itNPCSpawn)
	{
		sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*)itNPCSpawn->second;
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(pNPCSpwnTblData->mob_Tblidx);

		if (pNPCTblData)
		{
			CMonster::MonsterData * cr = new CMonster::MonsterData;

			cr->Level = pNPCTblData->byLevel;
			cr->CurEP = pNPCTblData->wBasic_EP;
			cr->CurLP = pNPCTblData->wBasic_LP;
			cr->FightMode = false;
			cr->IsDead = false;
			cr->isSpawned = false;
			cr->MonsterID = pNPCTblData->tblidx;
			cr->MonsterSpawnID = pNPCSpwnTblData->tblidx;
			cr->Spawn_Loc = pNPCSpwnTblData->vSpawn_Loc;
			cr->Spawn_Dir = pNPCSpwnTblData->vSpawn_Dir;
			cr->MaxEP = pNPCTblData->wBasic_EP;
			cr->MaxLP = pNPCTblData->wBasic_LP;
			cr->curPos = cr->Spawn_Loc;
			cr->UniqueID = CreateUniqueId();
			m_map_Npc.insert(std::make_pair(cr->UniqueID, cr));
		}
	}
	printf("%u NPCs loaded to list\n", m_map_Npc._Mysize);
}

void CMobManager::CreateMonsterList()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	//CWorldTable* pWorldTable = app->g_pTableContainer->GetWorldTable();
	CSpawnTable* pMOBSpawnTbl = app->g_pTableContainer->GetMobSpawnTable(1);

	for (CTable::TABLEIT itMOBSpawn = pMOBSpawnTbl->Begin(); itMOBSpawn != pMOBSpawnTbl->End(); ++itMOBSpawn)
	{
		sSPAWN_TBLDAT* pMOBSpwnTblData = (sSPAWN_TBLDAT*)itMOBSpawn->second;
		sMOB_TBLDAT* pMOBTblData = (sMOB_TBLDAT*)app->g_pTableContainer->GetMobTable()->FindData(pMOBSpwnTblData->mob_Tblidx);

		if (pMOBTblData)
		{
			CMonster::MonsterData * cr = new CMonster::MonsterData;

			cr->Level = pMOBTblData->byLevel;
			cr->CurEP = pMOBTblData->wBasic_EP;
			cr->CurLP = pMOBTblData->wBasic_LP;
			cr->FightMode = false;
			cr->IsDead = false;
			cr->isSpawned = false;
			cr->MonsterID = pMOBTblData->tblidx;
			cr->MonsterSpawnID = pMOBSpwnTblData->tblidx;
			cr->Spawn_Loc = pMOBSpwnTblData->vSpawn_Loc;
			cr->Spawn_Dir = pMOBSpwnTblData->vSpawn_Dir;
			cr->MaxEP = pMOBTblData->wBasic_EP;
			cr->MaxLP = pMOBTblData->wBasic_LP;
			cr->Spawn_Quantity = pMOBSpwnTblData->bySpawn_Quantity;
			cr->Move_DelayTime = pMOBSpwnTblData->byMove_DelayTime;
			cr->Run_Speed = pMOBTblData->fRun_Speed;
			cr->Run_Speed_origin = pMOBTblData->fRun_Speed_Origin;
			cr->Walk_Speed = pMOBTblData->fWalk_Speed;
			cr->Walk_Speed_origin = pMOBTblData->fWalk_Speed_Origin;
			cr->Spawn_Cool_Time = pMOBSpwnTblData->wSpawn_Cool_Time * 1000;
			cr->target = 0;
			cr->curPos = cr->Spawn_Loc;
			cr->Basic_aggro_point = pMOBTblData->byScan_Range;
			cr->Attack_range = pMOBTblData->fAttack_Range;
			cr->MaxchainAttackCount = pMOBTblData->byAttack_Animation_Quantity;
			cr->chainAttackCount = 0;
			cr->UniqueID = CreateUniqueId();
			m_map_Monster.insert(std::make_pair(cr->UniqueID, cr));
			//printf("level %d \n", cr->Level);
		}
	}
	printf("%u MOBs loaded to list\n", m_map_Monster._Mysize);
	
}

bool CMobManager::RunSpawnCheck(CNtlPacket * pPacket, sVECTOR3 curPos, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	std::vector<RwUInt32>::iterator handleSearch;
	RwUInt32 avHandle = NULL;
	if (pSession->cPlayersMain)
		avHandle = pSession->GetavatarHandle();
	//else
		//return false;
#pragma region monstercheck
	CMonster::MonsterData* creaturelist;
	
	for (IterType it = m_map_Monster.begin(); it != m_map_Monster.end(); it++)
	{
		creaturelist = it->second;
		handleSearch = std::find(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), avHandle);

		if (CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == true)
		{
			if ((creaturelist->IsDead == true) || (handleSearch != creaturelist->spawnedForHandle.end()))
				{
					//Your handle was found so dont spawn it.
					//Or Creature is dead, or There is more 
				}
				else
				{
					if (std::count(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), avHandle) <= MAX_MOB_ANOUNT)
					{
						CNtlPacket packet(sizeof(SpawnMOB));
						SpawnMOB* res = (SpawnMOB*)packet.GetPacketData();
						creaturelist->isSpawned = true;
						res->wOpCode = GU_OBJECT_CREATE;
						res->Type = OBJTYPE_MOB;
						res->Handle = creaturelist->UniqueID;
						res->Loc[0] = creaturelist->curPos.x;
						res->Loc[1] = creaturelist->curPos.y;
						res->Loc[2] = creaturelist->curPos.z;
						res->Dir[0] = creaturelist->Spawn_Dir.x;
						res->Dir[1] = creaturelist->Spawn_Dir.y;
						res->Dir[2] = creaturelist->Spawn_Dir.z;
						res->StateID = CHARSTATE_SPAWNING;
						res->IsFighting = creaturelist->FightMode;
						res->Tblidx = creaturelist->MonsterID;
						res->curEP = creaturelist->CurEP;
						res->maxEP = creaturelist->MaxEP;
						res->curLP = creaturelist->CurLP;
						res->maxLP = creaturelist->MaxLP;
						res->Size = 10;
						res->Unknown[0] = 1;//34
						res->Unknown3[0] = 0;//12
						res->Unknown4[0] = 0;
						res->Level = creaturelist->Level;
						creaturelist->spawnedForHandle.push_back(avHandle);
						packet.SetPacketLen(sizeof(SpawnMOB));
						g_pApp->Send(pSession->GetHandle(), &packet);
						//printf("Monster %u Spawned \n", creaturelist->MonsterID);
						//printf("level %d \n", res->Level);
					}
					else
						printf("Maximum amount of mobs reached for your handle\n ");
				}
		}
		
		else if ((creaturelist->isSpawned == true) && (CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == false) && (handleSearch != creaturelist->spawnedForHandle.end()))
		{	
			CNtlPacket packet(sizeof(sGU_OBJECT_DESTROY));
			sGU_OBJECT_DESTROY * res = (sGU_OBJECT_DESTROY*)packet.GetPacketData();
			res->wOpCode = GU_OBJECT_DESTROY;
			res->handle = creaturelist->UniqueID;
			creaturelist->target = 0;
			creaturelist->spawnedForHandle.erase(handleSearch);
		/*	if (handleSearch == creaturelist->spawnedForHandle.end())
			{
				creaturelist->isSpawned = false;
				creaturelist->target = 0;
			}*/
			packet.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
			g_pApp->Send(pSession->GetHandle(), &packet);
		}
		else if (handleSearch == creaturelist->spawnedForHandle.end())
		{
			creaturelist->isSpawned = false;
			creaturelist->target = 0;
		}

	}


#pragma endregion Monstercheckend
#pragma region npccheck

	for (IterType it = m_map_Npc.begin(); it != m_map_Npc.end(); it++)
	{
		creaturelist = it->second;
		handleSearch = std::find(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), avHandle);

		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(creaturelist->MonsterID);
		if (pNPCTblData && (CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == true))
		{
			if (handleSearch != creaturelist->spawnedForHandle.end())
			{
				//Your handle was found so dont spawn it.
			}
			else
			{
				if (std::count(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), avHandle) <= MAX_MOB_ANOUNT)
				{

					CNtlPacket packet(sizeof(SpawnNPC));
					SpawnNPC * sPacket = (SpawnNPC *)packet.GetPacketData();
					sPacket->wOpCode = GU_OBJECT_CREATE;
					sPacket->Type = OBJTYPE_NPC;
					sPacket->Handle = creaturelist->UniqueID;
					sPacket->Tblidx = creaturelist->MonsterID;
					sPacket->Loc[0] = creaturelist->Spawn_Loc.x;
					sPacket->Loc[1] = creaturelist->Spawn_Loc.y;
					sPacket->Loc[2] = creaturelist->Spawn_Loc.z;
					sPacket->Dir[0] = creaturelist->Spawn_Dir.x;
					sPacket->Dir[1] = creaturelist->Spawn_Dir.y;
					sPacket->Dir[2] = creaturelist->Spawn_Dir.z;
					sPacket->Size = 10;
					sPacket->StateID = CHARSTATE_SPAWNING;
					sPacket->Unknown3[0] = 0;
					sPacket->Unknown4[0] = 0;
					creaturelist->isSpawned = true;
					creaturelist->spawnedForHandle.push_back(avHandle);

					packet.SetPacketLen(sizeof(SpawnNPC));
					g_pApp->Send(pSession->GetHandle(), &packet);
					//printf("NPC %u Spawned \n", creaturelist->MonsterID);

				}
			}
		}
		else if ((creaturelist->isSpawned == true) && (CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == false) && (handleSearch != creaturelist->spawnedForHandle.end()))
		{
			CNtlPacket packet(sizeof(sGU_OBJECT_DESTROY));
			sGU_OBJECT_DESTROY * res = (sGU_OBJECT_DESTROY*)packet.GetPacketData();
			res->wOpCode = GU_OBJECT_DESTROY;
			res->handle = creaturelist->UniqueID;
			creaturelist->target = 0;
			creaturelist->spawnedForHandle.erase(handleSearch);
			/*	if (handleSearch == creaturelist->spawnedForHandle.end())
			{
			creaturelist->isSpawned = false;
			creaturelist->target = 0;
			}*/
			packet.SetPacketLen(sizeof(sGU_OBJECT_DESTROY));
			g_pApp->Send(pSession->GetHandle(), &packet);
		}
		else if (handleSearch == creaturelist->spawnedForHandle.end())
		{
			creaturelist->isSpawned = false;
			creaturelist->target = 0;
		}
	}

	return true;
#pragma endregion Npccheckend
}

float CMobManager::Distance(const sVECTOR3 mycurPos, const CNtlVector othercurPos)
{
	float first = (mycurPos.x - othercurPos.x) * (mycurPos.x - othercurPos.x);
	float second = (mycurPos.y - othercurPos.y) * (mycurPos.y - othercurPos.y);
	float third = (mycurPos.z - othercurPos.z) * (mycurPos.z - othercurPos.z);
	float distance = sqrt(first + second + third);
	return distance;
}

bool CMobManager::CreatureRangeCheck(sVECTOR3 mycurPos, CNtlVector othercurPos)
{
	float distance = Distance(mycurPos, othercurPos);
	if (distance < DISTANCE_TO_SPAWN)
	{
		return true;
	}
	return false;
}

void CMobManager::SpawnNpcAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	
	sVECTOR3 curpos = pSession->cPlayersMain->GetPlayerPosition();
	RwUInt32 avHandle = pSession->cPlayersMain->GetAvatarHandle();
	CMonster::MonsterData* creaturelist;
	for (IterType it = m_map_Npc.begin(); it != m_map_Npc.end(); it++)
	{
		creaturelist = (it->second);
		
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(creaturelist->MonsterID);
		if (pNPCTblData)
		{
			if ((CreatureRangeCheck(curpos, creaturelist->Spawn_Loc) == true) && (creaturelist->isSpawned == false))
			{	

					CNtlPacket packet(sizeof(SpawnNPC));
					SpawnNPC * sPacket = (SpawnNPC *)packet.GetPacketData();
					sPacket->wOpCode = GU_OBJECT_CREATE;
					sPacket->Type = OBJTYPE_NPC;
					sPacket->Handle = creaturelist->UniqueID;
					sPacket->Tblidx = creaturelist->MonsterID;
					sPacket->Loc[0] = creaturelist->Spawn_Loc.x;
					sPacket->Loc[1] = creaturelist->Spawn_Loc.y;
					sPacket->Loc[2] = creaturelist->Spawn_Loc.z;
					sPacket->Dir[0] = creaturelist->Spawn_Dir.x;
					sPacket->Dir[1] = creaturelist->Spawn_Dir.y;
					sPacket->Dir[2] = creaturelist->Spawn_Dir.z;
					sPacket->Size = 10;
					sPacket->StateID = CHARSTATE_SPAWNING;
					sPacket->Unknown3[0] = 0;
					sPacket->Unknown4[0] = 0;
					creaturelist->isSpawned = true;
					creaturelist->spawnedForHandle.push_back(avHandle);

					packet.SetPacketLen(sizeof(SpawnNPC));
					g_pApp->Send(pSession->GetHandle(), &packet);
			}
		}
	}
}
void CMobManager::SpawnMonsterAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{
	
	CGameServer * app = (CGameServer*)NtlSfxGetApp();

	sVECTOR3 curpos = pSession->cPlayersMain->GetPlayerPosition();
	RwUInt32 avHandle = pSession->cPlayersMain->GetAvatarHandle();

	CMonster::MonsterData* creaturelist;
	std::vector<RwUInt32>::iterator handleSearch;

	for (IterType it = m_map_Monster.begin(); it != m_map_Monster.end(); it++)
	{
		creaturelist = (it->second);
		sMOB_TBLDAT* pMobTblData = (sMOB_TBLDAT*)app->g_pTableContainer->GetMobTable()->FindData(creaturelist->MonsterID);
		if (pMobTblData)
		{
			if ((CreatureRangeCheck(curpos, creaturelist->Spawn_Loc) == true) && (creaturelist->isSpawned == false))
			{

					CNtlPacket packet(sizeof(SpawnMOB));
					SpawnMOB* res = (SpawnMOB*)packet.GetPacketData();
					creaturelist->isSpawned = true;
					res->wOpCode = GU_OBJECT_CREATE;
					res->Type = OBJTYPE_MOB;
					res->Handle = creaturelist->UniqueID;
					res->Loc[0] = creaturelist->curPos.x;
					res->Loc[1] = creaturelist->curPos.y;
					res->Loc[2] = creaturelist->curPos.z;
					res->Dir[0] = creaturelist->Spawn_Dir.x;
					res->Dir[1] = creaturelist->Spawn_Dir.y;
					res->Dir[2] = creaturelist->Spawn_Dir.z;
					res->StateID = CHARSTATE_SPAWNING;
					res->IsFighting = creaturelist->FightMode;
					res->Tblidx = creaturelist->MonsterID;
					res->curEP = creaturelist->CurEP;
					res->maxEP = creaturelist->MaxEP;
					res->curLP = creaturelist->CurLP;
					res->maxLP = creaturelist->MaxLP;
					res->Size = 10;
					res->Unknown3[0] = 0;
					res->Unknown4[0] = 0;
					res->Level = creaturelist->Level;
					creaturelist->spawnedForHandle.push_back(avHandle);
					packet.SetPacketLen(sizeof(SpawnMOB));
					g_pApp->Send(pSession->GetHandle(), &packet);
					//printf("level %d \n", res->Level);
			
			}
		}
	}	
}

//Searches through mobMap to see if handle exists.
bool CMobManager::FindCreature(RwUInt32 handle)
{
	IterType handleSearch = m_map_Monster.find(handle);
	if (handleSearch != m_map_Monster.end())
		return true;
	else
		return false;
}
//Search the NPC by Handle
TBLIDX CMobManager::FindNpc(RwUInt32 handle)
{
	IterType handleSearch = m_map_Npc.find(handle);
	if (handleSearch != m_map_Npc.end())
		return handleSearch->second->MonsterID;
	else
		return 0;
}
//Simple function to return MonsterData
CMonster::MonsterData*	CMobManager::GetMobByHandle(RwUInt32 Target)
{
	
	IterType handleSearch = m_map_Monster.find(Target);
	if (handleSearch != m_map_Monster.end())
	{
		CMonster::MonsterData * creaturelist = handleSearch->second;
		return creaturelist;
	}
	return NULL;
}

//Function to update the status of mob dying
bool CMobManager::UpdateDeathStatus(RwUInt32 MobID, bool death_status)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	IterType handleSearch = m_map_Monster.find(MobID);
	if (handleSearch != m_map_Monster.end())
	{
		CMonster::MonsterData * creaturelist = handleSearch->second;
		if(death_status == true)
		{
			creaturelist->isSpawned = false;
			creaturelist->isAggro = false;
			creaturelist->target = 0;
			creaturelist->KilledTime = timeGetTime();
			creaturelist->IsDead = death_status;
			creaturelist->CurLP = 0;
			creaturelist->CurEP = 0;
		}
		else
		{
			creaturelist->IsDead = death_status;
		}
		return true;
	}
	return true;
}

//Spawns a mob by its spawntblidx
void CMobManager::CreateMobByTblidx(RwUInt32 spawnTblidx, RwUInt32 avatarHandle)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(avatarHandle);
	sVECTOR3 curpos = plr->GetPlayerPosition();
	CSpawnTable* pMOBSpawnTbl = app->g_pTableContainer->GetMobSpawnTable(plr->GetWorldTblx());

		sSPAWN_TBLDAT* pMOBSpwnTblData = (sSPAWN_TBLDAT*)pMOBSpawnTbl->FindData(spawnTblidx);
		sMOB_TBLDAT* pMOBTblData = (sMOB_TBLDAT*)app->g_pTableContainer->GetMobTable()->FindData(pMOBSpwnTblData->mob_Tblidx);

		if (pMOBTblData)
		{
			CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
			sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();
			CMonster::MonsterData * cr = new CMonster::MonsterData;

			cr->Level = 0;// pMOBTblData->byLevel;
			cr->CurEP = 0;
			cr->CurLP = 0;
			cr->FightMode = false;
			cr->IsDead = false;
			cr->isSpawned = true;
			cr->MonsterID = 1581102;
			cr->MonsterSpawnID = 1581102;
			cr->Spawn_Loc = 0;
			cr->Spawn_Dir = 0;
			cr->MaxEP = 0;
			cr->MaxLP = 0;
			cr->Spawn_Quantity = 0;
			cr->Move_DelayTime = 0;
			cr->Run_Speed = 0;
			cr->Run_Speed_origin = 0;
			cr->Walk_Speed =0;
			cr->Walk_Speed_origin = 0;
			cr->Spawn_Cool_Time = 0;
			cr->target = 0;
			cr->curPos = 0;
			cr->Basic_aggro_point =0;
			cr->Attack_range = 0;
			cr->MaxchainAttackCount = 0;
			cr->chainAttackCount = 0;
			cr->UniqueID = CreateUniqueId();
			m_map_Monster.insert(std::make_pair(cr->UniqueID, cr));
			
			res->wOpCode = GU_OBJECT_CREATE;
			res->sObjectInfo.objType = OBJTYPE_MOB;
			res->handle = cr->UniqueID;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = cr->Spawn_Loc.x;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = cr->Spawn_Loc.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = cr->Spawn_Loc.z;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = cr->Spawn_Dir.x + rand() % 360;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = cr->Spawn_Dir.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = cr->Spawn_Dir.z;
			res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
			res->sObjectInfo.mobState.sCharStateBase.bFightMode = cr->FightMode;
			res->sObjectInfo.mobBrief.tblidx = cr->MonsterID;
			res->sObjectInfo.mobBrief.wCurEP = cr->CurEP;
			res->sObjectInfo.mobBrief.wMaxEP = cr->MaxEP;
			res->sObjectInfo.mobBrief.wCurLP = cr->CurLP;
			res->sObjectInfo.mobBrief.wMaxLP = cr->MaxLP;
			res->sObjectInfo.mobBrief.fLastRunningSpeed = cr->Run_Speed;
			res->sObjectInfo.mobBrief.fLastWalkingSpeed = cr->Walk_Speed;
			
			packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
			g_pApp->Send(plr->GetSession(), &packet);
		}
		plr = NULL;
		delete plr;
}


//Spawns an NPC by its SpawnTblidx
void CMobManager::CreateNPCByTblidx(RwUInt32 spawnTblidx, RwUInt32 playerHandle)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(playerHandle);
	sVECTOR3 curpos = plr->GetPlayerPosition();
	CSpawnTable* pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable(plr->GetWorldTblx());

	sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*)pNPCSpawnTbl->FindData(spawnTblidx);
	sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(pNPCSpwnTblData->mob_Tblidx);
	if (pNPCTblData)
	{
		CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
		sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();
		CMonster::MonsterData * cr = new CMonster::MonsterData;

		cr->Level = 0;
		cr->CurEP = 0;
		cr->CurLP = 0;
		cr->FightMode = false;
		cr->IsDead = false;
		cr->isSpawned = true;
		cr->MonsterID = 1581102;
		cr->MonsterSpawnID = 1581102;
		cr->Spawn_Loc = 0;
		cr->Spawn_Dir = 0;
		cr->MaxEP = 0;
		cr->MaxLP = 0;
		cr->curPos = 0;
		cr->UniqueID = CreateUniqueId();
		m_map_Npc.insert(std::make_pair(cr->UniqueID, cr)); 
		
		res->wOpCode = GU_OBJECT_CREATE;
		res->sObjectInfo.objType = OBJTYPE_NPC;
		res->handle = cr->UniqueID;
		res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = cr->Spawn_Loc.x;
		res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = cr->Spawn_Loc.y;
		res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = cr->Spawn_Loc.z;
		res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = cr->Spawn_Dir.x + rand() % 360;
		res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = cr->Spawn_Dir.y;
		res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = cr->Spawn_Dir.z;
		res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
		res->sObjectInfo.mobState.sCharStateBase.bFightMode = cr->FightMode;
		res->sObjectInfo.mobBrief.tblidx = cr->MonsterID;
		res->sObjectInfo.mobBrief.wCurEP = cr->CurEP;
		res->sObjectInfo.mobBrief.wMaxEP = cr->MaxEP;
		res->sObjectInfo.mobBrief.wCurLP = cr->CurLP;
		res->sObjectInfo.mobBrief.wMaxLP = cr->MaxLP;
		res->sObjectInfo.mobBrief.fLastRunningSpeed = cr->Run_Speed;
		res->sObjectInfo.mobBrief.fLastWalkingSpeed = cr->Walk_Speed;

		packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
		g_pApp->Send(plr->GetSession(), &packet);
	}
	plr = NULL;
	delete plr;

}


//void	CMonster::MonsterData::Attack(PlayersMain *plr, CGameServer *app)
//{
//	this->FightMode = true;
//	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
//	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();
//
//	res->wOpCode = GU_CHAR_ACTION_ATTACK;
//	res->hSubject = this->MonsterSpawnID;
//	res->hTarget = plr->GetAvatarHandle();
//	res->dwLpEpEventId = 255;
//	res->byBlockedAction = 255;
//	float formula = 0;
//	if (this->Level <= 5)
//		formula = rand() % 25 + 5;
//	else
//		formula = (this->Str * this->Level) * .08;
//	res->wAttackResultValue = formula;
//	res->fReflectedDamage = 0;
//	res->vShift = plr->GetPlayerPosition();
//	this->chainAttackCount += 1;
//	res->byAttackSequence = this->chainAttackCount;
//	res->bChainAttack = true;
//	res->byAttackResult = BATTLE_ATTACK_RESULT_HIT;
//	packet.SetPacketLen( sizeof(sGU_CHAR_ACTION_ATTACK) );
//	app->UserBroadcast(&packet);
//	plr->SetPlayerFight(true);
//	plr->SetPlayerDamage(res->wAttackResultValue);
//}
