
#include <string.h>

#include "dbo_common.h"
#include "dbo_data_table_pc.h"
#include "dbo_log.h"


int
dbo_data_table_pc::load(const char *rdf_file)
{
	return dbo_data_table::load(rdf_file, sizeof(dbo_data_table_pc_st));
}

int
dbo_data_table_pc::on_record(void *record, size_t record_size)
{
	dbo_data_table_pc_st *pc = (dbo_data_table_pc_st *) record;

	/* TODO: more error checking */

#if DBO_DEBUG
	dbo_log(DBO_LOG_LEVEL_INFO, "%s: %d %d %d %d [%s] [%s]\n",
			__FUNCTION__, pc->id,
			pc->race, pc->clazz, pc->gender,
			pc->model_child, pc->model_adult);
#endif

	memcpy(&pcs_[pc->race][pc->clazz][pc->gender],
			(void *) pc, sizeof(dbo_data_table_pc_st));

	return 0;
}


const dbo_data_table_pc_st *
dbo_data_table_pc::pc_data_get(enum dbo_char_race race,
		enum dbo_char_class clazz,
		enum dbo_char_gender gender)
{
	if (race < DBO_CHAR_RACE_FIRST ||
			race >= DBO_CHAR_RACE_COUNT ||
			clazz < DBO_CHAR_CLASS_FIRST ||
			clazz >= DBO_CHAR_CLASS_COUNT ||
			gender < DBO_CHAR_GENDER_FIRST ||
			gender >= DBO_CHAR_GENDER_COUNT)
	{
		return NULL;
	}

	return &pcs_[race][clazz][gender];
}

