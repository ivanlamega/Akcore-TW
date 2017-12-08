//***********************************************************************************
//
//	File		:	NtlObject.h
//
//	Begin		:	2006-04-07
//
//	Copyright	:	ⓒ NTL-Inc Co., Ltd
//
//	Author		:	Hyun Woo, Koo   ( zeroera@ntl-inc.com )
//
//	Desc		:	
//
//***********************************************************************************

#pragma once

#include "Character.h"
#include "Quest.h"


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
enum eOBJTYPE
{
	OBJTYPE_PC, // PC
	OBJTYPE_NPC, // NPC
	OBJTYPE_MOB, // MOB
	OBJTYPE_SUMMON_PET, // 스킬로 소환된 pet
	OBJTYPE_ITEM_PET, // 아이템으로 소환된 pet
	OBJTYPE_DROPMONEY, // 땅에 떨어진 돈
	OBJTYPE_DROPITEM, // 땅에 떨어진 아이템
	OBJTYPE_TOBJECT, // 트리거 오브젝트
	OBJTYPE_DYNAMIC,	// 동적 생성 오브젝트
	OBJTYPE_ITEM,	// ITEM TYPE은 필드에 생성되지 않는다. ( 캐릭터만 가지고 있다 )
	
	MAX_OBJTYPE,
	MAX_SPAWNOBJTYPE = OBJTYPE_DYNAMIC,	// 필드에 생성되는 오브젝트 타입의 MAX
	INVALID_OBJTYPE = 0xFF,

	OBJTYPE_FIRST = OBJTYPE_PC,
	OBJTYPE_LAST = MAX_OBJTYPE - 1,
};


// Object를 생성한 source
enum eDBO_OBJECT_SOURCE
{
	DBO_OBJECT_SOURCE_SKILL = 0,		// 스킬에 의해 생긴 object
	DBO_OBJECT_SOURCE_ITEM,				// 아이템에 의해 생긴 object

	DBO_OBJECT_SOURCE_UNKNOWN = 0xFF,

	DBO_OBJECT_SOURCE_FIRST = DBO_OBJECT_SOURCE_SKILL,
	DBO_OBJECT_SOURCE_LAST = DBO_OBJECT_SOURCE_ITEM
};


// Trigger object의 기능
enum eDBO_TRIGGER_OBJECT_FUNC
{
	eDBO_TRIGGER_OBJECT_FUNC_SELECTION				= 0x1,
	eDBO_TRIGGER_OBJECT_FUNC_PORTAL					= 0x2,
	eDBO_TRIGGER_OBJECT_FUNC_BIND					= 0x4,
	eDBO_TRIGGER_OBJECT_FUNC_SCOUTER				= 0x8,
	eDBO_TRIGGER_OBJECT_FUNC_RANK_BATTLE			= 0x10,
	eDBO_TRIGGER_OBJECT_FUNC_NORMAL_DRAGONBALL		= 0x20,
	eDBO_TRIGGER_OBJECT_FUNC_RAID_DRAGONBALL		= 0x40,
	eDBO_TRIGGER_OBJECT_FUNC_MAIL_BOX				= 0x80,
	eDBO_TRIGGER_OBJECT_FUNC_TIME_MACHINE			= 0x100,
	eDBO_TRIGGER_OBJECT_FUNC_NAMEKAN_SIGN			= 0x200,
	eDBO_TRIGGER_OBJECT_FUNC_PARTY_INSTANCE_DUNGEON = 0x400,
	eDBO_TRIGGER_OBJECT_FUNC_TIME_LEAP_QUEST		= 0x800,
	eDBO_TRIGGER_OBJECT_FUNC_HOIPOIMIX				= 0x1000,
	eDBO_TRIGGER_OBJECT_FUNC_DOJO_WAREHOUSE			= 0x2000,
	eDBO_TRIGGER_OBJECT_FUNC_DOJO_SEAL				= 0x4000,
};

// Dynamic Object의 state
enum eDYNAMIC_OBJECT_STATE
{
	eDYNAMIC_OBJECT_STATE_SPAWN,
	eDYNAMIC_OBJECT_STATE_IDLE,
	eDYNAMIC_OBJECT_STATE_DESPAWN,

	eDYNAMIC_OBJECT_STATE_NUMS,
	eDYNAMIC_OBJECT_STATE_INVALID = 0xFF
};

// Dynamic object function
enum eDYNAMIC_OBJECT_FUNCTION
{
	eDYNAMIC_OBJECT_FUNCTION_HOIPOIMIX = 0x1,

	eDYNAMIC_OBJECT_FUNCTION_NUMS,
	eDYNAMIC_OBJECT_FUNCTION_INVALID = 0xFF
};

// Dynamic Object의 기능
enum eDBO_DYNAMIC_OBJECT_TYPE
{
	eDBO_DYNAMIC_OBJECT_TYPE_HOIPOI_MIX_MACHINE		= 1,		// 호이포이 믹스 렌탈 머신
	eDBO_DYNAMIC_OBJECT_TYPE_CAPSULE_HOUSE,						// 호이포이 믹스 캐쉬 머신

