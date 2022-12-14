#include "DiskFile.h"

DiskFile::DiskFile(const std::string& filePath, int32_t block_size) :
	BLOCK_SIZE(block_size)
{
	// Create empty file
	std::fstream temp = std::fstream(filePath, std::ios::out);
	temp.close();

	// Open file stream
	file = std::fstream(filePath, std::ios::binary | std::ios::in | std::ios::out);
	if (!file.is_open()) {
		throw "File did not open!";
	}

}

DiskFile::~DiskFile()
{
	// Close file stream
	file.close();
}

void DiskFile::read(int64_t address , char* buffer, int32_t size)
{
	// Address of last byte that is requested to be read
	int64_t last_address = address + size - 1;

	// First block that would need to be read
	int64_t first_block = address / BLOCK_SIZE;
	// Last block that would need to be read
	int64_t last_block = last_address / BLOCK_SIZE;
	// Disk accesses for current operation
	int64_t current_operation_accesses = last_block - first_block + 1;

	// Add to counter
	counter_reads += current_operation_accesses;

	// Set pointer to address
	file.seekp(address);
	// Read 'size' bytes
	file.read(buffer, size);
	// Reset file if failed
	file.clear();
}

void DiskFile::write(int64_t address, const char* buffer, int32_t size)
{
	// Address of last byte that is requested to be written
	int64_t last_address = address + size - 1;

	// First block that would need to be written
	int64_t first_block = address / BLOCK_SIZE;
	// Last block that would need to be written
	int64_t last_block = last_address / BLOCK_SIZE;
	// Disk accesses for current operation
	int64_t current_operation_accesses = last_block - first_block + 1;

	// Add to counter
	counter_writes += current_operation_accesses;

	// Set pointer to address
	file.seekp(address);
	// Read 'size' bytes
	file.write(buffer, size);

	// Flush contents
	file.flush();
}
