#include "BTreePage.h"

BTreePage::BTreePage(int32_t page_size, int32_t max_records) :
	MAX_RECORDS(max_records),
	PAGE_SIZE(page_size)
{
	// Minimal size of page
	int32_t min_size = HEADER_OFFSET			// Size of header
		+ sizeof(int64_t)*(MAX_RECORDS+1)		// Size of child pages' addresses
		+ sizeof(BTreeRecord)*(MAX_RECORDS);	// Size of BTreeRecords

	if (PAGE_SIZE >= min_size) {
		// Page of size PAGE_SIZE will be able to contain MAX_RECORDS records
		content = new char[PAGE_SIZE];
	}
	else {
		// Page of size PAGE_SIZE will not be able to contain MAX_RECORDS records!
		throw "Page too small!";
	}
}

BTreePage::~BTreePage() {
	// Free memory
	delete[] content;
}

int64_t BTreePage::getAddress(int32_t idx)
{
	return *(int64_t*)&content[
		HEADER_OFFSET					// Size of header
		+ sizeof(int64_t) * idx			// Size of addresses
		+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	];
}

void BTreePage::setAddress(int32_t idx, int64_t address) {
	std::memcpy(&content[
		HEADER_OFFSET						// Size of header
			+ sizeof(int64_t) * idx			// Size of addresses
			+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	], &address, sizeof(address));
}

BTreeRecord BTreePage::getRecord(int32_t idx)
{
	// Get pointer to bytes
	const char* bytes = &content[
		HEADER_OFFSET					// Size of header
		+ sizeof(int64_t) * (idx+1)		// Size of addresses
		+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	];

	// Return BTreeRecord constructed from those bytes
	return BTreeRecord(bytes);
}

void BTreePage::setRecord(int32_t idx, BTreeRecord BTrecord) {
	memcpy(&content[
		HEADER_OFFSET					// Size of header
		+ sizeof(int64_t) * (idx + 1)	// Size of addresses
		+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	], &BTrecord.key, sizeof(BTrecord));
}

int64_t BTreePage::getParent()
{
	return *(int64_t*)&content[
		0
	];
}

void BTreePage::setParent(int64_t parent_address)
{
	memcpy(&content[
		0
	], &parent_address, sizeof(parent_address));
}

int32_t BTreePage::getSize() {
	return *(int32_t*)&content[
		sizeof(int64_t)
	];
}

void BTreePage::setSize(int32_t size) {
	memcpy(&content[
		sizeof(int64_t)
	], &size, sizeof(size));
}