	eDBO_DYNAMIC_OBJECT_TYPE_NONE = 0xFF,
};

// Dynamic object substate
enum eDYNAMIC_OBJECT_SUBSTATE
{
	eDYNAMIC_OBJECT_SUBSTATE_ALL_SHOW,
	eDYNAMIC_OBJECT_SUBSTATE_ONLY_SPAWN_AVATAR_SHOW,
	eDYNAMIC_OBJECT_SUBSTATE_ENEMY_HIDE,

	eDYNAMIC_OBJECT_SUBSTATE_NUMS,
	eDYNAMIC_OBJECT_SUBSTATE_INVALID = 0xFF
};

// Dynamic object substate
enum eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE
{
	eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE_NORMAL	= 1,
	eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE_RENTAL,
	eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE_NETP,
	eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE_CASH,

	eDYNAMIC_OBJECT_HOIPOI_MACHINE_TYPE_NONE = 0xFF,
};


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
const unsigned int MAX_PC = 3000;	// 전체 PC 수

const unsigned int MAX_NPC = 3000;	// 전체 NPC 수

const unsigned int MAX_MOB = 60000;	// 전체 MOB 수

const unsigned int MAX_SUMMON_PET = MAX_PC;	// 전체 Summon Pet 수

const unsigned int MAX_ITEM_PET = MAX_PC;	// 전체 Item Pet 수

const unsigned int MAX_DROP_ITEM = 3000;	// 전체 DROP 되는 ITEM 수

const unsigned int MAX_DROP_MONEY = 3000;	// 전체 DROP 되는 MONEY 수

const unsigned int MAX_PC_HAVE_ITEM = 200;	// PC가 가지는 최대 ITEM 수

const unsigned int MAX_TRIGGER_OBJECT = 20000; // 전체 정적 트리거 오브젝트 수

const unsigned int MAX_DYNAMIC_OBJECT = MAX_PC; // 전체 동적 오브젝트 수 PC 당 한개만 허용

const unsigned int MAX_BOT = MAX_NPC + MAX_MOB + MAX_SUMMON_PET + MAX_ITEM_PET;		// BOT 수 ( 서버 내부 처리용 )

const unsigned int MAX_ITEM = MAX_PC * MAX_PC_HAVE_ITEM;	// 전체 아이템 수

const unsigned int MAX_GAME_OBJECT = MAX_PC + MAX_BOT + MAX_ITEM + MAX_DROP_ITEM + MAX_DROP_MONEY + MAX_TRIGGER_OBJECT + MAX_DYNAMIC_OBJECT; // 전체 오브젝트 수

const float		   DBO_MAX_DISTANCE_OF_DYNAMIC_OBJECT = 20.0f;		// DYNAMIC OBJECT 설치 허용 최대 거리

const unsigned int DYNAMIC_OBJECT_CHECK_TERM = 1000;	// 다이나믹오브젝트의 주인과 거리 이격 체크 텀  seconds

const unsigned int DYNAMIC_OBJECT_DESPAWN_TERM = 1000;

const unsigned int DYNAMIC_OBJECT_SPAWN_TERM = 1000;
//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
const char * NtlGetObjectTypeString(eOBJTYPE eObjType);
#pragma pack(1)
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
typedef BYTE DYNAMIC_OBJECT_STATE;

struct sDYNAMIC_OBJECT_BRIEF
{
	BYTE		byObjectType;	// eDBO_DYNAMIC_OBJECT_TYPE
	TBLIDX		idObjectTblidx;	// 해당하는 tblidx(dynamic object가 아닌 hoipoi, capsule house등의 tblidx)
	HOBJECT		hOwner;
};


struct sDYNAMIC_OBJECT_STATE
{
	BYTE		byState; // Main State
	sVECTOR3	vCurLoc;
};
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#define NAME_LEN (16)
#define GUILD_LEN (16)
#pragma pack(push, 1)


struct Appear
{
	uint8_t Face;
	uint8_t Hair;
	uint8_t HairColor;
	uint8_t SkinColor;
};
struct sOBJECT_INFO
{
	BYTE					objType;

