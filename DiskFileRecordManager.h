#ifndef DISKFILE_RECORD_MANAGER_H
#define DISKFILE_RECORD_MANAGER_H

#include "DiskFile.h"
#include <list>

class DiskFileRecordManager : private DiskFile
{
private:
	// Size of page
	const int32_t BLOCK_SIZE;
	// Size of record
	const int32_t RECORD_SIZE;
	// Number of records that fit in one disk page
	const int32_t RECORDS_PER_BLOCK;
	// List of removed records
	std::list<int64_t> unused_records;
	// Number of next record
	int64_t next_record;
public:
	DiskFileRecordManager(const std::string& filePath, int32_t record_size, int32_t block_size = 4096);
	~DiskFileRecordManager();

	// Give unused page
	int64_t get_new_record_number();
	// Reutrn unused page
	void return_record(int64_t record_number);

	// Read one page
	void get_record_content(int64_t record_number, char* buffer);
	// Write one page
	void write_record_content(int64_t record_number, const char* buffer);

	// Counters
	using DiskFile::get_counter_writes;
	using DiskFile::get_counter_reads;
	using DiskFile::get_counter_all_op;
};

#endif
