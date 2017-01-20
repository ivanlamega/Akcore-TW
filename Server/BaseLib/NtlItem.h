#pragma once
#include "SharedType.h"
#include "SharedDef.h"
#include "Vector.h"
#include "Quest.h"
#include "NtlStringHandler.h"


//-----------------------------------------------------------------------------------
// ������ Ÿ��
//-----------------------------------------------------------------------------------
enum eITEM_TYPE
{
	ITEM_TYPE_GLOVE,
	ITEM_TYPE_STAFF,
	ITEM_TYPE_GUN,
	ITEM_TYPE_DUAL_GUN,
	ITEM_TYPE_CLAW,
	ITEM_TYPE_AXE,
	ITEM_TYPE_SCROLL,
	ITEM_TYPE_GEM,
	ITEM_TYPE_STICK,
	ITEM_TYPE_SWORD,
	ITEM_TYPE_FAN,
	ITEM_TYPE_WAND,
	ITEM_TYPE_BAZOOKA,
	ITEM_TYPE_BACK_PACK,
	ITEM_TYPE_INSTRUMENT,
	ITEM_TYPE_CLUB,
	ITEM_TYPE_DRUM,
	ITEM_TYPE_MASK,

	ITEM_TYPE_JACKET,
	ITEM_TYPE_PANTS,
	ITEM_TYPE_BOOTS,

	ITEM_TYPE_NECKLACE,
	ITEM_TYPE_EARRING,
	ITEM_TYPE_RING,

	ITEM_TYPE_SCOUTER,
	ITEM_TYPE_SCOUTER_PART,
	
	ITEM_TYPE_COSTUME,
	ITEM_TYPE_BAG,
	ITEM_TYPE_QUEST,
	ITEM_TYPE_STONE,

	ITEM_TYPE_RECOVER,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_UTILITY,
	ITEM_TYPE_CHARM,
	ITEM_TYPE_CAPSULE,			// VEHICLE Ż���̳� ȣ�����̸ӽ� ĸ���Ͽ콺�� ������ �����Ǵ� ĸ�����·ε� ������
	ITEM_TYPE_FUEL,
	ITEM_TYPE_JUNK,
	ITEM_TYPE_COLLECTION,
	ITEM_TYPE_WAREHOUSE,			// â�� �߰��� ��������ۿ� ����Ұ���
	ITEM_TYPE_DRAGONBALL,
	ITEM_TYPE_GAMBLE,
	ITEM_TYPE_MATERIAL,			// �������
	ITEM_TYPE_RECIPE,			
	ITEM_TYPE_HOIPOIROCK,
	ITEM_TYPE_DOGI,
	ITEM_TYPE_PURE_STONE,
	ITEM_TYPE_BLACK_STONE,
	ITEM_TYPE_NETPYSTORE,

	ITEM_TYPE_COUNT,
	ITEM_TYPE_UNKNOWN    = 0xFF,

	ITEM_TYPE_WEAPON_FIRST = ITEM_TYPE_GLOVE,
	ITEM_TYPE_WEAPON_LAST  = ITEM_TYPE_MASK,

	ITEM_TYPE_ARMOR_FIRST = ITEM_TYPE_JACKET,
	ITEM_TYPE_ARMOR_LAST  = ITEM_TYPE_BOOTS,

	ITEM_TYPE_GROUP_WEAPON = 0,
	ITEM_TYPE_GROUP_ARMOR  = 1,
	ITEM_TYPE_GROUP_ETC	   = 2,

	ITEM_TYPE_FIRST = ITEM_TYPE_GLOVE,
	ITEM_TYPE_LAST = ITEM_TYPE_COUNT - 1
};

//-----------------------------------------------------------------------------------
// �Ⱓ ������ ���� ���
//-----------------------------------------------------------------------------------
enum eDURATION_ITEM_PAY_TYPE
{
	DURATION_ITEM_PAY_TYPE_ZENNY,	// ����
	DURATION_ITEM_PAY_TYPE_NETPY,	// ����
	DURATION_ITEM_PAY_TYPE_CASH,	// ĳ��
};
//-----------------------------------------------------------------------------------
// ���� ������ Ÿ��
//-----------------------------------------------------------------------------------
enum eEQUIP_TYPE
{
	EQUIP_TYPE_MAIN_WEAPON,
	EQUIP_TYPE_SUB_WEAPON,
	EQUIP_TYPE_ARMOR,
	EQUIP_TYPE_SCOUTER,					// Scoute Body ( ��ī���� ���� ���°� )
	EQUIP_TYPE_COSTUME,
	EQUIP_TYPE_ACCESSORY,

	EQUIP_TYPE_COUNT,
	EQUIP_TYPE_UNKNOWN				= 0xFF,

	EQUIP_TYPE_FIRST = EQUIP_TYPE_MAIN_WEAPON,
	EQUIP_TYPE_LAST = EQUIP_TYPE_COUNT - 1
};


//-----------------------------------------------------------------------------------
// ������ ���
//-----------------------------------------------------------------------------------
enum eITEM_RANK
{
	ITEM_RANK_NOTHING,
	ITEM_RANK_NORMAL,
	ITEM_RANK_SUPERIOR,
	ITEM_RANK_EXCELLENT,
	ITEM_RANK_RARE,
	ITEM_RANK_LEGENDARY,

	ITEM_RANK_COUNT,
	ITEM_RANK_UNKNOWN				= 0xFF,

	ITEM_RANK_FIRST = ITEM_RANK_NOTHING,
	ITEM_RANK_LAST = ITEM_RANK_LEGENDARY
};

//-----------------------------------------------------------------------------------
// ������ �� Ÿ��
//-----------------------------------------------------------------------------------
enum eITEM_MODEL_TYPE
{
	ITEM_MODEL_TYPE_NONE,
	ITEM_MODEL_TYPE_RACE_GENDER,
	ITEM_MODEL_TYPE_RACE_GENDER_SKIN,

	ITEM_MODEL_TYPE_COUNT,
	ITEM_MODEL_TYPE_UNKNOWN				= 0xFF,

	ITEM_MODEL_TYPE_FIRST = ITEM_MODEL_TYPE_NONE,
	ITEM_MODEL_TYPE_LAST  = ITEM_MODEL_TYPE_RACE_GENDER_SKIN
};

//-----------------------------------------------------------------------------------
// ������ �� ������ ����
//-----------------------------------------------------------------------------------
enum eITEM_BAG_SIZE
{
	ITEM_BAG_SIZE_4_BY_1			= 4,
	ITEM_BAG_SIZE_4_BY_2			= 8,
	ITEM_BAG_SIZE_4_BY_3			= 12,
	ITEM_BAG_SIZE_4_BY_4			= 16,
};

//-----------------------------------------------------------------------------------
// ������ UPGRADE GRADE����
//-----------------------------------------------------------------------------------
enum eITEM_GRADE_LEVEL
{
	ITEM_GRADE_LEVEL_0 = 0,		
	ITEM_GRADE_LEVEL_1,			
	ITEM_GRADE_LEVEL_2,			
	ITEM_GRADE_LEVEL_3,			
	ITEM_GRADE_LEVEL_4,			

	ITEM_GRADE_LEVEL_INVALID	= 0xFF
};
//-----------------------------------------------------------------------------------
// �����̳� Ÿ��
//-----------------------------------------------------------------------------------
enum eCONTAINER_TYPE
{
	//DB Table( Item )�� ����
	CONTAINER_TYPE_BAGSLOT,				// ������ ���� (0~5) ������ ������ Ȱ��ȭ
	CONTAINER_TYPE_BAG1,				// ���� Bag�� Ȱ��ȭ �Ȱ�� �� ���濡 ���� �κ���...	
	CONTAINER_TYPE_BAG2,
	CONTAINER_TYPE_BAG3,
	CONTAINER_TYPE_BAG4,
	CONTAINER_TYPE_BAG5,

	CONTAINER_TYPE_NETPYSTORE,

	CONTAINER_TYPE_EQUIP,
	CONTAINER_TYPE_SCOUT,				// Place �� 7�� ����� ���� ���� Scouter Parts �� ��� ����

	//DB Table(Bank)�� ����
	CONTAINER_TYPE_BANKSLOT,
	CONTAINER_TYPE_BANK1,
	CONTAINER_TYPE_BANK2,
	CONTAINER_TYPE_BANK3,
	CONTAINER_TYPE_BANK4,

	//DB Table(Guild Bank)�� ����
	CONTAINER_TYPE_GUILD1,
	CONTAINER_TYPE_GUILD2,
	CONTAINER_TYPE_GUILD3,

	CONTAINER_TYPE_COUNT,
	CONTAINER_TYPE_NONE = 0xFF,

	CONTAINER_TYPE_BAG_FIRST = CONTAINER_TYPE_BAG1,
	CONTAINER_TYPE_BAG_LAST = CONTAINER_TYPE_NETPYSTORE,

	CONTAINER_TYPE_INVEN_FIRST = CONTAINER_TYPE_BAGSLOT,
	CONTAINER_TYPE_INVEN_LAST = CONTAINER_TYPE_SCOUT,
	
