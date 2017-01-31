//////////////////////////////////////////////////////////////////////
// PlayerManager.cpp: implementation of the CPlayerManager class.
// By Kalisto
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "GameServer.h"

DWORD ServerTick;
DWORD DuelTime;
CPlayerManager* g_pPlayerManager = NULL;

CPlayerManager::CPlayerManager()
{
	m_bRun = true;
}

CPlayerManager::~CPlayerManager()
{
	Release();
}

void CPlayerManager::Init()
{
	CreatePlayerThread();
}

void CPlayerManager::Release()
{

}


void CPlayerManager::Run()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	DWORD dwTickCur, dwTickOld, lpTick, mobTick;
	
	
	dwTickCur = ::timeGetTime();
	dwTickOld = ::timeGetTime();
	lpTick = ::timeGetTime();
	mobTick = ::timeGetTime();
	while (m_bRun)
	{
		if (m_map_Player.size() != 0)
		for (itterType it = m_map_Player.begin(); it != m_map_Player.end(); it++)
		{
			PlayersMain* plr = it->second;
			if ((plr) && (plr->myCCSession))
			{
				if (plr->GetPlayerDead() == false)
				{
					dwTickCur = ::timeGetTime();
					if (plr->GetPlayerFight() == true)
					{

						//printf("I'm fighting.\n");
						if (dwTickCur - dwTickOld >= 10000)
						{
							plr->SetPlayerFight(false);
							dwTickOld = dwTickCur;

						}
					}
					else if (plr->GetPlayerFight() == false)
					{
						

						if (plr->GetPcProfile()->dwCurLP <= 0)
							plr->SendThreadUpdateDeathStatus();

						if (dwTickCur - lpTick >= 2000)
						{
							if (plr->GetPcProfile()->dwCurLP < plr->GetPcProfile()->avatarAttribute.wBaseMaxLP || plr->GetPcProfile()->dwCurLP > plr->GetPcProfile()->avatarAttribute.wBaseMaxLP)
								plr->SendThreadUpdateOnlyLP();
						//	printf("Reg LP\n");
							if (plr->GetPcProfile()->wCurEP < plr->GetPcProfile()->avatarAttribute.wBaseMaxEP || plr->GetPcProfile()->wCurEP > plr->GetPcProfile()->avatarAttribute.wBaseMaxEP)
								plr->SendThreadUpdateOnlyEP();
						//	printf("Reg EP\n");
							if (plr->GetPcProfile()->dwCurAp < plr->GetPcProfile()->avatarAttribute.wBaseMaxAp || plr->GetPcProfile()->dwCurAp > plr->GetPcProfile()->avatarAttribute.wBaseMaxAp)
								plr->SendThreadUpdateOnlyAP();
						//	printf("Reg AP\n");							
							if (plr->GetPcProfile()->dwCurLP <= 30)
								plr->SendThreadUpdateEmergencyStatusTrue();
						//	printf("EmergencyStatusTrue\n");
							if (plr->GetPcProfile()->dwCurLP >= 30)
								plr->SendThreadUpdateEmergencyStatusFalse();
						//	printf("EmergencyStatusFalse\n");
							lpTick = dwTickCur;

							
						}
					}
					
					/*if ((plr->GetPcProfile()->wCurRP > 0) || plr->GetRpBallFull() > 0)
					{
						if (plr->GetPcProfile()->wCurRP <= 0)
							if (plr->GetRpBallFull() > 0)
							{
							plr->SetRpBallFull(1);
							plr->GetPcProfile()->wCurRP = (plr->GetPcProfile()->avatarAttribute.wBaseMaxRP / plr->GetRpBallCounter()) - 1;
							}
							else;
						else
							plr->GetPcProfile()->wCurRP -= 1;
						plr->SendThreadUpdateRP();
					}*/
					//if (plr->isKaioken == true) /* TEST */
					/*{
					plr->GetPcProfile()->wCurLP -= (500 * plr->GetCharState()->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
					plr->GetPcProfile()->wCurEP -= (500 * plr->GetCharState()->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
					}*/
					//plr->SendThreadUpdateEPLP();
					if (dwTickCur - mobTick >= 10)
					{
						mobTick = ::timeGetTime();
						if (plr->myCCSession)
						{
							//printf("RunSpawnCheck Fired\n");
							g_pMobManager->RunSpawnCheck(NULL, plr->GetPlayerPosition(), plr->myCCSession);
							plr->SetMob_SpawnTime(timeGetTime());
						}
					}
					if ((timeGetTime() - ServerTick) >= MINUTE)
					{

						printf("\n\r");
						printf("%d Players Online\n\r", GetTotalPlayers());
						printf("DBO> ");
						ServerTick = timeGetTime();
						if (plr->myCCSession)
						plr->SavePlayerData(app);
					}
					//if ((timeGetTime() - DuelTime) >= (MINUTE * 3) && plr->IsDueling)
					//{
					//	//GU_FREEBATTLE_END_NFY
					//	plr-.

					//}

				}
			}
		}
		Wait(1);// And no it's every second, it's only the amount regen is too high (this->pcProfile->avatarAttribute.wBaseMaxEP * 0.03) 3% every seconds it's for make some test this is not the last "release"			



	}
}

