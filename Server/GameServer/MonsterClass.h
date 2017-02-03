#include "NtlSfx.h"
#include "SharedType.h"
#include "SpawnTable.h"
#include <iostream>
//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
#include <vector>

#include <list>
#define DISTANCE_TO_SPAWN				100
#define DISTANCE_TO_DESPAWN				85
#define MAX_MOB_ANOUNT					100
#define MONSTER_SPAWN_UPDATE_TICK		1000
#define MONSTER_MOVE_UPDATE_TICK		10000
#define MAX_PLAYER_COUNT				512
class CClientSession;
class CGameServer;
class CNtlPacket;


class CMonster
{
public:

	struct MonsterData
	{
		TBLIDX			MonsterID;
		bool			FightMode;
		BYTE			StateID;
		DWORD			ConditionFlag;

		bool			ShowName;
		bool			SpawnAnimation;
		BYTE			Attack_Animation_Qty;
		BYTE			Attack_Type;
		BYTE			Battle_Attribute;
		BYTE			Con;
		BYTE			Dex;
		BYTE			Eng;
		BYTE			Foc;
		BYTE			Sol;
		BYTE			Str;
		BYTE			Grade;
		BYTE			HtbBlockRate;
		BYTE			Level;
		BYTE			Mob_type;
		BYTE			Property;
		BYTE			Scan_range;
		BYTE			Sight_range;
		BYTE			Use_skill_basis;

		TBLIDX			Drop_each_id;
		TBLIDX			Drop_quest_id;
		TBLIDX			Drop_type_id;
		TBLIDX			Drop_item_id;

		DWORD			Ai_bit_flag;
		DWORD			AllianceID;
		DWORD			DialogGroup;
		DWORD			Drop_Zenny;
		DWORD			Mob_group;
		float			Attack_range;
		float			Drop_zenny_rate;
		float			Fly_height;
		float			Radius;
		float			Radius_x;
		float			Radius_z;
		float			Run_Speed;
		float			Run_Speed_origin;
		float			Scale;
		float			Walk_Speed;
		float			Walk_Speed_origin;
		TBLIDX			Name;

		WORD			CurLP;
		WORD			CurEP;
		WORD			MaxLP;
		WORD			MaxEP;
		WORD			AttackSpeedRate;
		TBLIDX			use_Skill_Tblidx[6 + 1];
		WORD			Aggro_max_count;
		WORD			Attack_cool_time;
		WORD			Attack_rate;
		WORD			Attack_speed_rate;
		WORD			Basic_aggro_point;
		WORD			Basic_energy_defence;
		WORD			Basic_Offence;
		WORD			Basic_physical_defence;
		WORD			Block_rate;
		WORD			Curse_success_rate;
		WORD			Curse_tolerance_rate;
		WORD			Dodge_rate;
		WORD			Ep_Regeneration;
		WORD			Exp;
		WORD			Lp_Regeneration;
		WORD			Mob_Kind;
		WORD			Sight_angle;
		WORD			TMQ_Point;
		WORD			Use_skill_Lp;
		WORD			Use_skill_time;

		BYTE			DropEachRateControl;
		BYTE			DropEItemRateControl;
		BYTE			DropLItemRateControl;
		BYTE			DropNItemRateControl;
		BYTE			DropSItemRateControl;
		BYTE			DropTypeRateControl;

		TBLIDX			MonsterSpawnID;
		CNtlVector		Spawn_Loc;
		CNtlVector		Spawn_Dir;
		BYTE			Spawn_Loc_Range;
		BYTE			Spawn_Quantity;
		DWORD			Spawn_Cool_Time;
		BYTE			Spawn_Move_Type;
		BYTE			Wander_Range;
		BYTE			Move_Range;
		BYTE			Move_DelayTime;
		CNtlVector		FollowDistance;
		DWORD			Party_Index;
		bool			Party_Leader;
		SPAWNGROUPID	spawnGroupId;

		WORD			MapID;
		bool			IsDead;
		DWORD			KilledTime;
		bool			isSpawned;
		bool			isAggro;
//		void			isAggroByPlayer(PlayersMain *plr, CGameServer *app);
//		void			Attack(PlayersMain *plr, CGameServer *app);
		RwUInt32		DisplayID;
		RwUInt32		target;
		CNtlVector		curPos;
		BYTE			chainAttackCount;
		BYTE			MaxchainAttackCount;
		RwUInt32		UniqueID;
		std::vector<RwUInt32> spawnedForHandle;
	};

};