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

	//NPC 878

	CNtlPacket packet1(sizeof(SpawnNPC));
	SpawnNPC * res1 = (SpawnNPC *)packet1.GetPacketData();

	res1->wOpCode = GU_OBJECT_CREATE;
	res1->Type = OBJTYPE_NPC;
	res1->Handle = 1001;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res1->Tblidx = 6412103;
	res1->Loc[0] = 2101.540039;// curpos.x;
	res1->Loc[1] = 18.230000; //curpos.y;
	res1->Loc[2] = 5762.729980;// curpos.z;
	res1->Dir[0] = -0.957276;// curpos.x;
	res1->Dir[1] = -0.0; //curpos.y;
	res1->Dir[2] = -0.289177;// curpos.z;
	res1->Size = 10;
	//res9->Unknown[0] = 10.0;
	res1->Unknown3[0] = 0;
	res1->Unknown4[0] = 0;
	packet1.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet1);

	//NPC 877

	CNtlPacket packet2(sizeof(SpawnNPC));
	SpawnNPC * res2 = (SpawnNPC *)packet2.GetPacketData();

	res2->wOpCode = GU_OBJECT_CREATE;
	res2->Type = OBJTYPE_NPC;
	res2->Handle = 1002;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res2->Tblidx = 3142107;
	res2->Loc[0] = 2029.089966;// curpos.x;
	res2->Loc[1] = 9.860000; //curpos.y;
	res2->Loc[2] = 5730.109863;// curpos.z;
	res2->Dir[0] = -0.769039;// curpos.x;
	res2->Dir[1] = -0.0; //curpos.y;
	res2->Dir[2] = 0.639202;// curpos.z;
	res2->Size = 10;
	//res9->Unknown[0] = 10.0;
	res2->Unknown3[0] = 0;
	res2->Unknown4[0] = 0;
	packet2.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet2);

	//NPC 876

	CNtlPacket packet3(sizeof(SpawnNPC));
	SpawnNPC * res3 = (SpawnNPC *)packet3.GetPacketData();

	res3->wOpCode = GU_OBJECT_CREATE;
	res3->Type = OBJTYPE_NPC;
	res3->Handle = 1003;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res3->Tblidx = 4191114;
	res3->Loc[0] = 2023.170044;// curpos.x;
	res3->Loc[1] = 10.260000; //curpos.y;
	res3->Loc[2] = 5755.890137;// curpos.z;
	res3->Dir[0] = -0.841178;// curpos.x;
	res3->Dir[1] = -0.0; //curpos.y;
	res3->Dir[2] = -0.540758;// curpos.z;
	res3->Size = 10;
	res3->Unknown3[0] = 0;
	res3->Unknown4[0] = 0;
	packet3.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet3);

	//NPC 875

	CNtlPacket packet4(sizeof(SpawnNPC));
	SpawnNPC * res4 = (SpawnNPC *)packet4.GetPacketData();

	res4->wOpCode = GU_OBJECT_CREATE;
	res4->Type = OBJTYPE_NPC;
	res4->Handle = 1004;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res4->Tblidx = 1111115;
	res4->Loc[0] = 2623.080078;// curpos.x;
	res4->Loc[1] = -6.650000; //curpos.y;
	res4->Loc[2] = 5631.509766;// curpos.z;
	res4->Dir[0] = -0.390293;// curpos.x;
	res4->Dir[1] = -0.0; //curpos.y;
	res4->Dir[2] = -0.920691;// curpos.z;
	res4->Size = 10;
	res4->Unknown3[0] = 0;
	res4->Unknown4[0] = 0;
	packet4.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet4);

	//NPC 874

	CNtlPacket packet5(sizeof(SpawnNPC));
	SpawnNPC * res5 = (SpawnNPC *)packet5.GetPacketData();

	res5->wOpCode = GU_OBJECT_CREATE;
	res5->Type = OBJTYPE_NPC;
	res5->Handle = 1005;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res5->Tblidx = 6412104;
	res5->Loc[0] = 2638.929932;// curpos.x;
	res5->Loc[1] = -7.320000; //curpos.y;
	res5->Loc[2] = 5625.859863;// curpos.z;
	res5->Dir[0] = -0.209529;// curpos.x;
	res5->Dir[1] = -0.0; //curpos.y;
	res5->Dir[2] = -0.977802;// curpos.z;
	res5->Size = 10;
	res5->Unknown3[0] = 0;
	res5->Unknown4[0] = 0;
	packet5.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet5);

	//NPC 873

	CNtlPacket packet6(sizeof(SpawnNPC));
	SpawnNPC * res6 = (SpawnNPC *)packet6.GetPacketData();

	res6->wOpCode = GU_OBJECT_CREATE;
	res6->Type = OBJTYPE_NPC;
	res6->Handle = 1006;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res6->Tblidx = 1551103;
	res6->Loc[0] = 2623.310059;// curpos.x;
	res6->Loc[1] = -6.730000; //curpos.y;
	res6->Loc[2] = 5608.700195;// curpos.z;
	res6->Dir[0] = 0.410781;// curpos.x;
	res6->Dir[1] = -0.0; //curpos.y;
	res6->Dir[2] = 0.911734;// curpos.z;
	res6->Size = 10;
	res6->Unknown3[0] = 0;
	res6->Unknown4[0] = 0;
	packet6.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet6);

	//NPC 872

	CNtlPacket packet7(sizeof(SpawnNPC));
	SpawnNPC * res7 = (SpawnNPC *)packet7.GetPacketData();

	res7->wOpCode = GU_OBJECT_CREATE;
	res7->Type = OBJTYPE_NPC;
	res7->Handle = 1007;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res7->Tblidx = 1431105;
	res7->Loc[0] = 3595.550049;// curpos.x;
	res7->Loc[1] = -50.939999; //curpos.y;
	res7->Loc[2] = 4955.399902;// curpos.z;
	res7->Dir[0] = -0.929164;// curpos.x;
	res7->Dir[1] = -0.0; //curpos.y;
	res7->Dir[2] = -0.369667;// curpos.z;
	res7->Size = 10;
	res7->Unknown3[0] = 0;
	res7->Unknown4[0] = 0;
	packet7.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet7);

	//NPC 871

	CNtlPacket packet8(sizeof(SpawnNPC));
	SpawnNPC * res8 = (SpawnNPC *)packet8.GetPacketData();

	res8->wOpCode = GU_OBJECT_CREATE;
	res8->Type = OBJTYPE_NPC;
	res8->Handle = 1008;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res8->Tblidx = 3242108;
	res8->Loc[0] = 3593.199951;// curpos.x;
	res8->Loc[1] = -48.459999; //curpos.y;
	res8->Loc[2] = 4972.830078;// curpos.z;
	res8->Dir[0] = -0.932568;// curpos.x;
	res8->Dir[1] = -0.0; //curpos.y;
	res8->Dir[2] = -0.360994;// curpos.z;
	res8->Size = 10;
	res8->Unknown3[0] = 0;
	res8->Unknown4[0] = 0;
	packet8.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet8);

	//NPC 870

	CNtlPacket packet9(sizeof(SpawnNPC));
	SpawnNPC * res9 = (SpawnNPC *)packet9.GetPacketData();

	res9->wOpCode = GU_OBJECT_CREATE;
	res9->Type = OBJTYPE_NPC;
	res9->Handle = 1009;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res9->Tblidx = 3332302;
	res9->Loc[0] = 3684.709961;// curpos.x;
	res9->Loc[1] = -55.709999; //curpos.y;
	res9->Loc[2] = 4981.250000;// curpos.z;
	res9->Dir[0] = 0.998752;// curpos.x;
	res9->Dir[1] = -0.0; //curpos.y;
	res9->Dir[2] = -0.049938;// curpos.z;
	res9->Size = 10;
	res9->Unknown3[0] = 0;
	res9->Unknown4[0] = 0;
	packet9.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet9);

	//NPC 869

	CNtlPacket packet10(sizeof(SpawnNPC));
	SpawnNPC * res10 = (SpawnNPC *)packet10.GetPacketData();

	res10->wOpCode = GU_OBJECT_CREATE;
	res10->Type = OBJTYPE_NPC;
	res10->Handle = 1010;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res10->Tblidx = 3242115;
	res10->Loc[0] = 3687.229980;// curpos.x;
	res10->Loc[1] = -56.500000; //curpos.y;
	res10->Loc[2] = 4970.470215;// curpos.z;
	res10->Dir[0] = 0.911734;// curpos.x;
	res10->Dir[1] = -0.0; //curpos.y;
	res10->Dir[2] = 0.410781;// curpos.z;
	res10->Size = 10;
	res10->Unknown3[0] = 0;
	res10->Unknown4[0] = 0;
	packet10.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet10);

	//NPC 868

	CNtlPacket packet11(sizeof(SpawnNPC));
	SpawnNPC * res11 = (SpawnNPC *)packet11.GetPacketData();

	res11->wOpCode = GU_OBJECT_CREATE;
	res11->Type = OBJTYPE_NPC;
	res11->Handle = 1011;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res11->Tblidx = 3143113;
	res11->Loc[0] = 3687.260010;// curpos.x;
	res11->Loc[1] = -56.139999; //curpos.y;
	res11->Loc[2] = 4976.620117;// curpos.z;
	res11->Dir[0] = 0.999800;// curpos.x;
	res11->Dir[1] = -0.0; //curpos.y;
	res11->Dir[2] = 0.019996;// curpos.z;
	res11->Size = 10;
	res11->Unknown3[0] = 0;
	res11->Unknown4[0] = 0;
	packet11.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet11);

	//NPC 867

	CNtlPacket packet12(sizeof(SpawnNPC));
	SpawnNPC * res12 = (SpawnNPC *)packet12.GetPacketData();

	res12->wOpCode = GU_OBJECT_CREATE;
	res12->Type = OBJTYPE_NPC;
	res12->Handle = 1012;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res12->Tblidx = 5531101;
	res12->Loc[0] = 3693.459961;// curpos.x;
	res12->Loc[1] = -55.860001; //curpos.y;
	res12->Loc[2] = 4986.169922;// curpos.z;
	res12->Dir[0] = 0.828964;// curpos.x;
	res12->Dir[1] = -0.0; //curpos.y;
	res12->Dir[2] = -0.559301;// curpos.z;
	res12->Size = 10;
	res12->Unknown3[0] = 0;
	res12->Unknown4[0] = 0;
	packet12.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet12);


	//NPC 866

	CNtlPacket packet13(sizeof(SpawnNPC));
	SpawnNPC * res13 = (SpawnNPC *)packet13.GetPacketData();

	res13->wOpCode = GU_OBJECT_CREATE;
	res13->Type = OBJTYPE_NPC;
	res13->Handle = 1013;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res13->Tblidx = 4071108;
	res13->Loc[0] = 3702.739990;// curpos.x;
	res13->Loc[1] = -56.840000; //curpos.y;
	res13->Loc[2] = 4984.709961;// curpos.z;
	res13->Dir[0] = -0.529101;// curpos.x;
	res13->Dir[1] = -0.0; //curpos.y;
	res13->Dir[2] = -0.848559;// curpos.z;
	res13->Size = 10;
	res13->Unknown3[0] = 0;
	res13->Unknown4[0] = 0;
	packet13.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet13);

	//NPC 865

	CNtlPacket packet14(sizeof(SpawnNPC));
	SpawnNPC * res14 = (SpawnNPC *)packet14.GetPacketData();

	res14->wOpCode = GU_OBJECT_CREATE;
	res14->Type = OBJTYPE_NPC;
	res14->Handle = 1014;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res14->Tblidx = 5313201;
	res14->Loc[0] = 2974.820068;// curpos.x;
	res14->Loc[1] = -84.160004; //curpos.y;
	res14->Loc[2] = 4917.040039;// curpos.z;
	res14->Dir[0] = -0.628337;// curpos.x;
	res14->Dir[1] = -0.0; //curpos.y;
	res14->Dir[2] = -0.777941;// curpos.z;
	res14->Size = 10;
	res14->Unknown3[0] = 0;
	res14->Unknown4[0] = 0;
	packet14.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet14);

	//NPC 864

	CNtlPacket packet15(sizeof(SpawnNPC));
	SpawnNPC * res15 = (SpawnNPC *)packet15.GetPacketData();

	res15->wOpCode = GU_OBJECT_CREATE;
	res15->Type = OBJTYPE_NPC;
	res15->Handle = 1015;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res15->Tblidx = 2532122;
	res15->Loc[0] = 5304.319824;// curpos.x;
	res15->Loc[1] = -93.839996; //curpos.y;
	res15->Loc[2] = 4642.209961;// curpos.z;
	res15->Dir[0] = 0.721988;// curpos.x;
	res15->Dir[1] = -0.0; //curpos.y;
	res15->Dir[2] = 0.691905;// curpos.z;
	res15->Size = 10;
	res15->Unknown3[0] = 0;
	res15->Unknown4[0] = 0;
	packet15.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet15);

	//NPC 863

	CNtlPacket packet16(sizeof(SpawnNPC));
	SpawnNPC * res16 = (SpawnNPC *)packet16.GetPacketData();

	res16->wOpCode = GU_OBJECT_CREATE;
	res16->Type = OBJTYPE_NPC;
	res16->Handle = 1016;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res16->Tblidx = 6413104;
	res16->Loc[0] = 5314.819824;// curpos.x;
	res16->Loc[1] = -94.019997; //curpos.y;
	res16->Loc[2] = 4676.020020;// curpos.z;
	res16->Dir[0] = 0.800000;// curpos.x;
	res16->Dir[1] = -0.0; //curpos.y;
	res16->Dir[2] = -0.600000;// curpos.z;
	res16->Size = 10;
	res16->Unknown3[0] = 0;
	res16->Unknown4[0] = 0;
	packet16.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet16);

	//NPC 862

	CNtlPacket packet17(sizeof(SpawnNPC));
	SpawnNPC * res17 = (SpawnNPC *)packet17.GetPacketData();

	res17->wOpCode = GU_OBJECT_CREATE;
	res17->Type = OBJTYPE_NPC;
	res17->Handle = 1017;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res17->Tblidx = 8512126;
	res17->Loc[0] = 5303.140137;// curpos.x;
	res17->Loc[1] = -93.970001; //curpos.y;
	res17->Loc[2] = 4647.509766;// curpos.z;
	res17->Dir[0] = 0.932568;// curpos.x;
	res17->Dir[1] = -0.0; //curpos.y;
	res17->Dir[2] = 0.360994;// curpos.z;
	res17->Size = 10;
	res17->Unknown3[0] = 0;
	res17->Unknown4[0] = 0;
	packet17.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet17);

	//NPC 861

	CNtlPacket packet18(sizeof(SpawnNPC));
	SpawnNPC * res18 = (SpawnNPC *)packet18.GetPacketData();

	res18->wOpCode = GU_OBJECT_CREATE;
	res18->Type = OBJTYPE_NPC;
	res18->Handle = 1018;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res18->Tblidx = 3221209;
	res18->Loc[0] = 5343.689941;// curpos.x;
	res18->Loc[1] = -94.059998; //curpos.y;
	res18->Loc[2] = 4652.979980;// curpos.z;
	res18->Dir[0] = -0.848559;// curpos.x;
	res18->Dir[1] = -0.0; //curpos.y;
	res18->Dir[2] = 0.529101;// curpos.z;
	res18->Size = 10;
	res18->Unknown3[0] = 0;
	res18->Unknown4[0] = 0;
	packet18.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet18);

	//NPC 860

	CNtlPacket packet19(sizeof(SpawnNPC));
	SpawnNPC * res19 = (SpawnNPC *)packet19.GetPacketData();

	res19->wOpCode = GU_OBJECT_CREATE;
	res19->Type = OBJTYPE_NPC;
	res19->Handle = 1019;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res19->Tblidx = 7091111;
	res19->Loc[0] = 5326.359863;// curpos.x;
	res19->Loc[1] = -94.059998; //curpos.y;
	res19->Loc[2] = 4640.399902;// curpos.z;
	res19->Dir[0] = 0.000000;// curpos.x;
	res19->Dir[1] = -0.0; //curpos.y;
	res19->Dir[2] = 1.000000;// curpos.z;
	res19->Size = 10;
	res19->Unknown3[0] = 0;
	res19->Unknown4[0] = 0;
	packet19.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet19);

	//NPC 859

	CNtlPacket packet20(sizeof(SpawnNPC));
	SpawnNPC * res20 = (SpawnNPC *)packet20.GetPacketData();

	res20->wOpCode = GU_OBJECT_CREATE;
	res20->Type = OBJTYPE_NPC;
	res20->Handle = 1020;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res20->Tblidx = 3143117;
	res20->Loc[0] = 5307.080078;// curpos.x;
	res20->Loc[1] = -94.059998; //curpos.y;
	res20->Loc[2] = 4659.459961;// curpos.z;
	res20->Dir[0] = 0.898384;// curpos.x;
	res20->Dir[1] = -0.0; //curpos.y;
	res20->Dir[2] = 0.439210;// curpos.z;
	res20->Size = 10;
	res20->Unknown3[0] = 0;
	res20->Unknown4[0] = 0;
	packet20.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet20);

	//NPC 858

	CNtlPacket packet21(sizeof(SpawnNPC));
	SpawnNPC * res21 = (SpawnNPC *)packet21.GetPacketData();

	res21->wOpCode = GU_OBJECT_CREATE;
	res21->Type = OBJTYPE_NPC;
	res21->Handle = 1021;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res21->Tblidx = 4191102;
	res21->Loc[0] = 5336.750000;// curpos.x;
	res21->Loc[1] = -94.099998; //curpos.y;
	res21->Loc[2] = 4687.990234;// curpos.z;
	res21->Dir[0] = 0.000000;// curpos.x;
	res21->Dir[1] = -0.0; //curpos.y;
	res21->Dir[2] = 1.000000;// curpos.z;
	res21->Size = 10;
	res21->Unknown3[0] = 0;
	res21->Unknown4[0] = 0;
	packet21.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet21);

	//NPC 857

	CNtlPacket packet22(sizeof(SpawnNPC));
	SpawnNPC * res22 = (SpawnNPC *)packet22.GetPacketData();

	res22->wOpCode = GU_OBJECT_CREATE;
	res22->Type = OBJTYPE_NPC;
	res22->Handle = 1022;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res22->Tblidx = 2971101;
	res22->Loc[0] = 5320.080078;// curpos.x;
	res22->Loc[1] = -94.059998; //curpos.y;
	res22->Loc[2] = 4644.290039;// curpos.z;
	res22->Dir[0] = 0.029987;// curpos.x;
	res22->Dir[1] = -0.0; //curpos.y;
	res22->Dir[2] = 0.999550;// curpos.z;
	res22->Size = 10;
	res22->Unknown3[0] = 0;
	res22->Unknown4[0] = 0;
	packet22.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet22);

	//NPC 856

	CNtlPacket packet23(sizeof(SpawnNPC));
	SpawnNPC * res23 = (SpawnNPC *)packet23.GetPacketData();

	res23->wOpCode = GU_OBJECT_CREATE;
	res23->Type = OBJTYPE_NPC;
	res23->Handle = 1023;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res23->Tblidx = 2951101;
	res23->Loc[0] = 5309.740234;// curpos.x;
	res23->Loc[1] = -94.059998; //curpos.y;
	res23->Loc[2] = 4642.319824;// curpos.z;
	res23->Dir[0] = 0.100499;// curpos.x;
	res23->Dir[1] = -0.0; //curpos.y;
	res23->Dir[2] = 0.994937;// curpos.z;
	res23->Size = 10;
	res23->Unknown3[0] = 0;
	res23->Unknown4[0] = 0;
	packet23.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet23);

	//NPC 855

	CNtlPacket packet24(sizeof(SpawnNPC));
	SpawnNPC * res24 = (SpawnNPC *)packet24.GetPacketData();

	res24->wOpCode = GU_OBJECT_CREATE;
	res24->Type = OBJTYPE_NPC;
	res24->Handle = 1024;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res24->Tblidx = 8032101;
	res24->Loc[0] = 5311.759766;// curpos.x;
	res24->Loc[1] = -94.050003; //curpos.y;
	res24->Loc[2] = 4668.979980;// curpos.z;
	res24->Dir[0] = 0.867014;// curpos.x;
	res24->Dir[1] = -0.0; //curpos.y;
	res24->Dir[2] = -0.498284;// curpos.z;
	res24->Size = 10;
	res24->Unknown3[0] = 0;
	res24->Unknown4[0] = 0;
	packet24.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet24);

	//NPC 854

	CNtlPacket packet25(sizeof(SpawnNPC));
	SpawnNPC * res25 = (SpawnNPC *)packet25.GetPacketData();

	res25->wOpCode = GU_OBJECT_CREATE;
	res25->Type = OBJTYPE_NPC;
	res25->Handle = 1025;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res25->Tblidx = 3611101;
	res25->Loc[0] = 5349.959961;// curpos.x;
	res25->Loc[1] = -94.059998; //curpos.y;
	res25->Loc[2] = 4672.620117;// curpos.z;
	res25->Dir[0] = -0.985286;// curpos.x;
	res25->Dir[1] = -0.0; //curpos.y;
	res25->Dir[2] = 0.170917;// curpos.z;
	res25->Size = 10;
	res25->Unknown3[0] = 0;
	res25->Unknown4[0] = 0;
	packet25.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet25);

	//NPC 853

	CNtlPacket packet26(sizeof(SpawnNPC));
	SpawnNPC * res26 = (SpawnNPC *)packet26.GetPacketData();

	res26->wOpCode = GU_OBJECT_CREATE;
	res26->Type = OBJTYPE_NPC;
	res26->Handle = 1026;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res26->Tblidx = 4072103;
	res26->Loc[0] = 5273.859863;// curpos.x;
	res26->Loc[1] = -106.459999; //curpos.y;
	res26->Loc[2] = 4808.549805;// curpos.z;
	res26->Dir[0] = -0.289177;// curpos.x;
	res26->Dir[1] = -0.0; //curpos.y;
	res26->Dir[2] = -0.957276;// curpos.z;
	res26->Size = 10;
	res26->Unknown3[0] = 0;
	res26->Unknown4[0] = 0;
	packet26.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet26);

	//NPC 852

	CNtlPacket packet27(sizeof(SpawnNPC));
	SpawnNPC * res27 = (SpawnNPC *)packet27.GetPacketData();

	res27->wOpCode = GU_OBJECT_CREATE;
	res27->Type = OBJTYPE_NPC;
	res27->Handle = 1027;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res27->Tblidx = 3172102;
	res27->Loc[0] = 5296.569824;// curpos.x;
	res27->Loc[1] = -106.639999; //curpos.y;
	res27->Loc[2] = 4801.959961;// curpos.z;
	res27->Dir[0] = 0.258902;// curpos.x;
	res27->Dir[1] = -0.0; //curpos.y;
	res27->Dir[2] = 0.965904;// curpos.z;
	res27->Size = 10;
	res27->Unknown3[0] = 0;
	res27->Unknown4[0] = 0;
	packet27.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet27);

	//NPC 851

	CNtlPacket packet28(sizeof(SpawnNPC));
	SpawnNPC * res28 = (SpawnNPC *)packet28.GetPacketData();

	res28->wOpCode = GU_OBJECT_CREATE;
	res28->Type = OBJTYPE_NPC;
	res28->Handle = 1028;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res28->Tblidx = 4372203;
	res28->Loc[0] = 5311.740234;// curpos.x;
	res28->Loc[1] = -105.889999; //curpos.y;
	res28->Loc[2] = 4795.649902;// curpos.z;
	res28->Dir[0] = 0.069829;// curpos.x;
	res28->Dir[1] = -0.0; //curpos.y;
	res28->Dir[2] = 0.997559;// curpos.z;
	res28->Size = 10;
	res28->Unknown3[0] = 0;
	res28->Unknown4[0] = 0;
	packet28.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet28);

	//NPC 850

	CNtlPacket packet29(sizeof(SpawnNPC));
	SpawnNPC * res29 = (SpawnNPC *)packet29.GetPacketData();

	res29->wOpCode = GU_OBJECT_CREATE;
	res29->Type = OBJTYPE_NPC;
	res29->Handle = 1029;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res29->Tblidx = 5591201;
	res29->Loc[0] = 4858.479980;// curpos.x;
	res29->Loc[1] = -79.809998; //curpos.y;
	res29->Loc[2] = 4698.649902;// curpos.z;
	res29->Dir[0] = 0.258902;// curpos.x;
	res29->Dir[1] = -0.0; //curpos.y;
	res29->Dir[2] = -0.965904;// curpos.z;
	res29->Size = 10;
	res29->Unknown3[0] = 0;
	res29->Unknown4[0] = 0;
	packet29.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet29);

	//NPC 849

	CNtlPacket packet30(sizeof(SpawnNPC));
	SpawnNPC * res30 = (SpawnNPC *)packet30.GetPacketData();

	res30->wOpCode = GU_OBJECT_CREATE;
	res30->Type = OBJTYPE_NPC;
	res30->Handle = 1030;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res30->Tblidx = 8512131;
	res30->Loc[0] = 4255.149902;// curpos.x;
	res30->Loc[1] = -34.490002; //curpos.y;
	res30->Loc[2] = 4743.490234;// curpos.z;
	res30->Dir[0] = -0.855732;// curpos.x;
	res30->Dir[1] = -0.0; //curpos.y;
	res30->Dir[2] = -0.517419;// curpos.z;
	res30->Size = 10;
	res30->Unknown3[0] = 0;
	res30->Unknown4[0] = 0;
	packet30.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet30);

	//NPC 848

	CNtlPacket packet31(sizeof(SpawnNPC));
	SpawnNPC * res31 = (SpawnNPC *)packet31.GetPacketData();

	res31->wOpCode = GU_OBJECT_CREATE;
	res31->Type = OBJTYPE_NPC;
	res31->Handle = 1031;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res31->Tblidx = 8031501;
	res31->Loc[0] = 4234.350098;// curpos.x;
	res31->Loc[1] = -34.160000; //curpos.y;
	res31->Loc[2] = 4735.890137;// curpos.z;
	res31->Dir[0] = 0.731722;// curpos.x;
	res31->Dir[1] = -0.0; //curpos.y;
	res31->Dir[2] = -0.681604;// curpos.z;
	res31->Size = 10;
	res31->Unknown3[0] = 0;
	res31->Unknown4[0] = 0;
	packet31.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet31);

	//NPC 847

	CNtlPacket packet32(sizeof(SpawnNPC));
	SpawnNPC * res32 = (SpawnNPC *)packet32.GetPacketData();

	res32->wOpCode = GU_OBJECT_CREATE;
	res32->Type = OBJTYPE_NPC;
	res32->Handle = 1032;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res32->Tblidx = 2532128;
	res32->Loc[0] = 2879.790039;// curpos.x;
	res32->Loc[1] = -81.720001; //curpos.y;
	res32->Loc[2] = 4744.359863;// curpos.z;
	res32->Dir[0] = 0.617382;// curpos.x;
	res32->Dir[1] = -0.0; //curpos.y;
	res32->Dir[2] = -0.786664;// curpos.z;
	res32->Size = 10;
	res32->Unknown3[0] = 0;
	res32->Unknown4[0] = 0;
	packet32.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet32);

	//NPC 846

	CNtlPacket packet33(sizeof(SpawnNPC));
	SpawnNPC * res33 = (SpawnNPC *)packet33.GetPacketData();

	res33->wOpCode = GU_OBJECT_CREATE;
	res33->Type = OBJTYPE_NPC;
	res33->Handle = 1033;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res33->Tblidx = 6311103;
	res33->Loc[0] = 2860.209961;// curpos.x;
	res33->Loc[1] = -82.239998; //curpos.y;
	res33->Loc[2] = 4741.700195;// curpos.z;
	res33->Dir[0] = 0.019996;// curpos.x;
	res33->Dir[1] = -0.0; //curpos.y;
	res33->Dir[2] = -0.999800;// curpos.z;
	res33->Size = 10;
	res33->Unknown3[0] = 0;
	res33->Unknown4[0] = 0;
	packet33.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet33);

	//NPC 845

	CNtlPacket packet34(sizeof(SpawnNPC));
	SpawnNPC * res34 = (SpawnNPC *)packet34.GetPacketData();

	res34->wOpCode = GU_OBJECT_CREATE;
	res34->Type = OBJTYPE_NPC;
	res34->Handle = 1034;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res34->Tblidx = 4261105;
	res34->Loc[0] = 2886.469971;// curpos.x;
	res34->Loc[1] = -81.709999; //curpos.y;
	res34->Loc[2] = 4748.250000;// curpos.z;
	res34->Dir[0] = 0.100499;// curpos.x;
	res34->Dir[1] = -0.0; //curpos.y;
	res34->Dir[2] = -0.994937;// curpos.z;
	res34->Size = 10;
	res34->Unknown3[0] = 0;
	res34->Unknown4[0] = 0;
	packet34.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet34);

	//NPC 844

	CNtlPacket packet35(sizeof(SpawnNPC));
	SpawnNPC * res35 = (SpawnNPC *)packet35.GetPacketData();

	res35->wOpCode = GU_OBJECT_CREATE;
	res35->Type = OBJTYPE_NPC;
	res35->Handle = 1035;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res35->Tblidx = 2801203;
	res35->Loc[0] = 2876.649902;// curpos.x;
	res35->Loc[1] = -81.709999; //curpos.y;
	res35->Loc[2] = 4740.279785;// curpos.z;
	res35->Dir[0] = 0.867014;// curpos.x;
	res35->Dir[1] = -0.0; //curpos.y;
	res35->Dir[2] = -0.498284;// curpos.z;
	res35->Size = 10;
	res35->Unknown3[0] = 0;
	res35->Unknown4[0] = 0;
	packet35.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet35);

	//NPC 843

	CNtlPacket packet36(sizeof(SpawnNPC));
	SpawnNPC * res36 = (SpawnNPC *)packet36.GetPacketData();

	res36->wOpCode = GU_OBJECT_CREATE;
	res36->Type = OBJTYPE_NPC;
	res36->Handle = 1036;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res36->Tblidx = 5063103;
	res36->Loc[0] = 1974.520020;// curpos.x;
	res36->Loc[1] = -106.510002; //curpos.y;
	res36->Loc[2] = 4739.399902;// curpos.z;
	res36->Dir[0] = -0.769039;// curpos.x;
	res36->Dir[1] = -0.0; //curpos.y;
	res36->Dir[2] = -0.639202;// curpos.z;
	res36->Size = 10;
	res36->Unknown3[0] = 0;
	res36->Unknown4[0] = 0;
	packet36.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet36);

	//NPC 842

	CNtlPacket packet37(sizeof(SpawnNPC));
	SpawnNPC * res37 = (SpawnNPC *)packet37.GetPacketData();

	res37->wOpCode = GU_OBJECT_CREATE;
	res37->Type = OBJTYPE_NPC;
	res37->Handle = 1037;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res37->Tblidx = 7131122;
	res37->Loc[0] = 1963.650024;// curpos.x;
	res37->Loc[1] = -106.930000; //curpos.y;
	res37->Loc[2] = 4749.089844;// curpos.z;
	res37->Dir[0] = -0.920691;// curpos.x;
	res37->Dir[1] = -0.0; //curpos.y;
	res37->Dir[2] = -0.390293;// curpos.z;
	res37->Size = 10;
	res37->Unknown3[0] = 0;
	res37->Unknown4[0] = 0;
	packet37.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet37);

	//NPC 841

	CNtlPacket packet38(sizeof(SpawnNPC));
	SpawnNPC * res38 = (SpawnNPC *)packet38.GetPacketData();

	res38->wOpCode = GU_OBJECT_CREATE;
	res38->Type = OBJTYPE_NPC;
	res38->Handle = 1038;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res38->Tblidx = 4071109;
	res38->Loc[0] = 1984.189941;// curpos.x;
	res38->Loc[1] = -106.900002; //curpos.y;
	res38->Loc[2] = 4740.750000;// curpos.z;
	res38->Dir[0] = -0.328135;// curpos.x;
	res38->Dir[1] = -0.0; //curpos.y;
	res38->Dir[2] = -0.944631;// curpos.z;
	res38->Size = 10;
	res38->Unknown3[0] = 0;
	res38->Unknown4[0] = 0;
	packet38.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet38);

	//NPC 840

	CNtlPacket packet39(sizeof(SpawnNPC));
	SpawnNPC * res39 = (SpawnNPC *)packet39.GetPacketData();

	res39->wOpCode = GU_OBJECT_CREATE;
	res39->Type = OBJTYPE_NPC;
	res39->Handle = 1039;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res39->Tblidx = 4372206;
	res39->Loc[0] = 1960.939941;// curpos.x;
	res39->Loc[1] = -107.000000; //curpos.y;
	res39->Loc[2] = 4756.810059;// curpos.z;
	res39->Dir[0] = -0.808304;// curpos.x;
	res39->Dir[1] = -0.0; //curpos.y;
	res39->Dir[2] = -0.588765;// curpos.z;
	res39->Size = 10;
	res39->Unknown3[0] = 0;
	res39->Unknown4[0] = 0;
	packet39.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet39);

	//NPC 839

	CNtlPacket packet40(sizeof(SpawnNPC));
	SpawnNPC * res40 = (SpawnNPC *)packet40.GetPacketData();

	res40->wOpCode = GU_OBJECT_CREATE;
	res40->Type = OBJTYPE_NPC;
	res40->Handle = 1040;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res40->Tblidx = 8032115;
	res40->Loc[0] = 6297.209961;// curpos.x;
	res40->Loc[1] = -90.970001; //curpos.y;
	res40->Loc[2] = 4477.310059;// curpos.z;
	res40->Dir[0] = -0.997559;// curpos.x;
	res40->Dir[1] = -0.0; //curpos.y;
	res40->Dir[2] = 0.069829;// curpos.z;
	res40->Size = 10;
	res40->Unknown3[0] = 0;
	res40->Unknown4[0] = 0;
	packet40.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet40);

	//NPC 838

	CNtlPacket packet41(sizeof(SpawnNPC));
	SpawnNPC * res41 = (SpawnNPC *)packet41.GetPacketData();

	res41->wOpCode = GU_OBJECT_CREATE;
	res41->Type = OBJTYPE_NPC;
	res41->Handle = 1041;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res41->Tblidx = 4511504;
	res41->Loc[0] = 6299.229980;// curpos.x;
	res41->Loc[1] = -90.970001; //curpos.y;
	res41->Loc[2] = 4494.979980;// curpos.z;
	res41->Dir[0] = -0.977802;// curpos.x;
	res41->Dir[1] = -0.0; //curpos.y;
	res41->Dir[2] = 0.209529;// curpos.z;
	res41->Size = 10;
	res41->Unknown3[0] = 0;
	res41->Unknown4[0] = 0;
	packet41.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet41);
	//NPC 837

	CNtlPacket packet42(sizeof(SpawnNPC));
	SpawnNPC * res42 = (SpawnNPC *)packet42.GetPacketData();

	res42->wOpCode = GU_OBJECT_CREATE;
	res42->Type = OBJTYPE_NPC;
	res42->Handle = 1042;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res42->Tblidx = 4511101;
	res42->Loc[0] = 6297.240234;// curpos.x;
	res42->Loc[1] = -91.199997; //curpos.y;
	res42->Loc[2] = 4462.859863;// curpos.z;
	res42->Dir[0] = -0.808304;// curpos.x;
	res42->Dir[1] = -0.0; //curpos.y;
	res42->Dir[2] = 0.588765;// curpos.z;
	res42->Size = 10;
	res42->Unknown3[0] = 0;
	res42->Unknown4[0] = 0;
	packet42.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet42);

	//NPC 836

	CNtlPacket packet43(sizeof(SpawnNPC));
	SpawnNPC * res43 = (SpawnNPC *)packet43.GetPacketData();

	res43->wOpCode = GU_OBJECT_CREATE;
	res43->Type = OBJTYPE_NPC;
	res43->Handle = 1043;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res43->Tblidx = 1551104;
	res43->Loc[0] = 5784.040039;// curpos.x;
	res43->Loc[1] = -97.910004; //curpos.y;
	res43->Loc[2] = 4548.250000;// curpos.z;
	res43->Dir[0] = -0.707107;// curpos.x;
	res43->Dir[1] = -0.0; //curpos.y;
	res43->Dir[2] = 0.707107;// curpos.z;
	res43->Size = 10;
	res43->Unknown3[0] = 0;
	res43->Unknown4[0] = 0;
	packet43.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet43);

	//NPC 835

	CNtlPacket packet44(sizeof(SpawnNPC));
	SpawnNPC * res44 = (SpawnNPC *)packet44.GetPacketData();

	res44->wOpCode = GU_OBJECT_CREATE;
	res44->Type = OBJTYPE_NPC;
	res44->Handle = 1044;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res44->Tblidx = 8032113;
	res44->Loc[0] = 4943.609863;// curpos.x;
	res44->Loc[1] = -54.450001; //curpos.y;
	res44->Loc[2] = 4367.549805;// curpos.z;
	res44->Dir[0] = -0.617382;// curpos.x;
	res44->Dir[1] = -0.0; //curpos.y;
	res44->Dir[2] = 0.786664;// curpos.z;
	res44->Size = 10;
	res44->Unknown3[0] = 0;
	res44->Unknown4[0] = 0;
	packet44.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet44);

	//NPC 834

	CNtlPacket packet45(sizeof(SpawnNPC));
	SpawnNPC * res45 = (SpawnNPC *)packet45.GetPacketData();

	res45->wOpCode = GU_OBJECT_CREATE;
	res45->Type = OBJTYPE_NPC;
	res45->Handle = 1045;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res45->Tblidx = 5591202;
	res45->Loc[0] = 4926.549805;// curpos.x;
	res45->Loc[1] = -54.470001; //curpos.y;
	res45->Loc[2] = 4360.740234;// curpos.z;
	res45->Dir[0] = -0.258902;// curpos.x;
	res45->Dir[1] = -0.0; //curpos.y;
	res45->Dir[2] = 0.965904;// curpos.z;
	res45->Size = 10;
	res45->Unknown3[0] = 0;
	res45->Unknown4[0] = 0;
	packet45.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet45);

	//NPC 833

	CNtlPacket packet46(sizeof(SpawnNPC));
	SpawnNPC * res46 = (SpawnNPC *)packet46.GetPacketData();

	res46->wOpCode = GU_OBJECT_CREATE;
	res46->Type = OBJTYPE_NPC;
	res46->Handle = 1046;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res46->Tblidx = 3131203;
	res46->Loc[0] = 4929.470215;// curpos.x;
	res46->Loc[1] = -54.480000; //curpos.y;
	res46->Loc[2] = 4365.339844;// curpos.z;
	res46->Dir[0] = 0.994937;// curpos.x;
	res46->Dir[1] = -0.0; //curpos.y;
	res46->Dir[2] = 0.100499;// curpos.z;
	res46->Size = 10;
	res46->Unknown3[0] = 0;
	res46->Unknown4[0] = 0;
	packet46.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet46);

	//NPC 832

	CNtlPacket packet47(sizeof(SpawnNPC));
	SpawnNPC * res47 = (SpawnNPC *)packet47.GetPacketData();

	res47->wOpCode = GU_OBJECT_CREATE;
	res47->Type = OBJTYPE_NPC;
	res47->Handle = 1047;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res47->Tblidx = 5061201;
	res47->Loc[0] = 4903.640137;// curpos.x;
	res47->Loc[1] = -54.980000; //curpos.y;
	res47->Loc[2] = 4391.169922;// curpos.z;
	res47->Dir[0] = -0.731722;// curpos.x;
	res47->Dir[1] = -0.0; //curpos.y;
	res47->Dir[2] = -0.681604;// curpos.z;
	res47->Size = 10;
	res47->Unknown3[0] = 0;
	res47->Unknown4[0] = 0;
	packet47.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet47);

	//NPC 831

	CNtlPacket packet48(sizeof(SpawnNPC));
	SpawnNPC * res48 = (SpawnNPC *)packet48.GetPacketData();

	res48->wOpCode = GU_OBJECT_CREATE;
	res48->Type = OBJTYPE_NPC;
	res48->Handle = 1048;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res48->Tblidx = 3183103;
	res48->Loc[0] = 4906.680176;// curpos.x;
	res48->Loc[1] = -53.980000; //curpos.y;
	res48->Loc[2] = 4361.129883;// curpos.z;
	res48->Dir[0] = -0.977802;// curpos.x;
	res48->Dir[1] = -0.0; //curpos.y;
	res48->Dir[2] = 0.209529;// curpos.z;
	res48->Size = 10;
	res48->Unknown3[0] = 0;
	res48->Unknown4[0] = 0;
	packet48.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet48);

	//NPC 830

	CNtlPacket packet49(sizeof(SpawnNPC));
	SpawnNPC * res49 = (SpawnNPC *)packet49.GetPacketData();

	res49->wOpCode = GU_OBJECT_CREATE;
	res49->Type = OBJTYPE_NPC;
	res49->Handle = 1049;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res49->Tblidx = 3221202;
	res49->Loc[0] = 4701.540039;// curpos.x;
	res49->Loc[1] = -52.459999; //curpos.y;
	res49->Loc[2] = 4478.790039;// curpos.z;
	res49->Dir[0] = -0.741298;// curpos.x;
	res49->Dir[1] = -0.0; //curpos.y;
	res49->Dir[2] = -0.671176;// curpos.z;
	res49->Size = 10;
	res49->Unknown3[0] = 0;
	res49->Unknown4[0] = 0;
	packet49.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet49);

	//NPC 829

	CNtlPacket packet50(sizeof(SpawnNPC));
	SpawnNPC * res50 = (SpawnNPC *)packet50.GetPacketData();

	res50->wOpCode = GU_OBJECT_CREATE;
	res50->Type = OBJTYPE_NPC;
	res50->Handle = 1050;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res50->Tblidx = 8512149;
	res50->Loc[0] = 4188.020020;// curpos.x;
	res50->Loc[1] = -51.590000; //curpos.y;
	res50->Loc[2] = 4444.350098;// curpos.z;
	res50->Dir[0] = 0.911734;// curpos.x;
	res50->Dir[1] = -0.0; //curpos.y;
	res50->Dir[2] = 0.410781;// curpos.z;
	res50->Size = 10;
	res50->Unknown3[0] = 0;
	res50->Unknown4[0] = 0;
	packet50.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet50);

	//NPC 828

	CNtlPacket packet51(sizeof(SpawnNPC));
	SpawnNPC * res51 = (SpawnNPC *)packet51.GetPacketData();

	res51->wOpCode = GU_OBJECT_CREATE;
	res51->Type = OBJTYPE_NPC;
	res51->Handle = 1051;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res51->Tblidx = 6312104;
	res51->Loc[0] = 4201.060059;// curpos.x;
	res51->Loc[1] = -42.209999; //curpos.y;
	res51->Loc[2] = 4389.720215;// curpos.z;
	res51->Dir[0] = 0.559301;// curpos.x;
	res51->Dir[1] = -0.0; //curpos.y;
	res51->Dir[2] = 0.828964;// curpos.z;
	res51->Size = 10;
	res51->Unknown3[0] = 0;
	res51->Unknown4[0] = 0;
	packet51.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet51);

	//NPC 827

	CNtlPacket packet52(sizeof(SpawnNPC));
	SpawnNPC * res52 = (SpawnNPC *)packet52.GetPacketData();

	res52->wOpCode = GU_OBJECT_CREATE;
	res52->Type = OBJTYPE_NPC;
	res52->Handle = 1052;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res52->Tblidx = 1431104;
	res52->Loc[0] = 2929.010010;// curpos.x;
	res52->Loc[1] = -72.250000; //curpos.y;
	res52->Loc[2] = 4551.600098;// curpos.z;
	res52->Dir[0] = 0.855732;// curpos.x;
	res52->Dir[1] = -0.0; //curpos.y;
	res52->Dir[2] = -0.517419;// curpos.z;
	res52->Size = 10;
	res52->Unknown3[0] = 0;
	res52->Unknown4[0] = 0;
	packet52.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet52);

	//NPC 826

	CNtlPacket packet53(sizeof(SpawnNPC));
	SpawnNPC * res53 = (SpawnNPC *)packet53.GetPacketData();

	res53->wOpCode = GU_OBJECT_CREATE;
	res53->Type = OBJTYPE_NPC;
	res53->Handle = 1053;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res53->Tblidx = 3174101;
	res53->Loc[0] = 6099.140137;// curpos.x;
	res53->Loc[1] = -98.639999; //curpos.y;
	res53->Loc[2] = 4220.830078;// curpos.z;
	res53->Dir[0] = 0.410781;// curpos.x;
	res53->Dir[1] = -0.0; //curpos.y;
	res53->Dir[2] = -0.911734;// curpos.z;
	res53->Size = 10;
	res53->Unknown3[0] = 0;
	res53->Unknown4[0] = 0;
	packet53.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet53);

	//NPC 825

	CNtlPacket packet54(sizeof(SpawnNPC));
	SpawnNPC * res54 = (SpawnNPC *)packet54.GetPacketData();

	res54->wOpCode = GU_OBJECT_CREATE;
	res54->Type = OBJTYPE_NPC;
	res54->Handle = 1054;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res54->Tblidx = 5312101;
	res54->Loc[0] = 4878.140137;// curpos.x;
	res54->Loc[1] = -71.570000; //curpos.y;
	res54->Loc[2] = 4197.850098;// curpos.z;
	res54->Dir[0] = -0.310217;// curpos.x;
	res54->Dir[1] = -0.0; //curpos.y;
	res54->Dir[2] = 0.950666;// curpos.z;
	res54->Size = 10;
	res54->Unknown3[0] = 0;
	res54->Unknown4[0] = 0;
	packet54.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet54);

	//NPC 824 budokay need change

	CNtlPacket packet55(sizeof(SpawnNPC));
	SpawnNPC * res55 = (SpawnNPC *)packet55.GetPacketData();

	res55->wOpCode = GU_OBJECT_CREATE;
	res55->Type = OBJTYPE_NPC;
	res55->Handle = 1055;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res55->Tblidx = 7072105;
	res55->Loc[0] = 4549.709961;// curpos.x;
	res55->Loc[1] = -56.959999; //curpos.y;
	res55->Loc[2] = 4292.290039;// curpos.z;
	res55->Dir[0] = 0.209529;// curpos.x;
	res55->Dir[1] = -0.0; //curpos.y;
	res55->Dir[2] = -0.977802;// curpos.z;
	res55->Size = 10;
	res55->Unknown3[0] = 0;
	res55->Unknown4[0] = 0;
	packet55.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet55);

	//NPC 823 // budokay need change

	CNtlPacket packet56(sizeof(SpawnNPC));
	SpawnNPC * res56 = (SpawnNPC *)packet56.GetPacketData();

	res56->wOpCode = GU_OBJECT_CREATE;
	res56->Type = OBJTYPE_NPC;
	res56->Handle = 1056;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res56->Tblidx = 2532103;
	res56->Loc[0] = 4555.189941;// curpos.x;
	res56->Loc[1] = -56.959999; //curpos.y;
	res56->Loc[2] = 4295.740234;// curpos.z;
	res56->Dir[0] = 0.289177;// curpos.x;
	res56->Dir[1] = -0.0; //curpos.y;
	res56->Dir[2] = -0.957276;// curpos.z;
	res56->Size = 10;
	res56->Unknown3[0] = 0;
	res56->Unknown4[0] = 0;
	packet56.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet56);

	//NPC 822 Budokay need Change

	CNtlPacket packet57(sizeof(SpawnNPC));
	SpawnNPC * res57 = (SpawnNPC *)packet57.GetPacketData();

	res57->wOpCode = GU_OBJECT_CREATE;
	res57->Type = OBJTYPE_NPC;
	res57->Handle = 1057;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res57->Tblidx = 7071101;
	res57->Loc[0] = 4546.709961;// curpos.x;
	res57->Loc[1] = -56.959999; //curpos.y;
	res57->Loc[2] = 4283.569824;// curpos.z;
	res57->Dir[0] = 0.981720;// curpos.x;
	res57->Dir[1] = -0.0; //curpos.y;
	res57->Dir[2] = -0.190333;// curpos.z;
	res57->Size = 10;
	res57->Unknown3[0] = 0;
	res57->Unknown4[0] = 0;
	packet57.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet57);

	//NPC 821

	CNtlPacket packet58(sizeof(SpawnNPC));
	SpawnNPC * res58 = (SpawnNPC *)packet58.GetPacketData();

	res58->wOpCode = GU_OBJECT_CREATE;
	res58->Type = OBJTYPE_NPC;
	res58->Handle = 1058;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res58->Tblidx = 2804101;
	res58->Loc[0] = 4547.759766;// curpos.x;
	res58->Loc[1] = -56.959999; //curpos.y;
	res58->Loc[2] = 4298.359863;// curpos.z;
	res58->Dir[0] = 0.992734;// curpos.x;
	res58->Dir[1] = -0.0; //curpos.y;
	res58->Dir[2] = -0.120331;// curpos.z;
	res58->Size = 10;
	res58->Unknown3[0] = 0;
	res58->Unknown4[0] = 0;
	packet58.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet58);

	//NPC 820

	CNtlPacket packet59(sizeof(SpawnNPC));
	SpawnNPC * res59 = (SpawnNPC *)packet59.GetPacketData();

	res59->wOpCode = GU_OBJECT_CREATE;
	res59->Type = OBJTYPE_NPC;
	res59->Handle = 1059;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res59->Tblidx = 1654110;
	res59->Loc[0] = 3855.229980;// curpos.x;
	res59->Loc[1] = -54.630001; //curpos.y;
	res59->Loc[2] = 4123.890137;// curpos.z;
	res59->Dir[0] = -0.786664;// curpos.x;
	res59->Dir[1] = -0.0; //curpos.y;
	res59->Dir[2] = 0.617382;// curpos.z;
	res59->Size = 10;
	res59->Unknown3[0] = 0;
	res59->Unknown4[0] = 0;
	packet59.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet59);

	//NPC 819

	CNtlPacket packet60(sizeof(SpawnNPC));
	SpawnNPC * res60 = (SpawnNPC *)packet60.GetPacketData();

	res60->wOpCode = GU_OBJECT_CREATE;
	res60->Type = OBJTYPE_NPC;
	res60->Handle = 1060;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res60->Tblidx = 4511606;
	res60->Loc[0] = 3847.520020;// curpos.x;
	res60->Loc[1] = -55.619999; //curpos.y;
	res60->Loc[2] = 4141.509766;// curpos.z;
	res60->Dir[0] = -0.660628;// curpos.x;
	res60->Dir[1] = -0.0; //curpos.y;
	res60->Dir[2] = 0.750713;// curpos.z;
	res60->Size = 10;
	res60->Unknown3[0] = 0;
	res60->Unknown4[0] = 0;
	packet60.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet60);

	//NPC 818

	CNtlPacket packet61(sizeof(SpawnNPC));
	SpawnNPC * res61 = (SpawnNPC *)packet61.GetPacketData();

	res61->wOpCode = GU_OBJECT_CREATE;
	res61->Type = OBJTYPE_NPC;
	res61->Handle = 1061;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res61->Tblidx = 8032104;
	res61->Loc[0] = 3855.870117;// curpos.x;
	res61->Loc[1] = -54.770000; //curpos.y;
	res61->Loc[2] = 4133.279785;// curpos.z;
	res61->Dir[0] = -0.875578;// curpos.x;
	res61->Dir[1] = -0.0; //curpos.y;
	res61->Dir[2] = 0.483077;// curpos.z;
	res61->Size = 10;
	res61->Unknown3[0] = 0;
	res61->Unknown4[0] = 0;
	packet61.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet61);

	//NPC 817

	CNtlPacket packet62(sizeof(SpawnNPC));
	SpawnNPC * res62 = (SpawnNPC *)packet62.GetPacketData();

	res62->wOpCode = GU_OBJECT_CREATE;
	res62->Type = OBJTYPE_NPC;
	res62->Handle = 1062;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res62->Tblidx = 1591103;
	res62->Loc[0] = 2807.270020;// curpos.x;
	res62->Loc[1] = -80.470001; //curpos.y;
	res62->Loc[2] = 4122.040039;// curpos.z;
	res62->Dir[0] = 0.600000;// curpos.x;
	res62->Dir[1] = -0.0; //curpos.y;
	res62->Dir[2] = -0.800000;// curpos.z;
	res62->Size = 10;
	res62->Unknown3[0] = 0;
	res62->Unknown4[0] = 0;
	packet62.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet62);

	//NPC 816

	CNtlPacket packet63(sizeof(SpawnNPC));
	SpawnNPC * res63 = (SpawnNPC *)packet63.GetPacketData();

	res63->wOpCode = GU_OBJECT_CREATE;
	res63->Type = OBJTYPE_NPC;
	res63->Handle = 1063;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res63->Tblidx = 4372211;
	res63->Loc[0] = 2728.669922;// curpos.x;
	res63->Loc[1] = -80.489998; //curpos.y;
	res63->Loc[2] = 4255.819824;// curpos.z;
	res63->Dir[0] = 0.855732;// curpos.x;
	res63->Dir[1] = -0.0; //curpos.y;
	res63->Dir[2] = -0.517419;// curpos.z;
	res63->Size = 10;
	res63->Unknown3[0] = 0;
	res63->Unknown4[0] = 0;
	packet63.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet63);

	//NPC 815

	CNtlPacket packet64(sizeof(SpawnNPC));
	SpawnNPC * res64 = (SpawnNPC *)packet64.GetPacketData();

	res64->wOpCode = GU_OBJECT_CREATE;
	res64->Type = OBJTYPE_NPC;
	res64->Handle = 1064;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res64->Tblidx = 3331209;
	res64->Loc[0] = 2730.739990;// curpos.x;
	res64->Loc[1] = -80.290001; //curpos.y;
	res64->Loc[2] = 4246.540039;// curpos.z;
	res64->Dir[0] = -0.999550;// curpos.x;
	res64->Dir[1] = -0.0; //curpos.y;
	res64->Dir[2] = 0.029987;// curpos.z;
	res64->Size = 10;
	res64->Unknown3[0] = 0;
	res64->Unknown4[0] = 0;
	packet64.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet64);

	//NPC 814

	CNtlPacket packet65(sizeof(SpawnNPC));
	SpawnNPC * res65 = (SpawnNPC *)packet65.GetPacketData();

	res65->wOpCode = GU_OBJECT_CREATE;
	res65->Type = OBJTYPE_NPC;
	res65->Handle = 1065;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res65->Tblidx = 3331103;
	res65->Loc[0] = 2751.590088;// curpos.x;
	res65->Loc[1] = -81.879997; //curpos.y;
	res65->Loc[2] = 4252.629883;// curpos.z;
	res65->Dir[0] = 0.808304;// curpos.x;
	res65->Dir[1] = -0.0; //curpos.y;
	res65->Dir[2] = -0.588765;// curpos.z;
	res65->Size = 10;
	res65->Unknown3[0] = 0;
	res65->Unknown4[0] = 0;
	packet65.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet65);

	//NPC 813

	CNtlPacket packet66(sizeof(SpawnNPC));
	SpawnNPC * res66 = (SpawnNPC *)packet66.GetPacketData();

	res66->wOpCode = GU_OBJECT_CREATE;
	res66->Type = OBJTYPE_NPC;
	res66->Handle = 1066;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res66->Tblidx = 5313101;
	res66->Loc[0] = 2744.360107;// curpos.x;
	res66->Loc[1] = -79.150002; //curpos.y;
	res66->Loc[2] = 4259.700195;// curpos.z;
	res66->Dir[0] = 0.419058;// curpos.x;
	res66->Dir[1] = -0.0; //curpos.y;
	res66->Dir[2] = -0.907959;// curpos.z;
	res66->Size = 10;
	res66->Unknown3[0] = 0;
	res66->Unknown4[0] = 0;
	packet66.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet66);

	//NPC 812

	CNtlPacket packet67(sizeof(SpawnNPC));
	SpawnNPC * res67 = (SpawnNPC *)packet67.GetPacketData();

	res67->wOpCode = GU_OBJECT_CREATE;
	res67->Type = OBJTYPE_NPC;
	res67->Handle = 1067;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res67->Tblidx = 7611205;
	res67->Loc[0] = 2353.270020;// curpos.x;
	res67->Loc[1] = -57.810001; //curpos.y;
	res67->Loc[2] = 4131.069824;// curpos.z;
	res67->Dir[0] = 0.660628;// curpos.x;
	res67->Dir[1] = -0.0; //curpos.y;
	res67->Dir[2] = -0.750713;// curpos.z;
	res67->Size = 10;
	res67->Unknown3[0] = 0;
	res67->Unknown4[0] = 0;
	packet67.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet67);

	//NPC 811

	CNtlPacket packet68(sizeof(SpawnNPC));
	SpawnNPC * res68 = (SpawnNPC *)packet68.GetPacketData();

	res68->wOpCode = GU_OBJECT_CREATE;
	res68->Type = OBJTYPE_NPC;
	res68->Handle = 1068;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res68->Tblidx = 7611206;
	res68->Loc[0] = 1932.949951;// curpos.x;
	res68->Loc[1] = -54.950001; //curpos.y;
	res68->Loc[2] = 4306.640137;// curpos.z;
	res68->Dir[0] = 0.981720;// curpos.x;
	res68->Dir[1] = -0.0; //curpos.y;
	res68->Dir[2] = -0.190333;// curpos.z;
	res68->Size = 10;
	res68->Unknown3[0] = 0;
	res68->Unknown4[0] = 0;
	packet68.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet68);

	//NPC 810

	CNtlPacket packet69(sizeof(SpawnNPC));
	SpawnNPC * res69 = (SpawnNPC *)packet69.GetPacketData();

	res69->wOpCode = GU_OBJECT_CREATE;
	res69->Type = OBJTYPE_NPC;
	res69->Handle = 1069;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res69->Tblidx = 3131202;
	res69->Loc[0] = 6018.330078;// curpos.x;
	res69->Loc[1] = -94.099998; //curpos.y;
	res69->Loc[2] = 3980.899902;// curpos.z;
	res69->Dir[0] = -0.944631;// curpos.x;
	res69->Dir[1] = -0.0; //curpos.y;
	res69->Dir[2] = -0.328135;// curpos.z;
	res69->Size = 10;
	res69->Unknown3[0] = 0;
	res69->Unknown4[0] = 0;
	packet69.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet69);

	//NPC 809

	CNtlPacket packet70(sizeof(SpawnNPC));
	SpawnNPC * res70 = (SpawnNPC *)packet70.GetPacketData();

	res70->wOpCode = GU_OBJECT_CREATE;
	res70->Type = OBJTYPE_NPC;
	res70->Handle = 1070;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res70->Tblidx = 8512127;
	res70->Loc[0] = 5743.660156;// curpos.x;
	res70->Loc[1] = -87.620003; //curpos.y;
	res70->Loc[2] = 3937.679932;// curpos.z;
	res70->Dir[0] = -0.800000;// curpos.x;
	res70->Dir[1] = -0.0; //curpos.y;
	res70->Dir[2] = -0.600000;// curpos.z;
	res70->Size = 10;
	res70->Unknown3[0] = 0;
	res70->Unknown4[0] = 0;
	packet70.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet70);

	//NPC 808

	CNtlPacket packet71(sizeof(SpawnNPC));
	SpawnNPC * res71 = (SpawnNPC *)packet71.GetPacketData();

	res71->wOpCode = GU_OBJECT_CREATE;
	res71->Type = OBJTYPE_NPC;
	res71->Handle = 1071;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res71->Tblidx = 8032114;
	res71->Loc[0] = 5738.149902;// curpos.x;
	res71->Loc[1] = -87.190002; //curpos.y;
	res71->Loc[2] = 3927.520020;// curpos.z;
	res71->Dir[0] = -0.821109;// curpos.x;
	res71->Dir[1] = -0.0; //curpos.y;
	res71->Dir[2] = -0.570771;// curpos.z;
	res71->Size = 10;
	res71->Unknown3[0] = 0;
	res71->Unknown4[0] = 0;
	packet71.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet71);

	//NPC 807

	CNtlPacket packet72(sizeof(SpawnNPC));
	SpawnNPC * res72 = (SpawnNPC *)packet72.GetPacketData();

	res72->wOpCode = GU_OBJECT_CREATE;
	res72->Type = OBJTYPE_NPC;
	res72->Handle = 1072;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res72->Tblidx = 3183102;
	res72->Loc[0] = 5731.290039;// curpos.x;
	res72->Loc[1] = -87.230003; //curpos.y;
	res72->Loc[2] = 3933.919922;// curpos.z;
	res72->Dir[0] = -0.998752;// curpos.x;
	res72->Dir[1] = -0.0; //curpos.y;
	res72->Dir[2] = -0.049938;// curpos.z;
	res72->Size = 10;
	res72->Unknown3[0] = 0;
	res72->Unknown4[0] = 0;
	packet72.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet72);

	//NPC 806

	CNtlPacket packet73(sizeof(SpawnNPC));
	SpawnNPC * res73 = (SpawnNPC *)packet73.GetPacketData();

	res73->wOpCode = GU_OBJECT_CREATE;
	res73->Type = OBJTYPE_NPC;
	res73->Handle = 1073;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res73->Tblidx = 3331201;
	res73->Loc[0] = 5731.959961;// curpos.x;
	res73->Loc[1] = -87.150002; //curpos.y;
	res73->Loc[2] = 3930.239990;// curpos.z;
	res73->Dir[0] = -0.940376;// curpos.x;
	res73->Dir[1] = -0.0; //curpos.y;
	res73->Dir[2] = -0.340136;// curpos.z;
	res73->Size = 10;
	res73->Unknown3[0] = 0;
	res73->Unknown4[0] = 0;
	packet73.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet73);

	//NPC 805

	CNtlPacket packet74(sizeof(SpawnNPC));
	SpawnNPC * res74 = (SpawnNPC *)packet74.GetPacketData();

	res74->wOpCode = GU_OBJECT_CREATE;
	res74->Type = OBJTYPE_NPC;
	res74->Handle = 1074;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res74->Tblidx = 4072102;
	res74->Loc[0] = 5734.390137;// curpos.x;
	res74->Loc[1] = -87.269997; //curpos.y;
	res74->Loc[2] = 3926.989990;// curpos.z;
	res74->Dir[0] = -0.977802;// curpos.x;
	res74->Dir[1] = -0.0; //curpos.y;
	res74->Dir[2] = -0.209529;// curpos.z;
	res74->Size = 10;
	res74->Unknown3[0] = 0;
	res74->Unknown4[0] = 0;
	packet74.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet74);

	//NPC 804 Kokara

	CNtlPacket packet75(sizeof(SpawnNPC));
	SpawnNPC * res75 = (SpawnNPC *)packet75.GetPacketData();

	res75->wOpCode = GU_OBJECT_CREATE;
	res75->Type = OBJTYPE_NPC;
	res75->Handle = 1075;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res75->Tblidx = 2532129;
	res75->Loc[0] = 4518.109863;// curpos.x;
	res75->Loc[1] = -44.830002; //curpos.y;
	res75->Loc[2] = 4022.879883;// curpos.z;
	res75->Dir[0] = -0.120331;// curpos.x;
	res75->Dir[1] = -0.0; //curpos.y;
	res75->Dir[2] = -0.992734;// curpos.z;
	res75->Size = 10;
	res75->Unknown3[0] = 0;
	res75->Unknown4[0] = 0;
	packet75.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet75);

	//NPC 803

	CNtlPacket packet76(sizeof(SpawnNPC));
	SpawnNPC * res76 = (SpawnNPC *)packet76.GetPacketData();

	res76->wOpCode = GU_OBJECT_CREATE;
	res76->Type = OBJTYPE_NPC;
	res76->Handle = 1076;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res76->Tblidx = 2532119;
	res76->Loc[0] = 4498.520020;// curpos.x;
	res76->Loc[1] = -44.869999; //curpos.y;
	res76->Loc[2] = 4037.500000;// curpos.z;
	res76->Dir[0] = -0.019996;// curpos.x;
	res76->Dir[1] = -0.0; //curpos.y;
	res76->Dir[2] = -0.992734;// curpos.z;
	res76->Size = 10;
	res76->Unknown3[0] = 0;
	res76->Unknown4[0] = 0;
	packet76.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet76);

	//NPC 802

	CNtlPacket packet77(sizeof(SpawnNPC));
	SpawnNPC * res77 = (SpawnNPC *)packet77.GetPacketData();

	res77->wOpCode = GU_OBJECT_CREATE;
	res77->Type = OBJTYPE_NPC;
	res77->Handle = 1077;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res77->Tblidx = 2413101;
	res77->Loc[0] = 606.629883;// curpos.x;
	res77->Loc[1] = -45.049999; //curpos.y;
	res77->Loc[2] = 4044.139893;// curpos.z;
	res77->Dir[0] = -0.994937;// curpos.x;
	res77->Dir[1] = -0.0; //curpos.y;
	res77->Dir[2] = 0.100499;// curpos.z;
	res77->Size = 10;
	res77->Unknown3[0] = 0;
	res77->Unknown4[0] = 0;
	packet77.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet77);

	//NPC 801

	CNtlPacket packet78(sizeof(SpawnNPC));
	SpawnNPC * res78 = (SpawnNPC *)packet78.GetPacketData();

	res78->wOpCode = GU_OBJECT_CREATE;
	res78->Type = OBJTYPE_NPC;
	res78->Handle = 1078;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res78->Tblidx = 8512101;
	res78->Loc[0] = 4492.270020;// curpos.x;
	res78->Loc[1] = -44.759998; //curpos.y;
	res78->Loc[2] = 4034.909912;// curpos.z;
	res78->Dir[0] = 0.328135;// curpos.x;
	res78->Dir[1] = -0.0; //curpos.y;
	res78->Dir[2] = -0.944631;// curpos.z;
	res78->Size = 10;
	res78->Unknown3[0] = 0;
	res78->Unknown4[0] = 0;
	packet78.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet78);

	//NPC 800

	CNtlPacket packet79(sizeof(SpawnNPC));
	SpawnNPC * res79 = (SpawnNPC *)packet79.GetPacketData();

	res79->wOpCode = GU_OBJECT_CREATE;
	res79->Type = OBJTYPE_NPC;
	res79->Handle = 1079;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res79->Tblidx = 1351101;
	res79->Loc[0] = 4605.189941;// curpos.x;
	res79->Loc[1] = -44.680000; //curpos.y;
	res79->Loc[2] = 4086.159912;// curpos.z;
	res79->Dir[0] = 0.639202;// curpos.x;
	res79->Dir[1] = -0.0; //curpos.y;
	res79->Dir[2] = 0.769039;// curpos.z;
	res79->Size = 10;
	res79->Unknown3[0] = 0;
	res79->Unknown4[0] = 0;
	packet79.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet79);

	//NPC 799

	CNtlPacket packet80(sizeof(SpawnNPC));
	SpawnNPC * res80 = (SpawnNPC *)packet80.GetPacketData();

	res80->wOpCode = GU_OBJECT_CREATE;
	res80->Type = OBJTYPE_NPC;
	res80->Handle = 1080;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res80->Tblidx = 4191101;
	res80->Loc[0] = 4597.370117;// curpos.x;
	res80->Loc[1] = -44.650002; //curpos.y;
	res80->Loc[2] = 4094.239990;// curpos.z;
	res80->Dir[0] = 0.841178;// curpos.x;
	res80->Dir[1] = -0.0; //curpos.y;
	res80->Dir[2] = 0.540758;// curpos.z;
	res80->Size = 10;
	res80->Unknown3[0] = 0;
	res80->Unknown4[0] = 0;
	packet80.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet80);

	//NPC 798

	CNtlPacket packet81(sizeof(SpawnNPC));
	SpawnNPC * res81 = (SpawnNPC *)packet81.GetPacketData();

	res81->wOpCode = GU_OBJECT_CREATE;
	res81->Type = OBJTYPE_NPC;
	res81->Handle = 1081;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res81->Tblidx = 5591101;
	res81->Loc[0] = 4485.879883;// curpos.x;
	res81->Loc[1] = -44.349998; //curpos.y;
	res81->Loc[2] = 4066.419922;// curpos.z;
	res81->Dir[0] = 0.828964;// curpos.x;
	res81->Dir[1] = -0.0; //curpos.y;
	res81->Dir[2] = -0.559301;// curpos.z;
	res81->Size = 10;
	res81->Unknown3[0] = 0;
	res81->Unknown4[0] = 0;
	packet81.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet81);

	//NPC 797

	CNtlPacket packet82(sizeof(SpawnNPC));
	SpawnNPC * res82 = (SpawnNPC *)packet82.GetPacketData();

	res82->wOpCode = GU_OBJECT_CREATE;
	res82->Type = OBJTYPE_NPC;
	res82->Handle = 1082;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res82->Tblidx = 5041101;
	res82->Loc[0] = 4543.279785;// curpos.x;
	res82->Loc[1] = -44.259998; //curpos.y;
	res82->Loc[2] = 4066.919922;// curpos.z;
	res82->Dir[0] = 0.911734;// curpos.x;
	res82->Dir[1] = -0.0; //curpos.y;
	res82->Dir[2] = -0.410781;// curpos.z;
	res82->Size = 10;
	res82->Unknown3[0] = 0;
	res82->Unknown4[0] = 0;
	packet82.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet82);

	//NPC 796

	CNtlPacket packet83(sizeof(SpawnNPC));
	SpawnNPC * res83 = (SpawnNPC *)packet83.GetPacketData();

	res83->wOpCode = GU_OBJECT_CREATE;
	res83->Type = OBJTYPE_NPC;
	res83->Handle = 1083;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res83->Tblidx = 5061105;
	res83->Loc[0] = 4579.350098;// curpos.x;
	res83->Loc[1] = -44.630001; //curpos.y;
	res83->Loc[2] = 4013.370117;// curpos.z;
	res83->Dir[0] = -0.911734;// curpos.x;
	res83->Dir[1] = -0.0; //curpos.y;
	res83->Dir[2] = 0.410781;// curpos.z;
	res83->Size = 10;
	res83->Unknown3[0] = 0;
	res83->Unknown4[0] = 0;
	packet83.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet83);

	//NPC 795

	CNtlPacket packet84(sizeof(SpawnNPC));
	SpawnNPC * res84 = (SpawnNPC *)packet84.GetPacketData();

	res84->wOpCode = GU_OBJECT_CREATE;
	res84->Type = OBJTYPE_NPC;
	res84->Handle = 1084;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res84->Tblidx = 2972104;
	res84->Loc[0] = 4518.970215;// curpos.x;
	res84->Loc[1] = -45.000000; //curpos.y;
	res84->Loc[2] = 3969.580078;// curpos.z;
	res84->Dir[0] = 0.170917;// curpos.x;
	res84->Dir[1] = -0.0; //curpos.y;
	res84->Dir[2] = 0.985286;// curpos.z;
	res84->Size = 10;
	res84->Unknown3[0] = 0;
	res84->Unknown4[0] = 0;
	packet84.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet84);

	//NPC 794

	CNtlPacket packet85(sizeof(SpawnNPC));
	SpawnNPC * res85 = (SpawnNPC *)packet85.GetPacketData();

	res85->wOpCode = GU_OBJECT_CREATE;
	res85->Type = OBJTYPE_NPC;
	res85->Handle = 1085;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res85->Tblidx = 2801101;
	res85->Loc[0] = 4575.839844;// curpos.x;
	res85->Loc[1] = -44.279999; //curpos.y;
	res85->Loc[2] = 3985.080078;// curpos.z;
	res85->Dir[0] = -0.258902;// curpos.x;
	res85->Dir[1] = -0.0; //curpos.y;
	res85->Dir[2] = 0.965904;// curpos.z;
	res85->Size = 10;
	res85->Unknown3[0] = 0;
	res85->Unknown4[0] = 0;
	packet85.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet85);

	//NPC 793

	CNtlPacket packet86(sizeof(SpawnNPC));
	SpawnNPC * res86 = (SpawnNPC *)packet86.GetPacketData();

	res86->wOpCode = GU_OBJECT_CREATE;
	res86->Type = OBJTYPE_NPC;
	res86->Handle = 1086;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res86->Tblidx = 8511101;
	res86->Loc[0] = 4509.229980;// curpos.x;
	res86->Loc[1] = -45.000000; //curpos.y;
	res86->Loc[2] = 4031.330078;// curpos.z;
	res86->Dir[0] = -0.100499;// curpos.x;
	res86->Dir[1] = -0.0; //curpos.y;
	res86->Dir[2] = -0.994937;// curpos.z;
	res86->Size = 10;
	res86->Unknown3[0] = 0;
	res86->Unknown4[0] = 0;
	packet86.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet86);

	//NPC 792

	CNtlPacket packet87(sizeof(SpawnNPC));
	SpawnNPC * res87 = (SpawnNPC *)packet87.GetPacketData();

	res87->wOpCode = GU_OBJECT_CREATE;
	res87->Type = OBJTYPE_NPC;
	res87->Handle = 1087;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res87->Tblidx = 8411101;
	res87->Loc[0] = 4547.060059;// curpos.x;
	res87->Loc[1] = -45.000000; //curpos.y;
	res87->Loc[2] = 3996.250000;// curpos.z;
	res87->Dir[0] = -0.998752;// curpos.x;
	res87->Dir[1] = -0.0; //curpos.y;
	res87->Dir[2] = -0.049938;// curpos.z;
	res87->Size = 10;
	res87->Unknown3[0] = 0;
	res87->Unknown4[0] = 0;
	packet87.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet87);

	//NPC 791

	CNtlPacket packet88(sizeof(SpawnNPC));
	SpawnNPC * res88 = (SpawnNPC *)packet88.GetPacketData();

	res88->wOpCode = GU_OBJECT_CREATE;
	res88->Type = OBJTYPE_NPC;
	res88->Handle = 1088;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res88->Tblidx = 1111101;
	res88->Loc[0] = 4510.899902;// curpos.x;
	res88->Loc[1] = -44.980000; //curpos.y;
	res88->Loc[2] = 3952.080078;// curpos.z;
	res88->Dir[0] = 0.170917;// curpos.x;
	res88->Dir[1] = -0.0; //curpos.y;
	res88->Dir[2] = 0.985286;// curpos.z;
	res88->Size = 10;
	res88->Unknown3[0] = 0;
	res88->Unknown4[0] = 0;
	packet88.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet88);

	//NPC 790

	CNtlPacket packet89(sizeof(SpawnNPC));
	SpawnNPC * res89 = (SpawnNPC *)packet89.GetPacketData();

	res89->wOpCode = GU_OBJECT_CREATE;
	res89->Type = OBJTYPE_NPC;
	res89->Handle = 1089;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res89->Tblidx = 7131101;
	res89->Loc[0] = 4465.240234;// curpos.x;
	res89->Loc[1] = -45.000000; //curpos.y;
	res89->Loc[2] = 3995.010010;// curpos.z;
	res89->Dir[0] = 0.998752;// curpos.x;
	res89->Dir[1] = -0.0; //curpos.y;
	res89->Dir[2] = -0.049938;// curpos.z;
	res89->Size = 10;
	res89->Unknown3[0] = 0;
	res89->Unknown4[0] = 0;
	packet89.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet89);

	//NPC 789

	CNtlPacket packet90(sizeof(SpawnNPC));
	SpawnNPC * res90 = (SpawnNPC *)packet90.GetPacketData();

	res90->wOpCode = GU_OBJECT_CREATE;
	res90->Type = OBJTYPE_NPC;
	res90->Handle = 1090;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res90->Tblidx = 4041101;
	res90->Loc[0] = 4466.490234;// curpos.x;
	res90->Loc[1] = -43.400002; //curpos.y;
	res90->Loc[2] = 4020.399902;// curpos.z;
	res90->Dir[0] = 0.950666;// curpos.x;
	res90->Dir[1] = -0.0; //curpos.y;
	res90->Dir[2] = -0.310217;// curpos.z;
	res90->Size = 10;
	res90->Unknown3[0] = 0;
	res90->Unknown4[0] = 0;
	packet90.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet90);

	//NPC 788

	CNtlPacket packet91(sizeof(SpawnNPC));
	SpawnNPC * res91 = (SpawnNPC *)packet91.GetPacketData();

	res91->wOpCode = GU_OBJECT_CREATE;
	res91->Type = OBJTYPE_NPC;
	res91->Handle = 1091;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res91->Tblidx = 7091101;
	res91->Loc[0] = 4471.669922;// curpos.x;
	res91->Loc[1] = -43.220001; //curpos.y;
	res91->Loc[2] = 4028.570068;// curpos.z;
	res91->Dir[0] = 0.159546;// curpos.x;
	res91->Dir[1] = -0.0; //curpos.y;
	res91->Dir[2] = -0.987190;// curpos.z;
	res91->Size = 10;
	res91->Unknown3[0] = 0;
	res91->Unknown4[0] = 0;
	packet91.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet91);

	//NPC 787

	CNtlPacket packet92(sizeof(SpawnNPC));
	SpawnNPC * res92 = (SpawnNPC *)packet92.GetPacketData();

	res92->wOpCode = GU_OBJECT_CREATE;
	res92->Type = OBJTYPE_NPC;
	res92->Handle = 1092;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res92->Tblidx = 3331101;
	res92->Loc[0] = 4464.529785;// curpos.x;
	res92->Loc[1] = -42.480000; //curpos.y;
	res92->Loc[2] = 4024.870117;// curpos.z;
	res92->Dir[0] = 0.777941;// curpos.x;
	res92->Dir[1] = -0.0; //curpos.y;
	res92->Dir[2] = -0.628337;// curpos.z;
	res92->Size = 10;
	res92->Unknown3[0] = 0;
	res92->Unknown4[0] = 0;
	packet92.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet92);

	//NPC 786

	CNtlPacket packet93(sizeof(SpawnNPC));
	SpawnNPC * res93 = (SpawnNPC *)packet93.GetPacketData();

	res93->wOpCode = GU_OBJECT_CREATE;
	res93->Type = OBJTYPE_NPC;
	res93->Handle = 1093;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res93->Tblidx = 8031101;
	res93->Loc[0] = 4541.080078;// curpos.x;
	res93->Loc[1] = -43.299999; //curpos.y;
	res93->Loc[2] = 3957.850098;// curpos.z;
	res93->Dir[0] = -0.769039;// curpos.x;
	res93->Dir[1] = -0.0; //curpos.y;
	res93->Dir[2] = 0.639202;// curpos.z;
	res93->Size = 10;
	res93->Unknown3[0] = 0;
	res93->Unknown4[0] = 0;
	packet93.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet93);

	//NPC 785

	CNtlPacket packet94(sizeof(SpawnNPC));
	SpawnNPC * res94 = (SpawnNPC *)packet94.GetPacketData();

	res94->wOpCode = GU_OBJECT_CREATE;
	res94->Type = OBJTYPE_NPC;
	res94->Handle = 1094;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res94->Tblidx = 4181101;
	res94->Loc[0] = 4471.830078;// curpos.x;
	res94->Loc[1] = -41.900002; //curpos.y;
	res94->Loc[2] = 3956.179932;// curpos.z;
	res94->Dir[0] = 0.731722;// curpos.x;
	res94->Dir[1] = -0.0; //curpos.y;
	res94->Dir[2] = 0.681604;// curpos.z;
	res94->Size = 10;
	res94->Unknown3[0] = 0;
	res94->Unknown4[0] = 0;
	packet94.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet94);

	//NPC 784

	CNtlPacket packet95(sizeof(SpawnNPC));
	SpawnNPC * res95 = (SpawnNPC *)packet95.GetPacketData();

	res95->wOpCode = GU_OBJECT_CREATE;
	res95->Type = OBJTYPE_NPC;
	res95->Handle = 1095;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res95->Tblidx = 2804112;
	res95->Loc[0] = 4333.149902;// curpos.x;
	res95->Loc[1] = 9.280000; //curpos.y;
	res95->Loc[2] = 4093.590088;// curpos.z;
	res95->Dir[0] = 0.588765;// curpos.x;
	res95->Dir[1] = -0.0; //curpos.y;
	res95->Dir[2] = 0.808304;// curpos.z;
	res95->Size = 10;
	res95->Unknown3[0] = 0;
	res95->Unknown4[0] = 0;
	packet95.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet95);

	//NPC 783

	CNtlPacket packet96(sizeof(SpawnNPC));
	SpawnNPC * res96 = (SpawnNPC *)packet96.GetPacketData();

	res96->wOpCode = GU_OBJECT_CREATE;
	res96->Type = OBJTYPE_NPC;
	res96->Handle = 1096;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res96->Tblidx = 3142106;
	res96->Loc[0] = 1998.560059;// curpos.x;
	res96->Loc[1] = -55.939999; //curpos.y;
	res96->Loc[2] = 3923.010010;// curpos.z;
	res96->Dir[0] = -0.280000;// curpos.x;
	res96->Dir[1] = -0.0; //curpos.y;
	res96->Dir[2] = -0.960000;// curpos.z;
	res96->Size = 10;
	res96->Unknown3[0] = 0;
	res96->Unknown4[0] = 0;
	packet96.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet96);

	//NPC 782

	CNtlPacket packet97(sizeof(SpawnNPC));
	SpawnNPC * res97 = (SpawnNPC *)packet97.GetPacketData();

	res97->wOpCode = GU_OBJECT_CREATE;
	res97->Type = OBJTYPE_NPC;
	res97->Handle = 1097;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res97->Tblidx = 7611204;
	res97->Loc[0] = 1745.800049;// curpos.x;
	res97->Loc[1] = -52.139999; //curpos.y;
	res97->Loc[2] = 4014.780029;// curpos.z;
	res97->Dir[0] = -0.439210;// curpos.x;
	res97->Dir[1] = -0.0; //curpos.y;
	res97->Dir[2] = -0.898384;// curpos.z;
	res97->Size = 10;
	res97->Unknown3[0] = 0;
	res97->Unknown4[0] = 0;
	packet97.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet97);

	//NPC 781

	CNtlPacket packet98(sizeof(SpawnNPC));
	SpawnNPC * res98 = (SpawnNPC *)packet98.GetPacketData();

	res98->wOpCode = GU_OBJECT_CREATE;
	res98->Type = OBJTYPE_NPC;
	res9->Handle = 1098;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res98->Tblidx = 8751101;
	res98->Loc[0] = 5168.350098;// curpos.x;
	res98->Loc[1] = -66.620003; //curpos.y;
	res98->Loc[2] = 3626.429932;// curpos.z;
	res98->Dir[0] = 0.600000;// curpos.x;
	res98->Dir[1] = -0.0; //curpos.y;
	res98->Dir[2] = -0.800000;// curpos.z;
	res98->Size = 10;
	res98->Unknown3[0] = 0;
	res98->Unknown4[0] = 0;
	packet98.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet98);

	//NPC 780

	CNtlPacket packet99(sizeof(SpawnNPC));
	SpawnNPC * res99 = (SpawnNPC *)packet99.GetPacketData();

	res99->wOpCode = GU_OBJECT_CREATE;
	res99->Type = OBJTYPE_NPC;
	res99->Handle = 1099;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res99->Tblidx = 3362202;
	res99->Loc[0] = 3907.419922;// curpos.x;
	res99->Loc[1] = -18.719999; //curpos.y;
	res99->Loc[2] = 3667.639893;// curpos.z;
	res99->Dir[0] = -0.867014;// curpos.x;
	res99->Dir[1] = -0.0; //curpos.y;
	res99->Dir[2] = -0.498284;// curpos.z;
	res99->Size = 10;
	res99->Unknown3[0] = 0;
	res99->Unknown4[0] = 0;
	packet99.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet99);

	//NPC 779

	CNtlPacket packet100(sizeof(SpawnNPC));
	SpawnNPC * res100 = (SpawnNPC *)packet100.GetPacketData();

	res100->wOpCode = GU_OBJECT_CREATE;
	res100->Type = OBJTYPE_NPC;
	res100->Handle = 1100;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res100->Tblidx = 2802101;
	res100->Loc[0] = 3863.129883;// curpos.x;
	res100->Loc[1] = -21.889999; //curpos.y;
	res100->Loc[2] = 3714.979980;// curpos.z;
	res100->Dir[0] = -0.929164;// curpos.x;
	res100->Dir[1] = -0.0; //curpos.y;
	res100->Dir[2] = 0.369667;// curpos.z;
	res100->Size = 10;
	res100->Unknown3[0] = 0;
	res100->Unknown4[0] = 0;
	packet100.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet100);

	//NPC 778

	CNtlPacket packet101(sizeof(SpawnNPC));
	SpawnNPC * res101 = (SpawnNPC *)packet101.GetPacketData();

	res101->wOpCode = GU_OBJECT_CREATE;
	res101->Type = OBJTYPE_NPC;
	res101->Handle = 1101;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res101->Tblidx = 8755301;
	res101->Loc[0] = 3902.189941;// curpos.x;
	res101->Loc[1] = -18.110001; //curpos.y;
	res101->Loc[2] = 3663.370117;// curpos.z;
	res101->Dir[0] = -0.439210;// curpos.x;
	res101->Dir[1] = -0.0; //curpos.y;
	res101->Dir[2] = 0.898384;// curpos.z;
	res101->Size = 10;
	res101->Unknown3[0] = 0;
	res101->Unknown4[0] = 0;
	packet101.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet101);

	//NPC 777

	CNtlPacket packet102(sizeof(SpawnNPC));
	SpawnNPC * res102 = (SpawnNPC *)packet102.GetPacketData();

	res102->wOpCode = GU_OBJECT_CREATE;
	res102->Type = OBJTYPE_NPC;
	res102->Handle = 1102;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res102->Tblidx = 5591104;
	res102->Loc[0] = 3821.290039;// curpos.x;
	res102->Loc[1] = -12.560000; //curpos.y;
	res102->Loc[2] = 3819.800049;// curpos.z;
	res102->Dir[0] = 0.981720;// curpos.x;
	res102->Dir[1] = -0.0; //curpos.y;
	res102->Dir[2] = -0.190333;// curpos.z;
	res102->Size = 10;
	res102->Unknown3[0] = 0;
	res102->Unknown4[0] = 0;
	packet102.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet102);

	//NPC 776

	CNtlPacket packet103(sizeof(SpawnNPC));
	SpawnNPC * res103 = (SpawnNPC *)packet103.GetPacketData();

	res103->wOpCode = GU_OBJECT_CREATE;
	res103->Type = OBJTYPE_NPC;
	res103->Handle = 1103;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res103->Tblidx = 2804107;
	res103->Loc[0] = 3821.699951;// curpos.x;
	res103->Loc[1] = -12.460000; //curpos.y;
	res103->Loc[2] = 3810.860107;// curpos.z;
	res103->Dir[0] = 0.960000;// curpos.x;
	res103->Dir[1] = -0.0; //curpos.y;
	res103->Dir[2] = -0.280000;// curpos.z;
	res103->Size = 10;
	res103->Unknown3[0] = 0;
	res103->Unknown4[0] = 0;
	packet103.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet103);

	//NPC 775

	CNtlPacket packet104(sizeof(SpawnNPC));
	SpawnNPC * res104 = (SpawnNPC *)packet104.GetPacketData();

	res104->wOpCode = GU_OBJECT_CREATE;
	res104->Type = OBJTYPE_NPC;
	res104->Handle = 1104;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res104->Tblidx = 2413106;
	res104->Loc[0] = 2965.570068;// curpos.x;
	res104->Loc[1] = -41.880001; //curpos.y;
	res104->Loc[2] = 3752.870117;// curpos.z;
	res104->Dir[0] = -0.588765;// curpos.x;
	res104->Dir[1] = -0.0; //curpos.y;
	res104->Dir[2] = -0.808304;// curpos.z;
	res104->Size = 10;
	res104->Unknown3[0] = 0;
	res104->Unknown4[0] = 0;
	packet104.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet104);

	//NPC 774

	CNtlPacket packet105(sizeof(SpawnNPC));
	SpawnNPC * res105 = (SpawnNPC *)packet105.GetPacketData();

	res105->wOpCode = GU_OBJECT_CREATE;
	res105->Type = OBJTYPE_NPC;
	res105->Handle = 1105;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res105->Tblidx = 1351111;
	res105->Loc[0] = 2993.550049;// curpos.x;
	res105->Loc[1] = -50.480000; //curpos.y;
	res105->Loc[2] = 3584.370117;// curpos.z;
	res105->Dir[0] = 0.100499;// curpos.x;
	res105->Dir[1] = -0.0; //curpos.y;
	res105->Dir[2] = -0.994937;// curpos.z;
	res105->Size = 10;
	res105->Unknown3[0] = 0;
	res105->Unknown4[0] = 0;
	packet105.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet105);

	//NPC 773

	CNtlPacket packet106(sizeof(SpawnNPC));
	SpawnNPC * res106 = (SpawnNPC *)packet106.GetPacketData();

	res106->wOpCode = GU_OBJECT_CREATE;
	res106->Type = OBJTYPE_NPC;
	res106->Handle = 1106;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res106->Tblidx = 1351116;
	res106->Loc[0] = 2584.350098;// curpos.x;
	res106->Loc[1] = -50.470001; //curpos.y;
	res106->Loc[2] = 3704.830078;// curpos.z;
	res106->Dir[0] = 0.069829;// curpos.x;
	res106->Dir[1] = -0.0; //curpos.y;
	res106->Dir[2] = 0.997559;// curpos.z;
	res106->Size = 10;
	res106->Unknown3[0] = 0;
	res106->Unknown4[0] = 0;
	packet106.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet106);

	//NPC 772

	CNtlPacket packet107(sizeof(SpawnNPC));
	SpawnNPC * res107 = (SpawnNPC *)packet107.GetPacketData();

	res107->wOpCode = GU_OBJECT_CREATE;
	res107->Type = OBJTYPE_NPC;
	res107->Handle = 1101;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res107->Tblidx = 1351115;
	res107->Loc[0] = 2583.760010;// curpos.x;
	res107->Loc[1] = -50.320000; //curpos.y;
	res107->Loc[2] = 3714.899902;// curpos.z;
	res107->Dir[0] = 0.340136;// curpos.x;
	res107->Dir[1] = -0.0; //curpos.y;
	res107->Dir[2] = -0.940376;// curpos.z;
	res107->Size = 10;
	res107->Unknown3[0] = 0;
	res107->Unknown4[0] = 0;
	packet107.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet107);

	//NPC 771

	CNtlPacket packet108(sizeof(SpawnNPC));
	SpawnNPC * res108 = (SpawnNPC *)packet108.GetPacketData();

	res108->wOpCode = GU_OBJECT_CREATE;
	res108->Type = OBJTYPE_NPC;
	res108->Handle = 1108;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res108->Tblidx = 2532112;
	res108->Loc[0] = 5014.020020;// curpos.x;
	res108->Loc[1] = -61.080002; //curpos.y;
	res108->Loc[2] = 3517.560059;// curpos.z;
	res108->Dir[0] = 0.828964;// curpos.x;
	res108->Dir[1] = -0.0; //curpos.y;
	res108->Dir[2] = -0.559301;// curpos.z;
	res108->Size = 10;
	res108->Unknown3[0] = 0;
	res108->Unknown4[0] = 0;
	packet108.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet108);

	//NPC 770

	CNtlPacket packet109(sizeof(SpawnNPC));
	SpawnNPC * res109 = (SpawnNPC *)packet109.GetPacketData();

	res109->wOpCode = GU_OBJECT_CREATE;
	res109->Type = OBJTYPE_NPC;
	res109->Handle = 1109;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res109->Tblidx = 8512128;
	res109->Loc[0] = 5017.720215;// curpos.x;
	res109->Loc[1] = -61.040001; //curpos.y;
	res109->Loc[2] = 3517.389893;// curpos.z;
	res109->Dir[0] = 0.559301;// curpos.x;
	res109->Dir[1] = -0.0; //curpos.y;
	res109->Dir[2] = -0.828964;// curpos.z;
	res109->Size = 10;
	res109->Unknown3[0] = 0;
	res109->Unknown4[0] = 0;
	packet109.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet109);

	//NPC 769

	CNtlPacket packet110(sizeof(SpawnNPC));
	SpawnNPC * res110 = (SpawnNPC *)packet110.GetPacketData();

	res110->wOpCode = GU_OBJECT_CREATE;
	res110->Type = OBJTYPE_NPC;
	res110->Handle = 1110;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res110->Tblidx = 8032105;
	res110->Loc[0] = 5014.790039;// curpos.x;
	res110->Loc[1] = -61.000000; //curpos.y;
	res110->Loc[2] = 3509.750000;// curpos.z;
	res110->Dir[0] = 0.999550;// curpos.x;
	res110->Dir[1] = -0.0; //curpos.y;
	res110->Dir[2] = 0.029987;// curpos.z;
	res110->Size = 10;
	res110->Unknown3[0] = 0;
	res110->Unknown4[0] = 0;
	packet110.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet110);

	//NPC 768

	CNtlPacket packet111(sizeof(SpawnNPC));
	SpawnNPC * res111 = (SpawnNPC *)packet111.GetPacketData();

	res111->wOpCode = GU_OBJECT_CREATE;
	res111->Type = OBJTYPE_NPC;
	res111->Handle = 1111;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res111->Tblidx = 3143104;
	res111->Loc[0] = 5013.979980;// curpos.x;
	res111->Loc[1] = -61.000000; //curpos.y;
	res111->Loc[2] = 3495.709961;// curpos.z;
	res111->Dir[0] = 0.990149;// curpos.x;
	res111->Dir[1] = -0.0; //curpos.y;
	res111->Dir[2] = 0.140021;// curpos.z;
	res111->Size = 10;
	res111->Unknown3[0] = 0;
	res111->Unknown4[0] = 0;
	packet111.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet111);

	//NPC 767

	CNtlPacket packet112(sizeof(SpawnNPC));
	SpawnNPC * res112 = (SpawnNPC *)packet112.GetPacketData();

	res112->wOpCode = GU_OBJECT_CREATE;
	res112->Type = OBJTYPE_NPC;
	res112->Handle = 1112;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res112->Tblidx = 3221203;
	res112->Loc[0] = 5033.259766;// curpos.x;
	res112->Loc[1] = -61.000000; //curpos.y;
	res112->Loc[2] = 3506.709961;// curpos.z;
	res112->Dir[0] = -0.920691;// curpos.x;
	res112->Dir[1] = -0.0; //curpos.y;
	res112->Dir[2] = -0.390293;// curpos.z;
	res112->Size = 10;
	res112->Unknown3[0] = 0;
	res112->Unknown4[0] = 0;
	packet112.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet112);

	//NPC 766

	CNtlPacket packet113(sizeof(SpawnNPC));
	SpawnNPC * res113 = (SpawnNPC *)packet113.GetPacketData();

	res113->wOpCode = GU_OBJECT_CREATE;
	res113->Type = OBJTYPE_NPC;
	res113->Handle = 1113;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res113->Tblidx = 3142102;
	res113->Loc[0] = 5025.620117;// curpos.x;
	res113->Loc[1] = -61.000000; //curpos.y;
	res113->Loc[2] = 3507.580078;// curpos.z;
	res113->Dir[0] = 0.588765;// curpos.x;
	res113->Dir[1] = -0.0; //curpos.y;
	res113->Dir[2] = -0.808304;// curpos.z;
	res113->Size = 10;
	res113->Unknown3[0] = 0;
	res113->Unknown4[0] = 0;
	packet113.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet113);

	//NPC 765

	CNtlPacket packet114(sizeof(SpawnNPC));
	SpawnNPC * res114 = (SpawnNPC *)packet114.GetPacketData();

	res114->wOpCode = GU_OBJECT_CREATE;
	res114->Type = OBJTYPE_NPC;
	res114->Handle = 1114;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res114->Tblidx = 3183104;
	res114->Loc[0] = 5025.169922;// curpos.x;
	res114->Loc[1] = -61.000000; //curpos.y;
	res114->Loc[2] = 3498.729980;// curpos.z;
	res114->Dir[0] = 0.960000;// curpos.x;
	res114->Dir[1] = -0.0; //curpos.y;
	res114->Dir[2] = 0.280000;// curpos.z;
	res114->Size = 10;
	res114->Unknown3[0] = 0;
	res114->Unknown4[0] = 0;
	packet114.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet114);

	//NPC 764

	CNtlPacket packet115(sizeof(SpawnNPC));
	SpawnNPC * res115 = (SpawnNPC *)packet115.GetPacketData();

	res115->wOpCode = GU_OBJECT_CREATE;
	res115->Type = OBJTYPE_NPC;
	res115->Handle = 1115;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res115->Tblidx = 1551102;
	res115->Loc[0] = 5098.009766;// curpos.x;
	res115->Loc[1] = -62.160000; //curpos.y;
	res115->Loc[2] = 3417.469971;// curpos.z;
	res115->Dir[0] = 0.911734;// curpos.x;
	res115->Dir[1] = -0.0; //curpos.y;
	res115->Dir[2] = 0.410781;// curpos.z;
	res115->Size = 10;
	res115->Unknown3[0] = 0;
	res115->Unknown4[0] = 0;
	packet115.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet115);

	//NPC 763

	CNtlPacket packet116(sizeof(SpawnNPC));
	SpawnNPC * res116 = (SpawnNPC *)packet116.GetPacketData();

	res116->wOpCode = GU_OBJECT_CREATE;
	res116->Type = OBJTYPE_NPC;
	res116->Handle = 1116;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res116->Tblidx = 6312102;
	res116->Loc[0] = 5097.120117;// curpos.x;
	res116->Loc[1] = -62.160000; //curpos.y;
	res116->Loc[2] = 3421.189941;// curpos.z;
	res116->Dir[0] = 0.892413;// curpos.x;
	res116->Dir[1] = -0.0; //curpos.y;
	res116->Dir[2] = 0.451220;// curpos.z;
	res116->Size = 10;
	res116->Unknown3[0] = 0;
	res116->Unknown4[0] = 0;
	packet116.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet116);

	//NPC 762

	CNtlPacket packet117(sizeof(SpawnNPC));
	SpawnNPC * res117 = (SpawnNPC *)packet117.GetPacketData();

	res117->wOpCode = GU_OBJECT_CREATE;
	res117->Type = OBJTYPE_NPC;
	res117->Handle = 1117;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res117->Tblidx = 3171103;
	res117->Loc[0] = 3430.149902;// curpos.x;
	res117->Loc[1] = -84.389999; //curpos.y;
	res117->Loc[2] = 3343.090088;// curpos.z;
	res117->Dir[0] = 0.000000;// curpos.x;
	res117->Dir[1] = -0.0; //curpos.y;
	res117->Dir[2] = 1.000000;// curpos.z;
	res117->Size = 10;
	res117->Unknown3[0] = 0;
	res117->Unknown4[0] = 0;
	packet117.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet117);

	//NPC 761

	CNtlPacket packet118(sizeof(SpawnNPC));
	SpawnNPC * res118 = (SpawnNPC *)packet118.GetPacketData();

	res118->wOpCode = GU_OBJECT_CREATE;
	res118->Type = OBJTYPE_NPC;
	res118->Handle = 1118;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res118->Tblidx = 3142112;
	res118->Loc[0] = 3412.419922;// curpos.x;
	res118->Loc[1] = -84.419998; //curpos.y;
	res118->Loc[2] = 3352.449951;// curpos.z;
	res118->Dir[0] = 0.000000;// curpos.x;
	res118->Dir[1] = -0.0; //curpos.y;
	res118->Dir[2] = 1.000000;// curpos.z;
	res118->Size = 10;
	res118->Unknown3[0] = 0;
	res118->Unknown4[0] = 0;
	packet118.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet118);

	//NPC 760

	CNtlPacket packet119(sizeof(SpawnNPC));
	SpawnNPC * res119 = (SpawnNPC *)packet119.GetPacketData();

	res119->wOpCode = GU_OBJECT_CREATE;
	res119->Type = OBJTYPE_NPC;
	res119->Handle = 1119;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res119->Tblidx = 2801109;
	res119->Loc[0] = 3459.550049;// curpos.x;
	res119->Loc[1] = -83.930000; //curpos.y;
	res119->Loc[2] = 3329.590088;// curpos.z;
	res119->Dir[0] = -0.892413;// curpos.x;
	res119->Dir[1] = -0.0; //curpos.y;
	res119->Dir[2] = 0.451220;// curpos.z;
	res119->Size = 10;
	res119->Unknown3[0] = 0;
	res119->Unknown4[0] = 0;
	packet119.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet119);

	//NPC 759

	CNtlPacket packet120(sizeof(SpawnNPC));
	SpawnNPC * res120 = (SpawnNPC *)packet120.GetPacketData();

	res120->wOpCode = GU_OBJECT_CREATE;
	res120->Type = OBJTYPE_NPC;
	res120->Handle = 1120;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res120->Tblidx = 1351112;
	res120->Loc[0] = 2995.239990;// curpos.x;
	res120->Loc[1] = -50.389999; //curpos.y;
	res120->Loc[2] = 3567.919922;// curpos.z;
	res120->Dir[0] = 0.000000;// curpos.x;
	res120->Dir[1] = -0.0; //curpos.y;
	res120->Dir[2] = 1.000000;// curpos.z;
	res120->Size = 10;
	res120->Unknown3[0] = 0;
	res120->Unknown4[0] = 0;
	packet120.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet120);

	//NPC 758

	CNtlPacket packet121(sizeof(SpawnNPC));
	SpawnNPC * res121 = (SpawnNPC *)packet121.GetPacketData();

	res121->wOpCode = GU_OBJECT_CREATE;
	res121->Type = OBJTYPE_NPC;
	res121->Handle = 1121;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res121->Tblidx = 2532108;
	res121->Loc[0] = 2595.050049;// curpos.x;
	res121->Loc[1] = -42.110001; //curpos.y;
	res121->Loc[2] = 3421.919922;// curpos.z;
	res121->Dir[0] = 0.559301;// curpos.x;
	res121->Dir[1] = -0.0; //curpos.y;
	res121->Dir[2] = 0.828964;// curpos.z;
	res121->Size = 10;
	res121->Unknown3[0] = 0;
	res121->Unknown4[0] = 0;
	packet121.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet121);

	//NPC 757

	CNtlPacket packet122(sizeof(SpawnNPC));
	SpawnNPC * res122 = (SpawnNPC *)packet122.GetPacketData();

	res122->wOpCode = GU_OBJECT_CREATE;
	res122->Type = OBJTYPE_NPC;
	res122->Handle = 1122;//AcquireSerialId();//app->mob->AcquireMOBSerialId() this will get your Player Handle,need change "AcquireSerialId" because here is used to generate a Handler for the players! #Issue 6 Luiz45
	res122->Tblidx = 1353102;
	res122->Loc[0] = 2728.479980;// curpos.x;
	res122->Loc[1] = -25.430000; //curpos.y;
	res122->Loc[2] = 3350.709961;// curpos.z;
	res122->Dir[0] = -0.929164;// curpos.x;
	res122->Dir[1] = -0.0; //curpos.y;
	res122->Dir[2] = -0.369667;// curpos.z;
	res122->Size = 10;
	res122->Unknown3[0] = 0;
	res122->Unknown4[0] = 0;
	packet122.SetPacketLen(sizeof(SpawnNPC));
	g_pApp->Send(pSession->GetHandle(), &packet122);

	
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
