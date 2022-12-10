#ifndef BTREE_PAGE_H
#define BTREE_PAGE_H

#include <cstdint>
#include <iostream>

#include "BTreeRecord.h"

class BTreePage
{
	// Number of BTreeRecords that cannot be exceeded = 2 * d
	const int32_t MAX_RECORDS;
	// Size of this page in bytes
	const int32_t PAGE_SIZE;
	
	const int32_t HEADER_OFFSET = sizeof(int64_t) + sizeof(int32_t);

	// ------ PAGE CONTENT ------
	
	// Bytes of the page
	// 8 B - parent page address
	// 4 B - number of elements on page
	// (8 B, 16B) x number of elements - address of child page and BTreeRecord
	// 8B - address of child page
	char* content;


public:
	
	BTreePage(int32_t page_size, int32_t max_records);
	~BTreePage();
	

	// Return PAGE_SIZE bytes that represent page
	const char* get_page_content() { return content; };

	// Get parent page address
	int64_t getParent();

	// Set parent page address
	void setParent(int64_t parent_address);

	// Get size
	int32_t getSize();

	// Set size
	void setSize(int32_t size);

	// Get address of child page
	int64_t getAddress(int32_t idx);

	// Set address of child page
	void setAddress(int32_t idx, int64_t address);
	
	// Get record
	BTreeRecord getRecord(int32_t idx);

	// Set record
	void setRecord(int32_t idx, BTreeRecord BTrecord);
};

#endif