	union
	{
		struct // pc
		{
			sPC_BRIEF		pcBrief;
			sCHARSTATE		pcState;
		};
		struct // npc
		{
			sNPC_BRIEF		npcBrief;
			sCHARSTATE		npcState;
		};
		struct // mob
		{
			sMOB_BRIEF		mobBrief;
			sCHARSTATE		mobState;
		};
		struct // summon pet
		{
			sSUMMON_PET_BRIEF		summonPetBrief;
			sCHARSTATE				summonPetState;
		};
		struct // item pet
		{
			sITEM_PET_BRIEF			itemPetBrief;
			sCHARSTATE				itemPetState;
		};
		struct // item
		{
			sITEM_BRIEF		itemBrief;
			sITEM_STATE		itemState;
		};
		struct // money
		{
			sMONEY_BRIEF	moneyBrief;
			sMONEY_STATE	moneyState;
		};
		struct // trigger object
		{
			sTOBJECT_BRIEF	tobjectBrief;
			sTOBJECT_STATE	tobjectState;
		};
		struct // dynamic object
		{
			sDYNAMIC_OBJECT_BRIEF	dynamicObjBrief;
			sDYNAMIC_OBJECT_STATE	dynamicObjState;
		};
	};
};
struct SpawnMOB
{
	// The handle is the Unique number for reconyze that "thing", it can be, mob, npc, player, zenny, etc.
	uint16_t wOpCode;
	uint32_t Handle;
	uint8_t Type; // The type tell to the client, which is it, OBJTYPE.
	// Are the same as the old client.

	uint32_t Tblidx; // Table Index of Mob, located on Table_Mob_Data
	uint32_t curLP;
	uint16_t curEP;
	uint32_t maxLP;
	uint16_t maxEP;

	uint8_t Unknown[34];
	uint8_t Size; // Again use 10 as default
	uint8_t Level;
	uint8_t Unknown2;

	uint8_t StateID; // Again, moving, spawning, or just stand
	uint8_t Unknown3[12];
	uint8_t AspectID; // Default 255, Idk the aspects of mobs.
	uint8_t Unknown4[9];
	bool IsFighting;
	float Loc[3]; // X Y Z
	float Dir[3]; // X Y Z

};
struct SpawnNPC
{
	// The handle is the Unique number for reconyze that "thing", it can be, mob, npc, player, zenny, etc.
	uint16_t wOpCode;
	uint32_t Handle;
	uint8_t Type; // The type tell to the client, which is it, OBJTYPE.
	// Are the same as the old client.

	uint32_t Tblidx; // Table Index of Mob, located on Table_Mob_Data
	uint32_t curLP;
	uint16_t curEP;
	uint32_t maxLP;
	uint16_t maxEP;

	uint8_t Unknown[34];
	uint8_t Size; // Again use 10 as default
	uint8_t Level;
	uint8_t Unknown2;

	uint8_t StateID; // Again, moving, spawning, or just stand
	uint8_t Unknown3[12];
	//uint8_t AspectID; // Default 255, Idk the aspects of mobs.
	uint8_t Unknown4[9];
	bool IsFighting;
	float Loc[3]; // X Y Z
	float Dir[3]; // X Y Z

};
struct SpawnPlayer
{
	// The handle is the Unique number for reconyze that "thing", it can be, mob, npc, player, zenny, etc.
	uint16_t wOpCode;
	uint32_t Handle;
	uint8_t Type; // The type tell to the client, which is it, OBJTYPE.
	// the Unique identify index for characters, only characters
	uint32_t charID;
	// This is the Table Index from PC_Table
	uint32_t Tblidx;
	bool Adult;

	WCHAR Name[NAME_LEN + 1];
	WCHAR GuildName[GUILD_LEN + 1];

	// This is the same as old dbo lol, it contains
	// face, hair, haircolor, skincolor
	Appear appear;

	uint32_t maxLP;
	uint32_t curLP;
	uint16_t maxEP;
	uint16_t curEP;
	uint32_t maxAP;
	uint32_t curAP;

	uint8_t level;

	uint8_t Unknown[179]; // 179 bytes who know's what they do, just try to get what they do
	uint32_t mascotID; // This should be UINT32_INVALID or 0xFF.... etc, because if use 1 o 2, the mascot is wrong and client crash. //mascotID is a tblidx for a mascot is why -sangawku
	uint8_t Size; // This should be 10, is the size of the object, if u use more, it wil be more great, like Giant Namek. Like the object grown up. 10 is the default.
	uint8_t Unknown2[5]; // 5 more bytes
	uint8_t StateID; // The current state of the player, moving, etc, etc.
	uint8_t Unknown3[12];

	uint8_t AspectID; // Normal is 255 or 0xFF, for SSJ use 0, kid buu 1 and great namek 2, etc.
	uint8_t Unknown4[9];
	bool IsFighting;
	float Loc[3]; // X Y Z
	float Dir[3]; // X Y Z
	sITEM_BRIEF		sItemBrief[EQUIP_SLOT_TYPE_COUNT]; // dont work 
};
struct Drop
{
	
	uint16_t wOpCode;
	uint32_t Handle; // The handle is the Unique number for reconyze that "thing", it can be, mob, npc, player, zenny, etc.
	uint8_t Type; // The type tell to the client, which is it, OBJTYPE van be  Money item Character Mob Npc etc.
	uint32_t Tblidx; // Table Index of Item locad on Item_Drop_Data
	uint8_t Rank; // Again use 10 as default
	uint8_t Grade; 
	bool IsNew;
	float Loc[3]; // X Y Z
	float Dir[3]; // X Y Z

};
#pragma pack()



