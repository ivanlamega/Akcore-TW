#include "stdafx.h"
#include "NtlBitFlag.h"
#include "Avatar.h"

#include "NtlBitFlagManager.h"

CNtlAvatar::CNtlAvatar(void)
{
	Init();
}

CNtlAvatar::~CNtlAvatar(void)
{
}

void CNtlAvatar::Init()
{
	InitializeAttributeLink();
}

void CNtlAvatar::InitializeAttributeLink()
{
}

CNtlAvatar* CNtlAvatar::GetInstance()
{
	static CNtlAvatar avatar;
	return &avatar;
}

bool CNtlAvatar::UpdateAvatarAttribute(BYTE byAttributeTotalCount, void* pvRawData, sAVATAR_ATTRIBUTE* pAttributeData)
{
	BYTE* pbyCurrentPosition = (BYTE*)pvRawData;
	BYTE* pbyAttributeData = (BYTE*)pAttributeData;

	CNtlBitFlagManager changedFlag;
	if (false == changedFlag.Create(pvRawData, byAttributeTotalCount))
	{
		return false;
	}

	pbyCurrentPosition += changedFlag.GetBytesUsed();

	for (BYTE byIndex = ATTRIBUTE_TO_UPDATE_FIRST ; byIndex <= ATTRIBUTE_TO_UPDATE_LAST ; byIndex++)
	{
		if (false != changedFlag.IsSet(byIndex))
		{
			DWORD dwDataSize = m_attributeLogic[byIndex].pCopyAttributeFunction(pbyCurrentPosition, pbyAttributeData + m_attributeLogic[byIndex].dwFieldOffset);
			if (0 == dwDataSize)
			{
				return false;
			}
			else
			{
				pbyCurrentPosition += dwDataSize;
			}
		}
	}

	return true;
}
//Convert Attributes to Attribute Link - Luiz45
sAVATAR_ATTRIBUTE_LINK CNtlAvatar::ConvertAttributeToAttributeLink(sAVATAR_ATTRIBUTE* pAttributes)
{
	sAVATAR_ATTRIBUTE_LINK newAttribs;
	//Bases
	void* byBaseCon = &pAttributes->byBaseCon;
	void* byBaseStr = &pAttributes->byBaseStr;
	void* byBaseSol = &pAttributes->byBaseSol;
	void* byBaseFoc = &pAttributes->byBaseFoc;
	void* byBaseEng = &pAttributes->byBaseEng;
	void* byBaseDex = &pAttributes->byBaseDex;

	void* wBaseAttackRate = &pAttributes->wBaseAttackRate;
	void* wBaseAttackSpeedRate = &pAttributes->wBaseAttackSpeedRate;
	void* wBaseBlockRate = &pAttributes->wBaseBlockRate;
	void* wBaseCurseSuccessRate = &pAttributes->wBaseCurseSuccessRate;
	void* wBaseCurseToleranceRate = &pAttributes->wBaseCurseToleranceRate;
	void* wBaseDodgeRate = &pAttributes->wBaseDodgeRate;
	void* wBaseEnergyCriticalRate = &pAttributes->wBaseEnergyCriticalRate;
	void* wBaseEnergyDefence = &pAttributes->wBaseEnergyDefence;
	void* wBaseEnergyOffence = &pAttributes->wBaseEnergyOffence;
	void* wBasePhysicalCriticalRate = &pAttributes->wBasePhysicalCriticalRate;
	void* wBasePhysicalOffence = &pAttributes->wBasePhysicalOffence;
	void* wBasePhysicalDefence = &pAttributes->wBasePhysicalDefence;
	void* wBaseEpBattleRegen = &pAttributes->wBaseEpBattleRegen;
	void* wBaseEpRegen = &pAttributes->wBaseEpRegen;
	void* wBaseEpSitdownRegen = &pAttributes->wBaseEpSitdownRegen;
	void* wBaseApRegen = &pAttributes->wBaseApRegen;
	void* wBaseApSitdownRegen = &pAttributes->wBaseApSitdownRegen;
	void* wBaseApBattleRegen = &pAttributes->wBaseApBattleRegen;
	void* wBaseLpBattleRegen = &pAttributes->wBaseLpBattleRegen;
	void* wBaseLpRegen = &pAttributes->wBaseLpRegen;
	void* wBaseLpSitdownRegen = &pAttributes->wBaseLpSitdownRegen;
	void* wBaseRpRegen = &pAttributes->wBaseRpRegen;
	void* wBaseMaxEP = &pAttributes->wBaseMaxEP;
	void* wBaseMaxLP = &pAttributes->wBaseMaxLP;
	void* wBaseMaxAp = &pAttributes->wBaseMaxAp;
	void* wBaseMaxRP = &pAttributes->wBaseMaxRP;
	//Lasts
	void* byLastCon = &pAttributes->byLastCon;
	void* byLastStr = &pAttributes->byLastStr;
	void* byLastSol = &pAttributes->byLastSol;
	void* byLastFoc = &pAttributes->byLastFoc;
	void* byLastEng = &pAttributes->byLastEng;
	void* byLastDex = &pAttributes->byLastDex;
	
	void* wLastAttackRate = &pAttributes->wLastAttackRate;
	void* wLastAttackSpeedRate = &pAttributes->wLastAttackSpeedRate;
	void* wLastBlockRate = &pAttributes->wLastBlockRate;
	void* wLastCurseSuccessRate = &pAttributes->wLastCurseSuccessRate;
	void* wLastCurseToleranceRate = &pAttributes->wLastCurseToleranceRate;
	void* wLastDodgeRate = &pAttributes->wLastDodgeRate;
	void* wLastEnergyCriticalRate = &pAttributes->wLastEnergyCriticalRate;
	void* wLastEnergyOffence = &pAttributes->wLastEnergyOffence;
	void* wLastEnergyDefence = &pAttributes->wLastEnergyDefence;
	void* wLastPhysicalCriticalRate = &pAttributes->wLastPhysicalCriticalRate;
	void* wLastPhysicalOffence = &pAttributes->wLastPhysicalOffence;
	void* wLastPhysicalDefence = &pAttributes->wLastPhysicalDefence;
	void* wLastEpBattleRegen = &pAttributes->wLastEpBattleRegen;
	void* wLastEpRegen = &pAttributes->wLastEpRegen;
	void* wLastEpSitdownRegen = &pAttributes->wLastEpSitdownRegen;
	void* wLastApRegen = &pAttributes->wLastApRegen;
	void* wLastApSitdownRegen = &pAttributes->wLastApSitdownRegen;
	void* wLastApBattleRegen = &pAttributes->wLastApBattleRegen;
	void* wLastLpBattleRegen = &pAttributes->wLastLpBattleRegen;
	void* wLastLpRegen = &pAttributes->wLastLpRegen;
	void* wLastLpSitdownRegen = &pAttributes->wLastLpSitdownRegen;
	void* wLastRpRegen = &pAttributes->wLastRpRegen;
	void* wLastRpDimimutionRate = &pAttributes->wLastRpDimimutionRate;
	void* wLastMaxEP = &pAttributes->wLastMaxEP;
	void* wLastMaxAp = &pAttributes->wLastMaxAp;
	void* wLastMaxLP = &pAttributes->wLastMaxLP;
	void* wLastMaxRP = &pAttributes->wLastMaxRP;
	void* wGuardRate = &pAttributes->wGuardRate;
	void* wParalyzeToleranceRate = &pAttributes->wParalyzeToleranceRate;
	void* wTerrorToleranceRate = &pAttributes->wTerrorToleranceRate;
	void* wConfuseToleranceRate = &pAttributes->wConfuseToleranceRate;
	void* wStoneToleranceRate = &pAttributes->wStoneToleranceRate;
	void* wCandyToleranceRate = &pAttributes->wCandyToleranceRate;
	//Others
	void* uknown_rate1 = &pAttributes->unknown_rate1;
	void* uknown_rate2 = &pAttributes->unknown_rate2;
	void* unknown2 = &pAttributes->unknown2;
	//Arrays
	void* unknown3_0 = &pAttributes->unknown3[0];
	void* unknown3_1 = &pAttributes->unknown3[1];
	void* unknown3_2 = &pAttributes->unknown3[2];
	void* unknown3_3 = &pAttributes->unknown3[3];
	void* unknown3_4 = &pAttributes->unknown3[4];
	void* unknown3_5 = &pAttributes->unknown3[5];
	void* unknown3_6 = &pAttributes->unknown3[6];
	void* unknown3_7 = &pAttributes->unknown3[7];
	void* unknown3_8 = &pAttributes->unknown3[8];
	void* unknown3_9 = &pAttributes->unknown3[9];
	void* unknown3_10 = &pAttributes->unknown3[10];
	void* unknown3_11 = &pAttributes->unknown3[11];
	void* unknown3_12 = &pAttributes->unknown3[12];
	void* unknown3_13 = &pAttributes->unknown3[13];

	void* unknown4_0 = &pAttributes->unknown4[0];
	void* unknown4_1 = &pAttributes->unknown4[1];
	void* unknown4_2 = &pAttributes->unknown4[2];
	void* unknown4_3 = &pAttributes->unknown4[3];
	void* unknown4_4 = &pAttributes->unknown4[4];
	void* unknown4_5 = &pAttributes->unknown4[5];
	void* unknown4_6 = &pAttributes->unknown4[6];

	void* unknown5_0 = &pAttributes->unknown5[0];
	void* unknown5_1 = &pAttributes->unknown5[1];
	void* unknown5_2 = &pAttributes->unknown5[2];
	void* unknown5_3 = &pAttributes->unknown5[3];
	void* unknown5_4 = &pAttributes->unknown5[4];
	void* unknown5_5 = &pAttributes->unknown5[5];

	void* unknown6 = &pAttributes->unknown6;

	void* unknown_float1_0 = &pAttributes->unknown_float1[0];
	void* unknown_float1_1 = &pAttributes->unknown_float1[1];
	void* unknown_float1_2 = &pAttributes->unknown_float1[2];

	void* unknown_float2_0 = &pAttributes->unknown_float2[0];
	void* unknown_float2_1 = &pAttributes->unknown_float2[1];

	void* unknown_int16_0 = &pAttributes->unknown_int16[0];
	void* unknown_int16_1 = &pAttributes->unknown_int16[1];
	void* unknown_int16_2 = &pAttributes->unknown_int16[2];
	void* unknown_int16_3 = &pAttributes->unknown_int16[3];

	void* fBaseAirSpeed = &pAttributes->fBaseAirSpeed;
	void* fBaseAirDashSpeed = &pAttributes->fBaseAirDashSpeed;
	void* fBaseAirDash2Speed = &pAttributes->fBaseAirDash2Speed;
	void* fBaseRunSpeed = &pAttributes->fBaseRunSpeed;
	void* fLastAirSpeed = &pAttributes->fLastAirSpeed;
	void* fLastAirDashSpeed = &pAttributes->fLastAirDashSpeed;
	void* fLastAirDash2Speed = &pAttributes->fLastAirDash2Speed;
	void* fLastRunSpeed = &pAttributes->fLastRunSpeed;
	void* fBaseAttackRange = &pAttributes->fBaseAttackRange;
	void* fBleedingKeepTimeDown = &pAttributes->fBleedingKeepTimeDown;
	void* fCandyKeepTimeDown = &pAttributes->fCandyKeepTimeDown;
	void* fCastingTimeChangePercent = &pAttributes->fCastingTimeChangePercent;
	void* fConfuseKeepTimeDown = &pAttributes->fConfuseKeepTimeDown;
	void* fCoolTimeChangePercent = &pAttributes->fCoolTimeChangePercent;
	void* fCriticalBlockSuccessRate = &pAttributes->fCriticalBlockSuccessRate;
	void* fCurseBlockModeSuccessRate = &pAttributes->fCurseBlockModeSuccessRate;
	void* fDotTimeChangeAbsolute = &pAttributes->fDotTimeChangeAbsolute;
	void* fDotValueChangePercent = &pAttributes->fDotValueChangePercent;
	void* fEleganceDefence = &pAttributes->fEleganceDefence;
	void* fEleganceOffence = &pAttributes->fEleganceOffence;
	void* fEnergyCriticalDamageBonusRate = &pAttributes->fEnergyCriticalDamageBonusRate;
	void* fFunnyDefence = &pAttributes->fFunnyDefence;
	void* fFunnyOffence = &pAttributes->fFunnyOffence;
	void* fHonestDefence = &pAttributes->fHonestDefence;
	void* fHonestOffence = &pAttributes->fHonestOffence;
	void* fHtbBlockModeSuccessRate = &pAttributes->fHtbBlockModeSuccessRate;
	void* fItemUpgradeBonusRate = &pAttributes->fItemUpgradeBonusRate;
	void* fItemUpgradeBreakBonusRate = &pAttributes->fItemUpgradeBreakBonusRate;
	void* fKeepTimeChangePercent = &pAttributes->fKeepTimeChangePercent;
	void* fKnockdownBlockModeSuccessRate = &pAttributes->fKnockdownBlockModeSuccessRate;
	void* fLastAttackRange = &pAttributes->fLastAttackRange;
	void* fParalyzeKeepTimeDown = &pAttributes->fParalyzeKeepTimeDown;
	void* fPhysicalCriticalDamageBonusRate = &pAttributes->fPhysicalCriticalDamageBonusRate;
	void* fPoisonKeepTimeDown = &pAttributes->fPoisonKeepTimeDown;
	void* fRequiredEpChangePercent = &pAttributes->fRequiredEpChangePercent;
	void* fSitDownEpRegenBonusRate = &pAttributes->fSitDownEpRegenBonusRate;
	void* fSitDownLpRegenBonusRate = &pAttributes->fSitDownLpRegenBonusRate;
	void* fSkillDamageBlockModeSuccessRate = &pAttributes->fSkillDamageBlockModeSuccessRate;
	void* fStomachacheKeepTimeDown = &pAttributes->fStomachacheKeepTimeDown;
	void* fStoneKeepTimeDown = &pAttributes->fStoneKeepTimeDown;
	void* fStrangeDefence = &pAttributes->fStrangeDefence;
	void* fStrangeOffence = &pAttributes->fStrangeOffence;
	void* fTerrorKeepTimeDown = &pAttributes->fTerrorKeepTimeDown;
	void* fWildDefence = &pAttributes->fWildDefence;
	void* fWildOffence = &pAttributes->fWildOffence;
	
	//New Attribs
	newAttribs.pbyBaseCon = (WORD*)byBaseCon;
	newAttribs.pbyBaseStr = (WORD*)byBaseStr;
	newAttribs.pbyBaseSol = (WORD*)byBaseSol;
	newAttribs.pbyBaseFoc = (WORD*)byBaseFoc;
	newAttribs.pbyBaseEng = (WORD*)byBaseEng;
	newAttribs.pbyBaseDex = (WORD*)byBaseDex;

	newAttribs.pbyLastCon = (WORD*)byLastCon;
	newAttribs.pbyLastStr = (WORD*)byLastStr;
	newAttribs.pbyLastSol = (WORD*)byLastSol;
	newAttribs.pbyLastFoc = (WORD*)byLastFoc;
	newAttribs.pbyLastEng = (WORD*)byLastEng;
	newAttribs.pbyLastDex = (WORD*)byLastDex;

	newAttribs.pwBaseAttackRate = (WORD*)wBaseAttackRate;
	newAttribs.pwBaseAttackSpeedRate = (WORD*)wBaseAttackSpeedRate;
	newAttribs.pwBaseBlockRate = (WORD*)wBaseBlockRate;
	newAttribs.pwBaseCurseSuccessRate = (WORD*)wBaseCurseSuccessRate;
	newAttribs.pwBaseCurseToleranceRate = (WORD*)wBaseCurseToleranceRate;
	newAttribs.pwBaseDodgeRate = (WORD*)wBaseDodgeRate;
	newAttribs.pwBaseEnergyCriticalRate = (WORD*)wBaseEnergyCriticalRate;
	newAttribs.pwBaseEnergyOffence = (WORD*)wBaseEnergyOffence;
	newAttribs.pwBaseEnergyDefence = (WORD*)wBaseEnergyDefence;
	newAttribs.pwBasePhysicalCriticalRate = (WORD*)wBasePhysicalCriticalRate;
	newAttribs.pwBasePhysicalOffence = (WORD*)wBasePhysicalOffence;
	newAttribs.pwBasePhysicalDefence = (WORD*)wBasePhysicalDefence;
	newAttribs.pwBaseEpBattleRegen = (WORD*)wBaseEpBattleRegen;
	newAttribs.pwBaseEpRegen = (WORD*)wBaseEpRegen;
	newAttribs.pwBaseEpSitdownRegen = (WORD*)wBaseEpSitdownRegen;	
	newAttribs.pwBaseApBattleRegen = (WORD*)wBaseApBattleRegen;
	newAttribs.pwBaseApRegen = (WORD*)wBaseApRegen;
	newAttribs.pwBaseApSitdownRegen = (WORD*)wBaseApSitdownRegen;
	newAttribs.pwBaseLpBattleRegen = (WORD*)wBaseLpBattleRegen;
	newAttribs.pwBaseLpRegen = (WORD*)wBaseLpRegen;
	newAttribs.pwBaseLpSitdownRegen = (WORD*)wBaseLpSitdownRegen;
	newAttribs.pwBaseRpRegen = (WORD*)wBaseRpRegen;
	newAttribs.pwBaseMaxEP = (WORD*)wBaseMaxEP;
	newAttribs.pwBaseMaxLP = (DWORD*)wBaseMaxLP;
	newAttribs.pwBaseMaxAp = (DWORD*)wBaseMaxAp;
	newAttribs.pwBaseMaxRP = (WORD*)wBaseMaxRP;	
	newAttribs.pwBaseEpSitdownRegen = (WORD*)wBaseEpSitdownRegen;

	newAttribs.pwLastAttackRate = (WORD*)wLastAttackRate;
	newAttribs.pwLastAttackSpeedRate = (WORD*)wLastAttackSpeedRate;
	newAttribs.pwLastBlockRate = (WORD*)wLastBlockRate;
	newAttribs.pwLastCurseSuccessRate = (WORD*)wLastCurseSuccessRate;
	newAttribs.pwLastCurseToleranceRate = (WORD*)wLastCurseToleranceRate;
	newAttribs.pwLastDodgeRate = (WORD*)wLastDodgeRate;
	newAttribs.pwLastEnergyCriticalRate = (WORD*)wLastEnergyCriticalRate;
	newAttribs.pwLastEnergyOffence = (WORD*)wLastEnergyOffence;
	newAttribs.pwLastEnergyDefence = (WORD*)wLastEnergyDefence;
	newAttribs.pwLastPhysicalCriticalRate = (WORD*)wLastPhysicalCriticalRate;
	newAttribs.pwLastPhysicalOffence = (WORD*)wLastPhysicalOffence;
	newAttribs.pwLastPhysicalDefence = (WORD*)wLastPhysicalDefence;
	newAttribs.pwLastEpBattleRegen = (WORD*)wLastEpBattleRegen;
	newAttribs.pwLastEpRegen = (WORD*)wLastEpRegen;
	newAttribs.pwLastEpSitdownRegen = (WORD*)wLastEpSitdownRegen;	
	newAttribs.pwLastApBattleRegen = (WORD*)wLastApBattleRegen;
	newAttribs.pwLastApRegen = (WORD*)wLastApRegen;
	newAttribs.pwLastApSitdownRegen = (WORD*)wLastApSitdownRegen;
	newAttribs.pwLastLpBattleRegen = (WORD*)wLastLpBattleRegen;
	newAttribs.pwLastLpRegen = (WORD*)wLastLpRegen;
	newAttribs.pwLastLpSitdownRegen = (WORD*)wLastLpSitdownRegen;
	newAttribs.pwLastRpRegen = (WORD*)wLastRpRegen;
	newAttribs.pwLastRpDimimutionRate = (WORD*)wLastRpDimimutionRate;
	newAttribs.pwLastMaxEP = (WORD*)wLastMaxEP;
	newAttribs.pwLastMaxLP = (DWORD*)wLastMaxLP;	
	newAttribs.pwLastMaxAp = (DWORD*)wLastMaxAp;
	newAttribs.pwLastMaxRP = (WORD*)wLastMaxRP;
	
	newAttribs.pwGuardRate = (WORD*)wGuardRate;
	newAttribs.pwParalyzeToleranceRate = (WORD*)wParalyzeToleranceRate;
	newAttribs.pwTerrorToleranceRate = (WORD*)wTerrorToleranceRate;
	newAttribs.pwConfuseToleranceRate = (WORD*)wConfuseToleranceRate;
	newAttribs.pwCandyToleranceRate = (WORD*)wCandyToleranceRate;
	newAttribs.pwStoneToleranceRate = (WORD*)wStoneToleranceRate;

	newAttribs.pwunknown_rate1 = (WORD*)uknown_rate1;
	newAttribs.pwunknown_rate2 = (WORD*)uknown_rate2;
	newAttribs.pfunknown3[0] = (float*)unknown3_0;
	newAttribs.pfunknown3[1] = (float*)unknown3_1;
	newAttribs.pfunknown3[2] = (float*)unknown3_2;
	newAttribs.pfunknown3[3] = (float*)unknown3_3;
	newAttribs.pfunknown3[4] = (float*)unknown3_4;
	newAttribs.pfunknown3[5] = (float*)unknown3_5;
	newAttribs.pfunknown3[6] = (float*)unknown3_6;
	newAttribs.pfunknown3[7] = (float*)unknown3_7;
	newAttribs.pfunknown3[8] = (float*)unknown3_8;
	newAttribs.pfunknown3[9] = (float*)unknown3_9;
	newAttribs.pfunknown3[10] = (float*)unknown3_10;
	newAttribs.pfunknown3[11] = (float*)unknown3_11;
	newAttribs.pfunknown3[12] = (float*)unknown3_12;
	newAttribs.pfunknown3[13] = (float*)unknown3_13;

	newAttribs.pfunknown5[0] = (float*)unknown5_0;
	newAttribs.pfunknown5[1] = (float*)unknown5_1;
	newAttribs.pfunknown5[2] = (float*)unknown5_2;
	newAttribs.pfunknown5[3] = (float*)unknown5_3;
	newAttribs.pfunknown5[4] = (float*)unknown5_4;
	newAttribs.pfunknown5[5] = (float*)unknown5_5;

	newAttribs.pwunknown4[0] = (WORD*)unknown4_0;
	newAttribs.pwunknown4[1] = (WORD*)unknown4_1;
	newAttribs.pwunknown4[2] = (WORD*)unknown4_2;
	newAttribs.pwunknown4[3] = (WORD*)unknown4_3;
	newAttribs.pwunknown4[4] = (WORD*)unknown4_4;
	newAttribs.pwunknown4[5] = (WORD*)unknown4_5;
	newAttribs.pwunknown4[6] = (WORD*)unknown4_6;

	newAttribs.pwunknown6 = (WORD*)unknown6;

	newAttribs.pfunknown_float1[0] = (float*)unknown_float1_0;
	newAttribs.pfunknown_float1[1] = (float*)unknown_float1_1;
	newAttribs.pfunknown_float1[2] = (float*)unknown_float1_2;

	newAttribs.pfunknown_float2[0] = (float*)unknown_float2_0;
	newAttribs.pfunknown_float2[1] = (float*)unknown_float1_1;

	newAttribs.pwunknown_int16[0] = (WORD*)unknown_int16_0;
	newAttribs.pwunknown_int16[1] = (WORD*)unknown_int16_1;
	newAttribs.pwunknown_int16[2] = (WORD*)unknown_int16_2;
	newAttribs.pwunknown_int16[3] = (WORD*)unknown_int16_3;

	newAttribs.pfBaseAirSpeed = (float*)fBaseAirSpeed;
	newAttribs.pfBaseAirDashSpeed = (float*)fBaseAirDashSpeed;
	newAttribs.pfBaseAirDash2Speed = (float*)fBaseAirDash2Speed;

	newAttribs.pfLastAirSpeed = (float*)fLastAirSpeed;
	newAttribs.pfLastAirDashSpeed = (float*)fLastAirDashSpeed;
	newAttribs.pfLastAirDash2Speed = (float*)fLastAirDash2Speed;
	newAttribs.pfunknown2 = (float*)unknown2;

	newAttribs.pfBaseAttackRange = (float*)fBaseAttackRange;
	newAttribs.pfBleedingKeepTimeDown = (float*)fBleedingKeepTimeDown;
	newAttribs.pfCandyKeepTimeDown = (float*)fCandyKeepTimeDown;
	newAttribs.pfCastingTimeChangePercent = (float*)fCastingTimeChangePercent;
	newAttribs.pfConfuseKeepTimeDown = (float*)fConfuseKeepTimeDown;
	newAttribs.pfCoolTimeChangePercent = (float*)fCoolTimeChangePercent;
	newAttribs.pfCriticalBlockSuccessRate = (float*)fCriticalBlockSuccessRate;
	newAttribs.pfCurseBlockModeSuccessRate = (float*)fCurseBlockModeSuccessRate;
	newAttribs.pfDotTimeChangeAbsolute = (float*)fDotTimeChangeAbsolute;
	newAttribs.pfDotValueChangePercent = (float*)fDotValueChangePercent;
	newAttribs.pfEleganceOffence = (float*)fEleganceOffence;
	newAttribs.pfEleganceDefence = (float*)fEleganceDefence;
	newAttribs.pfEnergyCriticalDamageBonusRate = (float*)fEnergyCriticalDamageBonusRate;
	newAttribs.pfFunnyOffence = (float*)fFunnyOffence;
	newAttribs.pfFunnyDefence = (float*)fFunnyDefence;
	newAttribs.pfHonestOffence = (float*)fHonestOffence;
	newAttribs.pfHonestDefence = (float*)fHonestDefence;
	newAttribs.pfHtbBlockModeSuccessRate = (float*)fHtbBlockModeSuccessRate;
	newAttribs.pfItemUpgradeBonusRate = (float*)fItemUpgradeBonusRate;
	newAttribs.pfItemUpgradeBreakBonusRate = (float*)fItemUpgradeBreakBonusRate;
	newAttribs.pfKeepTimeChangePercent = (float*)fKeepTimeChangePercent;
	newAttribs.pfKnockdownBlockModeSuccessRate = (float*)fKnockdownBlockModeSuccessRate;
	newAttribs.pfLastAttackRange = (float*)fLastAttackRange;
	newAttribs.pfBaseRunSpeed = (float*)fBaseRunSpeed;
	newAttribs.pfLastRunSpeed = (float*)fLastRunSpeed;
	newAttribs.pfParalyzeKeepTimeDown = (float*)fParalyzeKeepTimeDown;
	newAttribs.pfPhysicalCriticalDamageBonusRate = (float*)fPhysicalCriticalDamageBonusRate;
	newAttribs.pfPoisonKeepTimeDown = (float*)fPoisonKeepTimeDown;
	newAttribs.pfRequiredEpChangePercent = (float*)fRequiredEpChangePercent;
	newAttribs.pfSitDownEpRegenBonusRate = (float*)fSitDownEpRegenBonusRate;
	newAttribs.pfSitDownLpRegenBonusRate = (float*)fSitDownLpRegenBonusRate;
	newAttribs.pfSkillDamageBlockModeSuccessRate = (float*)fSkillDamageBlockModeSuccessRate;
	newAttribs.pfStomachacheKeepTimeDown = (float*)fStomachacheKeepTimeDown;
	newAttribs.pfStoneKeepTimeDown = (float*)fStoneKeepTimeDown;
	newAttribs.pfStrangeDefence = (float*)fStrangeDefence;
	newAttribs.pfStrangeOffence = (float*)fStrangeOffence;
	newAttribs.pfTerrorKeepTimeDown = (float*)fTerrorKeepTimeDown;
	newAttribs.pfWildDefence = (float*)fWildDefence;
	newAttribs.pfWildOffence = (float*)fWildOffence;

	return newAttribs;
}

