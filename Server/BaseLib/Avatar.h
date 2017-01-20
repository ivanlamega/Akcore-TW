#pragma once

enum eATTRIBUTE_TO_UPDATE
{
	ATTRIBUTE_TO_UPDATE_STR_BASE,
	ATTRIBUTE_TO_UPDATE_STR_LAST,
	ATTRIBUTE_TO_UPDATE_CON_BASE,
	ATTRIBUTE_TO_UPDATE_CON_LAST,
	ATTRIBUTE_TO_UPDATE_FOC_BASE,
	ATTRIBUTE_TO_UPDATE_FOC_LAST,
	ATTRIBUTE_TO_UPDATE_DEX_BASE,
	ATTRIBUTE_TO_UPDATE_DEX_LAST,
	ATTRIBUTE_TO_UPDATE_SOL_BASE,
	ATTRIBUTE_TO_UPDATE_SOL_LAST,
	ATTRIBUTE_TO_UPDATE_ENG_BASE,
	ATTRIBUTE_TO_UPDATE_ENG_LAST,

	ATTRIBUTE_TO_UPDATE_MAX_LP_BASE,
	ATTRIBUTE_TO_UPDATE_MAX_LP_LAST,
	ATTRIBUTE_TO_UPDATE_MAX_EP_BASE,
	ATTRIBUTE_TO_UPDATE_MAX_EP_LAST,
	ATTRIBUTE_TO_UPDATE_MAX_AP_BASE,
	ATTRIBUTE_TO_UPDATE_MAX_AP_LAST,
	ATTRIBUTE_TO_UPDATE_MAX_RP_BASE,
	ATTRIBUTE_TO_UPDATE_MAX_RP_LAST,

	ATTRIBUTE_TO_UPDATE_LP_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_LP_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_LP_SITDOWN_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_LP_SITDOWN_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_LP_BATTLE_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_LP_BATTLE_REGENERATION_LAST,

	ATTRIBUTE_TO_UPDATE_EP_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_EP_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_EP_SITDOWN_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_EP_SITDOWN_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_EP_BATTLE_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_EP_BATTLE_REGENERATION_LAST,

	ATTRIBUTE_TO_UPDATE_AP_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_AP_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_AP_SITDOWN_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_AP_SITDOWN_REGENERATION_LAST,
	ATTRIBUTE_TO_UPDATE_AP_BATTLE_REGENERATION_BASE,
	ATTRIBUTE_TO_UPDATE_AP_BATTLE_REGENERATION_LAST,

	ATTRIBUTE_TO_UPDATE_UNKNOWN_INT16,

	ATTRIBUTE_TO_UPDATE_RP_CHARGE_SPEED_BASE,
	ATTRIBUTE_TO_UPDATE_RP_CHARGE_SPEED_LAST,
	ATTRIBUTE_TO_UPDATE_RP_DIMIMUTION,

	ATTRIBUTE_TO_UPDATE_PHYSICAL_OFFENCE_BASE,
	ATTRIBUTE_TO_UPDATE_PHYSICAL_OFFENCE_LAST,
	ATTRIBUTE_TO_UPDATE_PHYSICAL_DEFENCE_BASE,
	ATTRIBUTE_TO_UPDATE_PHYSICAL_DEFENCE_LAST,

	ATTRIBUTE_TO_UPDATE_ENERGY_OFFENCE_BASE,
	ATTRIBUTE_TO_UPDATE_ENERGY_OFFENCE_LAST,
	ATTRIBUTE_TO_UPDATE_ENERGY_DEFENCE_BASE,
	ATTRIBUTE_TO_UPDATE_ENERGY_DEFENCE_LAST,

	ATTRIBUTE_TO_UPDATE_ATTACK_RATE_BASE,
	ATTRIBUTE_TO_UPDATE_ATTACK_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_DODGE_RATE_BASE,
	ATTRIBUTE_TO_UPDATE_DODGE_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_BLOCK_RATE_BASE,
	ATTRIBUTE_TO_UPDATE_BLOCK_RATE_LAST,

