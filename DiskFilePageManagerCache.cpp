#include "DiskFilePageManagerCache.h"

int64_t DiskFilePageManagerCache::removePageFromMap(int32_t idx)
{
	std::map<int64_t, int32_t>::iterator it = cacheContent.begin();
	while (it != cacheContent.end()) {
		if (it->second == idx) {
			int64_t addr = it->first;
			cacheContent.erase(it);
			return addr;
		}
		it++;
	}
}

void DiskFilePageManagerCache::updateNextReplace()
{
	// Increment curret idx
	nextReplace++;
	// Set it back to 0 if it goes out of bound
	nextReplace %= CACHE_SIZE;
}

void DiskFilePageManagerCache::loadPageToSlot(int64_t page_num, int32_t slot)
{
	// Set content
	char* content = new char[BLOCK_SIZE];
	// Read from file
	DiskFilePageManager::get_page_content(page_num, content);
	// Page will free allocated memory
	cache[slot]->movePageContent(content, BLOCK_SIZE);
	// Reset dirty bit
	cache[slot]->resetDirty();
}

DiskFilePageManagerCache::DiskFilePageManagerCache(const std::string& filePath, int32_t cacheSize, int32_t block_size)
	:
	CACHE_SIZE(cacheSize),
	BLOCK_SIZE(block_size),
	DiskFilePageManager(filePath, block_size),
	cache(),
	cacheContent(),
	freeSlots(),
	nextReplace(0)
{
	for (int32_t i = 0; i < CACHE_SIZE; i++) {
		cache.push_back(new DiskPage(BLOCK_SIZE));
		freeSlots.push_back(i);
	}
}

DiskFilePageManagerCache::~DiskFilePageManagerCache()
{
	for (int32_t i = 0; i < CACHE_SIZE; i++) {
		delete cache[i];
	}
}

int64_t DiskFilePageManagerCache::getNewPageNumber()
{
	return DiskFilePageManager::get_new_page_number();
}

void DiskFilePageManagerCache::returnPage(int64_t page_number)
{
	// Find what idx in pages_data it had
	std::map<int64_t, int32_t>::iterator it = cacheContent.find(page_number);
	if (it != cacheContent.end()) {
		// Page was in cache

		// Find what slot in cache did page with page_number
		int32_t new_free_slot = cacheContent.find(page_number)->second;

		// Remove entry from map
		cacheContent.erase(page_number);

		// Add new free slot to list
		freeSlots.push_back(new_free_slot);
	}
	// After cache associated thigs update pageManager
	DiskFilePageManager::return_page(page_number);
}

void DiskFilePageManagerCache::getPageContent(int64_t page_number, char* buffer)
{
	std::map<int64_t, int32_t>::iterator it = cacheContent.find(page_number);
	if (it != cacheContent.end()) {
		// Page is in cache

		// Get pointer to page in cache
		DiskPage* page = cache[it->second];
		// Copy content
		memcpy(buffer, page->getPageContent(), BLOCK_SIZE);

		// End functin call
		return;
	}
	// Slot where page in cache will be
	int32_t slot;
	if (!freeSlots.empty()) {
		// Read page without erasing any data

		// Get free_slot
		slot = freeSlots.front();
		freeSlots.pop_front();
		// Insert pair (address, slot) to map
		cacheContent.insert(std::pair<int64_t, int32_t>(page_number, slot));
	}
	else {
		// Remove one page from cache and read new page

		// Set which page will be removed from cache
		slot = nextReplace;

		// Remove entry from map
		int64_t oldPageNumber = removePageFromMap(slot);

		// Check if page was modified
		if (cache[slot]->isDirty()) {
			// Write page out if it has been modified
			DiskFilePageManager::write_page_content(
				oldPageNumber,
				cache[slot]->getPageContent()
			);
		}
		// Insert pair (address, slot) to map
		cacheContent.insert(std::pair<int64_t, int32_t>(page_number, slot));
		// Update which page to replace in cache
		updateNextReplace();
	}
	// There must be a new page loaded
	loadPageToSlot(page_number, slot);

	// Copy content
	memcpy(buffer, cache[slot]->getPageContent(), BLOCK_SIZE);
}

