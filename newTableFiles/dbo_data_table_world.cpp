
#include <string.h>

#include "dbo_common.h"
#include "dbo_data_table_world.h"
#include "dbo_log.h"


dbo_data_table_world::~dbo_data_table_world()
{
	for (worlds_map::iterator it = worlds_.begin();
			it != worlds_.end();
			it++)
	{
		delete it->second;
	}
	worlds_.clear();
}


int
dbo_data_table_world::load(const char *rdf_file)
{
	return dbo_data_table::load(rdf_file, sizeof(dbo_data_table_world_st));
}

int
dbo_data_table_world::on_record(void *record, size_t record_size)
{
	dbo_data_table_world_st *world = new dbo_data_table_world_st;

	memcpy(world, record, sizeof(dbo_data_table_world_st));

	/* TODO: more error checking */

#if DBO_DEBUG
	dbo_log(DBO_LOG_LEVEL_INFO, "%d %s, is_dynamic %d, cnt %d, rule_type %d\n",
			world->id,
			world->name,
			world->is_dynamic,
			world->cnt,
			world->rule_type);
#endif

	worlds_[world->id] = world;

	return 0;
}


const dbo_data_table_world_st *
dbo_data_table_world::world_data_get(DBOID id)
{
	return worlds_[id];
}