	CONTAINER_TYPE_BANK_FIRST = CONTAINER_TYPE_BANKSLOT,
	CONTAINER_TYPE_BANK_LAST  = CONTAINER_TYPE_BANK4,

	CONTAINER_TYPE_GUILD_FIRST = CONTAINER_TYPE_GUILD1,
	CONTAINER_TYPE_GUILD_LAST  = CONTAINER_TYPE_GUILD3,

	CONTAINER_TYPE_FIRST = CONTAINER_TYPE_BAGSLOT,
	CONTAINER_TYPE_LAST = CONTAINER_TYPE_BANK4,

};

//-----------------------------------------------------------------------------------
// Bag Slot Position
//-----------------------------------------------------------------------------------
enum eBAGSLOT_POSITION
{
	BAGSLOT_POSITION_BAGSLOT_POSITION_0 = 0,
	BAGSLOT_POSITION_BAGSLOT_POSITION_1,
	BAGSLOT_POSITION_BAGSLOT_POSITION_2,
	BAGSLOT_POSITION_BAGSLOT_POSITION_3,
	BAGSLOT_POSITION_BAGSLOT_POSITION_4,

	BAGSLOT_POSITION_BAGSLOT_POSITION_NETPYSTORE = 5,

	BAGSLOT_POSITION_NONE = 0xFF,
};


//-----------------------------------------------------------------------------------
// ���� ������ ����
//-----------------------------------------------------------------------------------
enum eEQUIP_SLOT_TYPE
{
	EQUIP_SLOT_TYPE_HAND, 
	EQUIP_SLOT_TYPE_SUB_WEAPON,
	EQUIP_SLOT_TYPE_JACKET,
	EQUIP_SLOT_TYPE_PANTS,
	EQUIP_SLOT_TYPE_BOOTS,
	EQUIP_SLOT_TYPE_SCOUTER,
	EQUIP_SLOT_TYPE_QUEST,
	EQUIP_SLOT_TYPE_NECKLACE,
	EQUIP_SLOT_TYPE_EARRING_1,
	EQUIP_SLOT_TYPE_EARRING_2,
	EQUIP_SLOT_TYPE_RING_1,
	EQUIP_SLOT_TYPE_RING_2,

	EQUIP_SLOT_TYPE_DOGI,
	EQUIP_SLOT_TYPE_HAIR,
	EQUIP_SLOT_TYPE_ACCESSORY_1,
	EQUIP_SLOT_TYPE_ACCESSORY_2,
	EQUIP_SLOT_TYPE_ACCESSORY_3,

	EQUIP_SLOT_TYPE_COUNT,
	EQUIP_SLOT_TYPE_UNKNOWN	= 0xFF,

	EQUIP_SLOT_TYPE_FIRST	= EQUIP_SLOT_TYPE_HAND,
	EQUIP_SLOT_TYPE_LAST	= EQUIP_SLOT_TYPE_RING_2,
};

//-----------------------------------------------------------------------------------
// Costume�� �����ϰų� ������ ĳ���Ϳ��� ���� detach, attach�� �Ǿ�� �� Item�鿡
// ���� BitFlagSet�� Ȱ���ϵ��� �ϸ� ������ ��ȹ��Ʈ�� BitFlagTool�� �̿��Ͽ� Set��
// ���鶧 �ʿ��� ������� �����ϸ� ���α׷����� Ȯ���Ҽ� �ִ� �� FLAG�� ���ؼ��� 
// 0x01�� SHIFT�Ͽ� ����Ѵ�. ��� 32�������� �߰������ɼ� ������ �߰��������׿�
// ���ؼ��� �ſ� ���Ǹ� ���Ѵ�.
//
// 2008.03.24 ���
//-----------------------------------------------------------------------------------
enum eCOSTUME_BITFLAG_SLOT_TYPE
{
	eCBST_JACKET,
	eCBST_PANTS,
	eCBST_BOOTS,
	eCBST_TWO_HAND,
	eCBST_SUB_WEAPON_BACK,
	eCBST_HEAD,
	eCBST_FACE,
	eCBST_SCOUTER_AT_EYE,
};

enum eCOSTUME_BITFLAG_SLOT_TYPE_SHIFT
{
	eCBSTS_JACKET			= 0x01 << eCBST_JACKET,
	eCBSTS_PANTS			= 0x01 << eCBST_PANTS,
	eCBSTS_BOOTS			= 0x01 << eCBST_BOOTS,
	eCBSTS_TWO_HAND			= 0x01 << eCBST_TWO_HAND,
	eCBSTS_SUB_WEAPON_BACK	= 0x01 << eCBST_SUB_WEAPON_BACK,
	eCBSTS_HEAD				= 0x01 << eCBST_HEAD,
	eCBSTS_FACE				= 0x01 << eCBST_FACE,
	eCBSTS_SCOUTER_AT_EYE	= 0x01 << eCBST_SCOUTER_AT_EYE,
};

//-----------------------------------------------------------------------------------
// ������ ���̺� Ÿ��
//-----------------------------------------------------------------------------------
enum eITEM_TABLE_TYPE
{
	ITEM_TABLE_TYPE_ITEM,
	ITEM_TABLE_TYPE_BANK,
	ITEM_TABLE_TYPE_BANK_PUBLIC,
	ITEM_TABLE_TYPE_GUILD_BANK,

	ITEM_TABLE_TYPE_NONE = 0xFF,

	ITEM_TABLE_TYPE_FIRST = ITEM_TABLE_TYPE_ITEM,
	ITEM_TABLE_TYPE_LAST = ITEM_TABLE_TYPE_GUILD_BANK,
};

//-----------------------------------------------------------------------------------
// Bank ������ ���̺� Ÿ��
//-----------------------------------------------------------------------------------
enum eBANK_TABLE_TYPE
{
	BANK_TABLE_TYPE_BASIC,
	BANK_TABLE_TYPE_ADD1,
	BANK_TABLE_TYPE_ADD2,
	BANK_TABLE_TYPE_PUBLIC,

	BANK_TABLE_TYPE_NONE = 0xFF,

	BANK_TABLE_TYPE_FIRST = BANK_TABLE_TYPE_BASIC,
	BANK_TABLE_TYPE_LAST = BANK_TABLE_TYPE_PUBLIC,
};

//-----------------------------------------------------------------------------------
// ���� ������ ���� �˻�� ��Ʈ �÷���
//-----------------------------------------------------------------------------------
enum eEQUIP_SLOT_FLAG
{
	EQUIP_SLOT_FLAG_TYPE_HAND				 = 0x01 << EQUIP_SLOT_TYPE_HAND,
	EQUIP_SLOT_FLAG_TYPE_SUB_WEAPON			 = 0x01 << EQUIP_SLOT_TYPE_SUB_WEAPON,
	EQUIP_SLOT_FLAG_TYPE_JACKET				 = 0x01 << EQUIP_SLOT_TYPE_JACKET,
	EQUIP_SLOT_FLAG_TYPE_PANTS				 = 0x01 << EQUIP_SLOT_TYPE_PANTS,
	EQUIP_SLOT_FLAG_TYPE_BOOTS				 = 0x01 << EQUIP_SLOT_TYPE_BOOTS,
	EQUIP_SLOT_FLAG_TYPE_SCOUTER			 = 0x01 << EQUIP_SLOT_TYPE_SCOUTER,
	EQUIP_SLOT_FLAG_TYPE_QUEST				 = 0x01 << EQUIP_SLOT_TYPE_QUEST,
	EQUIP_SLOT_FLAG_TYPE_NECKLACE			 = 0x01 << EQUIP_SLOT_TYPE_NECKLACE,
	EQUIP_SLOT_FLAG_TYPE_EARRING_1			 = 0x01 << EQUIP_SLOT_TYPE_EARRING_1,
	EQUIP_SLOT_FLAG_TYPE_EARRING_2			 = 0x01 << EQUIP_SLOT_TYPE_EARRING_2,
	EQUIP_SLOT_FLAG_TYPE_RING_1				 = 0x01 << EQUIP_SLOT_TYPE_RING_1,
	EQUIP_SLOT_FLAG_TYPE_RING_2				 = 0x01 << EQUIP_SLOT_TYPE_RING_2,

	EQUIP_SLOT_FLAG_TYPE_DOGI		 = 0x01 << EQUIP_SLOT_TYPE_DOGI,
	EQUIP_SLOT_FLAG_TYPE_HAIR		 = 0x01 << EQUIP_SLOT_TYPE_HAIR,
	EQUIP_SLOT_FLAG_TYPE_ACCESSORY_1 = 0x01 << EQUIP_SLOT_TYPE_ACCESSORY_1,
	EQUIP_SLOT_FLAG_TYPE_ACCESSORY_2 = 0x01 << EQUIP_SLOT_TYPE_ACCESSORY_2,
	EQUIP_SLOT_FLAG_TYPE_ACCESSORY_3 = 0x01 << EQUIP_SLOT_TYPE_ACCESSORY_3,
};

