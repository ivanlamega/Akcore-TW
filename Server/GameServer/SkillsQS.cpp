#include "stdafx.h"
#include "SkillsQS.h"
#include "Character.h"
#include "GameServer.h"

PlayersSkills::PlayersSkills(int CharID)
{
	this->CharID = CharID;
}

PlayersSkills::~PlayersSkills()
{
}
//Returns Skills Profile
sSKILL_INFO* PlayersSkills::GetSkills()
{
	return this->aSkillInfo;
}
//Returns Buff Profile
sBUFF_INFO* PlayersSkills::GetSkillBuff()
{
	return this->aBuffInfo;
}
//Returns HTB Skill Profile
sHTB_SKILL_INFO* PlayersSkills::GetHTBSkills()
{
	return this->aHTBSkillnfo;
}
//Returns QuickSlot Profile
sQUICK_SLOT_PROFILE* PlayersSkills::GetQuickSlot()
{
	return this->asQuickSlotData;
}
//Return the Counter of Skills
BYTE PlayersSkills::GetSkillCount()
{
	return this->SkillCounter;
}
//Returns the Counter of Buffs Applied to this Char
BYTE PlayersSkills::GetSkillBuffCount()
{
	return this->SkillBuffCounter;
}
//Return the Counter of HTB Skills
BYTE PlayersSkills::GetHTBSkillCount()
{
	return this->HTBSkillCounter;
}
//Return the Counter of QuickSlot
BYTE PlayersSkills::GetQuickSlotCount()
{
	return this->QuickSlotCounter;
}
//Load Skills from specified char
void PlayersSkills::LoadSkills()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	size_t i = 0;

	db->prepare("SELECT * FROM skills WHERE owner_id = ? ORDER BY SlotID ASC");
	db->setInt(1, this->CharID);
	db->execute();

	this->SkillCounter = db->rowsCount();
	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	while (db->fetch())
	{
		sSKILL_TBLDAT* pSkillData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(db->getInt("skill_id")));
		this->aSkillInfo[i].bIsRpBonusAuto = db->getBoolean("RpBonusAuto");
		this->aSkillInfo[i].byRpBonusType = db->getInt("RpBonusType");
		this->aSkillInfo[i].bySlotId = db->getInt("SlotID");
		this->aSkillInfo[i].dwTimeRemaining = db->getInt("TimeRemaining");
		this->aSkillInfo[i].nExp = db->getInt("Exp");
		this->aSkillInfo[i].tblidx = db->getInt("skill_id");
		i++;
	}
}
//Load All Buffs Applied to this Char
void PlayersSkills::LoadSkillBuff()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	int iCountBuff = 0;

	db->prepare("SELECT * FROM skills WHERE owner_id = ?");
	db->setInt(1, this->CharID);
	db->execute();
	while (db->fetch())
	{
		int SkillId = db->getInt("skill_id");
		int iTimeRemaining = db->getInt("TimeRemaining");
		sSKILL_TBLDAT * pSkillBuffData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(SkillId));
		if (iCountBuff == ((NTL_MAX_BLESS_BUFF_CHARACTER_HAS + NTL_MAX_CURSE_BUFF_CHARACTER_HAS) - 1))
			break;
		if ((pSkillBuffData->byBuff_Group != INVALID_BUFF_GROUP) && (iTimeRemaining != 0))
		{
			this->aBuffInfo[iCountBuff].bySourceType = DBO_OBJECT_SOURCE_SKILL;//Need Check
			this->aBuffInfo[iCountBuff].dwInitialDuration = pSkillBuffData->dwKeepTimeInMilliSecs;
			this->aBuffInfo[iCountBuff].dwTimeRemaining = (iTimeRemaining * 1000);//to MilliSeconds
			this->aBuffInfo[iCountBuff].afEffectValue[0] = pSkillBuffData->fSkill_Effect_Value[0];
			this->aBuffInfo[iCountBuff].afEffectValue[1] = pSkillBuffData->fSkill_Effect_Value[1];
			this->aBuffInfo[iCountBuff].sourceTblidx = pSkillBuffData->tblidx;
			iCountBuff++;
		}
	}
	this->SkillBuffCounter = iCountBuff;
}
//Load All Htb Info from specified char
void PlayersSkills::LoadHTB()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	size_t i = 0;

	db->prepare("SELECT * FROM skills WHERE owner_id = ? ORDER BY SlotID ASC");
	db->setInt(1, this->CharID);
	db->execute();

	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	while (db->fetch())
	{
		//Added because Shenron Buffs and for help to detect type of skills
		//Note Shenron Buffs does not take ANY SLOTID
		sSKILL_TBLDAT* pSkillData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(db->getInt("skill_id")));
		if (pSkillData->bySkill_Class == NTL_SKILL_CLASS_HTB)
		{
			this->aHTBSkillnfo[i].bySlotId = db->getInt("SlotID");
			this->aHTBSkillnfo[i].dwTimeRemaining = db->getInt("TimeRemaining");
			this->aHTBSkillnfo[i].skillId = pSkillData->tblidx;
			i++;
		}
	}
	this->HTBSkillCounter = i;
}
//Load All QuickSlot
void PlayersSkills::LoadQuickSlot()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	MySQLConnWrapper *db2 = new MySQLConnWrapper;
	db2->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db2->connect();
	db2->switchDb(app->GetConfigFileDatabase());

	db->prepare("SELECT * FROM quickslot WHERE charId = ?");
	db->setInt(1, this->CharID);
	db->execute();
	db->fetch();

	CSkillTable * pSkillTable = app->g_pTableContainer->GetSkillTable();
	CItemTable * pItemTable = app->g_pTableContainer->GetItemTable();
	CUseItemTable * pItemUseTable = app->g_pTableContainer->GetUseItemTable();

	int i = 0;
	int SkillOrItem = 0;
	int slotID = 0;
	std::string query;

	while (i < 48)
	{
//Handles specific versions of visual studio
#if _MSC_VER <= 1600
		query = "slotId_" + std::to_string((double long)i);//To Who Use VS2010
#elif _MSC_VER == 1800
		query = "slotId_" + std::to_string(i);//To Who use VS2013
#endif

		SkillOrItem = db->getInt(query.c_str());
		this->asQuickSlotData[slotID].bySlot = 255;
		this->asQuickSlotData[slotID].tblidx = 0;
		sSKILL_TBLDAT* pSkillData = reinterpret_cast<sSKILL_TBLDAT*>(pSkillTable->FindData(SkillOrItem));
		sITEM_TBLDAT * pItemData = reinterpret_cast<sITEM_TBLDAT*>(pItemTable->FindData(SkillOrItem));
		if (pSkillData)
		{
			this->asQuickSlotData[slotID].bySlot = i;
			this->asQuickSlotData[slotID].tblidx = pSkillData->tblidx;
			this->asQuickSlotData[slotID].byType = (pSkillData->bySkill_Class == NTL_SKILL_CLASS_HTB ? QUICK_SLOT_TYPE_HTB_SKILL : QUICK_SLOT_TYPE_SKILL);
			slotID++;
		}
		else if (pItemData)
		{
			sUSE_ITEM_TBLDAT * pUseItemData = reinterpret_cast<sUSE_ITEM_TBLDAT*>(pItemUseTable->FindData(pItemData->Use_Item_Tblidx));
			if (pUseItemData)
			{
				MySQLConnWrapper *db2 = new MySQLConnWrapper;
				db2->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
				db2->connect();
				db2->switchDb(app->GetConfigFileDatabase());
				db2->prepare("SELECT id FROM items WHERE tblidx = ? AND owner_id = ?");
				db2->setInt(1, SkillOrItem);
				db2->setInt(2, this->CharID);
				db2->execute();
				db2->fetch();
				this->asQuickSlotData[slotID].bySlot = i;
				this->asQuickSlotData[slotID].tblidx = pItemData->tblidx;
				this->asQuickSlotData[slotID].hItem = db2->getInt("id");
				this->asQuickSlotData[slotID].byType = QUICK_SLOT_TYPE_ITEM;
				slotID++;
				delete db2;
			}
			else
			{
				this->asQuickSlotData[slotID].bySlot = i;
				this->asQuickSlotData[slotID].tblidx = pItemData->tblidx;
				this->asQuickSlotData[slotID].byType = QUICK_SLOT_TYPE_SOCIALACTION;
				slotID++;
			}
		}
		i++;
	}
	this->QuickSlotCounter = slotID;
}