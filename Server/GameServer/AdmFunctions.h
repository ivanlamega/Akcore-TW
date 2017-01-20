#pragma once

#include "SharedType.h"
#include "MobTable.h"
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <windows.h>

#ifndef ADM_FUNCTIONS_CLASS_H
#define ADM_FUNCTIONS_CLASS_H


class CGameServer;
class CClientSession;
class CNtlPacket;

class AdmFunctions
{
public:
	AdmFunctions(){};
	~AdmFunctions(){};

public:
	//MOBS
	void						CreateMonsterById(unsigned int uiMobId, CNtlPacket * pPacket, CClientSession * pSession);
	//ITEMS

	//MISC
};

#endif