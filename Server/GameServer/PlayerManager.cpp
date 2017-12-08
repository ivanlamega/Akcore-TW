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
	mPlayerMutex = new CNtlMutex();
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
	
	DWORD dwTickCur, dwTickOld, lpTick, token, mobTick;
	
	
	dwTickCur = ::timeGetTime();
	dwTickOld = ::timeGetTime();
	lpTick = ::timeGetTime();
	token = ::timeGetTime();
	mobTick = ::timeGetTime();
	while (m_bRun)
	{
		mPlayerMutex->Lock();
		if (m_map_Player.size() != 0)
		for (itterType it = m_map_Player.begin(); it != m_map_Player.end(); it++)
		{
			PlayersMain* plr = it->second;
			if ((plr) && (plr->myCCSession))
			{
				if (plr->GetPlayerDead() == false)//If PlayerDead Is False 
				{
					dwTickCur = ::timeGetTime();
					if (plr->GetPlayerFight() == true) //If GetPlayerFight Is True Run Time
					{
						
						//printf("I'm fighting.\n");
						if (dwTickCur - dwTickOld >= 10000) // If Time >=10000 Define Fihgt False
						{
							plr->SetPlayerFight(false);
							dwTickOld = dwTickCur;

						}
					}
					else if (plr->GetPlayerFight() == false) // Else If Player Figth Is False Check LP
					{				
						if (plr->GetPcProfile()->dwCurLP <= 0) //If LP Is <=0 Player Is Dead
							plr->SendThreadUpdateDeathStatus(); //Update Dead Status

						if (dwTickCur - lpTick >= 2000) //Send Time for Lp/Ep/Ap Reg
						{
							if (plr->GetPcProfile()->dwCurLP < plr->GetPcProfile()->avatarAttribute.wBaseMaxLP || plr->GetPcProfile()->dwCurLP > plr->GetPcProfile()->avatarAttribute.wBaseMaxLP)
							{
								plr->SendThreadUpdateOnlyLP(); //Send Update LP						
								printf("Reg LP\n");
							}
							if (plr->GetPcProfile()->wCurEP < plr->GetPcProfile()->avatarAttribute.wBaseMaxEP || plr->GetPcProfile()->wCurEP > plr->GetPcProfile()->avatarAttribute.wBaseMaxEP)
							{
								plr->SendThreadUpdateOnlyEP(); //Sende Update EP
								printf("Reg EP\n");
							}
							if (plr->GetPcProfile()->dwCurAp < plr->GetPcProfile()->avatarAttribute.wBaseMaxAp || plr->GetPcProfile()->dwCurAp > plr->GetPcProfile()->avatarAttribute.wBaseMaxAp)
							{
								plr->SendThreadUpdateOnlyAP(); //Send Update AP
								printf("Reg AP\n");
							}
							if (plr->GetPcProfile()->dwCurLP <= 30){ //If Curlp <=30 Run Emergency Status
								plr->SendThreadUpdateEmergencyStatusTrue(); //Update Emergency Status True
								printf("EmergencyStatusTrue\n");
							}
							if (plr->GetPcProfile()->dwCurLP >= 30){//If LP >=30 Disable Emergency Status
								plr->SendThreadUpdateEmergencyStatusFalse(); //Update Emergency Status False
								//printf("EmergencyStatusFalse\n");
							}
							lpTick = dwTickCur;

							
						}
					}
					//Update Token Point
					if (dwTickCur - token >= 120000)//Send Time For Next Token
					{
						plr->SendThreadUpdateTokenPoint();//Update Token Point
						token = dwTickCur;
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
					if (dwTickCur - mobTick >= 100)//Send Time For Check Mob 
					{						
						g_pMobManager->RunSpawnCheck(NULL, plr->GetPlayerPosition(), plr->myCCSession);
						plr->SetMob_SpawnTime(timeGetTime());						
						
					}
					
					if ((timeGetTime() - ServerTick) >= MINUTE && plr)
					{
						if (plr->myCCSession)
							plr->SavePlayerData(app);

						printf("\n\r");
						printf("%d Players Online\n\r", GetTotalPlayers());
						printf("DBO> ");
						ServerTick = timeGetTime();

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
		mPlayerMutex->Unlock();


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
	mPlayerMutex->Lock();
	printf("Someone Has Left The Game");
	this->m_map_Player.erase(playerHandle);
	this->PlayerCounter--;
	mPlayerMutex->Unlock();
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