	ATTRIBUTE_TO_UPDATE_CURSE_SUCCESS_BASE,
	ATTRIBUTE_TO_UPDATE_CURSE_SUCCESS_LAST,
	ATTRIBUTE_TO_UPDATE_CURSE_TOLERANCE_BASE,
	ATTRIBUTE_TO_UPDATE_CURSE_TOLERANCE_LAST,

	ATTRIBUTE_TO_UPDATE_UNKNOW_RATE1,
	ATTRIBUTE_TO_UPDATE_UNKNOW_RATE2,

	ATTRIBUTE_TO_UPDATE_PHYSICAL_CRITICAL_BASE,
	ATTRIBUTE_TO_UPDATE_PHYSICAL_CRITICAL_LAST,
	ATTRIBUTE_TO_UPDATE_ENERGY_CRITICAL_BASE,
	ATTRIBUTE_TO_UPDATE_ENERGY_CRITICAL_LAST,

	ATTRIBUTE_TO_UPDATE_UNKNOW_FLOAT1,
	ATTRIBUTE_TO_UPDATE_UNKNOW_FLOAT2,

	ATTRIBUTE_TO_UPDATE_RUN_SPEED_BASE,
	ATTRIBUTE_TO_UPDATE_RUN_SPEED_LAST,

	ATTRIBUTE_TO_UPDATE_AIR_SPEED_BASE,
	ATTRIBUTE_TO_UPDATE_AIR_SPEED_LAST,
	ATTRIBUTE_TO_UPDATE_AIR_DASH_SPEED_BASE,
	ATTRIBUTE_TO_UPDATE_AIR_DASH_SPEED_LAST,
	ATTRIBUTE_TO_UPDATE_AIR_DASH2_SPEED_BASE,
	ATTRIBUTE_TO_UPDATE_AIR_DASH2_SPEED_LAST,

	ATTRIBUTE_TO_UPDATE_ATTACK_SPEED_RATE_BASE,
	ATTRIBUTE_TO_UPDATE_ATTACK_SPEED_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_ATTACK_RANGE_BASE,
	ATTRIBUTE_TO_UPDATE_ATTACK_RANGE_LAST,

	ATTRIBUTE_TO_UPDATE_CASTING_TIME_CHANGE_PERCENT,
	ATTRIBUTE_TO_UPDATE_COOL_TIME_CHANGE_PERCENT,
	ATTRIBUTE_TO_UPDATE_KEEP_TIME_CHANGE_PERCENT,
	ATTRIBUTE_TO_UPDATE_DOT_VALUE_CHANGE_PERCENT,
	ATTRIBUTE_TO_UPDATE_DOT_TIME_CHANGE_ABSOLUTE,
	ATTRIBUTE_TO_UPDATE_REQUIRED_EP_CHANGE_PERCENT,

	ATTRIBUTE_TO_UPDATE_HONEST_OFFENCE,
	ATTRIBUTE_TO_UPDATE_HONEST_DEFENCE,
	ATTRIBUTE_TO_UPDATE_STRANGE_OFFENCE,
	ATTRIBUTE_TO_UPDATE_STRANGE_DEFENCE,
	ATTRIBUTE_TO_UPDATE_WILD_OFFENCE,
	ATTRIBUTE_TO_UPDATE_WILD_DEFENCE,
	ATTRIBUTE_TO_UPDATE_ELEGANCE_OFFENCE,
	ATTRIBUTE_TO_UPDATE_ELEGANCE_DEFENCE,
	ATTRIBUTE_TO_UPDATE_FUNNY_OFFENCE,
	ATTRIBUTE_TO_UPDATE_FUNNY_DEFENCE,

	ATTRIBUTE_TO_UPDATE_UNKNOW2,

	ATTRIBUTE_TO_UPDATE_PARALYZE_TOLERANCE_RATE,
	ATTRIBUTE_TO_UPDATE_TERROR_TOLERANCE_RATE,
	ATTRIBUTE_TO_UPDATE_CONFUSE_TOLERANCE_RATE,
	ATTRIBUTE_TO_UPDATE_STONE_TOLERANCE_RATE,
	ATTRIBUTE_TO_UPDATE_CANDY_TOLERANCE_RATE,