bool CNtlAvatar::SaveAvatarAttribute(CNtlBitFlagManager* pChangedFlag, sAVATAR_ATTRIBUTE_LINK* pAttributeDataLink, void* pvBuffer, DWORD* pwdDataSize)
{
	BYTE* pbyBuffer = (BYTE*)pvBuffer;
	BYTE* pbyAttributeDataLink = (BYTE*)pAttributeDataLink;

	*pwdDataSize = 0;

	::CopyMemory(pbyBuffer, pChangedFlag->GetRawData(), pChangedFlag->GetBytesUsed());

	pbyBuffer += pChangedFlag->GetBytesUsed();
	*pwdDataSize += pChangedFlag->GetBytesUsed();

	for (BYTE byIndex = ATTRIBUTE_TO_UPDATE_FIRST ; byIndex <= ATTRIBUTE_TO_UPDATE_LAST ; byIndex++)
	{
		if (false != pChangedFlag->IsSet(byIndex))
		{
			void* pvAttributeFieldLink = *((void**)(pbyAttributeDataLink + byIndex * sizeof(void*)));
			if (NULL == pvAttributeFieldLink)
			{
				continue;
			}

			DWORD dwDataSize = m_attributeLogic[byIndex].pCopyAttributeFunction(pvAttributeFieldLink, pbyBuffer);
			if (0 == dwDataSize)
			{
				return false;
			}
			else
			{
				pbyBuffer += dwDataSize;
				*pwdDataSize += dwDataSize;
			}
		}
	}

	return true;
}

