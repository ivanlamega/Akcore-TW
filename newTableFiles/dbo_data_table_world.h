
#ifndef DBO_DATA_TABLE_WORLD_H
#define DBO_DATA_TABLE_WORLD_H

#include "dbo_common.h"
#include "dbo_data_table.h"
#include <map>


#pragma pack(push, 4)
typedef struct
{
	DBOID id;

	char name[DBO_WORLD_NAME_LEN + 1];
	WCHAR disp_name[DBO_WORLD_NAME_LEN + 1];

	bool is_dynamic;

	uint32_t cnt;

#if DBO_VERSION_NEW
	/* TODO */
	DBOID unknown1;
#endif

	uint8_t door_type;

	uint32_t destroy_time;

	WCHAR spawn_table_mob[DBO_WORLD_SPAWN_TABLE_LEN + 1];
	WCHAR spawn_table_npc[DBO_WORLD_SPAWN_TABLE_LEN + 1];
	WCHAR spawn_table_obj[DBO_WORLD_SPAWN_TABLE_LEN + 1];

	dbo_vector3d pos_start;
	dbo_vector3d pos_end;

	dbo_vector3d pos_normal;

	dbo_vector3d pos_battle_start;
	dbo_vector3d pos_battle_end;

	dbo_vector3d pos_out_of_battle_start;
	dbo_vector3d pos_out_of_battle_end;

	dbo_vector3d pos_spectator_start;
	dbo_vector3d pos_spectator_end;

	dbo_vector3d pos_default;
	dbo_vector3d dir_default;

	dbo_vector3d pos_start1;
	dbo_vector3d dir_start1;

	dbo_vector3d pos_start2;
	dbo_vector3d dir_start2;

#if DBO_VERSION_NEW
	/* TODO */
	dbo_vector3d unknown2[6];
#endif

	float split_size;

	bool is_night;

	uint8_t static_time;

	/* TODO */
	uint16_t flags;

	uint8_t rule_type;	/* enum dbo_game_rule_type */

	DBOID world_rule_id;

	DBOID world_out_id;
	dbo_vector3d pos_world_out;
	dbo_vector3d dir_world_out;

	WCHAR resource_folder[DBO_WORLD_RESOURCE_FOLDER_LEN + 1];

	float bgm_reset_time;

#if DBO_VERSION_NEW
	/* TODO */
	uint32_t world_resource_id;
#else
	uint16_t world_resource_id;
#endif

	float free_camera_height;

	WCHAR resource_flash_enter[DBO_WORLD_RESOURCE_FLASH_LEN + 1];
	WCHAR resource_flash_leave[DBO_WORLD_RESOURCE_FLASH_LEN + 1];

	DBOID link_id;

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

	const dbo_data_table_world_st *world_data_get(DBOID id);

protected:
	int on_record(void *record, size_t record_size);

private:
	typedef std::map<DBOID, dbo_data_table_world_st *> worlds_map;
	worlds_map worlds_;
};



#endif

