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
	
	// Prints contents of one page
	static void printPage(std::ostream& os, const BTreePage& page, int64_t selfPageNum);

	// Returns the idx where key could be
	static int32_t binarySearch(const BTreePage& page, int64_t key);

	// Update children of a page
	void updateChildren(const BTreePage& page, int64_t pageNum);

	// Search recursively to find record.key and fill record.addr
	ReturnValue search(BTreeRecord& record, int64_t pageNum, int64_t* pageNumEnd);

	// We know that record is not in the tree
	void insertIntoPage(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum);

	// Try to insert into this specific page without spliting and compenstaion
	ReturnValue simpleInsertIntoPage(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum);

	// Try compenation
	ReturnValue compensate(int64_t pageNum);

	// Do compensate (it is possible)
	void doCompensate(BTreePage& left, BTreePage& parent, BTreePage& right, int32_t idxParent);

	// Do split
	void split(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum);

	// Print tree recursively
	void print(std::ostream& os, std::list<int64_t>& printQueue, int64_t* pageNumNewline);
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

	// Get counters
	int64_t getCounterReads() { return pageCache.get_counter_reads(); }
	int64_t getCounterWrites() { return pageCache.get_counter_writes(); }
	int64_t getCounterAllOp() { return pageCache.get_counter_all_op(); }
	int64_t getCounterWritesIfFlushed() { return pageCache.get_counter_writes_after_flush(); }
	int64_t getCounterAllOpIfFlushed() { return pageCache.get_counter_all_op_after_flush(); }
};

#endif