bool CNtlAvatar::FillAvatarAttribute(sAVATAR_ATTRIBUTE_LINK* pAttributeDataLink, sAVATAR_ATTRIBUTE* pAttributeData)
{
	BYTE* pbyAttributeDataLink = (BYTE*)pAttributeDataLink;
	BYTE* pbyAttributeData = (BYTE*)pAttributeData;

	for (BYTE byIndex = ATTRIBUTE_TO_UPDATE_FIRST ; byIndex <= ATTRIBUTE_TO_UPDATE_LAST ; byIndex++)
	{
		void* pvAttributeFieldLink = *((void**)(pbyAttributeDataLink + byIndex * sizeof(void*)));
		if (NULL == pvAttributeFieldLink)
		{
			continue;
		}

		DWORD dwDataSize = m_attributeLogic[byIndex].pCopyAttributeFunction(
																			pvAttributeFieldLink,
																			pbyAttributeData + m_attributeLogic[byIndex].dwFieldOffset);
		if (0 == dwDataSize)
		{
			return false;
		}
	}

	return true;
}

#ifndef ATTRIBUTE_LOGIC_DEFINE
#define ATTRIBUTE_LOGIC_DEFINE(field_name, type)					\
	{																\
		PtrToUlong(&(((sAVATAR_ATTRIBUTE*)NULL)->field_name)),		\
		CopyValueByType_##type										\
	}