	ATTRIBUTE_TO_UPDATE_PARALYZE_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_TERROR_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_CONFUSE_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_STONE_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_CANDY_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_BLEEDING_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_POISON_KEEP_TIME_DOWN,
	ATTRIBUTE_TO_UPDATE_STOMACHACHE_KEEP_TIME_DOWN,

	ATTRIBUTE_TO_UPDATE_CRITICAL_BLOCK_SUCCESS_RATE,

	ATTRIBUTE_TO_UPDATE_GUARD_RATE_BASE,

	ATTRIBUTE_TO_UPDATE_UNKNOW6,

	ATTRIBUTE_TO_UPDATE_SKILL_DAMAGE_BLOCK_MODE_SUCCESS_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_CURSE_BLOCK_MODE_SUCCESS_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_KNOCKDOWN_BLOCK_MODE_SUCCESS_RATE_LAST,
	ATTRIBUTE_TO_UPDATE_HTB_BLOCK_MODE_SUCCESS_RATE_LAST,

	ATTRIBUTE_TO_UPDATE_SIT_DOWN_LP_REGENERATION_UP,
	ATTRIBUTE_TO_UPDATE_SIT_DOWN_EP_REGENERATION_UP,
	ATTRIBUTE_TO_UPDATE_PHYSICAL_CRITICAL_DAMAGE_UP,
	ATTRIBUTE_TO_UPDATE_ENERGY_CRITICAL_DAMAGE_UP,

	ATTRIBUTE_TO_UPDATE_ITEM_UPGRADE_RATE_UP,
	ATTRIBUTE_TO_UPDATE_ITEM_BREAK_RATE_DOWN,

	ATTRIBUTE_TO_UPDATE_UNKNOW3,
	ATTRIBUTE_TO_UPDATE_UNKNOW4,
	ATTRIBUTE_TO_UPDATE_UNKNOW5,

	ATTRIBUTE_TO_UPDATE_COUNT,
	ATTRIBUTE_TO_UPDATE_UNKNOWN	= 0xFF,

	ATTRIBUTE_TO_UPDATE_FIRST = ATTRIBUTE_TO_UPDATE_STR_BASE,
	ATTRIBUTE_TO_UPDATE_LAST = ATTRIBUTE_TO_UPDATE_COUNT - 1,
};

#pragma pack(push, 1)

struct sAVATAR_ATTRIBUTE
{
	WORD byBaseStr;
	WORD byLastStr;
	WORD byBaseCon;
	WORD byLastCon;
	WORD byBaseFoc;
	WORD byLastFoc;
	WORD byBaseDex;
	WORD byLastDex;
	WORD byBaseSol;
	WORD byLastSol;
	WORD byBaseEng;
	WORD byLastEng;

	DWORD wBaseMaxLP;
	DWORD wLastMaxLP;
	WORD wBaseMaxEP;
	WORD wLastMaxEP;
	DWORD wBaseMaxAp;
	DWORD wLastMaxAp;
	WORD wBaseMaxRP;
	WORD wLastMaxRP;

	WORD wBaseLpRegen;
	WORD wLastLpRegen;
	WORD wBaseLpSitdownRegen;
	WORD wLastLpSitdownRegen;
	WORD wBaseLpBattleRegen;
	WORD wLastLpBattleRegen;

	WORD wBaseEpRegen;
	WORD wLastEpRegen;
	WORD wBaseEpSitdownRegen;
	WORD wLastEpSitdownRegen;
	WORD wBaseEpBattleRegen;
	WORD wLastEpBattleRegen;

	WORD wBaseApRegen;
	WORD wLastApRegen;
	WORD wBaseApSitdownRegen;
	WORD wLastApSitdownRegen;
	WORD wBaseApBattleRegen;
	WORD wLastApBattleRegen;

	WORD unknown_int16[4];//????