//-----------------------------------------------------------------------------------
// ������ �ɼ� ǰ��
//-----------------------------------------------------------------------------------
enum eOPTION_QUALITY
{
	OPTION_QUALITY_SUPERIOR,
	OPTION_QUALITY_EXCELLENT,
	OPTION_QUALITY_RARE,
	OPTION_QUALITY_LEGENDARY,
	OPTION_QUALITY_SET,

	OPTION_QUALITY_COUNT,
	OPTION_QUALITY_UNKNOWN	= 0xFF,

	OPTION_QUALITY_FIRST	= OPTION_QUALITY_SUPERIOR,
	OPTION_QUALITY_LAST  	= OPTION_QUALITY_SET,
};
//-----------------------------------------------------------------------------------
// ������ ��� Ÿ��
//-----------------------------------------------------------------------------------
enum eITEM_FUNC_TYPE
{
	ITEM_FUNC_TYPE_MAIN_WEAPON_OFF,
	ITEM_FUNC_TYPE_TRADE_FORBIDDEN,
	ITEM_FUNC_TYPE_STORE_FORBIDDEN,
	ITEM_FUNC_TYPE_SELL_FORBIDDEN,
};

//-----------------------------------------------------------------------------------
// ������ ��� Ÿ�� �÷���
//-----------------------------------------------------------------------------------
enum eITEM_FUNC_BIT_FLAG
{
	ITEM_FUNC_BIT_FLAG_MAIN_WEAPON_OFF  = 0x01 << ITEM_FUNC_TYPE_MAIN_WEAPON_OFF,
	ITEM_FUNC_BIT_FLAG_TRADE_FORBIDDEN  = 0x01 << ITEM_FUNC_TYPE_TRADE_FORBIDDEN,
	ITEM_FUNC_BIT_FLAG_STORE_FORBIDDEN  = 0x01 << ITEM_FUNC_TYPE_STORE_FORBIDDEN,
	ITEM_FUNC_BIT_FLAG_SELL_FORBIDDEN   = 0x01 << ITEM_FUNC_TYPE_SELL_FORBIDDEN,
};

//-----------------------------------------------------------------------------------
// ������ �ɼ� ���
//-----------------------------------------------------------------------------------
enum eOPTION_RANK
{
	OPTION_RANK_1 =	1,
	OPTION_RANK_2 =	3,
	OPTION_RANK_3 =	5,
	OPTION_RANK_4 =	10,
	OPTION_RANK_5 =	20,
	OPTION_RANK_6 =	25,
	OPTION_RANK_7 =	36,   // �ɼ� �̴����̺� ���� % ��Ҽ� ��� ��ġ�� 100%

	OPTION_RANK_COUNT = 7,
};
////-----------------------------------------------------------------------------------
//// ȣ������ ������ ���
////-----------------------------------------------------------------------------------
//enum eHOIPOI_RANK		//���׷��̵忡 �ʿ��� ȣ�����̽����� ����� ������ ���� ����� �޸��Ѵ�.
//{
//	HOIPOI_RANK_1 = 15, 
//	HOIPOI_RANK_2 =	100,
//};
//-----------------------------------------------------------------------------------
// ������ �ɼ� Quality���
//-----------------------------------------------------------------------------------
enum eOPTION_QUALITY_RANK		//���׷��̵忡 �ʿ��� ȣ�����̽����� ����� ������ ���� ����� �޸��Ѵ�.
{
	OPTION_QUALITY_RANK_0 = 100,
	OPTION_QUALITY_RANK_1 = 64, 
	OPTION_QUALITY_RANK_2 =	39,
	OPTION_QUALITY_RANK_3 =	19,
	OPTION_QUALITY_RANK_4 =	9,
	OPTION_QUALITY_RANK_5 = 4,
	OPTION_QUALITY_RANK_6 = 1,

	OPTION_QUALITY_RANK_COUNT = 7, 
};
//-----------------------------------------------------------------------------------
// ��ü ������Ʈ Ÿ�� (���� ���� ���ο�)
//-----------------------------------------------------------------------------------
enum eSTUFF_UPDATE_TYPE
{
	STUFF_UPDATE_INCREASE,
	STUFF_UPDATE_DECREASE,
	STUFF_UPDATE_REPLACE,

};

//-----------------------------------------------------------------------------------
// ������ ��� ���� CoolTime Group
//-----------------------------------------------------------------------------------
enum eITEM_COOL_TIME_GROUP
{
	ITEM_COOL_TIME_GROUP_DH_LP = 0,
	ITEM_COOL_TIME_GROUP_DH_EP,
	ITEM_COOL_TIME_GROUP_HOT_LP,
	ITEM_COOL_TIME_GROUP_HOT_EP,
	ITEM_COOL_TIME_GROUP_DH_RESCUE,
	ITEM_COOL_TIME_GROUP_DH_POISON,
	ITEM_COOL_TIME_GROUP_DH_STOMACHACHE,
	ITEM_COOL_TIME_GROUP_TELEPORT,
	ITEM_COOL_TIME_GROUP_CB_OFFENCE,
	ITEM_COOL_TIME_GROUP_CB_DEFENCE,
	ITEM_COOL_TIME_GROUP_HOT_FOOD,
	ITEM_COOL_TIME_GROUP_HOT_DRINK,

	ITEM_COOL_TIME_GROUP_COUNT,

	ITEM_COOL_TIME_GROUP_FIRST = ITEM_COOL_TIME_GROUP_DH_LP,
	ITEM_COOL_TIME_GROUP_LAST = ITEM_COOL_TIME_GROUP_COUNT - 1
};

//-----------------------------------------------------------------------------------
// ������ ��ī���� ���� ��� Ÿ������
//-----------------------------------------------------------------------------------
enum eSCOUTER_PARTS_TYPE
{
	SCOUTER_PARTS_NONE,
	SCOUTER_PARTS_UPGRADE_PREDICTION,
	SCOUTER_PARTS_ITEM_IDENTIFICATION,
	SCOUTER_PARTS_PC_INFO_VIEWER,
	SCOUTER_PARTS_PC_INFO_JAMMING,
	SCOUTER_PARTS_OBJECT_FINDER,
	SCOUTER_PARTS_MAILER,
	SCOUTER_PARTS_TRANSMITTER,
	SCOUTER_PARTS_DRAGON_BALL_SEARCH,
	SCOUTER_PARTS_QUEST_SEARCH,
	
	SCOUTER_PARTS_COUNT,

	SCOUTER_PARTS_FIRST = SCOUTER_PARTS_NONE ,
	SCOUTER_PARTS_LAST = SCOUTER_PARTS_DRAGON_BALL_SEARCH
};
//-----------------------------------------------------------------------------------
// �ɼ� ���̺��� ��ī���� ������ �̴����̺�
//-----------------------------------------------------------------------------------
enum eSCOUTER_INFO
{
	SCOUTER_INFO_BEGIN_DEFAULT_VALUE = 0,
	SCOUTER_INFO_LP = SCOUTER_INFO_BEGIN_DEFAULT_VALUE,
	SCOUTER_INFO_EP,
	SCOUTER_INFO_STR,
	SCOUTER_INFO_CON,
	SCOUTER_INFO_FOC,
	SCOUTER_INFO_DEX,
	SCOUTER_INFO_SOL,
	SCOUTER_INFO_ENG,
	SCOUTER_INFO_ATTACK_SPEED,
	SCOUTER_INFO_ATTACK_RATE,
	SCOUTER_INFO_DODGE_RATE,
	SCOUTER_INFO_BLOCK_RATE,
	SCOUTER_INFO_CURSE_SUCCESS_RATE,
	SCOUTER_INFO_CURSE_TOLERANCE_RATE,
	SCOUTER_INFO_UPGRADE_RATE_UP,			///< ������ Ȯ�� %d ��
	SCOUTER_INFO_ITEM_BREAK_RATE_DOWN,		///< ������ �ı� Ȯ�� -%d

	SCOUTER_INFO_COUNT,
	
	SCOUTER_INFO_FIRST = SCOUTER_INFO_LP,
	SCOUTER_INFO_LAST = SCOUTER_INFO_ITEM_BREAK_RATE_DOWN,
	
	SCOUTER_INFO_BEGIN_ONLY_ALLPY_MAX   = 30,	///< ���Ĵ� ���� ���� ����� ���� �ϳ��� ǥ�õȴ�

	SCOUTER_INFO_BEGIN_SPECIAL_FUNCTION = 100,	///< ���Ĵ� ���̺�� ����� �״�� �����ش�
	SCOUTER_INFO_RESERVATED = SCOUTER_INFO_BEGIN_SPECIAL_FUNCTION,
	SCOUTER_INFO_PC_INFO_JAMMING,									///< ������ ����
	SCOUTER_INFO_OBJECT_FINDER,										///< ���� ���� ã��
	SCOUTER_INFO_CONNECTOR,											///< ������ ����
	SCOUTER_INFO_TRANSMITTER,										///< ���� ����
	SCOUTER_INFO_DRAGON_BALL_SEARCH,								///< �巡�ﺼ ���� ���� ǥ��
	SCOUTER_INFO_ITEM_IDENTIFICATION,								///< ������ ����
	SCOUTER_INFO_PC_INFO_VIEWER,									///< ������ ������
	SCOUTER_INFO_QUEST_SEARCH,										///< �ֺ�����Ʈ �˻�
	
};

