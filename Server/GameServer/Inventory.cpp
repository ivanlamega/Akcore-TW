#include "stdafx.h"
#include "Inventory.h"
#include "GameServer.h"

PlayersInventory::PlayersInventory(int CharID)
{
	this->CharID = CharID;
	this->TotalItemsTrade = 0;
	this->ZennyToTrade = 0;
}
PlayersInventory::~PlayersInventory()
{
}
/*
  Return our sItem_Profile[] Struct
*/
sITEM_PROFILE* PlayersInventory::GetInventory()
{
	return this->aItemProfile;
}
//Return our sITEM_BRIEF struch
sITEM_BRIEF* PlayersInventory::GetEquippedItems()
{
	return this->sItemBrief;
}
//Returns our Trade items Vector
std::vector<sITEM_DATA> PlayersInventory::GetItemsToTrade()
{
	return this->vItemsToTrade;
}
//Returns how many zenny you will give to trade
DWORD PlayersInventory::GetZennyToTrade()
{
	return this->ZennyToTrade;
}
//Returns the status of Bag
int PlayersInventory::GetBagStatus()
{
	return this->myBagStatus;
}
//Return total Items to trade
BYTE PlayersInventory::GetTotalItemsTrade()
{
	return this->TotalItemsTrade;
}
//Returns Total items count
BYTE PlayersInventory::GetTotalItemsCount()
{
	return this->ItemsCount;
}
//Set Status Bag
void PlayersInventory::SetBagStatus(int bagStatus)
{
	this->myBagStatus = bagStatus;
}
//Sets how many zenny is to trade
void PlayersInventory::SetAmountZennyToTrade(DWORD dwAmount)
{
	this->ZennyToTrade = dwAmount;
}
/*
  Load Inventory from any Character and set in aItemProfile
*/
void PlayersInventory::LoadCharInventory()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	size_t i = 0;
	int iEquipement = 1;
	this->aItemProfile[0].handle = INVALID_HOBJECT;
	this->aItemProfile[0].tblidx = INVALID_HOBJECT;
	this->aItemProfile[0].byPlace = 0;
	this->aItemProfile[0].byPos = 0;
	this->aItemProfile[0].byStackcount = 0;
	this->aItemProfile[0].byRank = 0;
	this->aItemProfile[0].byGrade = 0;
	this->aItemProfile[0].byCurDur = 0;
	
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	db->prepare("SELECT * FROM items WHERE owner_id = ? ORDER BY place ASC");
	db->setInt(1, this->CharID);
	db->execute();
	while (db->fetch())
	{
		this->aItemProfile[i].handle = db->getInt("id");		
		this->aItemProfile[i].tblidx = db->getInt("tblidx");
		this->aItemProfile[i].byPlace = db->getInt("place");
		this->aItemProfile[i].byPos = db->getInt("pos");
		this->aItemProfile[i].byStackcount = db->getInt("count");
		this->aItemProfile[i].byRank = db->getInt("rank");
		this->aItemProfile[i].byGrade = db->getInt("grade");
		this->aItemProfile[i].byCurDur = db->getInt("durability");		
		i++;
	}
	//#Issue 1 Fix - Luiz45 OBS:There is no other way to re use the query results from above?
	for (int p = 0; p < NTL_MAX_EQUIP_ITEM_SLOT; p++)
	{
		db->prepare("select * from items WHERE place=7 AND pos=? AND owner_id=?");
		db->setInt(1, p);
		db->setInt(2, this->CharID);
		db->execute();
		db->fetch();
		if (db->rowsCount() == 0)
		{
			this->sItemBrief[p].tblidx = INVALID_TBLIDX;
		}
		else
		{

			this->sItemBrief[p].tblidx = db->getInt("tblidx");
		}
	}
	this->ItemsCount = i;
}
/*
  This method will add our Item to a Vector and will increment the counter and the how many items was added to trade
  Luiz45
*/
void PlayersInventory::AddItemToTrade(sITEM_DATA sItemTrade)
{
	/*for (int i = 0; i < 13; i++)
	{
		if (this->vItemsToTrade.empty() == false)
			continue;
		else
		{
			this->vItemsToTrade.push_back(sItemTrade);
			this->TotalItemsTrade++;
		}
	}*/
}
/*
 This method will remove or Update our Vector searching by Item handle
 Luiz45
*/
void PlayersInventory::RemoveUpdateTrade(int itemHandle, bool bRemove, int iStackCount)
{
	/*for (int i = 0; i < 13; i++)
	{
		if (this->vItemsToTrade.empty() == false)
		{
			if ((i < this->vItemsToTrade.size()) && (this->vItemsToTrade[i].itemId == itemHandle))
			{
				if (bRemove)
				{
					this->vItemsToTrade.pop_back();			
					this->TotalItemsTrade--;
				}
				else
				{
					this->vItemsToTrade[i].byStackcount = iStackCount;
				}
			}
			else
				break;
		}
	}*/
}
/*
 This methods add a item to our list needed for trade
 Luiz45
*/
void PlayersInventory::AddItemToInventory(sITEM_DATA sItem)
{
	if (this->GetTotalItemsCount() < NTL_MAX_COUNT_USER_HAVE_INVEN_ITEM)
	{
		for (int i = 0; i <= this->GetTotalItemsCount(); i++)
		{
			if (i == this->GetTotalItemsCount())
			{
				this->aItemProfile[i].bNeedToIdentify = false;
				this->aItemProfile[i].handle = sItem.itemId;
				this->aItemProfile[i].tblidx = sItem.itemNo;
				this->aItemProfile[i].byStackcount = sItem.byStackcount;
				this->aItemProfile[i].byRank = sItem.byRank;
				this->aItemProfile[i].byCurDur = sItem.byRank;
				this->aItemProfile[i].byGrade = sItem.byGrade;
				this->aItemProfile[i].byPlace = sItem.byPlace;
				this->aItemProfile[i].byPos = sItem.byPosition;
			}
		}
		this->ItemsCount++;
	}	
}
/*
 This methods Remove a item to our list needed for trade
 Luiz45
*/
void PlayersInventory::RemoveItemFromInventory(int itemHandle)
{
	for (int i = 0; i < this->GetTotalItemsCount(); i++)
	{
		if (this->aItemProfile[i].handle == itemHandle)
		{
			this->aItemProfile[i].bNeedToIdentify = false;
			this->aItemProfile[i].handle = INVALID_TBLIDX;
			this->aItemProfile[i].tblidx = INVALID_TBLIDX;
			this->aItemProfile[i].byStackcount = INVALID_TBLIDX;
			this->aItemProfile[i].byRank = INVALID_TBLIDX;
			this->aItemProfile[i].byCurDur = INVALID_TBLIDX;
			this->aItemProfile[i].byGrade = INVALID_TBLIDX;
			this->aItemProfile[i].byPlace = INVALID_TBLIDX;
			this->aItemProfile[i].byPos = INVALID_TBLIDX;
		}
	}
}
/*
 This method need return only the bag available to use
 if the user have the bag then return only
 Luiz45
*/
int PlayersInventory::CheckAvailableBag()
{
	int iAvaibleBag = 0;
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	for (int i = 0; i < sizeof(this->GetInventory()); i++)
	{
		if (this->GetInventory()[i].byPlace == CONTAINER_TYPE_BAGSLOT)
		{
			sITEM_TBLDAT* pItemTbl = reinterpret_cast<sITEM_TBLDAT*>(app->g_pTableContainer->GetItemTable()->FindData(this->GetInventory()[i].tblidx));
			//Main Bag
			if (this->GetInventory()[i].byPos == (CONTAINER_TYPE_BAG1-1))
			{
				if (this->CheckAvailableSlot(CONTAINER_TYPE_BAG1) >= pItemTbl->byBag_Size)
					continue;
				else
					iAvaibleBag = CONTAINER_TYPE_BAG1;
			}
			//Secondary Bag
			else if (this->GetInventory()[i].byPos == (CONTAINER_TYPE_BAG2-1))
			{
				if (this->CheckAvailableSlot(CONTAINER_TYPE_BAG2) >= pItemTbl->byBag_Size)
					continue;
				else
					iAvaibleBag = CONTAINER_TYPE_BAG2;
			}
			//Third Bag
			else if (this->GetInventory()[i].byPos == (CONTAINER_TYPE_BAG3-1))
			{
				if (this->CheckAvailableSlot(CONTAINER_TYPE_BAG3) >= pItemTbl->byBag_Size)
					continue;
				else
					iAvaibleBag = CONTAINER_TYPE_BAG3;
			}
			//Fourth Bag
			else if (this->GetInventory()[i].byPos == (CONTAINER_TYPE_BAG4-1))
			{
				if (this->CheckAvailableSlot(CONTAINER_TYPE_BAG4) >= pItemTbl->byBag_Size)
					continue;
				else
					iAvaibleBag = CONTAINER_TYPE_BAG4;
			}
			//Five Bag
			else if (this->GetInventory()[i].byPos == (CONTAINER_TYPE_BAG5-1))
			{
				if (this->CheckAvailableSlot(CONTAINER_TYPE_BAG5) >= pItemTbl->byBag_Size)
					continue;
				else
					iAvaibleBag = CONTAINER_TYPE_BAG5;
			}
		}		
	}

	return (iAvaibleBag==0?99:iAvaibleBag);
}
/*
 This method receive the Bag container to check if have any slot available
 he will check the slots stored in our struct sItemBrief...
*/
int PlayersInventory::CheckAvailableSlot(int iBagToCheck)
{
	int iAvaibleSlot = 0;
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	int arInventSlot[NTL_MAX_BAG_ITEM_SLOT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int iOccupedSlot = 0;
	int iMaxBagSz = 0;
	for (int i = 0; i < this->GetTotalItemsCount(); i++)
	{
		if (this->GetInventory()[i].byPlace == CONTAINER_TYPE_BAGSLOT)
		{
			//Its because we have 5 bags and in the game he identify by CONTAINER TYPE BAGSLOT
			if (this->GetInventory()[i].byPos == (iBagToCheck - 1))
			{
				sITEM_TBLDAT* pItemTbl = reinterpret_cast<sITEM_TBLDAT*>(app->g_pTableContainer->GetItemTable()->FindData(this->GetInventory()[i].tblidx));
				iMaxBagSz = pItemTbl->byBag_Size;
			}			
		}
		else if ((this->GetInventory()[i].byPlace) == iBagToCheck)
		{
			
			if (this->GetInventory()[i].handle != 0)
			{
				iOccupedSlot = this->GetInventory()[i].byPos;
				arInventSlot[iOccupedSlot] = 1;
			}			
		}							
	}
	for (int p = 0; p < NTL_MAX_BAG_ITEM_SLOT; p++)
	{
		if (arInventSlot[p] == 0)
			return p;
		else if ((NTL_MAX_BAG_ITEM_SLOT - 1) == (p))
			return iMaxBagSz;
	}
}