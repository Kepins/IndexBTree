#include "RecordManager.h"

RecordManager::RecordManager(const std::string& filePath, int32_t cache_size, int32_t block_size) 
	:
	BLOCK_SIZE(block_size),
	RECORDS_PER_PAGE(BLOCK_SIZE/RECORD_SIZE),
	pageManager(filePath, cache_size, block_size)
{
}

RecordManager::~RecordManager()
{
}

int64_t RecordManager::insertRecord(const Record& record)
{
	// Where the record will be inserted
	int64_t recordNum;

	int64_t pageNum;
	int64_t offset;
	if (!unused_records.empty()) {
		recordNum = unused_records.front();
		unused_records.pop_front();
		offset = (recordNum % RECORDS_PER_PAGE) * RECORD_SIZE;
	}
	else {
		recordNum = nextRecordNum++;
		offset = (recordNum % RECORDS_PER_PAGE) * RECORD_SIZE;
		if (offset == 0) {
			pageNum = pageManager.getNewPageNumber();
			char* pageContent = new char[BLOCK_SIZE];
			memcpy(pageContent + offset, &record.content, RECORD_SIZE);
			pageManager.writePageContent(pageNum, pageContent);
			
			return recordNum;
		}
	}
	pageNum = recordNum / RECORDS_PER_PAGE;

	char* pageContent = new char[BLOCK_SIZE];
	pageManager.getPageContent(pageNum, pageContent);
	memcpy(pageContent + offset, &record.content, RECORD_SIZE);
	pageManager.writePageContent(pageNum, pageContent);
	delete[] pageContent;

	return recordNum;
}

Record RecordManager::getRecord(int64_t recordNum)
{
	int64_t pageNum = recordNum / RECORDS_PER_PAGE;
	int64_t offset = (recordNum % RECORDS_PER_PAGE) * RECORD_SIZE;

	Record record;

	char* pageContent = new char[BLOCK_SIZE];

	pageManager.getPageContent(pageNum, pageContent);
	memcpy(&record.content, pageContent + offset, RECORD_SIZE);
	delete[] pageContent;

	return record;
}

void RecordManager::updateRecord(int64_t recordNum, const Record& record)
{
	int64_t pageNum = recordNum / RECORDS_PER_PAGE;
	int64_t offset = (recordNum % RECORDS_PER_PAGE) * RECORD_SIZE;

	char* pageContent = new char[BLOCK_SIZE];

	pageManager.getPageContent(pageNum, pageContent);
	memcpy(pageContent + offset, &record.content, RECORD_SIZE);
	pageManager.writePageContent(pageNum, pageContent);
	delete[] pageContent;
}

void RecordManager::deleteRecord(int64_t recordNum)
{
	unused_records.push_back(recordNum);
}