// ������ ó�� ��� [7/23/2008 SGpro]
enum eITEM_PROCESS_TYPE
{
	 ITEM_PROCESS_USEITEM = 0 // Item Use�� ���� ��� Replace�� [7/23/2008 SGpro]
};

//-----------------------------------------------------------------------------------
// ������ ������ Ÿ��
//-----------------------------------------------------------------------------------
enum eITEM_RECIPE_TYPE
{
	ITEM_RECIPE_TYPE_FOOD_DRINK,
	ITEM_RECIPE_TYPE_MAIN_WEAPON,
	ITEM_RECIPE_TYPE_ARMOR,
	ITEM_RECIPE_TYPE_ACCESSORY,
	ITEM_RECIPE_TYPE_SCOUTER,
	ITEM_RECIPE_TYPE_SUB_WEAPON,
};

//-----------------------------------------------------------------------------------
// ������ ���Ž� �ʿ��� ����
//-----------------------------------------------------------------------------------
enum eITEM_NEED_FUNCTION
{
	ITEM_NEED_FUNCTION_FOUNDATION,				// ���弳��
	ITEM_NEED_FUNCTION_MAKING_DOGI,				// ��������
	ITEM_NEED_FUNCTION_BUYING_VEHICLE,			// Ż�ͱ���	
	ITEM_NEED_FUNCTION_BUYING_MANUAL,			// ��������

	ITEM_NEED_FUNCTION_FIRST = ITEM_NEED_FUNCTION_FOUNDATION ,
	ITEM_NEED_FUNCTION_LAST  = ITEM_NEED_FUNCTION_BUYING_MANUAL
};

//-----------------------------------------------------------------------------------
// ������ ���� ����
//-----------------------------------------------------------------------------------
const int			NTL_MAX_ITEM_STACK_COUNT	= 20; // ���� �������� �ִ� ���� ����

const DWORD			NTL_DEF_ITEM_ATTACK_SPEEED = 1000; // �������� ���� ���� ���ǵ�

const int			NTL_MAX_SYSTEM_EFFECT_COUNT	= 4;

const int			NTL_MAX_OPTION_COUNT		= 21; // �ɼǾ������� ���� �ɼ� �ִ� ���� 

const int			NTL_DEF_LUCKY_CHANCE_RATE   = 10;  // ��Ű ����ÿ� �ö󰡴� ��� ����

const int			NTL_DEF_MULTI_OPTION_RATE   = 30;  // superial item �� multi option ����

const int			NTL_MAX_MERCHANT_COUNT		= 24;  // �����ǿ� �����ϴ� �ִ밹��

//const int			NTL_MAX_BASIC_DROP			= 12;  // �⺻ ������ ��� �ִ��

const int			NTL_MAX_SUPERIOR_DROP		= 18;  // Superior ������ ��� �ִ��

const int			NTL_MAX_LEGENDARY_DROP		= 18;  // ����ũ ������ ��� �ִ�� 

const int			NTL_MAX_NORMAL_DROP			= 18;  // �븻 ������ ��� �ִ�� 

const int			NTL_MAX_EXCELLENT_DROP		= 18;  // ������Ʈ ������ ��� �ִ�� 

const int			NTL_MAX_EACH_DROP			= 12;  // ��ġ ������ ��� �ִ�� 

const int			NTL_MAX_TYPE_DROP			= 12;  // Ÿ�� ������ ��� �ִ�� 

const int			NTL_MAX_DROP_TABLE_SELECT	= 8;   // ������ ��� ���� �ִ��

const int			NTL_MAX_IDENTIFY_DROP_RATE	= 10;  // Unidentifyed ������ ��� Ȯ��% 

const int			NTL_TICK_OF_DROP_OWNNER		= 30000; // �����������ð� 30000�̸���ũ �� 30��

const int			NTL_TICK_OF_DROP_DISAPPEAR	= 40000; // ������ ������½ð� 40��	

const int			NTL_MAX_MERCHANT_TAB_COUNT  = 6; 

const int			NTL_DEF_OPTION_LEVEL_COUNT  = 20;   // 100Level �� ��� 5������ 20���� 

const DWORD			NTL_MAX_EFFECT_IN_ITEM = 2;		// �������� ���� �� �ִ� system effect�� �ִ� ����

const DWORD			NTL_MAX_OPTION_IN_ITEM = 2;		// �������� ���� �� �ִ� Option�� �ִ� ����

const float			NTL_MAX_MERCHANT_DISTANCE = 3.0f;	// ������ ���ΰ��� ��� �ִ� �Ÿ�

const float			NTL_MAX_MERCHANT_DISTANCE_EXTRA = 1.0f;	// ������ ���ΰ��� ��� �ִ� �Ÿ��� ���� ����(for sync.)

const float			NTL_MAX_NAMEKAN_DISTANCE = 15.0f;	// ������ ����ĭ������ ��� �ִ� �Ÿ�

const float			NTL_MAX_BUS_DISTANCE = 20.0f;		// PC BUS�� ��� �ִ� �Ÿ�

const float			NTL_MAX_LOOTING_DISTANCE = 5.0f;	// �������̳� ���ϸ� �ֿ� ���� ��� �ִ� �Ÿ�

const float			NTL_MAX_LOOTING_DISTANCE_EXTRA = 1.0f;	// �������̳� ���ϸ� �ֿ� ���� ��� �ִ� �Ÿ��� ���� ����(for sync.)

const float			NTL_MAX_OBJECT_DISTANCE = 5.0f;	// �������̳� ���ϸ� �ֿ� ���� ��� �ִ� �Ÿ�

const float			NTL_MAX_OBJECT_DISTANCE_EXTRA = 1.0f;	// �������̳� ���ϸ� �ֿ� ���� ��� �ִ� �Ÿ��� ���� ����(for sync.)

const BYTE			NTL_DUR_UNKNOWN				= 0xFF;

const int			NTL_UNIDENTIFY_LEGNEDARY_RATE = 50;

const int			NTL_UNIDENTIFY_RARE_RATE      = 30;

const int			NTL_UNIDENTIFY_EXCELLENT_RATE = 20;

const int			NTL_UNIDENTIFY_SUPERIOR_RATE  = 10;

const int			NTL_UNIDENTIFY_NORMAL_RATE  = 5;

const int			NTL_ITEM_MAX_GRADE			= 15;

const int			NTL_ITEM_MAX_RANK			= ITEM_RANK_LAST;	// NONE ������ �� ������ ����

const int			NTL_ITEM_COUNT_GRADE		= 5;

const int			NTL_ITEM_UPGRADE_EQUIP_COUNT = 5;	// Cheat �� �ø� ����� �� ����

const int			NTL_ITEM_OPTION_BIND_LEVEL	= 5;	//  �ɼ��� ���϶� ���� ���� ���� 

const DWORD			NTL_ITEM_ALL_USE_FLAG       = 0xFFFFFFFF;

const BYTE			NTL_UNSTACKABLE_ITEM_COUNT	= 1;

const int			NTL_ITEM_MAX_DRAGONBALL		= 7;

const int			NTL_ITEM_MAX_BALL			= 9;	// ���,��ũ �� ����

const float			DBO_ITEM_CLASS_DEFENCE_BONUS_IN_PERCENT = 20.0f;		// ������ ���� Ưȭ ���ʽ�

const int			NTL_ITEM_MAX_UPGRADE_POSITION = 14;

const int			NTL_SHOP_ITEM_IDENTIFY_ZENNY = 100;

const int			NTL_ITEM_RECIPE_MAX_COUNT = 100;

const int			NTL_ITEM_RECIPE_NEED_ITEM_COUNT = 20;

const DWORD			DBO_MAX_COUNT_RECIPE_MATERIAL_ITEM = 5;

const DWORD			DBO_HOIPOIMIX_BASIC_EXP = 100;

const BYTE			DBO_HOIPOIMIX_CHANGE_EP_RATE = 10;			// ������ �ͽ��� ��� EP�� ���� �����Ƿ����� DBO_HOIPOIMIX_CHANGE_EP_RATE �� ���Ѱ�

const BYTE			DBO_HOIPOIMIX_CASH_RATE_MIN = 50;

const BYTE			DBO_HOIPOIMIX_CASH_RATE_MAX = 200;

const BYTE			DBO_HOIPOIMIX_MIX_LEVEL_MAX = 100;
//-----------------------------------------------------------------------------------
// ����īƮ ����ü
//-----------------------------------------------------------------------------------
struct sSHOP_BUY_CART
{
	BYTE byMerchantTab;
	BYTE byItemPos;
	BYTE byStack;
};
struct sSHOP_SELL_CART
{
	BYTE byPlace;
	BYTE byPos;
	BYTE byStack;
};

