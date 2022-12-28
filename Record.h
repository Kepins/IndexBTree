#ifndef RECORD_H
#define RECORD_H

#include <cstdint>

const int32_t NUM_ELEMENTS = 10;

struct Record
{
	// Number of elements
	static const int numElements = NUM_ELEMENTS;
	// Size of record
	static const size_t size = NUM_ELEMENTS * sizeof(int32_t);
	// Array of elements
	int32_t content[NUM_ELEMENTS];
};


#endif