void DiskFilePageManagerCache::writePageContent(int64_t page_number, const char* buffer)
{
	std::map<int64_t, int32_t>::iterator it = cacheContent.find(page_number);
	if (it != cacheContent.end()) {
		// Page is in cache

		// Set content of this page
		cache[it->second]->copyPageContent(buffer, BLOCK_SIZE);
		return;
	}
	// Page is not in cache

	// Slot where page in cache will be
	int32_t slot;
	if (freeSlots.size() > 0) {
		// Get free_slot
		slot = freeSlots.front();
		freeSlots.pop_front();
		// Insert pair (address, slot) to map
		cacheContent.insert(std::pair<int64_t, int32_t>(page_number, slot));
	}
	else {
		// Set which page will be removed from cache
		slot = nextReplace;

		// Remove entry from map
		int64_t addr = removePageFromMap(slot);

		// Write page out if it has been modified
		if (cache[slot]->isDirty()) {
			DiskFilePageManager::write_page_content(addr, cache[slot]->getPageContent());
		}

		// Insert pair (address, slot) to map
		cacheContent.insert(std::pair<int64_t, int32_t>(page_number, slot));

		// Update which page to replace in pages_data
		updateNextReplace();
	}
	// Set page in cache[nextReplace] to data passed to method
	cache[slot]->copyPageContent(buffer, BLOCK_SIZE);
}

void DiskFilePageManagerCache::flushPages()
{
	std::map<int64_t, int32_t>::iterator it = cacheContent.begin();
	// Iterate over every entry in map
	while (it != cacheContent.end()) {
		// Page number of entry in map
		int64_t page_number = it->first;
		// Idx in pages_data of entry in map
		int32_t idx = it->second;
		// Check if page has been modified
		if (cache[idx]->isDirty()) {
			// If page was modified

			// Write it's contnent to file
			DiskFilePageManager::write_page_content(page_number, cache[idx]->getPageContent());
			cache[idx]->resetDirty();
		}
		// Get next entry
		it++;
	}
}

void DiskFilePageManagerCache::getPageContentNoCount(int64_t page_number, char* buffer)
{
	std::map<int64_t, int32_t>::iterator it = cacheContent.find(page_number);
	if (it != cacheContent.end()) {
		// Page is in cache

		// Get pointer to page in cache
		DiskPage* page = cache[it->second];
		// Copy content
		memcpy(buffer, page->getPageContent(), BLOCK_SIZE);

		// End functin call
		return;
	}
	int64_t before_counter = DiskFilePageManager::get_counter_all_op();

	// Read from file
	DiskFilePageManager::get_page_content(page_number, buffer);

	// Add number of accesses to counter
	noCount += DiskFilePageManager::get_counter_all_op() - before_counter;
}

int64_t DiskFilePageManagerCache::getCounterWrites()
{
	return DiskFilePageManager::get_counter_writes() - noCount;
}

int64_t DiskFilePageManagerCache::getCounterReads()
{
	return DiskFilePageManager::get_counter_reads();
}

int64_t DiskFilePageManagerCache::getCounterAllOp()
{
	return DiskFilePageManager::get_counter_all_op() - noCount;
}

int64_t DiskFilePageManagerCache::getHowManyDirty()
{
	// Will store how many pages in cache are dirty
	int64_t howManyDirty = 0;

	std::map<int64_t, int32_t>::iterator it = cacheContent.begin();
	// Iterate over every entry in map
	while (it != cacheContent.end()) {
		// Page number of entry in map
		int64_t page_number = it->first;
		// Idx in pages_data of entry in map
		int32_t idx = it->second;
		// Check if page has been modified
		if (cache[idx]->isDirty()) {
			// If page was modified

			// Increment howManyDirty
			++howManyDirty;
		}
		// Get next entry
		it++;
	}
	// Return how many dirty pages are in cache
	return howManyDirty;
}