struct sEMPTY_INVEN
{
	BYTE byPlace;
	BYTE byPos;
};
#pragma pack(1)

struct sSHOP_BUY_INVEN
{
	BYTE byPlace;
	BYTE byPos;
	BYTE byStack;
	TBLIDX tblItem;
	BYTE byRank;
	BYTE byCurrentDurability;
	BYTE byGrade;
	BYTE byRestrictType;			// �ͼӻ��� eITEM_RESTRICT_TYPE
	WCHAR awchMaker[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1]; // ������

	TBLIDX aOptionTblidx[NTL_MAX_OPTION_IN_ITEM];

	BYTE byDurationType; //eDURATIONTYPE
	DBOTIME nUseStartTime;// �κ��丮�� ���� ��¥
	DBOTIME nUseEndTime;// ��븸�� �Ⱓ
};
#pragma pack()
struct sSHOP_SELL_INVEN
{
	ITEMID itemID;
	BYTE   byStack;
	BYTE   byPlace;
	BYTE   byPos;
};


//-----------------------------------

struct sINVEN_SLOT
{
	BYTE	byPlace;
	BYTE	byPos;
};

struct sITEMID_SLOT : public sINVEN_SLOT
{
	HOBJECT	hItem;
	ITEMID	itemId;
};

struct sHITEM_SLOT : public sINVEN_SLOT
{
	HOBJECT	hItem;
};

// query���� ��ſ��� ���
struct sITEM_MOVE_DATA
{
	sITEMID_SLOT	sSrc;
	sITEMID_SLOT	sDest;
};

// client���� ��ſ��� ���
struct sITEM_MOVE_INFO
{
	sHITEM_SLOT		sSrc;	// �̵��� �����۰� �̵��� �������� �ִ� ��ġ.
	sHITEM_SLOT		sDest;	// �������� �̹� �ִ� �����۰� �������� ��ġ
};


//-----------------------------------------------------------------------------------
// �κ��丮 �˻� Ÿ�� (���� ���ο�)
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// �κ��丮 ���� ����
//-----------------------------------------------------------------------------------
const int			NTL_MAX_BAGSLOT_COUNT		=  6; // ���� ������ ����

const int			NTL_MAX_BANKSLOT_COUNT		=  4; // â�� ������ ����

const int			NTL_MAX_GUILD_BANK_COUNT	=  3; // ��� â�� ����

const int			NTL_MAX_EQUIP_ITEM_SLOT		= EQUIP_SLOT_TYPE_COUNT;

const int			NTL_MAX_ITEM_SLOT			= 16; // ������ �ִ� ũ��

const int			NTL_MAX_BAG_ITEM_SLOT		= 16; // ������ �ִ� ũ��

const int			NTL_MAX_SCOUTER_ITEM_SLOT	= 16; // ��ī���� ������ �ִ� ũ��

const int			NTL_MAX_BANK_ITEM_SLOT		= 16; // ������ �ִ� ũ��

const int			NTL_MAX_GUILD_ITEM_SLOT		= 16; // ��� â���� �κ� �ִ� ũ��

const int			NTL_MAX_NETPYSTORE_ITEM_SLOT = 16; //Netpy Store�� �κ� �ִ� ũ��

const int			NTL_MAX_COUNT_GUILD_HAVE_TOTAL_ITEM	= NTL_MAX_GUILD_BANK_COUNT * NTL_MAX_GUILD_ITEM_SLOT; // ��� â���� �ִ� ũ��


const int			NTL_MAX_BUY_SHOPPING_CART	= 12;  // ����īƮ�� ����ִ� �ִ� ũ��

const int			NTL_MAX_SELL_SHOPPING_CART	= 12;  // ����īƮ�� �ȼ��ִ� �ִ� ũ��

const int			NTL_MAX_SHOPPING_CART		= ( (NTL_MAX_BUY_SHOPPING_CART * sizeof(sSHOP_BUY_CART) ) + (NTL_MAX_SELL_SHOPPING_CART * sizeof(sSHOP_SELL_CART) ) );  // ����īƮ�� �ִ� ũ��

const int			NTL_MAX_NEWBIE_ITEM			= 10;  // ������ �ο����� �ִ� ������

const int			NTL_MAX_NEWBIE_SKILL		= 7;  // ������ �ο����� �ִ� ��ų

const int			NTL_MAX_SCOUTER_WATT		= 1000;

const int			NTL_MAX_SET_ITEM_COUNT		= 3;  // jacket pants boots

const int			NTL_SET_ITEM_SOLO_COUNT		= 1;  // solo count

const int			NTL_SET_ITEM_SEMI_COUNT		= 2;  // semi count

const int			NTL_SET_ITEM_FULL_COUNT		= 3;  // full count

const int			NTL_MAX_BUY_HLSSHOP_CART	= 10;  // full count
//===================================================================================
// ȣ������ ������ �ͽ�
//===================================================================================

enum eITEM_MIX_RESULT_TYPE
{
	ITEM_MIX_RESULT_TYPE_FAIL,				// ����
	ITEM_MIX_RESULT_TYPE_SUCCESS,			// ����
	ITEM_MIX_RESULT_TYPE_GREAT_SUCCESS,		// �뼺��

	ITEM_MIX_RESULT_TYPE_NUMS				// �������� ����
};

enum eRECIPE_TYPE
{
	eRECIPE_NORMAL,
	eRECIPE_MAINWEAPON,		
	eRECIPE_ARMOR,			
	eRECIPE_ACCESSORY,
	eRECIPE_SCOUTER,
	eRECIPE_SUBWEAPON,	

	eRECIPE_INVALID = 0xFF,
};

// ȣ������ �ͽ� ���� ������
struct sHOIPOIMIX_DATA
{
	bool			bNormalStart;	// ��ֵ�Ͽ��� // ����Ʈ ���� Ȥ�� ����
	
	bool			bSpecialStart;	// ���伣 �ͽ� ��Ͽ���
	BYTE			bySpecialType;	// eRECIPE_TYPE

	BYTE			byMixLevel;
	WORD			dwMixExp;
	WORD			unknown;
	
	//BYTE			byNormalLevel;	 
	//DWORD			dwNormalExp;
	//BYTE			bySpecialLevel; 	
	//DWORD			dwSpecialExp;
};

// ȣ������ ������ ������
struct sRECIPE_DATA
{
	TBLIDX		recipeTblidx;
	BYTE		byRecipeType;		//eRECIPE_TYPE
};

struct sITEM_BASIC_DATA
{
	int	   nItemID;		// DB������ ���⿡ int������
	BYTE   byPlace;
	BYTE   byPos;
	BYTE   byStack;		
};

struct sITEM_MIX_MAIN_DATA
{
	sITEM_BASIC_DATA		asBasicData[NTL_MAX_BAGSLOT_COUNT * NTL_MAX_BAG_ITEM_SLOT];
};

//-----------------------------------------------------------------------------------
// ������ ���� ���� ����
//-----------------------------------------------------------------------------------
struct sITEM_REPAIR
{
	ITEMID itemID;
	BYTE byPlace;
	BYTE byPosition;
	BYTE byDur;			// �ø� �ƽ� ������
};

//-----------------------------------------------------------------------------------
// �κ��丮 �˻� Ÿ�� (���� ���ο�)
//-----------------------------------------------------------------------------------
enum eSTUFF_ENABLE_TYPE
{
	eSTUFF_ENABLE_NONE,			// �˻� ���� ���
	eSTUFF_ENABLE_BASIC,		// �⺻ �˻縸	
	eSTUFF_ENABLE_ALL,			// ��� �˻�

};

//-----------------------------------------------------------------------------------
// ������ ��� �Ⱓ
//-----------------------------------------------------------------------------------
enum eDURATIONTYPE
{
	 eDURATIONTYPE_NORMAL = 0
	,eDURATIONTYPE_FLATSUM			//������
	,eDURATIONTYPE_METERRATE		//������

	,eDURATIONTYPE_COUNT
};

//-----------------------------------------------------------------------------------
// �ͼ� ������ ����
//-----------------------------------------------------------------------------------
enum eITEM_RESTRICT_TYPE
{
	ITEM_RESTRICT_TYPE_NONE,			// �ͼ� ������ �ƴ� �˻����
	ITEM_RESTRICT_TYPE_READY,			// �ͼ� ������ (���� �������� ����)
	ITEM_RESTRICT_TYPE_LIMIT,			// �ͼӵ� ( ������ �ͼӵ� )
};


#pragma pack(1)

// ������ DB ����
struct sITEM_DATA
{
	ITEMID			itemId;					// Serial Number
	TBLIDX			itemNo;					// Template Number
	CHARACTERID		charId;					// Owner Serial
	BYTE			byPlace;					// eCONTAINER_TYPE
	BYTE			byPosition;
	BYTE			byStackcount;
	BYTE			byRank;
	BYTE			byCurrentDurability;		// ���� ������ 
	bool			bNeedToIdentify;
	BYTE			byGrade;				// ������ ���׷��̵� ���
	BYTE			byBattleAttribute;		// ����Ϳ��� cf) NtlBattle.h eBATTLE_ATTRIBUTE 

