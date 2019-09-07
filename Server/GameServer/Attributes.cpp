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
	dbo_data_table_pc *pc = new dbo_data_table_pc();
	pc->load("data/table_pc_data.rdf");
	const dbo_data_table_pc_st *pcDat = pc->pc_data_get(db->getInt("Race"), db->getInt("Class"), db->getInt("Gender"));
	//Load All Attributes One time only - Luiz  IN ORDER --Kalisto
	//STR 
	this->sPlayerAttribute.byBaseStr = pcDat->table_char.str;// db->getInt("BaseStr") + db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastStr = 14 * 1 + db->getInt("Level")*0.4;
	//Constitucion
	this->sPlayerAttribute.byBaseCon = pcDat->table_char.con;// db->getInt("BaseCon") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastCon = 9 * 1 + db->getInt("Level")*0.4;
	//Focus
	this->sPlayerAttribute.byBaseFoc = pcDat->table_char.foc;// db->getInt("BaseFoc") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastFoc = 9 * 1 + db->getInt("Level")*0.4;
	//Dextry
	this->sPlayerAttribute.byBaseDex = pcDat->table_char.dex;// db->getInt("BaseDex") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastDex = 14 * 1 + db->getInt("Level")*0.4;
	//Soul
	this->sPlayerAttribute.byBaseSol = pcDat->table_char.sol;// *db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastSol = 9 * 1 + db->getInt("Level")*0.4;
	//Energy
	this->sPlayerAttribute.byBaseEng = pcDat->table_char.eng;// db->getInt("BaseEng") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.byLastEng = 7 * 1 + db->getInt("Level")*0.4;	
	//EP/LP
	this->sPlayerAttribute.wBaseMaxLP = 600 + (pcDat->table_char.str * 10) + (db->getInt("LastMaxLP") * 0.2);
	this->sPlayerAttribute.wLastMaxLP = 600 + (pcDat->table_char.str * 10) + (db->getInt("LastMaxLP") * 0.2);
	this->sPlayerAttribute.wBaseMaxEP = 600 + (pcDat->table_char.eng * 10) + (db->getInt("LastMaxEP") * 0.2);
	this->sPlayerAttribute.wLastMaxEP = 600 + (pcDat->table_char.eng * 10) + (db->getInt("LastMaxEP") * 0.2);
	//Physical Atack
	this->sPlayerAttribute.wBasePhysicalOffence = pcDat->basic_physical_offence;// db->getInt("BasePhysicalOffence") * db->getInt("Level")*1.5;
	this->sPlayerAttribute.wLastPhysicalOffence = 28 + this->sPlayerAttribute.byLastStr * db->getInt("Level")*0.10;
	//Physical Defese
	this->sPlayerAttribute.wBasePhysicalDefence = pcDat->table_char.basic_physical_defence;// db->getInt("BasePhysicalDefence") * db->getInt("Level")*1.2;
	this->sPlayerAttribute.wLastPhysicalDefence = 10 * db->getInt("Level")*0.2;
	//Energy Atack
	this->sPlayerAttribute.wBaseEnergyOffence = pcDat->basic_energy_offence;// db->getInt("BaseEnergyOffence") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyOffence = 15 + this->sPlayerAttribute.byLastSol * db->getInt("Level")*0.07;
	//Energy Defese
	this->sPlayerAttribute.wBaseEnergyDefence = pcDat->table_char.basic_energy_defence;// db->getInt("BaseEnergyDefence") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyDefence = 10 * db->getInt("Level")*0.2;
	//Hit Rate
	this->sPlayerAttribute.wBaseAttackRate = pcDat->table_char.attack_rate;// db->getInt("BaseAttackRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastAttackRate = 70 + this->sPlayerAttribute.byLastFoc * db->getInt("Level")*0.3;
	//Dogge Rate
	this->sPlayerAttribute.wBaseDodgeRate = pcDat->table_char.dodge_rate;// db->getInt("BaseDodgeRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastDodgeRate = 90 + this->sPlayerAttribute.byLastDex * db->getInt("Level")*0.2;
	//Physical Critical Rate
	this->sPlayerAttribute.wBasePhysicalCriticalRate = 0;// db->getInt("BasePhysicalCriticalRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastPhysicalCriticalRate = 7 + this->sPlayerAttribute.byLastDex * db->getInt("Level")*0.012;
	//Energy Critical Rate
	this->sPlayerAttribute.wBaseEnergyCriticalRate = 0;// db->getInt("BaseEnergyCriticalRate") * db->getInt("Level")*0.2;
	this->sPlayerAttribute.wLastEnergyCriticalRate = 6 + this->sPlayerAttribute.byLastFoc * db->getInt("Level")*0.01;
	//RP 
	this->sPlayerAttribute.wBaseMaxRP = 0;// db->getInt("BaseMaxRP");
	this->sPlayerAttribute.wLastMaxRP = 0;// db->getInt("LastMaxRP");
	//Block Rate 
	this->sPlayerAttribute.wBaseBlockRate = pcDat->table_char.block_rate;//db->getInt("BaseBlockRate");
	this->sPlayerAttribute.wLastBlockRate = 0;// db->getInt("LastBlockRate");
	//Run Speed
	this->sPlayerAttribute.fLastRunSpeed = (float)pcDat->child_run_speed;// (float)db->getDouble("LastRunSpeed");
	//Atack Speed
	this->sPlayerAttribute.wBaseAttackSpeedRate = 0;// (float)db->getInt("BaseAttackSpeedRate");
	this->sPlayerAttribute.wLastAttackSpeedRate = 0;//(float)db->getInt("LastAttackSpeedRate");
	//Atack Range
	this->sPlayerAttribute.fBaseAttackRange = pcDat->table_char.attack_range / 2;// (float)db->getInt("BaseAttackRange");
	this->sPlayerAttribute.fLastAttackRange = pcDat->table_char.attack_range / 2;// (float)db->getInt("LastAttackRange");
	//nao sei....
	this->sPlayerAttribute.unknown_int16[0] = 0;
	this->sPlayerAttribute.unknown_int16[1] = 0;
	this->sPlayerAttribute.unknown_int16[2] = 0;
	this->sPlayerAttribute.unknown_int16[3] = 0;
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
	this->sPlayerAttribute.wBaseRpRegen = 0;
	this->sPlayerAttribute.wLastRpRegen = 0;
	//RP diminution
	this->sPlayerAttribute.wLastRpDimimutionRate = 0;
	//Curse Sucess Rate
	this->sPlayerAttribute.wBaseCurseSuccessRate = pcDat->table_char.curse_success_rate;
	this->sPlayerAttribute.wLastCurseSuccessRate = 0;
	//Curse Tolerance Rate
	this->sPlayerAttribute.wBaseCurseToleranceRate = pcDat->table_char.curse_tolerance_rate;
	this->sPlayerAttribute.wLastCurseToleranceRate = 0;
	//Nao sei
	this->sPlayerAttribute.fCastingTimeChangePercent = 0;
	this->sPlayerAttribute.fCoolTimeChangePercent = 0;
	this->sPlayerAttribute.fKeepTimeChangePercent = 0;
	this->sPlayerAttribute.fDotValueChangePercent = 0;
	this->sPlayerAttribute.fDotTimeChangeAbsolute = 0;
	this->sPlayerAttribute.fRequiredEpChangePercent = 0;
	//Atribute Ofense/Defese
	this->sPlayerAttribute.fHonestOffence = 0;//nao
	this->sPlayerAttribute.fHonestDefence = 0;//nao
	this->sPlayerAttribute.fStrangeOffence = 0;//nao
	this->sPlayerAttribute.fStrangeDefence = 0;//nao
	this->sPlayerAttribute.fWildOffence = 0;//nao
	this->sPlayerAttribute.fWildDefence = 0;//nao
	this->sPlayerAttribute.fEleganceOffence = 0;//nao
	this->sPlayerAttribute.fEleganceDefence = 0;//nao
	this->sPlayerAttribute.fFunnyOffence = 0;//nao
	this->sPlayerAttribute.fFunnyDefence = 0;//nao

	this->sPlayerAttribute.wParalyzeToleranceRate = 0;//nao
	this->sPlayerAttribute.wTerrorToleranceRate = 0;//nao
	this->sPlayerAttribute.wConfuseToleranceRate = 0;//nao
	this->sPlayerAttribute.wStoneToleranceRate = 0;//nao
	this->sPlayerAttribute.wCandyToleranceRate = 0;//nao
	this->sPlayerAttribute.fParalyzeKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fTerrorKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fConfuseKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fStoneKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fCandyKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fBleedingKeepTimeDown = 0;//nao
	this->sPlayerAttribute.fPoisonKeepTimeDown = 0;
	this->sPlayerAttribute.fStomachacheKeepTimeDown = 0;
	this->sPlayerAttribute.fCriticalBlockSuccessRate = 0;
	this->sPlayerAttribute.wGuardRate = 0;
	this->sPlayerAttribute.fSkillDamageBlockModeSuccessRate = 0;
	this->sPlayerAttribute.fCurseBlockModeSuccessRate = 0;
	this->sPlayerAttribute.fKnockdownBlockModeSuccessRate = 0;

	this->sPlayerAttribute.fHtbBlockModeSuccessRate = 0; // Bleeding defense TW
	this->sPlayerAttribute.fSitDownLpRegenBonusRate = 0; // posion defense tw
	this->sPlayerAttribute.fSitDownEpRegenBonusRate = 0; // abdominal pain defense
	this->sPlayerAttribute.fPhysicalCriticalDamageBonusRate = 0;//Burn Defense

	this->sPlayerAttribute.fEnergyCriticalDamageBonusRate = 1;
	this->sPlayerAttribute.fItemUpgradeBonusRate = 2;
	this->sPlayerAttribute.fItemUpgradeBreakBonusRate = 3;
	this->sPlayerAttribute.fBaseAirDash2Speed = 30.0f;//Dash Fly2 TW
	this->sPlayerAttribute.fLastAirDash2Speed = 30.0f;//Dash Fly2 TW
	this->sPlayerAttribute.fBaseAirDashSpeed = 20.0f;//Dash Fly TW
	this->sPlayerAttribute.fLastAirDashSpeed = 20.0f;//Dash Fly TW
	this->sPlayerAttribute.fBaseRunSpeed = (float)pcDat->child_run_speed_origin;//Base Run TW
	this->sPlayerAttribute.fLastAirSpeed = 15;//LastAir Speed TW
	this->sPlayerAttribute.wLastMaxAp = 450000;// db->getInt("LastMaxAp");//Max AP
	this->sPlayerAttribute.wBaseMaxAp = 450000;// db->getInt("BaseMaxAp");//Base Max Ap
	this->sPlayerAttribute.wBaseApBattleRegen = 5000;//Regen In Battle AP TW
	this->sPlayerAttribute.wLastApBattleRegen = 5000;//LAst Regen in Battle Ap TW
	this->sPlayerAttribute.wBaseApRegen = 5000;//Base Ap Regen TW
	this->sPlayerAttribute.wLastApRegen = 5000;//Base While Sitting Regen AP TW
	this->sPlayerAttribute.wBaseApSitdownRegen = 5000;//AP Regen TW
	this->sPlayerAttribute.wLastApSitdownRegen = 5000;//AP Regen TW

	
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