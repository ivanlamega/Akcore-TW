
#ifndef DBO_DATA_TABLE_NEWBIE_H
#define DBO_DATA_TABLE_NEWBIE_H

#include "dbo_common.h"
#include "dbo_data_table.h"


#pragma pack(push, 4)
typedef struct
{
	DBOID id;

	uint8_t race;
	uint8_t clazz;

	DBOID world_id;
	DBOID tutorial_world_id;

	dbo_vector3d spawn_loc;
	dbo_vector3d spawn_dir;

	dbo_vector3d bind_loc;
	dbo_vector3d bind_dir;

	DBOID item_id[DBO_NEWBIE_ITEM_MAX];
	uint8_t item_slot[DBO_NEWBIE_ITEM_MAX];
	uint8_t item_stack[DBO_NEWBIE_ITEM_MAX];

	DBOID map_name;

	DBOID skill_id[DBO_NEWBIE_SKILL_MAX];

	dbo_newbie_quickslot quickslot[DBO_NEWBIE_QUICKSLOT_MAX];

	uint8_t portal_id;

	uint8_t unknown1;	/* 0x0f ? */

#if DBO_VERSION_NEW
	uint8_t unknown2;	/* 0x08 ? */
#endif

	DBOID item_id1;
	uint8_t item_pos1;
	uint8_t item_stack1;

#if DBO_VERSION_NEW
	uint8_t unknown3;	/* 0x01 ? */
	uint8_t unknown4;	/* 0x00 ? */
#endif

} dbo_data_table_newbie_st;
#pragma pack(pop)

class dbo_data_table_newbie: public dbo_data_table
{
public:
	dbo_data_table_newbie() {};
	virtual ~dbo_data_table_newbie() {};

	int load(const char *rdf_file);

	const dbo_data_table_newbie_st *newbie_data_get(enum dbo_char_race race,
			enum dbo_char_class clazz);

protected:
	int on_record(void *record, size_t record_size);

private:
	dbo_data_table_newbie_st newbies_[DBO_CHAR_RACE_COUNT][DBO_CHAR_CLASS_COUNT];
};


#endif

