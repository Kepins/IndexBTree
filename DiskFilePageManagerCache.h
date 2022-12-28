#ifndef DISKFILE_PAGE_MANAGER_CACHE_H
#define DISKFILE_PAGE_MANAGER_CACHE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "DiskFilePageManager.h"
#include "DiskPage.h"




class DiskFilePageManagerCache : DiskFilePageManager
{
private:
	// Size of page
	const int32_t BLOCK_SIZE;
	// Cache size
	const int32_t CACHE_SIZE;
	
	std::vector<DiskPage*> cache;
	std::map<int64_t, int32_t> cacheContent;
	std::list<int32_t> freeSlots;
	int32_t nextReplace;

	// Removes entry with value of idx from map
	int64_t removePageFromMap(int32_t idx);

	// Updates which page will be replaced in cache later
	void updateNextReplace();

	// Loads page from file to slot in cache
	void loadPageToSlot(int64_t page_num, int32_t slot);

	// How many accesses to substract
	int64_t noCount=0;

public:
	DiskFilePageManagerCache(const std::string& filePath, int32_t cacheSize, int32_t block_size = 4096);
	~DiskFilePageManagerCache();

	// Give unused page
	int64_t getNewPageNumber();
	// Reutrn unused page
	void returnPage(int64_t page_number);

	// Read one page
	void getPageContent(int64_t page_number, char* buffer);
	// Write one page
	void writePageContent(int64_t page_number, const char* buffer);
	// Flush all dirty pages to memory
	void flushPages();

	// Read page without incrementing counters
	void getPageContentNoCount(int64_t page_number, char* buffer);

	// Counters
	int64_t getCounterWrites();
	int64_t getCounterReads();
	int64_t getCounterAllOp();
	int64_t getHowManyDirty();

};



#endif