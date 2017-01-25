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
	//CreateMobThread();
	//CreateNpcList();
	//CreateMonsterList();
	
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
		}
	}
}

bool CMobManager::RunSpawnCheck(CNtlPacket * pPacket, sVECTOR3 curPos, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
#pragma region monstercheck
	CMonster::MonsterData* creaturelist;
	
	for (IterType it = m_map_Monster.begin(); it != m_map_Monster.end(); it++)
	{
		creaturelist = it->second;
		std::vector<RwUInt32>::iterator handleSearch = std::find(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), pSession->GetavatarHandle());

		if (CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == true)
		{
			if (creaturelist->IsDead == false && creaturelist->isSpawned == false)
			{
				if (handleSearch != creaturelist->spawnedForHandle.end())
				{
					//Your handle was found so dont spawn it.
				}
				else
				{
					CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
					sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();
					creaturelist->isSpawned = true;
					res->wOpCode = GU_OBJECT_CREATE;
					res->sObjectInfo.objType = OBJTYPE_MOB;
					res->handle = creaturelist->UniqueID;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = 0;// creaturelist->curPos.x;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = 0;// creaturelist->curPos.y;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = 0;// creaturelist->curPos.z;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = 0;// creaturelist->Spawn_Dir.x;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = 0;// creaturelist->Spawn_Dir.y;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = 0;//creaturelist->Spawn_Dir.z;
					res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
					res->sObjectInfo.mobState.sCharStateBase.bFightMode = creaturelist->FightMode;
					res->sObjectInfo.mobBrief.tblidx = 1581102;
					res->sObjectInfo.mobBrief.wCurEP = 0;
					res->sObjectInfo.mobBrief.wMaxEP = 0;
					res->sObjectInfo.mobBrief.wCurLP = 0;
					res->sObjectInfo.mobBrief.wMaxLP = 0;
					res->sObjectInfo.mobBrief.fLastRunningSpeed = 0;
					res->sObjectInfo.mobBrief.fLastWalkingSpeed = 0;
					creaturelist->spawnedForHandle.push_back(pSession->GetavatarHandle());
					packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
					g_pApp->Send(pSession->GetHandle(), &packet);
				}
			}
		}
		
		else if ((creaturelist->isSpawned == true && CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == false && handleSearch != creaturelist->spawnedForHandle.end()))
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
		std::vector<RwUInt32>::iterator handleSearch = std::find(creaturelist->spawnedForHandle.begin(), creaturelist->spawnedForHandle.end(), pSession->GetavatarHandle());
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(creaturelist->MonsterID);

		if (pNPCTblData && app->mob->CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == true)
		{
			if (handleSearch != creaturelist->spawnedForHandle.end())
			{
				//Your handle was found so dont spawn it.
			}
			else
			{
				CNtlPacket packet(sizeof(SpawnNPC));
				SpawnNPC * sPacket = (SpawnNPC *)packet.GetPacketData();

				sPacket->wOpCode = GU_OBJECT_CREATE;
				sPacket->Type = OBJTYPE_NPC;
				sPacket->Handle = creaturelist->UniqueID;
				sPacket->Tblidx = creaturelist->MonsterID;
				sPacket->Loc[0] = creaturelist->Spawn_Loc.x;// curpos.x;
				sPacket->Loc[1] = creaturelist->Spawn_Loc.y;; //curpos.y;
				sPacket->Loc[2] = -creaturelist->Spawn_Loc.z;;// curpos.z;
				sPacket->Dir[0] = creaturelist->Spawn_Dir.x;// curpos.x;
				sPacket->Dir[1] = creaturelist->Spawn_Dir.y; //curpos.y;
				sPacket->Dir[2] = creaturelist->Spawn_Dir.z;// curpos.z;
				sPacket->Size = 10;
				sPacket->Unknown3[0] = 0;
				sPacket->Unknown4[0] = 0;
				creaturelist->isSpawned = true;
				creaturelist->spawnedForHandle.push_back(pSession->GetavatarHandle());
				packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
				g_pApp->Send(pSession->GetHandle(), &packet);

			}
		}
	}
	return true;
