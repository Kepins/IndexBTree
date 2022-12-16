#include "BTreePageCache.h"

BTreePageCache::BTreePageCache(const std::string& filePath, int32_t pages_cache, int32_t page_size, int32_t max_elements)
	: 
	PAGE_SIZE(page_size), 
	MAX_ELEMENTS(max_elements),
	pageManager(filePath, PAGE_SIZE),
	pages_data()
{
	// Set number of pages
	this->pages_num = pages_cache;
	
	// Create pages in cache
	for (int i = 0; i < pages_num; i++) {
		// Allocate memory for each page
		pages_data.push_back(new BTreePage(PAGE_SIZE, MAX_ELEMENTS));
		free_slots.push_back(i);
	}
	// Set current idx to 0
	this->idx_pages_data = 0;
}

BTreePageCache::~BTreePageCache()
{
	// Free all alocated memory
	for (int i = 0; i < pages_num; i++) {
		// Delete one page from memory
		delete pages_data[i];
	}
}

int64_t BTreePageCache::RemovePageFromMap(int32_t idx)
{
	std::map<int64_t, int32_t>::iterator it = pages_map.begin();
	while (it != pages_map.end()) {
		if (it->second == idx) {
			int64_t addr = it->first;
			pages_map.erase(it);
			return addr;
		}
		it++;
	}
}

void BTreePageCache::loadPageToSlot(int64_t page_num, int32_t slot) {
	// Set content
	char* content = new char[PAGE_SIZE];
	// Read from file
	pageManager.get_page_content(page_num, content);
	// Page will free allocated memory
	pages_data[slot]->movePageContent(content);
	// Reset dirty bit
	pages_data[slot]->resetDirty();
}

void BTreePageCache::updateIdxPagesData() {
	// Increment curret idx
	idx_pages_data++;
	// Set it back to 0 if it goes out of bound
	idx_pages_data %= pages_num;
}

BTreePage BTreePageCache::getPage(int64_t page_number) {
	
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		// Page is in cache

		// Return entry in pages_data contains page
		return *pages_data[it->second];
	}
	// Slot where page in cache will be
	int32_t slot;
	if (free_slots.size() > 0) {
		// Read page without erasing any data

		// Get free_slot
		slot = free_slots.front();
		free_slots.pop_front();
		// Insert pair (address, slot) to map
		pages_map.insert(std::pair<int64_t, int32_t>(page_number, slot));
	}
	else {
		// Remove one page from pages_data and read new page
		
		// Set which page will be removed from cache
		slot = idx_pages_data;

		// Remove entry from map
		int64_t old_page_number = RemovePageFromMap(slot);

		// Check if page was modified
		if (pages_data[slot]->isDirty()) {
			// Write page out if it has been modified
			pageManager.write_page_content(
				old_page_number, 
				pages_data[slot]->getPageContent()
			);
		}
		// Insert pair (address, slot) to map
		pages_map.insert(std::pair<int64_t, int32_t>(page_number, slot));
		// Update which page to replace in pages_data
		updateIdxPagesData();
	}
	// There must be a new page loaded
	loadPageToSlot(page_number, slot);

	return *pages_data[slot];
}

void BTreePageCache::setPage(int64_t page_number, const BTreePage& page)
{
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		// Page is in cache

		// Set content of this page
		*pages_data[it->second] = page;
		return;
	}
	// Page is not in cache

	// Slot where page in cache will be
	int32_t slot;
	if (free_slots.size() > 0) {
		// Get free_slot
		slot = free_slots.front();
		free_slots.pop_front();
		// Insert pair (address, slot) to map
		pages_map.insert(std::pair<int64_t, int32_t>(page_number, slot));
	}
	else {
		// Set which page will be removed from cache
		slot = idx_pages_data;

		// Remove entry from map
		int64_t addr = RemovePageFromMap(slot);

		// Write page out if it has been modified
		if (pages_data[slot]->isDirty()) {
			pageManager.write_page_content(addr, pages_data[slot]->getPageContent());
		}

		// Insert pair (address, slot) to map
		pages_map.insert(std::pair<int64_t, int32_t>(page_number, slot));

		// Update which page to replace in pages_data
		updateIdxPagesData();
	}
	// Set page in pages_data[idx_pages_data] to argument passed to method
	*pages_data[slot] = page;

}

void BTreePageCache::returnPage(int64_t page_number)
{
	// Find what idx in pages_data it had
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		// Page was in cache

		// Find what slot in cache did page with page_number
		int32_t new_free_slot = pages_map.find(page_number)->second;

		// Remove entry from map
		pages_map.erase(page_number);

		// Add new free slot to list
		free_slots.push_back(new_free_slot);
	}
	// After cache associated thigs update pageManager
	pageManager.return_page(page_number);
}

void BTreePageCache::flushPages()
{
	std::map<int64_t, int32_t>::iterator it = pages_map.begin();
	// Iterate over every entry in map
	while (it != pages_map.end()) {
		// Page number of entry in map
		int64_t page_number = it->first;
		// Idx in pages_data of entry in map
		int32_t idx = it->second;
		// Check if page has been modified
		if (pages_data[idx]->isDirty()) {
			// If page was modified

			// Write it's contnent to file
			pageManager.write_page_content(page_number, pages_data[idx]->getPageContent());
			pages_data[idx]->resetDirty();
		}
		// Get next entry
		it++;
	}
}

