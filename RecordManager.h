#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#include "Record.h"
#include <list>
#include "DiskFilePageManagerCache.h"

class RecordManager
{
	const int32_t BLOCK_SIZE;

	const int32_t RECORD_SIZE = Record::size;

	const int32_t RECORDS_PER_PAGE;

	std::list<int64_t> unused_records;

	int64_t nextRecordNum = 0;
	DiskFilePageManagerCache pageManager;
public:
	RecordManager(const std::string& filePath, int32_t cacheSize, int32_t block_size=4096);
	~RecordManager();

	int64_t insertRecord(const Record& record);
	Record getRecord(int64_t recordNum);
	void updateRecord(int64_t recordNum, const Record& record);
	void deleteRecord(int64_t recordNum);

	// Counters
	int64_t getCounterWrites() { return pageManager.getCounterWrites(); }
	int64_t getCounterReads() { return pageManager.getCounterReads(); }
	int64_t getCounterAllOp() { return pageManager.getCounterAllOp(); }
	int64_t getHowManyDirty() { return pageManager.getHowManyDirty(); }
};


#endif
