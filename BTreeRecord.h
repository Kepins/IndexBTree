#ifndef BTREE_RECORD_H
#define BTREE_RECORD_H

#include <cstdint>

// Struct that represents one record in BTree
struct BTreeRecord
{
	// Key of record
	int64_t key;
	// Address of record in main file
	int64_t address;


	// Constructor that takes all fields
	BTreeRecord(int64_t key, int64_t address);
	// Constructor that takes bytes which represent BTreeRecord in memory
	BTreeRecord(const char* bytes);

};

#endif