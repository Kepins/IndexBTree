#ifndef BTREE_PAGE_BUFFER_MANAGER_H
#define BTREE_PAGE_BUFFER_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <list>

#include "BTreePage.h"
#include "DiskFilePageManager.h"

class BTreePageCache
{
private:
	// Bytes in one page
	const int32_t PAGE_SIZE;
	// Number of elements on page
	const int32_t MAX_ELEMENTS;

	// Number of pages in buffer
	int32_t pages_num;
	
	// Maps address to idx in pages_data
	std::map<int64_t, int32_t> pages_map;
	// Buffer to hold pages
	std::vector<BTreePage*> pages_data;

	// Free slots in pages_data
	std::list<int32_t> free_slots;

	// Idx which page to replace in pages_data
	int32_t idx_pages_data;

	// Removes entry with value of idx from map
	int64_t RemovePageFromMap(int32_t idx);

	// Loads page from file to slot in pages_data
	void loadPageToSlot(int64_t page_num, int32_t slot);

	// Update which page to replace in pages_data
	void updateIdxPagesData();

	// Page manager to get/return pages and read/write to them
	DiskFilePageManager pageManager;

public:
	// Constructor
	BTreePageCache(const std::string& filePath, int32_t pages_cache, int32_t page_size, int32_t max_elements);
	// Destructor
	~BTreePageCache();

	// Get page content
	BTreePage getPage(int64_t page_number);
	// Set page content
	void setPage(int64_t page_number, const BTreePage& page);
	// Return page to pool of availabel pages
	void returnPage(int64_t page_number);
	// Flush all dirty pages to memory
	void flushPages();

	// DiskFilePageManager functionality
	int64_t getNewPageNumber() { return pageManager.get_new_page_number(); }
	// Counters
	int64_t get_counter_writes() { return pageManager.get_counter_writes(); }
	int64_t get_counter_reads() { return pageManager.get_counter_reads(); }
	int64_t get_counter_all_op() { return pageManager.get_counter_all_op(); }
};

#endif