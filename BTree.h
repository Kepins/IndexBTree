#ifndef BTREE_H
#define BTREE_H

#include "BTreePage.h"
#include "BTreeRecord.h"

#include "BTreePageCache.h"

#include <iostream>


class BTree
{
private:
	// Tree order usually refered to as d
	const int32_t ORDER;
	// Current height of the tree
	int32_t height;
	// Address of root node
	int64_t root_addr;
	// Manager to access disk pages
	BTreePageCache pageCache;
	
public:
	BTree(const std::string& filePath, int32_t page_size, int32_t order, int32_t cache_size = 30);
	~BTree();

	// Inserts record
	int insert(const BTreeRecord& record);
	// Uses key to find record and fills address
	int get_record_addr(BTreeRecord& record);
	// Uses only key from record and removes record with same key
	int remove(const BTreeRecord& record);
	// Prints tree
	void print(std::ostream& os);
};

#endif