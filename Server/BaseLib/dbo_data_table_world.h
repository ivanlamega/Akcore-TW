
#ifndef DBO_DATA_TABLE_WORLD_H
#define DBO_DATA_TABLE_WORLD_H

//#include "dbo_common.h"
#include "dbo_data_table.h"

#include <map>


#pragma pack(push, 4)
typedef struct
{
	DWORD id;

	char name[NTL_MAX_SIZE_WORLD_NAME+ 1];
	WCHAR disp_name[NTL_MAX_SIZE_WORLD_NAME + 1];

	bool is_dynamic;

	uint32_t cnt;

#if DBO_VERSION_NEW
	/* TODO */
	DWORD unknown1;
#endif

	uint8_t door_type;

	uint32_t destroy_time;

	WCHAR spawn_table_mob[DBO_MAX_LENGTH_WORLD_MOB_SPAWN_TABLE_NAME + 1];
	WCHAR spawn_table_npc[DBO_MAX_LENGTH_WORLD_NPC_SPAWN_TABLE_NAME + 1];
	WCHAR spawn_table_obj[DBO_MAX_LENGTH_WORLD_OBJ_SPAWN_TABLE_NAME + 1];

	CNtlVector pos_start;
	CNtlVector pos_end;

	CNtlVector pos_normal;

	CNtlVector pos_battle_start;
	CNtlVector pos_battle_end;

	CNtlVector pos_out_of_battle_start;
	CNtlVector pos_out_of_battle_end;

	CNtlVector pos_spectator_start;
	CNtlVector pos_spectator_end;

	CNtlVector pos_default;
	CNtlVector dir_default;

	CNtlVector pos_start1;
	CNtlVector dir_start1;

	CNtlVector pos_start2;
	CNtlVector dir_start2;

#if DBO_VERSION_NEW
	/* TODO */
	CNtlVector unknown2[6];
#endif

	float split_size;

	bool is_night;

	uint8_t static_time;

	/* TODO */
	uint16_t flags;

	uint8_t rule_type;	/* enum dbo_game_rule_type */

	DWORD world_rule_id;

	DWORD world_out_id;
	CNtlVector pos_world_out;
	CNtlVector dir_world_out;

	WCHAR resource_folder[DBO_MAX_LENGTH_WORLD_RESOURCE_FOLDER_NAME + 1];

	float bgm_reset_time;

#if DBO_VERSION_NEW
	/* TODO */
	uint32_t world_resource_id;
#else
	uint16_t world_resource_id;
#endif

	float free_camera_height;

	WCHAR resource_flash_enter[DBO_MAX_LENGTH_WORLD_RESOURCE_FLASH_NAME + 1];
	WCHAR resource_flash_leave[DBO_MAX_LENGTH_WORLD_RESOURCE_FLASH_NAME + 1];

	DWORD link_id;

#if DBO_VERSION_NEW
	/* TODO */
	uint8_t unknown3[11];
	uint32_t unknown4;
#endif

} dbo_data_table_world_st;
#pragma pack(pop)

class dbo_data_table_world: public dbo_data_table
{
public:
	dbo_data_table_world() {};
	virtual ~dbo_data_table_world();

	int load(const char *rdf_file);

	const dbo_data_table_world_st *world_data_get(DWORD id);

protected:
	int on_record(void *record, size_t record_size);

private:
	typedef std::map<DWORD, dbo_data_table_world_st *> worlds_map;
	worlds_map worlds_;
};



#endif