	BYTE			byRestrictType;			// �ͼӻ��� eITEM_RESTRICT_TYPE
	WCHAR			awchMaker[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1]; // ������

	TBLIDX			aOptionTblidx[NTL_MAX_OPTION_IN_ITEM];

	BYTE			byDurationType; //eDURATIONTYPE
	DBOTIME			nUseStartTime;// �κ��丮�� ���� ��¥
	DBOTIME			nUseEndTime;// ��븸�� �Ⱓ

};

//-----------------------------------------------------------------------------------
// ������ �Ⱓ ���� ����
//-----------------------------------------------------------------------------------
struct sITEM_DURATION
{
	ITEMID			itemID;
	BYTE			byPlace;
	BYTE			byPosition;
	DBOTIME			nUseEndTime;// ��븸�� �Ⱓ
};

// ������ ���� ����
struct sITEM_BRIEF
{
	TBLIDX			tblidx; // item table index
	BYTE			byRank;
	BYTE            byGrade; // item effect
	BYTE			byBattleAttribute;
	TBLIDX			aOptionTblidx[NTL_MAX_OPTION_IN_ITEM];
};

struct sITEM_SUMMARY
{

	TBLIDX			tblidx; // item table index
	BYTE			byRank;
	BYTE			byGrade;
	BYTE			byBattleAttribute;

};

// ������ ����
struct sITEM_STATE
{
	sVECTOR3		vCurLoc; // ���� ��ġ
	bool			bNeedToIdentify;
	bool			bIsNew;
};
struct sITEM_EFFECT
{
	WORD            wType;
	DWORD            dwValue;
};


// by Szczeepan
struct sITEM_EXTRA_EFFECT
{
	WORD            wType;
	DWORD            dwValue;
};

// ������ ��ü ����
struct sITEM_PROFILE
{
	HOBJECT            handle;            // 아이템 handle
	TBLIDX            tblidx;            // item table index
	BYTE            byPlace;        // eCONTAINER_TYPE
	BYTE            byPos;
	BYTE            byStackcount;
	BYTE            byRank;
	BYTE            byCurDur;        // 내구도
	bool            bNeedToIdentify;
	BYTE            byGrade;        // 아이템 업그레이드 등급
	BYTE            byBattleAttribute;        // 진기맹여락 cf) NtlBattle.h eBATTLE_ATTRIBUTE


	BYTE            byRestrictType;            // 귀속상태 eITEM_RESTRICT_TYPE
	WCHAR            awchMaker[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1]; // 제작자


	TBLIDX            aOptionTblidx[NTL_MAX_OPTION_IN_ITEM];


	sITEM_EFFECT    aitemEffect[6]; // by Szczeepan
	sITEM_EXTRA_EFFECT aitemExtraEffect[2];// by Szczeepan
	BYTE            byDurationType; //eDURATIONTYPE
	DBOTIME            nUseStartTime;// 인벤토리에 들어온 날짜
	DBOTIME            nUseEndTime;// 사용만료 기간
	//BYTE            unknown1[48];
};
// ��ũ ������ ��ü ����
struct sBANK_PROFILE
{
	HOBJECT			handle;			// ������ handle
	TBLIDX			tblidx;			// item table index
	BYTE			byPos;
	BYTE			byStackcount;
	BYTE			byRank;
	BYTE			byCurDur;		// ������ �ƽ�
	bool			bNeedToIdentify;
	BYTE			byGrade;		// ������ ���׷��̵� ���
	BYTE			byBattleAttribute;		// ����Ϳ��� cf) NtlBattle.h eBATTLE_ATTRIBUTE 

	BYTE			byRestrictType;			// �ͼӻ��� eITEM_RESTRICT_TYPE
	WCHAR			awchMaker[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1]; // ������

	TBLIDX			aOptionTblidx[NTL_MAX_OPTION_IN_ITEM];

	BYTE			byDurationType; //eDURATIONTYPE
	DBOTIME			nUseStartTime;// �κ��丮�� ���� ��¥
	DBOTIME			nUseEndTime;// ��븸�� �Ⱓ
};


// Zenny ���� ����
struct sMONEY_BRIEF
{
	DWORD			dwZenny;
};


// Zenny ����
struct sMONEY_STATE
{
	sVECTOR3		vCurLoc; // ���� ��ġ
	bool			bIsNew;
};


// GameServer���� Query Server�� ������ ������ ��û�Ҷ� ���
struct sITEM_DELETE_DATA
{
	BYTE			byPlace;	// ���
	BYTE			byPos;		// ��ġ
	HOBJECT			hItem;		// ������ �ڵ�
	ITEMID			itemId;		// ������ Serial
	TBLIDX			tblidx;     // ������ tblidx		

public:
	sITEM_DELETE_DATA( void )
		: byPlace( INVALID_BYTE )
		, byPos( INVALID_BYTE )
		, hItem( INVALID_HOBJECT )
		, itemId( INVALID_ITEMID )
		, tblidx( INVALID_TBLIDX )
	{
	}

};

//-----------------------------------------------------------------------
// TRADE
//-----------------------------------------------------------------------
struct sTRADE_INVEN
{
	HOBJECT	hItem;			// ������ �ڵ�
	ITEMID itemSerial;		// �ű� �������� DB �ø���
	BYTE byDstPlace;		// �Ű��� ��
	BYTE byDstPos;			// �Ű��� ��
	BYTE byDstCount;		// �ű� ����	
	ITEMID	itemNewSerial;	// ���߽����� �Ϻ� �������� �Ű����� ���� ���ܳ� �ø���
};
const int	TRADE_INVEN_MAX_COUNT = 12;
const DWORD NTL_TRADE_UPDATE_INTERVAL = 5 * 1000;		//5 ��
const DWORD NTL_TRADE_REPLY_WAIT_TIME = 30 * 1000;      // �ŷ���û�� ����ϴ� �ð� 30�ʵڿ��� ��������� �ڵ�ĵ��

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//  [11/20/2006 niam]
// �������� ������ ���� Ȥ�� ���� �� �� ���
//-----------------------------------------------------------------------


// ���� ������
//	- ������ ���� ���� �̿ܿ� ������ �����ϴµ��� �ʿ��� �߰����� �������� ����
//	- �Ϲ������� ������������ ������ ���۵��� �����ʰ� ���� �״�� �ٽ� ���� ������ �ǵ��� �ش�.



// ITEM_CREATE_EX �޽����� �ѹ��� ������ �� �ִ� �ִ� �������� ����
const int	ITEM_CREATE_EX_MAX_COUNT = 10;

// ITEM_CREATE_EX �޽����� �ѹ��� ������ �� �ִ� �ִ� �������� ����
const int	ITEM_DELETE_EX_MAX_COUNT = 10;



//-----------------------------------------------------------------------
//	Desc : GQ_CREATE_ITEM_EX_REQ ���� ����ϴ� ����ü ����
//

struct sITEM_TS_SUB_DATA
{
	BYTE		byTSType;				// eTSM_MANAGER_TYPE      �����ؾ� ��->   TS_TYPE_QUEST_CS, TS_TYPE_PC_TRIGGER_CS, TS_TYPE_OBJECT_TRIGGER_S
	HOBJECT		hObject;				// Trigger Object �� �ڵ� ObjectTrigger���� ����Ѵ�
	sTSM_SERIAL	sSerial;
};


// ������ ���� Ÿ��
enum eITEM_CREATE_TYPE
{
	eITEM_CREATE_TYPE_TS_MIDDLE,				// TS Middle ���� ��û
};


// ������ ������ �������ݿ� ���� ���� �����͸� ������ ����ü
union uITEM_CREATE_SUB_DATA
{
										// ��			   - ����
	sITEM_TS_SUB_DATA	sTSSubData;		// Trigger System ���� ���
};


//-----------------------------------------------------------------------
//	Desc : GQ_DELETE_ITEM_EX_REQ ���� ����ϴ� ����ü ����
//

// ������ ���� Ÿ��
enum eITEM_DELETE_TYPE
{
	eITEM_DELETE_TYPE_TS_MIDDLE,				// TS Middle ���� ��û
	eITEM_DELETE_TYPE_TMQ,						// TMQ���� ��û
};


// ������ ������ �������ݿ� ���� ���� �����͸� ������ ����ü
union uITEM_DELETE_SUB_DATA
{
											// ��				- ����
	sITEM_TS_SUB_DATA	sTSSubData;		// Trigger System ���� ���
};

struct sITEM_POSITION_DATA
{
	HOBJECT			hItem;		// ������ �ڵ�
	BYTE			byPlace;	// ���
	BYTE			byPos;		// ��ġ
};



//-----------------------------------------------------------------------------
// ������ �ڵ� ����

struct sITEM_PLACE
{
	BYTE byPlace;
	BYTE byPos;
};

