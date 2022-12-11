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

	// Blocks writes
	int64_t counter_writes = 0;

	// Blocks reads
	int64_t counter_reads = 0;

public:
	DiskFile(const std::string& filePath, int32_t block_size = 4096);
	virtual ~DiskFile();

	void read(int64_t address, char* buffer, int32_t size);
	void write(int64_t address, char* buffer, int32_t size);

	int64_t get_counter_writes() { return counter_writes; }
	int64_t get_counter_reads() { return counter_reads; }
	int64_t get_counter_all_op() { return counter_reads + counter_writes; }
};

#endif
