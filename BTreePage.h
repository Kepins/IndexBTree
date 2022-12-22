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

	// Has page been modified
	bool dirty = false;

	// ------ PAGE CONTENT ------
	
	// Bytes of the page
	// 8 B - parent page address
	// 4 B - number of elements on page
	// (8 B, 16B) x number of elements - address of child page and BTreeRecord
	// 8B - address of child page
	char* content;


public:
	// Constructor
	BTreePage(int32_t page_size, int32_t max_records);
	// Copy constructor
	BTreePage(const BTreePage& src);
	// Move constructor
	BTreePage(BTreePage&& t);
	// Assign operator
	BTreePage& operator=(const BTreePage& oth);
	// Assign move operator
	BTreePage& operator=(BTreePage&& t);
	// Destructor
	~BTreePage();
	

	// Return true is page has been modified
	bool isDirty() const { return dirty; }

	// Reset dirty
	void resetDirty() { this->dirty = false; }

	// Return pointer to PAGE_SIZE bytes that represent page
	const char* getPageContent() const { return content; };

	// Copy page content to bytes
	void copyPageContent(const char* bytes);

	// Move page content
	void movePageContent(char* bytes);

	// Get parent page address
	int64_t getParent() const;

	// Set parent page address
	void setParent(int64_t parent_address);

	// Get size
	int32_t getSize() const;

	// Set size
	void setSize(int32_t size);

	// Get address of child page
	int64_t getAddress(int32_t idx) const;

	// Set address of child page
	void setAddress(int32_t idx, int64_t address);
	
	// Get record
	BTreeRecord getRecord(int32_t idx) const;

	// Set record
	void setRecord(int32_t idx, BTreeRecord BTrecord);
};

#endif