#ifndef DISKFILE_PAGE_MANAGER_H
#define DISKFILE_PAGE_MANAGER_H

#include "DiskFile.h"
#include <list>

class DiskFilePageManager : private DiskFile
{
private:
	// Size of page
	const int32_t BLOCK_SIZE;
	// List of returned pages
	std::list<int64_t> unused_pages;
	// Number of next page
	int64_t next_page;
public:
	DiskFilePageManager(const std::string& filePath, int32_t block_size = 4096);
	~DiskFilePageManager();

	// Give unused page
	int64_t get_new_page_number();
	// Reutrn unused page
	void return_page(int64_t page_number);

	// Read one page
	void get_page_content(int64_t page_number, char* buffer);
	// Write one page
	void write_page_content(int64_t page_number, const char* buffer);

	// Counters
	using DiskFile::get_counter_writes;
	using DiskFile::get_counter_reads;
	using DiskFile::get_counter_all_op;
};

#endif