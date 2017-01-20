#include "stdafx.h"
#include <list>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Character.h"

#ifndef PLAYERS_INVENTORY_H
#define PLAYERS_INVENTORY_H

class PlayersInventory{
	//----------------Constructor & Destructor --------------//
	public:
		PlayersInventory(int CharID);
		~PlayersInventory();
	//Members & Functions
	private:
		int CharID;
		DWORD ZennyToTrade;
		BYTE ItemsCount;
		BYTE TotalItemsTrade;
		int myBagStatus;
		std::vector<sITEM_DATA> vItemsToTrade;
		sITEM_PROFILE aItemProfile[NTL_MAX_COUNT_USER_HAVE_INVEN_ITEM];
		sITEM_BRIEF sItemBrief[EQUIP_SLOT_TYPE_COUNT];
	//Public Functions
	public:
		void LoadCharInventory();
		void AddItemToTrade(sITEM_DATA sItemTrade);
		void AddItemToInventory(sITEM_DATA sItemAdd);
		void SetBagStatus(int statusCode);
		void RemoveItemFromInventory(int itemHandle);
		void SetAmountZennyToTrade(DWORD dwAmount);
		void RemoveUpdateTrade(int itemHandle, bool bRemove, int iStackCount = 0);
		int CheckAvailableSlot(int iBagToCheck);
		int CheckAvailableBag();
		BYTE GetTotalItemsCount();
		BYTE GetTotalItemsTrade();
		int GetBagStatus();
		DWORD GetZennyToTrade();
		std::vector<sITEM_DATA> GetItemsToTrade();
		sITEM_PROFILE* GetInventory();
		sITEM_BRIEF* GetEquippedItems();
};

#endif