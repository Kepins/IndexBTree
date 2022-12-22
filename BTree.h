#ifndef BTREE_H
#define BTREE_H

#include "BTreePage.h"
#include "BTreeRecord.h"

#include "BTreePageCache.h"

#include <iostream>

enum class ReturnValue
{
	OK,
	NOT_FOUND,
	ALREADY_EXISTS,
	NOT_POSSIBLE
};

class BTree
{
private:
	// Tree order usually refered to as d
	const int32_t ORDER;
	// Current height of the tree
	int32_t height;
	// Address of root node
	int64_t root_addr;

	// Value to represent abbsence of a page
	static const int64_t NIL = INT64_MAX;

	// Manager to access disk pages
	BTreePageCache pageCache;
	
	// Search recursively to find record.key and fill record.addr
	ReturnValue search(BTreeRecord& record, int64_t pageNum, int64_t* pageNumEnd);

	// Returns the idx where key could be
	static int32_t bisection(const BTreePage& page, int64_t key);

	// Insert into this page
	ReturnValue insertIntoPage(int64_t pageNum, const BTreeRecord& record);

	// Try compenation
	ReturnValue compensate(int64_t pageNum);
public:
	BTree(const std::string& filePath, int32_t page_size, int32_t order, int32_t cache_size = 30);
	~BTree();

	// Inserts record
	ReturnValue insert(const BTreeRecord& record);
	// Uses key to find record and fills address
	ReturnValue search(BTreeRecord& record);
	// Uses only key from record and removes record with same key
	ReturnValue remove(const BTreeRecord& record);
	// Prints tree
	void print(std::ostream& os);
};

#endif