struct sITEM_AUTO_EQUIP_DATA
{
	// �̵� ������
	ITEMID					srcItemId;
	HOBJECT					hSrcItem;
	BYTE					bySrcEquipPlace;
	BYTE					bySrcEquipPos;

	BYTE					byDestPlace;	// �̵��� �� ��(����ִ� ��ġ�̾�� �Ѵ�.)
	BYTE					byDestPos;

	// ���� & ���� �ϴ� ������
	sITEM_DATA				sEquipItem;
};


struct sITEM_AUTO_EQUIP_ROLLBACK_DATA
{
	// ���� ������
	ITEMID					delItemId;
	HOBJECT					hDelEquipItem;
};


//-----------------------------------------------------------------------------


WORD Dbo_GetFinalOffence(WORD wBaseOffence, BYTE byGrade);
WORD Dbo_GetFinalDefence(WORD wBaseDefence, BYTE byGrade);

const BYTE			NTL_MAX_PRIVATESHOP_INVENTORY			= 36;	// ���� ������ �ִ� ũ�� (���� : sizeof(BYTE)���� ũ�� �ȵȴ�) (���λ��� GUI�� ���� ������ ����� �ö󰡴��� �������� �Ѵ�)
const int			NTL_MAX_PRIVATESHOP_BUY_SHOPPING_CART	= 12;	// ����īƮ�� ����ִ� �ִ� ũ��
const BYTE			PRIVATESHOP_ITEMPOS_INVALID				= 0xFF;
const DWORD			NTL_PRIVATESHOP_ITEM_BUY_ZENNY_DEFAULT	= 1;	//���λ��� �ȶ� �⺻ ���ϰ�

//-- SGpro --------------------------------------------------------------------------
//  Purpose : Game Srv�� Query Srv ���̿��� ���[7/2/2007 SGpro]
//  Return  : 
//-----------------------------------------------------------------------------------
struct sPRIVATESHOP_ITEM_POS_DATA
{
	bool	bIsSaveItem; //true�̸� Shop�� �������� ����� �� (Buy �� Query Srv�� �ʿ���)
	BYTE	byPlace;
	BYTE	byPos; //���λ��� �κ��丮 Slot Pos
	BYTE	byInventoryPlace; //�κ��丮�� Place (Buy �� Query Srv�� �ʿ���)
	BYTE	byInventoryPos;//�κ��丮�� Pos (Buy �� Query Srv�� �ʿ���)
	ITEMID	itemID;
};

struct sINVEN_ITEM_POS_DATA
{
	BYTE byPlace;
	BYTE byPos;
	ITEMID	itemID;
};


//-- SGpro --------------------------------------------------------------------------
//  Purpose : Game Srv�� User PC ���̿��� ���[7/3/2007 SGpro]
//  Return  : 
//-----------------------------------------------------------------------------------
struct sPRIVATESHOP_ITEM_POS_DATA_UG
{
	BYTE	byPlace;
	BYTE	byPos;
};

//-- SGpro --------------------------------------------------------------------------
//  Purpose : [7/2/2007 SGpro]
//  Return  :
//-----------------------------------------------------------------------------------
enum ePRIVATESHOP_STATE
{
	 PRIVATESHOP_STATE_CLOSE			= 0 //������
	,PRIVATESHOP_STATE_OPEN					//������
	,PRIVATESHOP_STATE_BUSINESS_WAIT		//���� ���
	,PRIVATESHOP_STATE_BUSINESS				//������
	,PRIVATESHOP_STATE_NONE					//���λ��� ���°� �ƴϴ�
	,PRIVATESHOP_STATE_NULL					//���λ����� �ѹ��� �������� ���� ����
	,PRIVATESHOP_STATE_DATALOADING			//Query Srv���� ����Ÿ�� ���⸦ ��ٸ��� ����
};

// �����ڸ� ������ ������. 
// ���ο��� ���˴ϴ�. [10/2/2007 SGpro]
struct sSUMMARY_PRIVATESHOP_SHOP_DATA
{
	bool bIsOwnerEmpty; //true�̸� ������ �ڸ��� ����
	BYTE byShopState; //Shop State ����(ePRIVATESHOP_STATE)
	WCHAR wcPrivateShopName[NTL_MAX_PRIVATESHOP_NAME_IN_UNICODE + 1];
};// end of struct sSUMMARY_PRIVATESHOP_SHOP_DATA


//-- server & client ���� PrivateShop Data --------------------------------------
struct sPRIVATESHOP_SHOP_DATA
{
	HOBJECT hOwner;								// ���� ���� CharacterID	
	PSHOP_WAITTIME waittimeCashShopStart;		// ���� ���� �� ��¥
	PSHOP_WAITTIME waittimeCashShopEnd;			// ���� �� ������
	sSUMMARY_PRIVATESHOP_SHOP_DATA sSummaryPrivateShopData;	
	WCHAR wcNotice[NTL_MAX_PRIVATESHOP_NOTICE_IN_UNICODE + 1];

public: 
	sPRIVATESHOP_SHOP_DATA()
		: hOwner ( INVALID_HOBJECT )
		, waittimeCashShopStart ( 0 )
		, waittimeCashShopEnd ( 0 )
	{
		sSummaryPrivateShopData.bIsOwnerEmpty = false;
		sSummaryPrivateShopData.byShopState = PRIVATESHOP_STATE_NULL;
		ZeroMemory( sSummaryPrivateShopData.wcPrivateShopName,	sizeof(WCHAR) * ( NTL_MAX_PRIVATESHOP_NAME_IN_UNICODE + 1 ) );
		ZeroMemory( wcNotice,			sizeof(WCHAR) * ( NTL_MAX_PRIVATESHOP_NOTICE_IN_UNICODE + 1 ) );
	}
};//end of struct sPRIVATESHOP_SHOP_DATA


// -- ��Ŷ�� PrivateShop Data�� ------------------------------------------------------
// wcNotice�� �������� ���ؾ� �ϱ� ������ struct sPRIVATESHOP_SHOP_DATA�� �и��� �ߴ�
// Notice�� ���� Size�̱� ������ �ݵ�� 
// struct sPACKET_PRIVATESHOP_SHOP_DATA�� ���� �ؿ� �־�� �ȴ� [10/2/2007 SGpro]
struct sPACKET_PRIVATESHOP_SHOP_DATA
{
	HOBJECT hOwner;// ���� ���� CharacterID	
	PSHOP_WAITTIME waittimeCashShopStart; // ���� ���� �� ��¥
	PSHOP_WAITTIME waittimeCashShopEnd; // ���� �� ������
	sSUMMARY_PRIVATESHOP_SHOP_DATA sSummaryPrivateShopData;	
	BYTE byNoticeSize;
	WCHAR wcNotice[NTL_MAX_PRIVATESHOP_NOTICE_IN_UNICODE + 1];

public: 
	sPACKET_PRIVATESHOP_SHOP_DATA()
		: hOwner ( INVALID_HOBJECT )
		, waittimeCashShopStart ( 0 )
		, waittimeCashShopEnd ( 0 )
		, byNoticeSize ( 0 )
	{
		sSummaryPrivateShopData.bIsOwnerEmpty = false;
		sSummaryPrivateShopData.byShopState = PRIVATESHOP_STATE_NONE;
		ZeroMemory( sSummaryPrivateShopData.wcPrivateShopName,	sizeof(WCHAR) * ( NTL_MAX_PRIVATESHOP_NAME_IN_UNICODE + 1 ) );
		ZeroMemory( wcNotice,			sizeof(WCHAR) * ( NTL_MAX_PRIVATESHOP_NOTICE_IN_UNICODE + 1 ) );
	}
};// end of struct sPRIVATESHOP_SHOP_DATA

// -- SGpro --------------------------------------------------------------------------
//  Purpose : [7/2/2007 SGpro]
//  Return  :
// -----------------------------------------------------------------------------------
struct sPRIVATESHOP_ITEM_DATA
{
	enum eITEMSTATE
	{
		PRIVATESHOP_GENERAL					= 0, // �Ϲ�
		PRIVATESHOP_BUSINESS_ITEM			= 1, // �������� ������(���� �������� ������ �� ����)
		PRIVATESHOP_VISITOR_SELECT_ITEM		= 2, // ������ ��
		PRIVATESHOP_VISITOR_UNSELECT_ITEM	= 3, // ������ �� Ǯ��
		PRIVATESHOP_INVALID					= 0xFF,
	};

	HOBJECT							hItem;
	BYTE							byPrivateShopInventorySlotPos;
	bool							bIsSaveItem; // true�̸� Shop�� �������� ����� ��
	sITEM_DATA						sItem;// bIsSaveItem�� ���� true�� ��� ä���� UserPC�� ������
	DWORD							dwZenny;
	BYTE							byItemState; // eITEMSTATE ���� ���´�

