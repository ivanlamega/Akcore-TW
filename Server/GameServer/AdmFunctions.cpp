#include "stdafx.h"
#include "AdmFunctions.h"
#include "GameServer.h"

void  AdmFunctions::CreateMonsterById(unsigned int uiMobId, CNtlPacket * pPacket, CClientSession * pSession)
{
	//CGameServer * app = (CGameServer*)NtlSfxGetApp();
	//sVECTOR3 curpos = pSession->plr->GetPosition();
	//CMobTable* pMyMobTable = app->g_pTableContainer->GetMobTable();
	//sMOB_TBLDAT* mob = reinterpret_cast<sMOB_TBLDAT*>(pMyMobTable->FindData(uiMobId));
	//if (mob != NULL)
	//{
	//	CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
	//	sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();

	//	res->wOpCode = GU_OBJECT_CREATE;
	//	res->sObjectInfo.objType = OBJTYPE_MOB;
	//	res->handle = app->mob->AcquireMOBSerialId();
	//	res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = curpos.x;
	//	res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = curpos.y;
	//	res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = curpos.z;
	//	res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = curpos.x + rand() % 360;
	//	res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = curpos.y;
	//	res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = curpos.z;
	//	res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
	//	res->sObjectInfo.mobState.sCharStateBase.bFightMode = false;
	//	res->sObjectInfo.mobBrief.tblidx = uiMobId;
	//	res->sObjectInfo.mobBrief.wCurEP = mob->wBasic_EP;
	//	res->sObjectInfo.mobBrief.wMaxEP = mob->wBasic_EP;
	//	res->sObjectInfo.mobBrief.wCurLP = mob->wBasic_LP;
	//	res->sObjectInfo.mobBrief.wMaxLP = mob->wBasic_LP;
	//	res->sObjectInfo.mobBrief.fLastRunningSpeed = mob->fRun_Speed;
	//	res->sObjectInfo.mobBrief.fLastWalkingSpeed = mob->fWalk_Speed;
	//	//pSession->InsertIntoMyMonsterList(creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID); will insert into list after modifying spawn/despawn check new function

	//	packet.SetPacketLen(sizeof(sGU_OBJECT_CREATE));
	//	g_pApp->Send(pSession->GetHandle(), &packet);
	//}

}