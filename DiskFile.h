#ifndef DISKFILE_H
#define DISKFILE_H

#include <fstream>

class DiskFile
{
private:
	// File stream to opened file
	std::fstream file;
	
	// Disk page size
	const int32_t BLOCK_SIZE;

	// Number of blocks in file
	const int64_t NUM_BLOCKS;

	// Biggest address
	const int64_t BIGGEST_ADDRESS;

	// Blocks writes
	int64_t counter_writes = 0;

	// Blocks reads
	int64_t counter_reads = 0;

public:
	DiskFile(const std::string& filePath, int64_t blocks, int32_t block_size = 4096);
	~DiskFile();

	void read(int64_t address, char* buffer, int32_t size);
	void write(int64_t address, char* buffer, int32_t size);
};

#endif