void CPlayerManager::CreatePlayerThread()
{

	pThread = CNtlThreadFactory::CreateThread(this, "CPMThread");
	pThread->Start();
	this->PlayerCounter = 0;
	ServerTick = timeGetTime();

}
//-------------------------NEW METHODS-----------------------//
//Add new player in our MAP
void CPlayerManager::AddNewPlayer(RwUInt32 playerHandle, HSESSION PlayerSession, int CharID, int AccountID)
{
	PlayersMain* tmpPlayer = new PlayersMain(PlayerSession, CharID, AccountID);
	tmpPlayer->SetAvatarHandle(playerHandle);
	this->m_map_Player.insert(std::make_pair(playerHandle,tmpPlayer));
	this->PlayerCounter++;
	tmpPlayer = NULL;
	delete tmpPlayer;
}
//Get PlayerMain By AvatarHandle
PlayersMain* CPlayerManager::GetPlayer(RwUInt32 playerHandle)
{
	PlayersMain* player;
	for (itterType it = this->m_map_Player.begin(); it != this->m_map_Player.end(); it++)
	{
		player = it->second;
		if (player)
		{
			if (player->GetAvatarHandle() == playerHandle)
			{
				return player;
			}
		}
	}
	return NULL;//If return null then you got a fucking error
}
//Get PlayerMain by CharID
PlayersMain* CPlayerManager::GetPlayerByID(RwUInt32 playerID)
{
	PlayersMain* player;
	for (itterType it = this->m_map_Player.begin(); it != this->m_map_Player.end(); it++)
	{
		player = it->second;
		if (player)
		{
			if (player->GetCharID() == playerID)
			{
				return player;
			}
		}
	}
	return NULL;//If return null then you got a fucking error
}
//Remove the player from our map
void CPlayerManager::RemovePlayer(RwUInt32 playerHandle)
{
	this->m_map_Player.erase(playerHandle);
	this->PlayerCounter--;
}
//Get Total of Player in Manager
int CPlayerManager::GetTotalPlayers()
{
	return this->PlayerCounter;
}

//Send teh movement sync packet and set all player states to standing;
void	CPlayerManager::SendSyncPacket(RwUInt32 PlayerHandle, sVECTOR3 loc, sVECTOR3 dir, BYTE byMovFlag)
{
	CNtlPacket packet2(sizeof(sGU_CHAR_AIR_MOVE_SYNC));
	sGU_CHAR_AIR_MOVE_SYNC * res = (sGU_CHAR_AIR_MOVE_SYNC*)packet2.GetPacketData();
	res->handle = PlayerHandle;
		res->wOpCode = GU_CHAR_AIR_MOVE_SYNC;
	
	packet2.SetPacketLen(sizeof(sGU_CHAR_FIGHTMODE));
	//g_pApp->(this->GetHandle(), &packet2);



}