#endif

CNtlAvatar::sATTRIBUTE_LOGIC CNtlAvatar::m_attributeLogic[ATTRIBUTE_TO_UPDATE_COUNT] =
{
	ATTRIBUTE_LOGIC_DEFINE(byBaseStr, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastStr, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byBaseCon, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastCon, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byBaseFoc, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastFoc, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byBaseDex, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastDex, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byBaseSol, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastSol, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byBaseEng, BYTE),
	ATTRIBUTE_LOGIC_DEFINE(byLastEng, BYTE),

	ATTRIBUTE_LOGIC_DEFINE(wBaseMaxLP, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastMaxLP, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseMaxEP, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastMaxEP, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseMaxAp, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastMaxAp, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseMaxRP, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastMaxRP, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseLpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastLpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseLpSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastLpSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseLpBattleRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastLpBattleRegen, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseEpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseEpSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEpSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseEpBattleRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEpBattleRegen, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseApRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastApRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseApSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastApSitdownRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseApBattleRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastApBattleRegen, WORD),

	ATTRIBUTE_LOGIC_DEFINE(unknown_int16, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseRpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastRpRegen, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastRpDimimutionRate, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBasePhysicalOffence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastPhysicalOffence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBasePhysicalDefence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastPhysicalDefence, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseEnergyOffence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEnergyOffence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseEnergyDefence, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEnergyDefence, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseAttackRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastAttackRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseDodgeRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastDodgeRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseBlockRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastBlockRate, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBaseCurseSuccessRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastCurseSuccessRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseCurseToleranceRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastCurseToleranceRate, WORD),

	ATTRIBUTE_LOGIC_DEFINE(unknown_rate1, WORD),
	ATTRIBUTE_LOGIC_DEFINE(unknown_rate2, WORD),

	ATTRIBUTE_LOGIC_DEFINE(wBasePhysicalCriticalRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastPhysicalCriticalRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wBaseEnergyCriticalRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastEnergyCriticalRate, WORD),

	ATTRIBUTE_LOGIC_DEFINE(unknown_float1, float),
	ATTRIBUTE_LOGIC_DEFINE(unknown_float2, float),

	ATTRIBUTE_LOGIC_DEFINE(fBaseRunSpeed, float),
	ATTRIBUTE_LOGIC_DEFINE(fLastRunSpeed, float),

	ATTRIBUTE_LOGIC_DEFINE(fBaseAirSpeed, float),
	ATTRIBUTE_LOGIC_DEFINE(fLastAirSpeed, float),
	ATTRIBUTE_LOGIC_DEFINE(fBaseAirDashSpeed, float),
	ATTRIBUTE_LOGIC_DEFINE(fLastAirDashSpeed, float),
	ATTRIBUTE_LOGIC_DEFINE(fBaseAirDash2Speed, float),
	ATTRIBUTE_LOGIC_DEFINE(fLastAirDash2Speed, float),

	ATTRIBUTE_LOGIC_DEFINE(wBaseAttackSpeedRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wLastAttackSpeedRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(fBaseAttackRange, float),
	ATTRIBUTE_LOGIC_DEFINE(fLastAttackRange, float),

	ATTRIBUTE_LOGIC_DEFINE(fCastingTimeChangePercent, float),
	ATTRIBUTE_LOGIC_DEFINE(fCoolTimeChangePercent, float),
	ATTRIBUTE_LOGIC_DEFINE(fKeepTimeChangePercent, float),
	ATTRIBUTE_LOGIC_DEFINE(fDotValueChangePercent, float),
	ATTRIBUTE_LOGIC_DEFINE(fDotTimeChangeAbsolute, float),
	ATTRIBUTE_LOGIC_DEFINE(fRequiredEpChangePercent, float),

	ATTRIBUTE_LOGIC_DEFINE(fHonestOffence, float),
	ATTRIBUTE_LOGIC_DEFINE(fHonestDefence, float),
	ATTRIBUTE_LOGIC_DEFINE(fStrangeOffence, float),
	ATTRIBUTE_LOGIC_DEFINE(fStrangeDefence, float),
	ATTRIBUTE_LOGIC_DEFINE(fWildOffence, float),
	ATTRIBUTE_LOGIC_DEFINE(fWildDefence, float),
	ATTRIBUTE_LOGIC_DEFINE(fEleganceOffence, float),
	ATTRIBUTE_LOGIC_DEFINE(fEleganceDefence, float),
	ATTRIBUTE_LOGIC_DEFINE(fFunnyOffence, float),
	ATTRIBUTE_LOGIC_DEFINE(fFunnyDefence, float),

	ATTRIBUTE_LOGIC_DEFINE(unknown2, float),

	ATTRIBUTE_LOGIC_DEFINE(wParalyzeToleranceRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wTerrorToleranceRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wConfuseToleranceRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wStoneToleranceRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(wCandyToleranceRate, WORD),

	ATTRIBUTE_LOGIC_DEFINE(fParalyzeKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fTerrorKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fConfuseKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fStoneKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fCandyKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fBleedingKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fPoisonKeepTimeDown, float),
	ATTRIBUTE_LOGIC_DEFINE(fStomachacheKeepTimeDown, float),

	ATTRIBUTE_LOGIC_DEFINE(fCriticalBlockSuccessRate, float),

	ATTRIBUTE_LOGIC_DEFINE(wGuardRate, WORD),
	ATTRIBUTE_LOGIC_DEFINE(unknown6, WORD),

	ATTRIBUTE_LOGIC_DEFINE(fSkillDamageBlockModeSuccessRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fCurseBlockModeSuccessRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fKnockdownBlockModeSuccessRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fHtbBlockModeSuccessRate, float),

	ATTRIBUTE_LOGIC_DEFINE(fSitDownLpRegenBonusRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fSitDownEpRegenBonusRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fPhysicalCriticalDamageBonusRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fEnergyCriticalDamageBonusRate, float),

	ATTRIBUTE_LOGIC_DEFINE(fItemUpgradeBonusRate, float),
	ATTRIBUTE_LOGIC_DEFINE(fItemUpgradeBreakBonusRate, float),

	ATTRIBUTE_LOGIC_DEFINE(unknown3, float),
	ATTRIBUTE_LOGIC_DEFINE(unknown4, WORD),
	ATTRIBUTE_LOGIC_DEFINE(unknown5, float),
};

#undef ATTRIBUTE_LOGIC_DEFINE

DWORD CNtlAvatar::CopyValueByType_BYTE(void* pvValue, void* pvBuffer)
{
	BYTE* pbyBuffer = (BYTE*)pvBuffer;

	*pbyBuffer = *((BYTE*)pvValue);

	return sizeof(BYTE);
}

DWORD CNtlAvatar::CopyValueByType_WORD(void* pvValue, void* pvBuffer)
{
	WORD* pbyBuffer = (WORD*)pvBuffer;

	*pbyBuffer = *((WORD*)pvValue);

	return sizeof(WORD);
}

DWORD CNtlAvatar::CopyValueByType_float(void* pvValue, void* pvBuffer)
{
	float* pbyBuffer = (float*)pvBuffer;

	*pbyBuffer = *((float*)pvValue);

	return sizeof(float);
}