	WORD wBaseRpRegen;
	WORD wLastRpRegen;
	WORD wLastRpDimimutionRate;

	WORD wBasePhysicalOffence;
	WORD wLastPhysicalOffence;
	WORD wBasePhysicalDefence;
	WORD wLastPhysicalDefence;

	WORD wBaseEnergyOffence;
	WORD wLastEnergyOffence;
	WORD wBaseEnergyDefence;
	WORD wLastEnergyDefence;

	WORD wBaseAttackRate;
	WORD wLastAttackRate;
	WORD wBaseDodgeRate;
	WORD wLastDodgeRate;
	WORD wBaseBlockRate;
	WORD wLastBlockRate;

	WORD wBaseCurseSuccessRate;
	WORD wLastCurseSuccessRate;
	WORD wBaseCurseToleranceRate;
	WORD wLastCurseToleranceRate;

	WORD unknown_rate1;// ??
	WORD unknown_rate2;// ??

	WORD wBasePhysicalCriticalRate;
	WORD wLastPhysicalCriticalRate;
	WORD wBaseEnergyCriticalRate;
	WORD wLastEnergyCriticalRate;

	float unknown_float1[2];//??
	float unknown_float2[2];//??

	float fBaseRunSpeed;
	float fLastRunSpeed;

	float fBaseAirSpeed;
	float fLastAirSpeed;
	float fBaseAirDashSpeed;
	float fLastAirDashSpeed;
	float fBaseAirDash2Speed;
	float fLastAirDash2Speed;
	

	WORD wBaseAttackSpeedRate;
	WORD wLastAttackSpeedRate;
	float fBaseAttackRange;
	float fLastAttackRange;

	float fCastingTimeChangePercent;
	float fCoolTimeChangePercent;
	float fKeepTimeChangePercent;
	float fDotValueChangePercent;
	float fDotTimeChangeAbsolute;
	float fRequiredEpChangePercent;



	float fHonestOffence;
	float fHonestDefence;
	float fStrangeOffence;
	float fStrangeDefence;
	float fWildOffence;
	float fWildDefence;
	float fEleganceOffence;
	float fEleganceDefence;
	float fFunnyOffence;
	float fFunnyDefence;

	float unknown2;//??

	WORD wParalyzeToleranceRate;
	WORD wTerrorToleranceRate;
	WORD wConfuseToleranceRate;
	WORD wStoneToleranceRate;
	WORD wCandyToleranceRate;

	float fParalyzeKeepTimeDown;
	float fTerrorKeepTimeDown;
	float fConfuseKeepTimeDown;
	float fStoneKeepTimeDown;
	float fCandyKeepTimeDown;
	float fBleedingKeepTimeDown;
	float fPoisonKeepTimeDown;
	float fStomachacheKeepTimeDown;

	float fCriticalBlockSuccessRate;

	WORD wGuardRate;
	WORD unknown6;

	float fSkillDamageBlockModeSuccessRate;
	float fCurseBlockModeSuccessRate;
	float fKnockdownBlockModeSuccessRate;

	float fHtbBlockModeSuccessRate;//bleeding defese
	float fSitDownLpRegenBonusRate;// posion defese
	float fSitDownEpRegenBonusRate;// defesa sangramento
	float fPhysicalCriticalDamageBonusRate; //burn defese

	float fEnergyCriticalDamageBonusRate;
	float fItemUpgradeBonusRate;
	float fItemUpgradeBreakBonusRate;

	float unknown3[14];//??
	WORD unknown4[7];//??
	float unknown5[6];//??
};

struct sAVATAR_ATTRIBUTE_LINK
{
	WORD* pbyBaseStr;
	WORD* pbyLastStr;
	WORD* pbyBaseCon;
	WORD* pbyLastCon;
	WORD* pbyBaseFoc;
	WORD* pbyLastFoc;
	WORD* pbyBaseDex;
	WORD* pbyLastDex;
	WORD* pbyBaseSol;
	WORD* pbyLastSol;
	WORD* pbyBaseEng;
	WORD* pbyLastEng;

