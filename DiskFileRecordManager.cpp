#include "DiskFileRecordManager.h"

DiskFileRecordManager::DiskFileRecordManager(const std::string& filePath, int32_t record_size, int32_t block_size) :
	DiskFile(filePath, block_size),
	next_record(0),
	BLOCK_SIZE(block_size),
	RECORD_SIZE(record_size),
	RECORDS_PER_BLOCK(block_size/record_size)
{
}

DiskFileRecordManager::~DiskFileRecordManager() 
{
}

int64_t DiskFileRecordManager::get_new_record_number()
{
	// Check if list is not empty
	if (!unused_records.empty()) {
		// Get first record addr from list
		int64_t page = unused_records.front();
		// Pop first record addr from list
		unused_records.pop_front();
		// Return record addr
		return page;
	}

	// Return and increment record number
	return next_record++;
}

void DiskFileRecordManager::return_record(int64_t record_number)
{
	// Add returned page to list
	unused_records.push_back(record_number);
}

void DiskFileRecordManager::get_record_content(int64_t record_number, char* buffer)
{
	// Addr of block start
	int64_t block_start = (record_number / RECORDS_PER_BLOCK) * BLOCK_SIZE;
	// Offset on that block
	int64_t offset = (record_number % RECORDS_PER_BLOCK) * RECORD_SIZE;

	// Read one record
	DiskFile::read(block_start + offset, buffer, RECORD_SIZE);
}

void DiskFileRecordManager::write_record_content(int64_t record_number, const char* buffer)
{
	// Addr of block start
	int64_t block_start = (record_number / RECORDS_PER_BLOCK) * BLOCK_SIZE;
	// Offset on that block
	int64_t offset = (record_number % RECORDS_PER_BLOCK) * RECORD_SIZE;

	// Write one record
	DiskFile::write(block_start + offset, buffer, RECORD_SIZE);
}