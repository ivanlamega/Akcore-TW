
#ifndef DBO_DATA_TABLE_NEWBIE_H
#define DBO_DATA_TABLE_NEWBIE_H


#include "dbo_data_table.h"

const unsigned int DBO_NEWBIE_QUICKSLOT_MAX = 5;
#pragma pack(push, 4)
typedef struct
{
	DWORD id;

	uint8_t race;
	uint8_t clazz;

	DWORD world_id;
	DWORD tutorial_world_id;

	CNtlVector spawn_loc;
	CNtlVector spawn_dir;

	CNtlVector bind_loc;
	CNtlVector bind_dir;

	DWORD item_id[NTL_MAX_NEWBIE_QUICKSLOT_COUNT];
	uint8_t item_slot[NTL_MAX_NEWBIE_ITEM];
	uint8_t item_stack[NTL_MAX_ITEM_STACK_COUNT];

	DWORD map_name;

	DWORD skill_id[NTL_MAX_NEWBIE_SKILL];

	sQUICK_SLOT_DATA quickslot[DBO_NEWBIE_QUICKSLOT_MAX];

	uint8_t portal_id;

	uint8_t unknown1;	/* 0x0f ? */

//#if DBO_VERSION_NEW
	uint8_t unknown2;	/* 0x08 ? */
//#endif

	DWORD item_id1;
	uint8_t item_pos1;
	uint8_t item_stack1;

//#if DBO_VERSION_NEW
	uint8_t unknown3;	/* 0x01 ? */
	uint8_t unknown4;	/* 0x00 ? */
//#endif

} dbo_data_table_newbie_st;
#pragma pack(pop)

class dbo_data_table_newbie: public dbo_data_table
{
public:
	dbo_data_table_newbie() {};
	virtual ~dbo_data_table_newbie() {};

	int load(const char *rdf_file);

	const dbo_data_table_newbie_st *newbie_data_get(int race, int clazz);

protected:
	int on_record(void *record, size_t record_size);

private:
	dbo_data_table_newbie_st newbies_[RACE_COUNT][PC_CLASS_COUNT];
};


#endif

