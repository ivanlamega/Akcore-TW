#include "stdafx.h"
#include "Attributes.h"
#include "GameServer.h"

PlayerAttributes::PlayerAttributes(HSESSION PlayerSession)
{
	this->PlayerSession = PlayerSession;
}

PlayerAttributes::~PlayerAttributes()
{
}
/**
 *  Load Attributes From DB this will set all sPlayerAttribute with Database Values
 *  To Return this values you need use getAvatarAttribute();
*/
void PlayerAttributes::LoadAttributesFromDB(int charID)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	db->prepare("SELECT * FROM characters WHERE CharID = ?");
	db->setInt(1, charID);
	db->execute();
	db->fetch();
	int level = db->getInt("Level");
	sPC_TBLDAT *pTblData;
	//Load All Attributes One time only - Luiz  IN ORDER --Kalisto
	//STR 
	this->sPlayerAttribute.byBaseStr = db->getInt("BaseStr") + db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastStr = db->getInt("LastStr") + db->getInt("Level")*0.4;
	//Constitucion
	this->sPlayerAttribute.byBaseCon = db->getInt("BaseCon") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastCon = db->getInt("LAstCon") + db->getInt("Level")*0.4;
	//Focus
	this->sPlayerAttribute.byBaseFoc = db->getInt("BaseFoc") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastFoc = db->getInt("LastFoc") + db->getInt("Level")*0.4;
	//Dextry
	this->sPlayerAttribute.byBaseDex = db->getInt("BaseDex") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastDex = db->getInt("LastDex") + db->getInt("Level")*0.4;
	//Soul
	this->sPlayerAttribute.byBaseSol = db->getInt("BaseSol") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastSol = db->getInt("LastSol") + db->getInt("Level")*0.4;
	//Energy
	this->sPlayerAttribute.byBaseEng = db->getInt("BaseEng") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastEng = db->getInt("LastEng") + db->getInt("Level")*0.4;	
	//EP/LP
	this->sPlayerAttribute.wBaseMaxLP = db->getInt("BaseMaxLP");
	this->sPlayerAttribute.wLastMaxLP = db->getInt("LastMaxLP");
	this->sPlayerAttribute.wBaseMaxEP = db->getInt("BaseMaxEP");
	this->sPlayerAttribute.wLastMaxEP = db->getInt("LastMaxEP");
	//Physical Atack
	this->sPlayerAttribute.wBasePhysicalOffence = db->getInt("BasePhysicalOffence") * db->getInt("Level")*1.5;
	this->sPlayerAttribute.wLastPhysicalOffence = db->getInt("LastPhysicalOffence") * db->getInt("Level")*0.10;
	//Physical Defese
	this->sPlayerAttribute.wBasePhysicalDefence = db->getInt("BasePhysicalDefence") * db->getInt("Level")*1.2;
	this->sPlayerAttribute.wLastPhysicalDefence = db->getInt("LastPhysicalDefence") *db->getInt("Level")*0.2;
	//Energy Atack
	this->sPlayerAttribute.wBaseEnergyOffence = db->getInt("BaseEnergyOffence") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyOffence = db->getInt("LastEnergyOffence") * db->getInt("Level")*0.07;
	//Energy Defese
	this->sPlayerAttribute.wBaseEnergyDefence = db->getInt("BaseEnergyDefence") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyDefence = db->getInt("LastEnergyDefence") * db->getInt("Level")*0.2;
	//Hit Rate
	this->sPlayerAttribute.wBaseAttackRate = db->getInt("BaseAttackRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastAttackRate = db->getInt("LastAttackRate") * db->getInt("Level")*0.3;
	//Dogge Rate
	this->sPlayerAttribute.wBaseDodgeRate = db->getInt("BaseDodgeRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastDodgeRate = db->getInt("LastDodgeRate") * db->getInt("Level")*0.2;
	//Physical Critical Rate
	this->sPlayerAttribute.wBasePhysicalCriticalRate = db->getInt("BasePhysicalCriticalRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastPhysicalCriticalRate = db->getInt("LastPhysicalCriticalRate") * db->getInt("Level")*0.012;
	//Energy Critical Rate
	this->sPlayerAttribute.wBaseEnergyCriticalRate = db->getInt("BaseEnergyCriticalRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyCriticalRate = db->getInt("LastEnergyCriticalRate") * db->getInt("Level")*0.01;
	//RP 
	this->sPlayerAttribute.wBaseMaxRP = db->getInt("BaseMaxRP");
	this->sPlayerAttribute.wLastMaxRP = db->getInt("LastMaxRP");
	//Block Rate 
	this->sPlayerAttribute.wBaseBlockRate = db->getInt("BaseBlockRate");
	this->sPlayerAttribute.wLastBlockRate = db->getInt("LastBlockRate");
	//Run Speed
	this->sPlayerAttribute.fLastRunSpeed = (float)db->getDouble("LastRunSpeed");
	//Atack Speed
	this->sPlayerAttribute.wBaseAttackSpeedRate = (float)db->getInt("BaseAttackSpeedRate");
	this->sPlayerAttribute.wLastAttackSpeedRate = (float)db->getInt("LastAttackSpeedRate");
	//Atack Range
	this->sPlayerAttribute.fLastAttackRange = (float)db->getInt("LastAttackRange");
	this->sPlayerAttribute.fBaseAttackRange = (float)db->getInt("BaseAttackRange");
	//LP Get up Reg
	this->sPlayerAttribute.wBaseLpRegen = 20;
	this->sPlayerAttribute.wLastLpRegen = 20;
	//LP Sit Down Reg
	this->sPlayerAttribute.wBaseLpSitdownRegen = 6;
	this->sPlayerAttribute.wLastLpSitdownRegen = 7;
	//LP Reg in Batle
	this->sPlayerAttribute.wBaseLpBattleRegen = 8;
	this->sPlayerAttribute.wLastLpBattleRegen = 9;
	//EP Get UP Reg
	this->sPlayerAttribute.wBaseEpRegen = 20;
	this->sPlayerAttribute.wLastEpRegen = 20;
	//EP Sit Down Reg
	this->sPlayerAttribute.wBaseEpSitdownRegen = 12;
	this->sPlayerAttribute.wLastEpSitdownRegen = 13;
	//EP Reg in Batle
	this->sPlayerAttribute.wBaseEpBattleRegen = 14;
	this->sPlayerAttribute.wLastEpBattleRegen = 15;
	//Rp incress rate
	this->sPlayerAttribute.wBaseRpRegen = 1;
	this->sPlayerAttribute.wLastRpRegen = 1;
	//RP diminution
	this->sPlayerAttribute.wLastRpDimimutionRate = 1;
	//Curse Sucess Rate
	this->sPlayerAttribute.wBaseCurseSuccessRate = 1;
	this->sPlayerAttribute.wLastCurseSuccessRate = 1;
	//Curse Tolerance Rate
	this->sPlayerAttribute.wBaseCurseToleranceRate = 1;
	this->sPlayerAttribute.wLastCurseToleranceRate = 1;
	//Nao sei
	this->sPlayerAttribute.fCastingTimeChangePercent = 1.0f;
	this->sPlayerAttribute.fCoolTimeChangePercent = 1.0f;
	this->sPlayerAttribute.fKeepTimeChangePercent = 1.0f;
	this->sPlayerAttribute.fDotValueChangePercent = 1.0f;
	this->sPlayerAttribute.fDotTimeChangeAbsolute = 1.0f;
	this->sPlayerAttribute.fRequiredEpChangePercent = 1.0f;
	//Atribute Ofense/Defese
	this->sPlayerAttribute.fHonestOffence = 1.0f;
	this->sPlayerAttribute.fHonestDefence = 1.0f;
	this->sPlayerAttribute.fStrangeOffence = 1.0f;
	this->sPlayerAttribute.fStrangeDefence = 1.0f;
	this->sPlayerAttribute.fWildOffence = 1.0f;
	this->sPlayerAttribute.fWildDefence = 1.0f;
	this->sPlayerAttribute.fEleganceOffence = 1.0f;
	this->sPlayerAttribute.fEleganceDefence = 1.0f;
	this->sPlayerAttribute.fFunnyOffence = 1.0f;
	this->sPlayerAttribute.fFunnyDefence = 1.0f;

	this->sPlayerAttribute.wParalyzeToleranceRate = 1;
	this->sPlayerAttribute.wTerrorToleranceRate = 1;
	this->sPlayerAttribute.wConfuseToleranceRate = 1;
	this->sPlayerAttribute.wStoneToleranceRate = 1;
	this->sPlayerAttribute.wCandyToleranceRate = 1;
	this->sPlayerAttribute.fParalyzeKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fTerrorKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fConfuseKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fStoneKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fCandyKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fBleedingKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fPoisonKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fStomachacheKeepTimeDown = 1.0f;
	this->sPlayerAttribute.fCriticalBlockSuccessRate = 1.0f;
	this->sPlayerAttribute.wGuardRate = 1;
	this->sPlayerAttribute.fSkillDamageBlockModeSuccessRate = 1.0f;
	this->sPlayerAttribute.fCurseBlockModeSuccessRate = 1.0f;
	this->sPlayerAttribute.fKnockdownBlockModeSuccessRate = 1.0f;

	this->sPlayerAttribute.fHtbBlockModeSuccessRate = 1.0f; // Bleeding defense TW
	this->sPlayerAttribute.fSitDownLpRegenBonusRate = 1.0f; // posion defense tw
	this->sPlayerAttribute.fSitDownEpRegenBonusRate = 1.0f; // abdominal pain defense
	this->sPlayerAttribute.fPhysicalCriticalDamageBonusRate = 1.0f; //Burn Defense

	this->sPlayerAttribute.fEnergyCriticalDamageBonusRate = 1;
	this->sPlayerAttribute.fItemUpgradeBonusRate = 2;
	this->sPlayerAttribute.fItemUpgradeBreakBonusRate = 3;
	this->sPlayerAttribute.fBaseAirDash2Speed = 50.0f;//Dash Fly2 TW
	this->sPlayerAttribute.fLastAirDash2Speed = 50.0f;//Dash Fly2 TW
	this->sPlayerAttribute.fBaseAirDashSpeed = 30.0f;//Dash Fly TW
	this->sPlayerAttribute.fLastAirDashSpeed = 30.0f;//Dash Fly TW
	this->sPlayerAttribute.fBaseRunSpeed = 30;//Base Run TW
	this->sPlayerAttribute.fLastAirSpeed = 15;//LastAir Speed TW
	this->sPlayerAttribute.wLastMaxAp = 0x1B17A;// db->getInt("LastMaxAp");//Max AP
	this->sPlayerAttribute.wBaseMaxAp = 0x1B17A;// db->getInt("BaseMaxAp");//Base Max Ap
	this->sPlayerAttribute.wBaseApBattleRegen = 5;//Regen In Battle AP TW
	this->sPlayerAttribute.wLastApBattleRegen = 5;//LAst Regen in Battle Ap TW
	this->sPlayerAttribute.wBaseApRegen = 5;//Base Ap Regen TW
	this->sPlayerAttribute.wLastApRegen = 5;//Base While Sitting Regen AP TW
	this->sPlayerAttribute.wBaseApSitdownRegen = 5;//AP Regen TW
	this->sPlayerAttribute.wLastApSitdownRegen = 5;//AP Regen TW

	
}
//This method will return a sAVATAR_ATTRIBUTE struct
sAVATAR_ATTRIBUTE PlayerAttributes::GetAvatarAttribute()
{
	return this->sPlayerAttribute;
}
//This method will return a sAVATAR_ATTRIBUTE_LINK
//Initially the sPlayerAttributeLink is not filled, you need call UpdateAvatarAttribute(RwUint32 Handle) to Fill
sAVATAR_ATTRIBUTE_LINK PlayerAttributes::GetAvatarAttributeLink()
{
	return this->sPlayerAttributeLink;
}
/**
* As said you need send your handle and this will update all attributes and will send a Packet(GU_AVATAR_ATTRIBUTE)
*/
void PlayerAttributes::UpdateAvatarAttributes(RwUInt32 Handle)
{
	CNtlPacket packet(sizeof(sGU_AVATAR_ATTRIBUTE_UPDATE));
	sGU_AVATAR_ATTRIBUTE_UPDATE * res = (sGU_AVATAR_ATTRIBUTE_UPDATE *)packet.GetPacketData();
	PlayersMain* plr = g_pPlayerManager->GetPlayer(Handle);
	memcpy(&this->sPlayerAttribute, &plr->GetPcProfile()->avatarAttribute, sizeof(sAVATAR_ATTRIBUTE));

	CNtlBitFlagManager changedFlag;
	
	changedFlag.Create(&this->sPlayerAttribute, ATTRIBUTE_TO_UPDATE_COUNT);
	this->sPlayerAttributeLink = CNtlAvatar::GetInstance()->ConvertAttributeToAttributeLink(&this->sPlayerAttribute);

	DWORD buffer[2048];//Thanks Daneos
	DWORD datasize;

	for (BYTE byIndex = ATTRIBUTE_TO_UPDATE_FIRST; byIndex <= ATTRIBUTE_TO_UPDATE_LAST; byIndex++)
	{
		changedFlag.Set(byIndex);
	}

	if (CNtlAvatar::GetInstance()->SaveAvatarAttribute(&changedFlag, &this->sPlayerAttributeLink, &buffer, &datasize) == false)
	{
		printf("ERROR IN UPDATE ATTRIBUTE");
	}

	memcpy(res->abyFlexibleField, &buffer, ((UCHAR_MAX - 1) / 8 + 1) + sizeof(sAVATAR_ATTRIBUTE));

	res->byAttributeTotalCount = ATTRIBUTE_TO_UPDATE_LAST;
	res->hHandle = Handle;
	res->wOpCode = GU_AVATAR_ATTRIBUTE_UPDATE;

	packet.SetPacketLen(sizeof(sGU_AVATAR_ATTRIBUTE_UPDATE));
	g_pApp->Send(this->PlayerSession, &packet);
	plr = NULL;
	delete plr;
}
/*
* This function will update the Attributes by Chip Info - Need be Implemented - Luiz45
*/
void PlayerAttributes::UpdateStatsUsingScouterChips(RwUInt32 Handle, TBLIDX OptionTblidx,bool bDecrease)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CItemOptionTable* pItemOptionTable = app->g_pTableContainer->GetItemOptionTable();
	sITEM_OPTION_TBLDAT* sItemOptTbl = reinterpret_cast<sITEM_OPTION_TBLDAT*>(pItemOptionTable->FindData(OptionTblidx));
	PlayersMain* plr = g_pPlayerManager->GetPlayer(Handle);
	//Let's Check what we gonna update
	switch (sItemOptTbl->byScouterInfo)
	{
		case SCOUTER_INFO_LP:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wBaseMaxLP -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wBaseMaxLP += sItemOptTbl->nValue[0];
		}
		break;
		case SCOUTER_INFO_EP:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wBaseMaxEP -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wBaseMaxEP += sItemOptTbl->nValue[0];
		}
		break;
		case SCOUTER_INFO_ATTACK_RATE:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastAttackRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastAttackRate += sItemOptTbl->nValue[0];
		}
		break;
		case SCOUTER_INFO_ATTACK_SPEED:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastAttackSpeedRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastAttackSpeedRate += sItemOptTbl->nValue[0];
		}
		break;
		case SCOUTER_INFO_BLOCK_RATE:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastBlockRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastBlockRate += sItemOptTbl->nValue[0];
		}
		break;
		case SCOUTER_INFO_CON:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastCon -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastCon += sItemOptTbl->nValue[0];
		}
		break;		
		case SCOUTER_INFO_CURSE_SUCCESS_RATE:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastCurseSuccessRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastCurseSuccessRate += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_CURSE_TOLERANCE_RATE:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastCurseToleranceRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastCurseToleranceRate += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_DEX:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastDex -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastDex += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_DODGE_RATE:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.wLastDodgeRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.wLastDodgeRate += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_ENG:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastEng -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastEng += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_FOC:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastFoc -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastFoc += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_ITEM_BREAK_RATE_DOWN:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.fItemUpgradeBreakBonusRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.fItemUpgradeBreakBonusRate += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_SOL:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastSol -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastSol += sItemOptTbl->nValue[0];
		}
			break;
		case SCOUTER_INFO_STR:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.byLastStr -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.byLastStr += sItemOptTbl->nValue[0];			
		}
			break;
		case SCOUTER_INFO_UPGRADE_RATE_UP:
		{
			if (bDecrease)
				plr->GetPcProfile()->avatarAttribute.fItemUpgradeBonusRate -= sItemOptTbl->nValue[0];
			else
				plr->GetPcProfile()->avatarAttribute.fItemUpgradeBonusRate += sItemOptTbl->nValue[0];
		}
			break;
		default:
		{
			//Not Implemented yet
			printf("Case not implemented yet, case struct number: %i", sItemOptTbl->byScouterInfo);
		}
			break;
	}
	memcpy(&this->sPlayerAttribute, &plr->GetPcProfile()->avatarAttribute, sizeof(sAVATAR_ATTRIBUTE));
	this->UpdateAvatarAttributes(Handle);
}