return false;
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
	CMonster::MonsterData* creaturelist;
	CSpawnTable* pNPCSpawnTbl;
	for (IterType it = m_map_Npc.begin(); it != m_map_Npc.end(); it++)
	{
		creaturelist = (it->second);
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData(creaturelist->MonsterID);
		if (pNPCTblData)
		{
			if ((CreatureRangeCheck(curpos, creaturelist->Spawn_Loc) == true && creaturelist->isSpawned == false))
			{

				CNtlPacket packet(sizeof(SpawnNPC));
				SpawnNPC * sPacket = (SpawnNPC *)packet.GetPacketData();
				sPacket->wOpCode = GU_OBJECT_CREATE;
				sPacket->Type = OBJTYPE_NPC;
				sPacket->Handle = creaturelist->UniqueID;
				sPacket->Loc[0] = creaturelist->Spawn_Loc.x;
				sPacket->Loc[1] = creaturelist->Spawn_Loc.y;
				sPacket->Loc[2] = creaturelist->Spawn_Loc.z;
				sPacket->Dir[0] = creaturelist->Spawn_Dir.x;
				sPacket->Dir[0] = creaturelist->Spawn_Dir.y;
				sPacket->Dir[0] = creaturelist->Spawn_Dir.z;
				sPacket->Size = 10.0;
				sPacket->Unknown3[0]=0;
				sPacket->Unknown4[0]=0;
				packet.SetPacketLen(sizeof(SpawnNPC));
				g_pApp->Send(pSession->GetHandle(), &packet);

			}
		}
	}

	/*
	//NPC 879

	CNtlPacket packet(sizeof(SpawnNPC));
	SpawnNPC * res = (SpawnNPC *)packet.GetPacketData();

	res->wOpCode = GU_OBJECT_CREATE;
	res->Type = OBJTYPE_NPC;
	res->Handle = 1000;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res->Tblidx = 1654103;
	res->Loc[0] = 2066.959961;// curpos.x;
	res->Loc[1] = 18.590000; //curpos.y;
	res->Loc[2] = 5787.549805;// curpos.z;
	res->Dir[0] = -0.328135;// curpos.x;
	res->Dir[1] = -0.0; //curpos.y;
	res->Dir[2] = -0.944631;// curpos.z;
	res->Size = 10;
	//res9->Unknown[0] = 10.0;
	res->Unknown3[0] = 0;
	res->Unknown4[0] = 0;
	packet.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet);
	*/

	
}
void CMobManager::SpawnMonsterAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{

	//Mob 27325
	CNtlPacket packet(sizeof(SpawnMOB));
	SpawnMOB * res = (SpawnMOB *)packet.GetPacketData();

	res->wOpCode = GU_OBJECT_CREATE;
	res->Type = OBJTYPE_MOB;
	res->Handle = 10;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res->Tblidx = 11111101;
	res->Loc[0] = 4745.970215;// curpos.x;
	res->Loc[1] = -61.810001; //curpos.y;
	res->Loc[2] = 4070.149902;// curpos.z;
	res->Dir[0] = 0.000000;
	res->Dir[1] = -0.0;
	res->Dir[2] = 1.000000;
	res->Size = 10;
	res->curEP = 500;
	res->maxEP = 500;
	res->curLP = 50000;
	res->maxLP = 50000;
	res->Level = 70;
	res->StateID = 35;

	packet.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet);

	//Mob 27326
	CNtlPacket packet1(sizeof(SpawnMOB));
	SpawnMOB * res1 = (SpawnMOB *)packet1.GetPacketData();

	res1->wOpCode = GU_OBJECT_CREATE;
	res1->Type = OBJTYPE_MOB;
	res1->Handle = 11;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1->Tblidx = 11151101;
	res1->Loc[0] = 4717.310059;// curpos.x;
	res1->Loc[1] = -62.299999; //curpos.y;
	res1->Loc[2] = 4068.080078;// curpos.z;
	res1->Dir[0] = 0.000000;
	res1->Dir[1] = -0.0;
	res1->Dir[2] = 1.000000;
	res1->Size = 10;
	res1->curEP = 500;
	res1->maxEP = 500;
	res1->curLP = 60000000000000;
	res1->maxLP = 60000000000000;
	res1->Level = 70;
	res1->StateID = 35;

	packet1.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet1);

	//Mob 27327
	CNtlPacket packet2(sizeof(SpawnMOB));
	SpawnMOB * res2 = (SpawnMOB *)packet2.GetPacketData();

	res2->wOpCode = GU_OBJECT_CREATE;
	res2->Type = OBJTYPE_MOB;
	res2->Handle = 12;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res2->Tblidx = 11211101;
	res2->Loc[0] = 4680.009766;// curpos.x;
	res2->Loc[1] = -62.630001; //curpos.y;
	res2->Loc[2] = 4078.719971;// curpos.z;
	res2->Dir[0] = 0.000000;
	res2->Dir[1] = -0.0;
	res2->Dir[2] = 1.000000;
	res2->Size = 10;
	res2->curEP = 500;
	res2->maxEP = 500;
	res2->curLP = 600;
	res2->maxLP = 600;
	res2->Level = 2;
	res2->StateID = 35;

	packet2.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet2);

	//Mob 27328
	CNtlPacket packet3(sizeof(SpawnMOB));
	SpawnMOB * res3 = (SpawnMOB *)packet3.GetPacketData();

	res3->wOpCode = GU_OBJECT_CREATE;
	res3->Type = OBJTYPE_MOB;
	res3->Handle = 13;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res3->Tblidx = 11213101;
	res3->Loc[0] = 4767.720215;// curpos.x;
	res3->Loc[1] = -57.910000; //curpos.y;
	res3->Loc[2] = 4039.780029;// curpos.z;
	res3->Dir[0] = 0.000000;
	res3->Dir[1] = -0.0;
	res3->Dir[2] = 1.000000;
	res3->Size = 10;
	res3->curEP = 500;
	res3->maxEP = 500;
	res3->curLP = 600;
	res3->maxLP = 600;
	res3->Level = 2;
	res3->StateID = 35;

	packet3.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet3);

	//Mob 27329
	CNtlPacket packet4(sizeof(SpawnMOB));
	SpawnMOB * res4 = (SpawnMOB *)packet4.GetPacketData();

	res4->wOpCode = GU_OBJECT_CREATE;
	res4->Type = OBJTYPE_MOB;
	res4->Handle = 14;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res4->Tblidx = 12251101;
	res4->Loc[0] = 4796.229980;// curpos.x;
	res4->Loc[1] = -58.000000; //curpos.y;
	res4->Loc[2] = 4038.610107;// curpos.z;
	res4->Dir[0] = 0.000000;
	res4->Dir[1] = -0.0;
	res4->Dir[2] = 1.000000;
	res4->Size = 10;
	res4->curEP = 500;
	res4->maxEP = 500;
	res4->curLP = 600;
	res4->maxLP = 600;
	res4->Level = 2;
	res4->StateID = 35;

	packet4.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet4);

	//Mob 27330
	CNtlPacket packet5(sizeof(SpawnMOB));
	SpawnMOB * res5 = (SpawnMOB *)packet5.GetPacketData();

	res5->wOpCode = GU_OBJECT_CREATE;
	res5->Type = OBJTYPE_MOB;
	res5->Handle = 15;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res5->Tblidx = 12351201;
	res5->Loc[0] = 4827.600098;// curpos.x;
	res5->Loc[1] = -57.610001; //curpos.y;
	res5->Loc[2] = 4051.560059;// curpos.z;
	res5->Dir[0] = 0.000000;
	res5->Dir[1] = -0.0;
	res5->Dir[2] = 1.000000;
	res5->Size = 10;
	res5->curEP = 500;
	res5->maxEP = 500;
	res5->curLP = 600;
	res5->maxLP = 600;
	res5->Level = 2;
	res5->StateID = 35;

	packet5.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet5);

	//Mob 27331
	CNtlPacket packet6(sizeof(SpawnMOB));
	SpawnMOB * res6 = (SpawnMOB *)packet6.GetPacketData();

	res6->wOpCode = GU_OBJECT_CREATE;
	res6->Type = OBJTYPE_MOB;
	res6->Handle = 16;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res6->Tblidx = 1411101;
	res6->Loc[0] = 4829.810059;// curpos.x;
	res6->Loc[1] = -57.930000; //curpos.y;
	res6->Loc[2] = 4039.189941;// curpos.z;
	res6->Dir[0] = 0.000000;
	res6->Dir[1] = -0.0;
	res6->Dir[2] = 1.000000;
	res6->Size = 10;
	res6->curEP = 500;
	res6->maxEP = 500;
	res6->curLP = 600;
	res6->maxLP = 600;
	res6->Level = 2;
	res6->StateID = 35;

	packet6.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet6);

	//Mob 27332
	CNtlPacket packet7(sizeof(SpawnMOB));
	SpawnMOB * res7 = (SpawnMOB *)packet7.GetPacketData();

	res7->wOpCode = GU_OBJECT_CREATE;
	res7->Type = OBJTYPE_MOB;
	res7->Handle = 17;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res7->Tblidx = 1411102;
	res7->Loc[0] = 4790.100098;// curpos.x;
	res7->Loc[1] = -57.070000; //curpos.y;
	res7->Loc[2] = 4019.909912;// curpos.z;
	res7->Dir[0] = 0.000000;
	res7->Dir[1] = -0.0;
	res7->Dir[2] = 1.000000;
	res7->Size = 10;
	res7->curEP = 500;
	res7->maxEP = 500;
	res7->curLP = 600;
	res7->maxLP = 600;
	res7->Level = 2;
	res7->StateID = 35;

	packet7.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet7);

	//Mob 27333
	CNtlPacket packet8(sizeof(SpawnMOB));
	SpawnMOB * res8 = (SpawnMOB *)packet8.GetPacketData();

	res8->wOpCode = GU_OBJECT_CREATE;
	res8->Type = OBJTYPE_MOB;
	res8->Handle = 18;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res8->Tblidx = 1411102;
	res8->Loc[0] = 4833.790039;// curpos.x;
	res8->Loc[1] = -66.349998; //curpos.y;
	res8->Loc[2] = 4084.409912;// curpos.z;
	res8->Dir[0] = 0.000000;
	res8->Dir[1] = -0.0;
	res8->Dir[2] = 1.000000;
	res8->Size = 10;
	res8->curEP = 500;
	res8->maxEP = 500;
	res8->curLP = 600;
	res8->maxLP = 600;
	res8->Level = 2;
	res8->StateID = 35;

	packet8.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet8);

	//Mob 27334
	CNtlPacket packet9(sizeof(SpawnMOB));
	SpawnMOB * res9 = (SpawnMOB *)packet9.GetPacketData();

	res9->wOpCode = GU_OBJECT_CREATE;
	res9->Type = OBJTYPE_MOB;
	res9->Handle = 19;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res9->Tblidx = 1411101;
	res9->Loc[0] = 4846.910156;// curpos.x;
	res9->Loc[1] = -66.389999; //curpos.y;
	res9->Loc[2] = 4065.129883;// curpos.z;
	res9->Dir[0] = 0.000000;
	res9->Dir[1] = -0.0;
	res9->Dir[2] = 1.000000;
	res9->Size = 10;
	res9->curEP = 500;
	res9->maxEP = 500;
	res9->curLP = 600;
	res9->maxLP = 600;
	res9->Level = 2;
	res9->StateID = 35;

	packet9.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet9);

	//Mob 27335
	CNtlPacket packet10(sizeof(SpawnMOB));
	SpawnMOB * res10 = (SpawnMOB *)packet10.GetPacketData();

	res10->wOpCode = GU_OBJECT_CREATE;
	res10->Type = OBJTYPE_MOB;
	res10->Handle = 20;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res10->Tblidx = 1411103;
	res10->Loc[0] = 4746.560059;// curpos.x;
	res10->Loc[1] = -54.240002; //curpos.y;
	res10->Loc[2] = 3948.580078;// curpos.z;
	res10->Dir[0] = 0.000000;
	res10->Dir[1] = -0.0;
	res10->Dir[2] = 1.000000;
	res10->Size = 10;
	res10->curEP = 500;
	res10->maxEP = 500;
	res10->curLP = 600;
	res10->maxLP = 600;
	res10->Level = 2;
	res10->StateID = 35;

	packet10.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet10);

	//Mob 27336
	CNtlPacket packet11(sizeof(SpawnMOB));
	SpawnMOB * res11 = (SpawnMOB *)packet11.GetPacketData();

	res11->wOpCode = GU_OBJECT_CREATE;
	res11->Type = OBJTYPE_MOB;
	res11->Handle = 21;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res11->Tblidx = 1411103;
	res11->Loc[0] = 4771.689941;// curpos.x;
	res11->Loc[1] = -51.380001; //curpos.y;
	res11->Loc[2] = 3957.850098;// curpos.z;
	res11->Dir[0] = 0.000000;
	res11->Dir[1] = -0.0;
	res11->Dir[2] = 1.000000;
	res11->Size = 10;
	res11->curEP = 500;
	res11->maxEP = 500;
	res11->curLP = 600;
	res11->maxLP = 600;
	res11->Level = 2;
	res11->StateID = 35;

	packet11.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet11);

	//Mob 27337
	CNtlPacket packet12(sizeof(SpawnMOB));
	SpawnMOB * res12 = (SpawnMOB *)packet12.GetPacketData();

	res12->wOpCode = GU_OBJECT_CREATE;
	res12->Type = OBJTYPE_MOB;
	res12->Handle = 22;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res12->Tblidx = 1411103;
	res12->Loc[0] = 4827.259766;// curpos.x;
	res12->Loc[1] = -47.250000; //curpos.y;
	res12->Loc[2] = 3921.860107;// curpos.z;
	res12->Dir[0] = 0.000000;
	res12->Dir[1] = -0.0;
	res12->Dir[2] = 1.000000;
	res12->Size = 10;
	res12->curEP = 500;
	res12->maxEP = 500;
	res12->curLP = 600;
	res12->maxLP = 600;
	res12->Level = 2;
	res12->StateID = 35;

	packet12.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet12);

	//Mob 27338
	CNtlPacket packet13(sizeof(SpawnMOB));
	SpawnMOB * res13 = (SpawnMOB *)packet13.GetPacketData();

	res13->wOpCode = GU_OBJECT_CREATE;
	res13->Type = OBJTYPE_MOB;
	res13->Handle = 23;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res13->Tblidx = 1411101;
	res13->Loc[0] = 4783.500000;// curpos.x;
	res13->Loc[1] = -60.910000; //curpos.y;
	res13->Loc[2] = 4073.489990;// curpos.z;
	res13->Dir[0] = 0.000000;
	res13->Dir[1] = -0.0;
	res13->Dir[2] = 1.000000;
	res13->Size = 10;
	res13->curEP = 500;
	res13->maxEP = 500;
	res13->curLP = 600;
	res13->maxLP = 600;
	res13->Level = 2;
	res13->StateID = 35;

	packet13.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet13);

	//Mob 27339
	CNtlPacket packet14(sizeof(SpawnMOB));
	SpawnMOB * res14 = (SpawnMOB *)packet14.GetPacketData();

	res14->wOpCode = GU_OBJECT_CREATE;
	res14->Type = OBJTYPE_MOB;
	res14->Handle = 24;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res14->Tblidx = 1411102;
	res14->Loc[0] = 4853.220215;// curpos.x;
	res14->Loc[1] = -59.180000; //curpos.y;
	res14->Loc[2] = 3958.370117;// curpos.z;
	res14->Dir[0] = 0.000000;
	res14->Dir[1] = -0.0;
	res14->Dir[2] = 1.000000;
	res14->Size = 10;
	res14->curEP = 500;
	res14->maxEP = 500;
	res14->curLP = 600;
	res14->maxLP = 600;
	res14->Level = 2;
	res14->StateID = 35;

	packet14.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet14);

	//Mob 27340
	CNtlPacket packet15(sizeof(SpawnMOB));
	SpawnMOB * res15 = (SpawnMOB *)packet15.GetPacketData();

	res15->wOpCode = GU_OBJECT_CREATE;
	res15->Type = OBJTYPE_MOB;
	res15->Handle = 25;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res15->Tblidx = 1411102;
	res15->Loc[0] = 4775.839844;// curpos.x;
	res15->Loc[1] = -56.160000; //curpos.y;
	res15->Loc[2] = 4012.959961;// curpos.z;
	res15->Dir[0] = 0.000000;
	res15->Dir[1] = -0.0;
	res15->Dir[2] = 1.000000;
	res15->Size = 10;
	res15->curEP = 500;
	res15->maxEP = 500;
	res15->curLP = 600;
	res15->maxLP = 600;
	res15->Level = 2;
	res15->StateID = 35;

	packet15.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet15);

	//Mob 27341
	CNtlPacket packet16(sizeof(SpawnMOB));
	SpawnMOB * res16 = (SpawnMOB *)packet6.GetPacketData();

	res16->wOpCode = GU_OBJECT_CREATE;
	res16->Type = OBJTYPE_MOB;
	res16->Handle = 26;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res16->Tblidx = 1411102;
	res16->Loc[0] = 4807.060059;// curpos.x;
	res16->Loc[1] = -56.360001; //curpos.y;
	res16->Loc[2] = 3992.080078;// curpos.z;
	res16->Dir[0] = 0.000000;
	res16->Dir[1] = -0.0;
	res16->Dir[2] = 1.000000;
	res16->Size = 10;
	res16->curEP = 500;
	res16->maxEP = 500;
	res16->curLP = 600;
	res16->maxLP = 600;
	res16->Level = 2;
	res16->StateID = 35;

	packet16.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet16);

	//Mob 27342
	CNtlPacket packet17(sizeof(SpawnMOB));
	SpawnMOB * res17 = (SpawnMOB *)packet17.GetPacketData();

	res17->wOpCode = GU_OBJECT_CREATE;
	res17->Type = OBJTYPE_MOB;
	res17->Handle = 27;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res17->Tblidx = 1411101;
	res17->Loc[0] = 815.180176;// curpos.x;
	res17->Loc[1] = -58.439999; //curpos.y;
	res17->Loc[2] = 4027.770020;// curpos.z;
	res17->Dir[0] = 0.000000;
	res17->Dir[1] = -0.0;
	res17->Dir[2] = 1.000000;
	res17->Size = 10;
	res17->curEP = 500;
	res17->maxEP = 500;
	res17->curLP = 600;
	res17->maxLP = 600;
	res17->Level = 2;
	res17->StateID = 35;

	packet17.SetPacketLen(sizeof(SpawnMOB));
	g_pApp->Send(pSession->GetHandle(), &packet17);

	
	
	
}

//Searches through mobMap to see if handle exists.
bool CMobManager::FindCreature(RwUInt32 handle)
{
//	IterType handleSearch = m_map_Monster.find(handle);
//	if (handleSearch != m_map_Monster.end())
//		return true;
//	else
		return false;
}
//Search the NPC by Handle
TBLIDX CMobManager::FindNpc(RwUInt32 handle)
{
//	IterType handleSearch = m_map_Npc.find(handle);
//	if (handleSearch != m_map_Npc.end())
//		return handleSearch->second->MonsterID;
//	else
		return 0;
}
//Simple function to return MonsterData
CMonster::MonsterData*	CMobManager::GetMobByHandle(RwUInt32 Target)
{
	
//	IterType handleSearch = m_map_Monster.find(Target);
//	if (handleSearch != m_map_Monster.end())
	{
//		CMonster::MonsterData * creaturelist = handleSearch->second;
//		return creaturelist;
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