	DWORD* pwBaseMaxLP;
	DWORD* pwLastMaxLP;
	WORD* pwBaseMaxEP;
	WORD* pwLastMaxEP;
	DWORD* pwBaseMaxAp;
	DWORD* pwLastMaxAp;
	WORD* pwBaseMaxRP;
	WORD* pwLastMaxRP;

	WORD* pwBaseLpRegen;
	WORD* pwLastLpRegen;
	WORD* pwBaseLpSitdownRegen;
	WORD* pwLastLpSitdownRegen;
	WORD* pwBaseLpBattleRegen;
	WORD* pwLastLpBattleRegen;

	WORD* pwBaseEpRegen;
	WORD* pwLastEpRegen;
	WORD* pwBaseEpSitdownRegen;
	WORD* pwLastEpSitdownRegen;
	WORD* pwBaseEpBattleRegen;
	WORD* pwLastEpBattleRegen;

	WORD* pwBaseApRegen;
	WORD* pwLastApRegen;
	WORD* pwBaseApSitdownRegen;
	WORD* pwLastApSitdownRegen;
	WORD* pwBaseApBattleRegen;
	WORD* pwLastApBattleRegen;

	WORD* pwunknown_int16[4];//????

	WORD* pwBaseRpRegen;
	WORD* pwLastRpRegen;
	WORD* pwLastRpDimimutionRate;

	WORD* pwBasePhysicalOffence;
	WORD* pwLastPhysicalOffence;
	WORD* pwBasePhysicalDefence;
	WORD* pwLastPhysicalDefence;

	WORD* pwBaseEnergyOffence;
	WORD* pwLastEnergyOffence;
	WORD* pwBaseEnergyDefence;
	WORD* pwLastEnergyDefence;

	WORD* pwBaseAttackRate;
	WORD* pwLastAttackRate;
	WORD* pwBaseDodgeRate;
	WORD* pwLastDodgeRate;
	WORD* pwBaseBlockRate;
	WORD* pwLastBlockRate;

	WORD* pwBaseCurseSuccessRate;
	WORD* pwLastCurseSuccessRate;
	WORD* pwBaseCurseToleranceRate;
	WORD* pwLastCurseToleranceRate;

	WORD* pwunknown_rate1;// ??
	WORD* pwunknown_rate2;// ??

	WORD* pwBasePhysicalCriticalRate;
	WORD* pwLastPhysicalCriticalRate;
	WORD* pwBaseEnergyCriticalRate;
	WORD* pwLastEnergyCriticalRate;

	float* pfunknown_float1[2];//??
	float* pfunknown_float2[2];//??

	float* pfBaseRunSpeed;
	float* pfLastRunSpeed;

	float* pfBaseAirSpeed;
	float* pfLastAirSpeed;
	float* pfBaseAirDashSpeed;
	float* pfLastAirDashSpeed;
	float* pfBaseAirDash2Speed;
	float* pfLastAirDash2Speed;

	WORD* pwBaseAttackSpeedRate;
	WORD* pwLastAttackSpeedRate;
	float* pfBaseAttackRange;
	float* pfLastAttackRange;

	float* pfCastingTimeChangePercent;
	float* pfCoolTimeChangePercent;
	float* pfKeepTimeChangePercent;
	float* pfDotValueChangePercent;
	float* pfDotTimeChangeAbsolute;
	float* pfRequiredEpChangePercent;

	float* pfHonestOffence;
	float* pfHonestDefence;
	float* pfStrangeOffence;
	float* pfStrangeDefence;
	float* pfWildOffence;
	float* pfWildDefence;
	float* pfEleganceOffence;
	float* pfEleganceDefence;
	float* pfFunnyOffence;
	float* pfFunnyDefence;

	float* pfunknown2;//??

	WORD* pwParalyzeToleranceRate;
	WORD* pwTerrorToleranceRate;
	WORD* pwConfuseToleranceRate;
	WORD* pwStoneToleranceRate;
	WORD* pwCandyToleranceRate;

