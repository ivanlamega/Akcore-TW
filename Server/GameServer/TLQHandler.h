#pragma once

#include "SharedType.h"
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <windows.h>

#ifndef TLQ_HANDLER_CLASS_H
#define TLQ_HANDLER_CLASS_H

class CClientSession;
class CGameServer;
class CNtlPacket;

class TLQHandler
{
	///-------------Constructor & Destructor-------------///
public:
	TLQHandler();
	~TLQHandler();
	///--------------------------------------------------///
	
private:
	//Members
	NTL_TS_T_ID questID;
	NTL_TS_T_ID GetTlqID();
	PlayersMain* myPlayer;
public:
	//Functions	
	void SetTLQAccessForPlayer(NTL_TS_T_ID QuestID, PlayersMain* cPlayersMain);
	void LoadMobForTLQ(NTL_TS_T_ID QuestID, RwUInt32 avatarHandle);
	void SendDirectPlayAckForTLQ();
};

#endif