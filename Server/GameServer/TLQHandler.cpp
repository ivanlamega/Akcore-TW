#include "stdafx.h"
#include "GameServer.h"

TLQHandler::TLQHandler()
{
}
TLQHandler::~TLQHandler()
{
}
//Return current TLQ
NTL_TS_T_ID TLQHandler::GetTlqID()
{
	return this->questID;
}
//Sets the TLQ For the player and send him to TLQ Quest
void TLQHandler::SetTLQAccessForPlayer(NTL_TS_T_ID QuestID, PlayersMain* cPlayersMain)
{
	this->questID = QuestID;
	this->myPlayer = cPlayersMain;
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	//* pWorldTable = app->g_pTableContainer->GetWorldTable();
	CNtlPacket packetQ(sizeof(sGU_AVATAR_WORLD_INFO));
	sGU_AVATAR_WORLD_INFO*as = (sGU_AVATAR_WORLD_INFO*)packetQ.GetPacketData();
	
	////Teleport to Initial Location of TLQ 1
	//if (QuestID == 6000)
	//{
	//	sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(120000));
	//	as->worldInfo.hTriggerObjectOffset = 100000;
	//	as->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
	//	as->worldInfo.tblidx = 120000;
	//	as->worldInfo.worldID = 120000;
	//	//printf("WorldID:%d\n\r", pWorldTbldat->dwWorldResourceID);
	//	as->vCurDir.x = pWorldTbldat->vStart1Dir.x;
	//	as->vCurDir.y = 0;
	//	as->vCurDir.z = pWorldTbldat->vStart1Dir.z;
	//	as->vCurLoc.x = pWorldTbldat->vStart1Loc.x;
	//	as->vCurLoc.y = pWorldTbldat->vStart1Loc.y;
	//	as->vCurLoc.z = pWorldTbldat->vStart1Loc.z;
	//	as->wOpCode = GU_AVATAR_WORLD_INFO;
	//	packetQ.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	//	g_pApp->Send(this->myPlayer->GetSession(), &packetQ);
	//	cPlayersMain->SetWorldID(120000);
	//	cPlayersMain->SetWorldTblidx(120000);
	//}
	////Teleport to the Main World
	//else if (QuestID == 6002)
	//{
	//	sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(1));
	//	as->worldInfo.hTriggerObjectOffset = 100000;
	//	as->worldInfo.sRuleInfo.byRuleType = GAMERULE_NORMAL;
	//	as->worldInfo.tblidx = 1;
	//	as->worldInfo.worldID = 1;
	//	//printf("WorldID:%d\n\r", pWorldTbldat->dwWorldResourceID);
	//	as->vCurDir.x = pWorldTbldat->vStart1Dir.x;
	//	as->vCurDir.y = 0;
	//	as->vCurDir.z = pWorldTbldat->vStart1Dir.z;
	//	as->vCurLoc.x = pWorldTbldat->vStart1Loc.x;
	//	as->vCurLoc.y = pWorldTbldat->vStart1Loc.y;
	//	as->vCurLoc.z = pWorldTbldat->vStart1Loc.z;
	//	as->wOpCode = GU_AVATAR_WORLD_INFO;
	//	packetQ.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	//	g_pApp->Send(this->myPlayer->GetSession(), &packetQ);
	//}
	//else if (QuestID == 989898)
	//{
	//	sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(120000));
	//	as->worldInfo.hTriggerObjectOffset = 100000;
	//	as->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
	//	as->worldInfo.tblidx = 120000;
	//	as->worldInfo.worldID = 120000;
	//	//printf("WorldID:%d\n\r", pWorldTbldat->dwWorldResourceID);
	//	as->vCurDir.x = pWorldTbldat->vStart1Dir.x;
	//	as->vCurDir.y = 0;
	//	as->vCurDir.z = pWorldTbldat->vStart1Dir.z;
	//	as->vCurLoc.x = pWorldTbldat->vStart1Loc.x;
	//	as->vCurLoc.y = pWorldTbldat->vStart1Loc.y;
	//	as->vCurLoc.z = pWorldTbldat->vStart1Loc.z;
	//	as->wOpCode = GU_AVATAR_WORLD_INFO;
	//	packetQ.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	//	g_pApp->Send(this->myPlayer->GetSession(), &packetQ);
	//}
	//else if (QuestID == 989898)
	//{
	//	sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(120000));
	//	as->worldInfo.hTriggerObjectOffset = 100000;
	//	as->worldInfo.sRuleInfo.byRuleType = GAMERULE_TLQ;
	//	as->worldInfo.tblidx = 120000;
	//	as->worldInfo.worldID = 120000;
	//	//printf("WorldID:%d\n\r", pWorldTbldat->dwWorldResourceID);
	//	as->vCurDir.x = pWorldTbldat->vStart1Dir.x;
	//	as->vCurDir.y = 0;
	//	as->vCurDir.z = pWorldTbldat->vStart1Dir.z;
	//	as->vCurLoc.x = pWorldTbldat->vStart1Loc.x;
	//	as->vCurLoc.y = pWorldTbldat->vStart1Loc.y;
	//	as->vCurLoc.z = pWorldTbldat->vStart1Loc.z;
	//	as->wOpCode = GU_AVATAR_WORLD_INFO;
	//	packetQ.SetPacketLen(sizeof(sGU_AVATAR_WORLD_INFO));
	//	g_pApp->Send(this->myPlayer->GetSession(), &packetQ);
	//}
}
//This Send the SendDirectPlay to the current quest
void TLQHandler::SendDirectPlayAckForTLQ()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_CHAR_DIRECT_PLAY));
	sGU_CHAR_DIRECT_PLAY* res = (sGU_CHAR_DIRECT_PLAY*)packet.GetPacketData();

	CDirectionLinkTable* pLinkTbl = app->g_pTableContainer->GetDirectionLinkTable();
	//Just to tell the client we won't play any script
	if (this->GetTlqID() == 6000)
	{
		res->bSynchronize = true;
		res->byPlayMode = 1;
		res->directTblidx = 0;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 1 Stage 1 - Spawn the Metour
	else if (this->GetTlqID() == 454544)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1026));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1026;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;
		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 2 Stage 0 - dont know
	else if (this->GetTlqID() == 95654)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1036));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1036;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 2 Stage 1 - dont know
	else if (this->GetTlqID() == 95654)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1031));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1031;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 2 Stage 2 - dont know
	else if (this->GetTlqID() == 95654)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1032));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1032;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 3 Stage 1 - dont know
	else if (this->GetTlqID() == 554)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1041));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1041;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 3 Stage 2 - dont know
	else if (this->GetTlqID() == 554)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1042));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1042;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ 3 Stage 3 - dont know
	else if (this->GetTlqID() == 554)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1044));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1044;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
	//TLQ Stage 4 - dont know
	else if (this->GetTlqID() == 554)
	{
		sDIRECTION_LINK_TBLDAT *pLinkTblData = reinterpret_cast<sDIRECTION_LINK_TBLDAT*>(pLinkTbl->FindData(1043));

		res->bSynchronize = false;
		res->byPlayMode = 1;
		res->directTblidx = 1043;
		res->hSubject = this->myPlayer->GetAvatarHandle();
		res->wOpCode = GU_CHAR_DIRECT_PLAY;

		packet.SetPacketLen(sizeof(sGU_CHAR_DIRECT_PLAY));
		g_pApp->Send(this->myPlayer->GetSession(), &packet);
	}
}
//Load Mob to player X for TLQ Y
void TLQHandler::LoadMobForTLQ(NTL_TS_T_ID QuestID, RwUInt32 avatarHandle)
{
	if (QuestID == 11600)
	{
		g_pMobManager->CreateMobByTblidx(3, avatarHandle);
	}	
}