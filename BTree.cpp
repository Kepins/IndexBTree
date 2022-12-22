#include "BTree.h"


BTree::BTree(const std::string& filePath, int32_t page_size, int32_t order, int32_t cache_size) :
	ORDER(order),
	pageCache(filePath, cache_size, page_size, ORDER * 2)
{
	height = 0;
	root_addr = NIL;
}

BTree::~BTree()
{
}

int32_t BTree::bisection(const BTreePage& page, int64_t key) {
	int32_t l = 0;
	int32_t r = page.getSize() - 1;
	int32_t idx = (l + r) / 2;

	while (l < r) {
		int32_t keyAtIdx = page.getRecord(idx).key;
		if (keyAtIdx == key) {
			return idx;
		}
		if (key > keyAtIdx) {
			l = idx + 1;
		}
		else {
			r = idx - 1;
		}
		idx = (l + r) / 2;
	}
	return idx;
}

ReturnValue BTree::insertIntoPage(int64_t pageNum, const BTreeRecord& record)
{
	// Get page that search ended on
	BTreePage page = pageCache.getPage(pageNum);

	// Check if we can simply insert record to this page
	if (page.getSize() < 2 * ORDER) {
		// Where record needs to be inserted
		int32_t pos = bisection(page, record.key);
		// Check if key of the record at pos is smaller then record.key
		if (page.getRecord(pos).key < record.key) {
			// Insert after that record
			pos++;
		}

		// Move everything after one position further
		for (int32_t i = page.getSize(); i > pos; --i) {
			// Write to pos i+1 record from pos i
			page.setRecord(i, page.getRecord(i - 1));
		}
		// Set ptr to address after last record to NIL
		page.setAddress(page.getSize() + 1, NIL);

		// After moving everything inser record to proper position
		page.setRecord(pos, record);

		// Increase number of elements in this page
		page.setSize(page.getSize() + 1);

		// Write changes out
		pageCache.setPage(pageNum, page);

		// Return OK
		return ReturnValue::OK;
	}

	// Return that we cannot simply insert record to page
	return ReturnValue::NOT_POSSIBLE;
}


ReturnValue BTree::search(BTreeRecord& record, int64_t pageNum, int64_t* pageNumEnd) {
	if (pageNum == NIL) {
		return ReturnValue::NOT_FOUND;
	}
	BTreePage page = pageCache.getPage(pageNum);
	int32_t idx_bisect = bisection(page, record.key);
	BTreeRecord recordFound = page.getRecord(idx_bisect);
	if (recordFound.key == record.key) {
		record.address = recordFound.address;
		*pageNumEnd = pageNum;
		return ReturnValue::OK;
	}
	int64_t pageNumNext;
	if (record.key < page.getRecord(0).key) {
		pageNumNext = page.getAddress(0);
	}
	else if (record.key > page.getRecord(page.getSize() - 1).key)
	{
		pageNumNext = page.getAddress(page.getSize());
	}
	else {
		pageNumNext = page.getAddress(idx_bisect + 1);
	}

	if (pageNumNext == NIL) {
		*pageNumEnd = pageNum;
	}

	return search(record, pageNumNext, pageNumEnd);
}

ReturnValue BTree::insert(const BTreeRecord& record) {
	if (height == 0) {
		root_addr = pageCache.getNewPageNumber();
		BTreePage root_page(pageCache.getPage(root_addr));
		root_page.setParent(NIL);
		root_page.setSize(1);
		root_page.setAddress(0, NIL);
		root_page.setRecord(0, record);
		root_page.setAddress(1, NIL);
		height = 1;
		pageCache.setPage(root_addr, root_page);
		return ReturnValue::OK;
	}
	// Page that search ends on
	int64_t pageNumEnd;
	// Search fills addr in record so we need a copy
	BTreeRecord record_cpy(record);
	// Get page number where we need to insert record
	if (search(record_cpy, root_addr, &pageNumEnd) == ReturnValue::OK) {
		// If search ended successfully return error that record alread exists 
		return ReturnValue::ALREADY_EXISTS;
	}
	// Try to simply insert record to the page
	if (insertIntoPage(pageNumEnd, record) == ReturnValue::OK) {
		// Return OK if it was successful
		return ReturnValue::OK;
	}

	// There is an overflow

	// Try to compensate
	if (compensate(pageNumEnd) == ReturnValue::OK) {
		// If it was possible


		return ReturnValue::OK;
	}
	// Compensation not possible do a split


	return ReturnValue::OK;
}

ReturnValue BTree::search(BTreeRecord& record) {
	// Not used
	int64_t pageNumEnd;
	// Search from root
	return search(record, this->root_addr, &pageNumEnd);
}

ReturnValue BTree::remove(const BTreeRecord& record) {
	return ReturnValue::OK;
}