	sPRIVATESHOP_ITEM_DATA()
		: hItem ( INVALID_HOBJECT )
		, byPrivateShopInventorySlotPos ( PRIVATESHOP_ITEMPOS_INVALID )
		, dwZenny ( NTL_PRIVATESHOP_ITEM_BUY_ZENNY_DEFAULT )
		, byItemState ( PRIVATESHOP_INVALID )
		, bIsSaveItem ( false )
	{
		ZeroMemory( &sItem, sizeof( sItem ));
	}
};

// ShopSystem ��� �ϴ� ������ �ȱ� Sell_Price, dwCost, dwOptionCost -> S C O

#define NTL_SELL_PRICE(S, C, O)	  ( ( ( ( (float)(S) / 100.0 ) * (float)C) + O) )

// ItemUpgrade
//-----------------------------------------------------------------------------------
// ������ UPGRADE Scouter Add Rate ����
//-----------------------------------------------------------------------------------
enum eITEM_UPGRADE_RATE
{
	ITEM_UPGRADE_RATE_0 = 0,//-10.0f,	
	ITEM_UPGRADE_RATE_1 ,//-5.0f,			
	ITEM_UPGRADE_RATE_2 ,// 0.0f,			
	ITEM_UPGRADE_RATE_3 ,// 5.0f,			
	ITEM_UPGRADE_RATE_4 ,// 10.0f,			
	ITEM_UPGRADE_RATE_5 ,// 20.0f,	

};

float Dbo_UpgradeSuccessRate( BYTE byGrade, BYTE byStoneType );
float Dbo_ItemDestroyRateByUpgradeFail( const BYTE byRank, const BYTE byItemBattleAttribute, const BYTE byPosNum, const float fAddRateByScouter );// ���׷��̵� ���н� �������� �ı��� Ȯ��
// �߾� �������� Rank �Ӽ� ��ް� �ٱ��� �׵θ��� �����ѼӼ�1~5�� ��ī���� ���뿡 ���� �߰� Ȯ���� �־� ����Ȯ���� ����
BYTE Dbo_GetUpgradeRate( BYTE byItemBattleAttribute, BYTE byPosNum );
//	�� ������ ������� �������� Ȯ��
float Dbo_GetAttributeRate( BYTE byItemBattleAttribute, BYTE byPosNum );
// �Ӽ��� ���� Ȯ���� ���ϱ�.
BYTE Dbo_GetHoipoiStoneCount( const BYTE byStoneType, const BYTE byItemType, BYTE byGrade );
// ������ Ÿ�԰� ��޿� ���� �ҿ� ȣ������ ���� ��������
DWORD Dbo_GetRepairPay( DWORD dwCost, BYTE byDur, BYTE byCurDur);
// ������ �ͽ� EXP
DWORD Dbo_GetHoipoiMixEXP( bool bIsNormalSuccess, bool bIsGreatSuccess, BYTE byMadeLevel, BYTE byNeedMixLevel );
// ������ �ͽ� ���� ���� �ʿ� ����ġ
DWORD Dbo_GetHoipoiMixNeedLevelExp( BYTE byCurLevel );
// ������ �ͽ� ����
DWORD Dbo_GetHoipoiMixFare( DWORD dwCost, BYTE byDiscountRate = 0);
// ���� ����� ���� ���� ���� ����
BYTE Dbo_GetHoipoiMixNeedLevelForJobSet( const BYTE byRecipeType );
// ���� ����� ���� ���� �ʿ��� ����
DWORD Dbo_GetHoipoiMixNeedZennyForJobSet( const BYTE byRecipeType );

// Item Copy
bool Dbo_SetItemData( sITEM_DATA* const pItemData, ITEMID itemId, TBLIDX itemNo, CHARACTERID charId, BYTE byPlace, BYTE byPosition, BYTE byStackcount, BYTE byRank, BYTE byCurrentDurability, bool bNeedToIdentify, BYTE byGrade, BYTE byBattleAttribute, BYTE byRestrictType, const WCHAR* const awchMaker, TBLIDX* const aOptionTblidx, BYTE byDurationType, DBOTIME nUseStartTime, DBOTIME nUseEndTime );
bool Dbo_SetItemData_CheckNeedToIdentify( sITEM_DATA* const pItemData, ITEMID itemId, TBLIDX itemNo, CHARACTERID charId, BYTE byPlace, BYTE byPosition, BYTE byStackcount, BYTE byRank, BYTE byCurrentDurability, bool bNeedToIdentify, BYTE byGrade, BYTE byBattleAttribute, BYTE byRestrictType, WCHAR* const awchMaker, TBLIDX* const aOptionTblidx, BYTE byDurationType, DBOTIME nUseStartTime, DBOTIME nUseEndTime );
// Item Profile
bool Dbo_SetItemProfile( sITEM_PROFILE* const pItemProfile, HOBJECT handle, TBLIDX tblidx, BYTE byPlace, BYTE byPos, BYTE byStackcount, BYTE byRank, BYTE byCurDur, bool bNeedToIdentify, BYTE byGrade, BYTE byBattleAttribute, BYTE byRestrictType, WCHAR* const awchMaker, TBLIDX* const aOptionTblidx, BYTE byDurationType, DBOTIME nUseStartTime, DBOTIME nUseEndTime );
// Bank Item Profile
//bool Dbo_SetBankItemProfile( sBANK_PROFILE* const pItemProfile, HOBJECT handle, TBLIDX tblidx, BYTE byPos, BYTE byStackcount, BYTE byRank, BYTE byCurDur, bool bNeedToIdentify, BYTE byGrade, BYTE byBattleAttribute, BYTE byRestrictType, WCHAR* const awchMaker, TBLIDX* const aOptionTblidx, BYTE byDurationType, DBOTIME nUseStartTime, DBOTIME nUseEndTime );
// Item Brief
bool Dbo_SetItemBrief( sITEM_BRIEF* const pItemBrief, TBLIDX tblidx, BYTE byRank, BYTE byGrade, BYTE byBattleAttribute, TBLIDX* const aOptionTblidx );
// Item Shop
bool Dbo_SetShopBuyInven( sSHOP_BUY_INVEN* const pShopBuyInven, BYTE byPlace, BYTE byPos, BYTE byStack, TBLIDX tblItem, BYTE byRank, BYTE byCurrentDurability, BYTE byGrade, BYTE byRestrictType, WCHAR* const awchMaker, TBLIDX* const aOptionTblidx, BYTE byDurationType, DBOTIME nUseStartTime, DBOTIME nUseEndTime );
// Item -> Data
bool Dbo_SetItemDataToQryData( const sITEM_DATA& rItemDataOrg, int& itemNo, int& charId, BYTE& byPlace, BYTE& byPosition, BYTE& byStackcount, BYTE& byRank, BYTE& byCurrentDurability, BYTE& byNeedToIdentify, BYTE& byGrade, BYTE& byBattleAttribute, BYTE& byRestrictType, char* pMakerInHex, size_t nMakerInHex_Size, int* aOptionTblidx, size_t nOptionTblidx_Size, BYTE& byDurationType, __int64& nUseStartTime, __int64& nUseEndTime );
bool Dbo_SetItemDataToData( const sITEM_DATA& rItemDataOrg, ITEMID& itemId, TBLIDX& itemNo, CHARACTERID& charId, BYTE& byPlace, BYTE& byPosition, BYTE& byStackcount, BYTE& byRank, BYTE& byCurrentDurability, bool& bNeedToIdentify, BYTE& byGrade, BYTE& byBattleAttribute, BYTE& byRestrictType, WCHAR* pMakerInHex, size_t nMakerInHex_Size, TBLIDX* pOptionTblidx, size_t nOptionTblidx_Size, eDURATIONTYPE& eDurationType, DBOTIME& nUseStartTime, DBOTIME& nUseEndTime );
bool Dbo_SetAutoEquipItemDataToQryData( const sITEM_AUTO_EQUIP_DATA& rItemAutoEquipData, int& srcItemId, HOBJECT& hSrcItem, BYTE& bySrcEquipPlace, BYTE& bySrcEquipPos, BYTE& byDestPlace, BYTE& byDestPos, int& itemNo, int& charId, BYTE& byPlace, BYTE& byPosition, BYTE& byStackcount, BYTE& byRank, BYTE& byCurrentDurability, BYTE& byNeedToIdentify, BYTE& byGrade, BYTE& byBattleAttribute, BYTE& byRestrictType, char* pMakerInHex, size_t nMakerInHex_Size, int* aOptionTblidx, size_t nOptionTblidx_Size, BYTE& byDurationType, __int64& nUseStartTime, __int64& nUseEndTime );
bool Dbo_SetShopBuyInvenToQryData( const sSHOP_BUY_INVEN& rShopBuyInven, BYTE& byPlace, BYTE& byPos, BYTE& byStack, int& tblItem, BYTE& byRank, BYTE& byCurrentDurability, BYTE& byGrade, BYTE& byRestrictType, char* pMakerInHex, size_t nMakerInHex_Size, int* aOptionTblidx, size_t nOptionTblidx_Size, BYTE& byDurationType, __int64& nUseStartTime, __int64& nUseEndTime );

#pragma pack()
