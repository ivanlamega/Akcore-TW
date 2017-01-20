
#ifndef DBO_DATA_TABLE_H
#define DBO_DATA_TABLE_H

#include <stdio.h>
#include <stdint.h>

class dbo_data_table
{
public:
	dbo_data_table() {};
	virtual ~dbo_data_table() {};

	virtual int load(const char *rdf_file) = 0;

protected:
	int load(const char *rdf_file, size_t record_size);

	/* return 0 means good, anything else means it should stop reading */
	virtual int on_record(void *record, size_t record_size) = 0;

private:
};




#endif

