#include "BTreePageCache.h"

int64_t BTreePageCache::remove_page_from_map(int32_t idx)
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
	for (int i = 0; i < pages_num; i++) {
		delete pages_data[i];
	}
}

const BTreePage* BTreePageCache::getPage(int64_t page_number) {
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		// Return pointer to BTreePage of given address
		return pages_data[it->second];
	}
	else {
		if (free_slots.size() > 0) {
			// Read page without erasing any data

			// Get free_slot
			int32_t free_slot = free_slots.front();
			free_slots.pop_front();

			// Set content
			char* content = new char [PAGE_SIZE];
			pageManager.get_page_content(page_number, content);
			pages_data[free_slot]->setPageContent(content);
			pages_data[free_slot]->resetDirty();
			delete[] content;
			

			pages_map.insert(std::pair<int64_t, int32_t>(page_number, free_slot));

			return pages_data[free_slot];
		}
		else {
			// Remove one page from buffer and read page
			
			// Remove entry from map
			int64_t addr = remove_page_from_map(idx_pages_data);

			int32_t free_slot = idx_pages_data;

			// Write page out if it has been modified
			if (pages_data[free_slot]->isDirty()) {
				pageManager.write_page_content(addr, pages_data[free_slot]->getPageContent());
			}

			// Set content
			char* content = new char[PAGE_SIZE];
			pageManager.get_page_content(page_number, content);
			pages_data[free_slot]->setPageContent(content);
			pages_data[free_slot]->resetDirty();
			delete[] content;

			pages_map.insert(std::pair<int64_t, int32_t>(page_number, free_slot));

			// Increment curret idx
			idx_pages_data++;
			// Set it back to 0 if it goes out of bound
			idx_pages_data %= pages_num;

			return pages_data[free_slot];
		}
	}
}

void BTreePageCache::setPageContent(int64_t page_number, const char* content)
{
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		// Set content of this page
		pages_data[it->second]->setPageContent(content);
	}
	else {
		// Remove entry from map
		int64_t addr = remove_page_from_map(idx_pages_data);

		// Write page out if it has been modified
		if (pages_data[idx_pages_data]->isDirty()) {
			pageManager.write_page_content(addr, pages_data[idx_pages_data]->getPageContent());
		}

		pages_data[idx_pages_data]->setPageContent(content);
		pages_map.insert(std::pair<int64_t, int32_t>(page_number, idx_pages_data));

		// Increment curret idx
		idx_pages_data++;
		// Set it back to 0 if it goes out of bound
		idx_pages_data %= pages_num;
	}
}

void BTreePageCache::returnPage(int64_t page_number)
{
	// Find what idx in pages_data it had
	std::map<int64_t, int32_t>::iterator it = pages_map.find(page_number);
	if (it != pages_map.end()) {
		int32_t new_free_slot = pages_map.find(page_number)->second;

		// Remove entry from map
		pages_map.erase(page_number);

		// Add new free slot to list
		free_slots.push_back(new_free_slot);
	}
	pageManager.return_page(page_number);
}

void BTreePageCache::flushPages()
{
	std::map<int64_t, int32_t>::iterator it = pages_map.begin();
	while (it != pages_map.end()) {
		int64_t addr = it->first;
		int32_t idx = it->second;
		if (pages_data[idx]->isDirty()) {
			pageManager.write_page_content(addr, pages_data[idx]->getPageContent());
			pages_data[idx]->resetDirty();
		}

		it++;
	}
}

