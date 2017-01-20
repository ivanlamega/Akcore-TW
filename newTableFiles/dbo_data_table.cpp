
#include <string.h>
#include <stdio.h>

#include "dbo_common.h"
#include "dbo_data_table.h"
#include "dbo_log.h"


int
dbo_data_table::load(const char *rdf_file, size_t record_size)
{
	uint8_t padding;
	char *record;
	size_t ret;
	uint32_t data_size;
	FILE *f;

	f = fopen(rdf_file, "rb");

	if (!f)
	{
		return -1;
	}

#if DBO_VERSION_NEW
	/* seems to be always 0 */
	fread(&padding, sizeof(padding), 1, f);
#endif

	fread(&data_size, sizeof(data_size), 1, f);

	/* seems to be always 1 */
	fread(&padding, sizeof(padding), 1, f);

	record = new char[record_size];

	memset(record, 0, record_size);

	while (!feof(f))
	{
		ret = fread(record, record_size, 1, f);

		if (ret == 1)
		{
			if (on_record(record, ret) != 0)
			{
				return -1;
			}
		}
		else if (!feof(f))
		{
			dbo_log(DBO_LOG_LEVEL_ERROR, "%s: error loading [%s]: read %d vs %d\n",
					__FUNCTION__, rdf_file, record_size, ret);
			return -1;
		}
	}

	delete[] record;
	fclose(f);
	f = NULL;

	return 0;
}


