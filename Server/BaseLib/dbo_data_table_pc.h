
#ifndef DBO_DATA_TABLE_PC_H
#define DBO_DATA_TABLE_PC_H

//#include "dbo_common.h"
#include "dbo_data_table.h"
#define DBO_VERSION_NEW 1

typedef struct
{
#if DBO_VERSION_NEW
	uint32_t basic_lp;

#endif

	uint16_t basic_ep;

	uint16_t basic_physical_defence;
	uint16_t basic_energy_defence;

#if DBO_VERSION_NEW
	uint16_t str;
	uint16_t con;
	uint16_t foc;
	uint16_t dex;
	uint16_t sol;
	uint16_t eng;
#endif

	float scale;

	uint16_t attack_speed_rate;
	uint8_t attack_type;

	float attack_range;

	uint16_t attack_rate;
	uint16_t dodge_rate;
	uint16_t block_rate;
	uint16_t curse_success_rate;
	uint16_t curse_tolerance_rate;

	float radius;

	uint16_t basic_aggro_point;

} dbo_data_table_char_st;


typedef struct
{
	DWORD id;

	dbo_data_table_char_st table_char;

	uint8_t race;
	uint8_t gender;
	uint8_t clazz;

	DWORD previous_class_id;

	uint32_t class_bit_flag;

	char model_child[NTL_MAX_LENGTH_MODEL_NAME + 1];
	char model_adult[NTL_MAX_LENGTH_MODEL_NAME + 1];

	float child_run_speed_origin;
	float child_run_speed;
	float adult_run_speed_origin;
	float adult_run_speed;

#if DBO_VERSION_NEW
	float unknown1[12];
#endif
	
	uint16_t basic_physical_offence;
	uint16_t basic_energy_offence;

#if DBO_VERSION_NEW
	uint32_t unknown2;
#endif

	uint8_t level_up_lp;
	uint8_t level_up_ep;
	uint8_t level_up_rp;
	uint8_t level_up_physical_offence;
	uint8_t level_up_physical_defence;
	uint8_t level_up_energy_offence;
	uint8_t level_up_energy_defence;
	
	float level_up_str;
	float level_up_con;
	float level_up_foc;
	float level_up_dex;
	float level_up_sol;
	float level_up_eng;
#if DBO_VERSION_NEW
	uint32_t unknown3;
#endif
} dbo_data_table_pc_st;

class dbo_data_table_pc: public dbo_data_table
{
public:
	dbo_data_table_pc() {};
	virtual ~dbo_data_table_pc() {};

	int load(const char *rdf_file);

	const dbo_data_table_pc_st *pc_data_get(int race,
			int clazz,
			int gender);

protected:
	int on_record(void *record, size_t record_size);

private:
	dbo_data_table_pc_st pcs_[RACE_COUNT][PC_CLASS_COUNT][GENDER_COUNT];

};


#endif

