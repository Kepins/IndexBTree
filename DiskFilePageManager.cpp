#include "DiskFilePageManager.h"

DiskFilePageManager::DiskFilePageManager(const std::string& filePath, int32_t block_size) :
	DiskFile(filePath, block_size),
	next_page(0),
	BLOCK_SIZE(block_size)
{
}

DiskFilePageManager::~DiskFilePageManager()
{
}

int64_t DiskFilePageManager::get_new_page_number()
{
	// Check if list is not empty
	if (!unused_pages.empty()) {
		// Get first page from list
		int64_t page = unused_pages.front();
		// Pop first page from list
		unused_pages.pop_front();
		// Return page
		return page;
	}

	// Return and increment page number
	return next_page++;
}

void DiskFilePageManager::return_page(int64_t page_number)
{
	// Add returned page to list
	unused_pages.push_back(page_number);
}

void DiskFilePageManager::get_page_content(int64_t page_number, char* buffer)
{
	// Read one page
	DiskFile::read(page_number * BLOCK_SIZE, buffer, BLOCK_SIZE);
}

void DiskFilePageManager::write_page_content(int64_t page_number, const char* buffer)
{
	// Write one page
	DiskFile::write(page_number * BLOCK_SIZE, buffer, BLOCK_SIZE);
}
