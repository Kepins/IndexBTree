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

BTreePage::BTreePage(const BTreePage& src) :
	MAX_RECORDS(src.MAX_RECORDS),
	PAGE_SIZE(src.PAGE_SIZE)
{
	this->dirty = src.dirty;
	if (this->PAGE_SIZE != src.PAGE_SIZE) {
		throw "Pages of differnt sizes!";
	}
	this->content = new char[PAGE_SIZE];
	memcpy(this->content, src.content, PAGE_SIZE);
}

BTreePage::BTreePage(BTreePage&& t) :
	MAX_RECORDS(t.MAX_RECORDS),
	PAGE_SIZE(t.PAGE_SIZE)
{
	this->dirty = t.dirty;
	this->content = t.content;
	t.content = nullptr;
}

BTreePage& BTreePage::operator=(const BTreePage& oth)
{
	this->dirty = oth.dirty;
	if (this->PAGE_SIZE != oth.PAGE_SIZE) {
		throw "Pages of differnt sizes!";
	}
	memcpy(this->content, oth.content, PAGE_SIZE);
	return *this;
}

BTreePage& BTreePage::operator=(BTreePage&& t)
{
	this->dirty = t.dirty;
	if (this->PAGE_SIZE != t.PAGE_SIZE) {
		throw "Pages of differnt sizes!";
	}
	this->content = t.content;
	t.content = nullptr;

	return *this;
}

BTreePage::~BTreePage() {
	// Free memory
	if (content != nullptr) {
		delete[] content;
	}
}

int64_t BTreePage::getAddress(int32_t idx) const
{
	return *(int64_t*)&content[
		HEADER_OFFSET					// Size of header
		+ sizeof(int64_t) * idx			// Size of addresses
		+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	];
}

void BTreePage::setAddress(int32_t idx, int64_t address) {
	this->dirty = true;
	std::memcpy(&content[
		HEADER_OFFSET						// Size of header
			+ sizeof(int64_t) * idx			// Size of addresses
			+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	], &address, sizeof(address));
}

BTreeRecord BTreePage::getRecord(int32_t idx) const
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
	this->dirty = true;
	memcpy(&content[
		HEADER_OFFSET					// Size of header
		+ sizeof(int64_t) * (idx + 1)	// Size of addresses
		+ sizeof(BTreeRecord) * idx		// Size of BTreeRecords
	], &BTrecord.key, sizeof(BTrecord));
}

void BTreePage::copyPageContent(const char* bytes)
{
	this->dirty = true;
	memcpy(this->content, bytes, PAGE_SIZE);
}

void BTreePage::movePageContent(char* bytes) {
	this->dirty = true;
	delete[] content;
	this->content = bytes;
}

int64_t BTreePage::getParent() const
{
	return *(int64_t*)&content[
		0
	];
}

void BTreePage::setParent(int64_t parent_address)
{
	this->dirty = true;
	memcpy(&content[
		0
	], &parent_address, sizeof(parent_address));
}

int32_t BTreePage::getSize() const 
{
	return *(int32_t*)&content[
		sizeof(int64_t)
	];
}

void BTreePage::setSize(int32_t size) {
	this->dirty = true;
	memcpy(&content[
		sizeof(int64_t)
	], &size, sizeof(size));
}