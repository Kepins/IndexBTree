#ifndef BTREE_PAGE_BUFFER_MANAGER_H
#define BTREE_PAGE_BUFFER_MANAGER_H

#include <string>
#include <map>

#include "BTreePage.h"

class BTreePageBufferManager
{
private:
	const int32_t PAGE_SIZE;
	const int32_t MAX_ELEMENTS;

	// Number of pages in buffer
	int32_t pages_num;
	// Flags to each page in pages_data
	char* pages_flags;
	// Maps address to BTreePage in pages_data
	std::map<int64_t, BTreePage*> pages_map;
	// Buffer to hold pages
	BTreePage** pages_data;

	// Idx which page to replace in pages_data
	int32_t idx_pages_data;

public:
	BTreePageBufferManager(int32_t pages_cache, int32_t page_size, int32_t max_elements);
	~BTreePageBufferManager();

	const BTreePage* getPage(int64_t address);
	void flushPages();
};

#endif