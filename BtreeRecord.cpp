#include "BTreeRecord.h"

// Constructor that takes all fields
BTreeRecord::BTreeRecord(int64_t key, int64_t address) {
	this->key = key;
	this->address = address;
}

// Constructor that takes bytes which represent BTreeRecord in memory
BTreeRecord::BTreeRecord(const char* bytes) {
	key = *(int64_t*)&bytes[0];
	address = *(int64_t*)&bytes[sizeof(key)];
}
