#pragma once

#include "SharedType.h"
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <windows.h>

#ifndef CHAT_FUNCTIONS_CLASS_H
#define CHAT_FUNCTIONS_CLASS_H


class CChatServer;
class CClientSession;
class CNtlPacket;

class ChatFunctionsClass
{
public:
	ChatFunctionsClass(){};
	~ChatFunctionsClass(){};

public:
	void						AddRemoveFriend(int CharID, int TargetID, bool bRemove = false,bool toBlackList = false);
	void						AddRemoveBlackList(int CharID, int TargetID, bool bRemove = false);
};

#endif