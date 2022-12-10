#include "BTreePageBufferManager.h"

BTreePageBufferManager::BTreePageBufferManager(int32_t pages_cache, int32_t page_size, int32_t max_elements)
	: 
	PAGE_SIZE(page_size), 
	MAX_ELEMENTS(max_elements)
{
	// Set number of pages
	this->pages_num = pages_cache;
	// Allocate memory for flags
	this->pages_flags = new char[this->pages_num];
	// Allocate memory for pointers array
	this->pages_data = new BTreePage*[pages_num];
	for (int i = 0; i < pages_num; i++) {
		// Allocate memory for each page
		this->pages_data[i] = new BTreePage(page_size, max_elements);
	}
	// Set current idx to 0
	this->idx_pages_data = 0;
}

BTreePageBufferManager::~BTreePageBufferManager()
{
	// Delete array with flags
	delete[] this->pages_flags;
	for (int i = 0; i < pages_num; i++) {
		// Delete each page
		delete pages_data[i];
	}
	// Delte pointers array
	delete[] this->pages_data;
}

const BTreePage* BTreePageBufferManager::getPage(int64_t address) {
	std::map<int64_t, BTreePage*>::iterator it = pages_map.find(address);
	if (it != pages_map.end()) {
		// Return pointer to BTreePage of given address
		return it->second;
	}
	else {
		if (pages_map.size() < pages_num) {
			// Read page without erasing any data
		}
		else {
			// Remove one page from buffer and read page
		}
		// Increment curret idx
		idx_pages_data++;
		// Set it back to 0 if it goes out of bound
		idx_pages_data %= pages_num;
	}
}

void flushPages() {
	
}