	float* pfParalyzeKeepTimeDown;
	float* pfTerrorKeepTimeDown;
	float* pfConfuseKeepTimeDown;
	float* pfStoneKeepTimeDown;
	float* pfCandyKeepTimeDown;
	float* pfBleedingKeepTimeDown;
	float* pfPoisonKeepTimeDown;
	float* pfStomachacheKeepTimeDown;

	float* pfCriticalBlockSuccessRate;

	WORD*  pwGuardRate;
	WORD*  pwunknown6;

	float* pfSkillDamageBlockModeSuccessRate;
	float* pfCurseBlockModeSuccessRate;
	float* pfKnockdownBlockModeSuccessRate;
	float* pfHtbBlockModeSuccessRate;

	float* pfSitDownLpRegenBonusRate;
	float* pfSitDownEpRegenBonusRate;

	float* pfPhysicalCriticalDamageBonusRate;
	float* pfEnergyCriticalDamageBonusRate;

	float* pfItemUpgradeBonusRate;
	float* pfItemUpgradeBreakBonusRate;

	float* pfunknown3[14];//??
	WORD*  pwunknown4[7];//??
	float* pfunknown5[6];//??
};

#pragma pack(pop)

class CNtlBitFlagManager;

class CNtlAvatar
{
public:
	struct sATTRIBUTE_LOGIC
	{
		DWORD dwFieldOffset;
		DWORD (*pCopyAttributeFunction)(void* pvBuffer, void* pvValue);
	};

protected:
	CNtlAvatar(void);
public:
	virtual ~CNtlAvatar(void);

protected:
	void Init();

	virtual void InitializeAttributeLink();

public:
	static CNtlAvatar* GetInstance();

public:
	// 패킷의 raw 데이타로부터 sAVATAR_ATTRIBUTE 구조체를 업데이트한다.(주로 클라이언트에서 사용하게 된다.)
	// Updates sAVATAR_ATTRIBUTE structure with using raw data in a packet.(This function will be used mainly on client-side.)
	//
	// (패킷 -> 메타 데이타 + sAVATAR_ATTRIBUTE)
	// (Packet -> Meta data + sAVATAR_ATTRIBUTE)
	//
	static bool UpdateAvatarAttribute(BYTE byAttributeTotalCount, void* pvRawData, sAVATAR_ATTRIBUTE* pAttributeData);

	// 속성 데이타를 패킷의 raw 데이타로 저장한다.(주로 서버에서 사용하게 된다.)
	// Generates raw data in a packet with using attribute data.(This function will be used mainly on server-side.)
	//
	// (메타 데이타 + 속성 데이타 -> 패킷)
	// (Meta data + Attribute data -> Packet)
	//
	static bool SaveAvatarAttribute(CNtlBitFlagManager* pChangedFlag, sAVATAR_ATTRIBUTE_LINK* pAttributeDataLink, void* pvBuffer, DWORD* pwdDataSize);

	// 속성 데이타를 sAVATAR_ATTRIBUTE 구조체로 복사한다.(주로 서버에서 사용하게 된다.)
	// Copies attribute data into sAVATAR_ATTRIBUTE structure.(This function will be used mainly on server-side.)
	//
	// (속성 데이타 -> sAVATAR_ATTRIBUTE)
	// (Attribute data -> sAVATAR_ATTRIBUTE)
	//
	static bool FillAvatarAttribute(sAVATAR_ATTRIBUTE_LINK* pAttributeDataLink, sAVATAR_ATTRIBUTE* pAttributeData);
    //Convert Attributes - Luiz45
	static sAVATAR_ATTRIBUTE_LINK ConvertAttributeToAttributeLink(sAVATAR_ATTRIBUTE* pAttribute);

protected:
	static sATTRIBUTE_LOGIC m_attributeLogic[ATTRIBUTE_TO_UPDATE_COUNT];

	static DWORD CopyValueByType_BYTE(void* pvValue, void* pvBuffer);
	static DWORD CopyValueByType_WORD(void* pvValue, void* pvBuffer);
	static DWORD CopyValueByType_float(void* pvValue, void* pvBuffer);
};