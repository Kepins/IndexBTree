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

int32_t BTree::binarySearch(const BTreePage& page, int64_t key) {
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

ReturnValue BTree::simpleInsertIntoPage(int64_t pageNum, const BTreeRecord& record)
{
	// Get page that search ended on
	BTreePage page = pageCache.getPage(pageNum);

	// Check if we can simply insert record to this page
	if (page.getSize() < 2 * ORDER) {
		// Where record needs to be inserted
		int32_t pos = binarySearch(page, record.key);
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

ReturnValue BTree::compensate(int64_t pageNum)
{
	// Compensation of root is not possible
	if (pageNum == root_addr) {
		return ReturnValue::NOT_POSSIBLE;
	}

	// Get page that needs compensation
	BTreePage page = pageCache.getPage(pageNum);
	// Get parent of this page
	BTreePage parent = pageCache.getPage(page.getParent());

	// Where page is in parent list
	int32_t childIdx = 0;
	while (parent.getAddress(childIdx) != pageNum) {
		++childIdx;
	}

	// Is compensation with left sibling possible
	if (childIdx > 0) {
		// Get left sibling
		BTreePage sibling(pageCache.getPage(parent.getAddress(childIdx - 1)));
		if (sibling.getSize() < 2 * ORDER) {
			// Compensation will happen
			doCompensate(sibling, parent, page, childIdx);
			// Save all the changes done to pages
			pageCache.setPage(page.getParent(), parent);
			pageCache.setPage(pageNum, page);
			pageCache.setPage(parent.getAddress(childIdx - 1), sibling);
			return ReturnValue::OK;
		}
	}
	// Is compensation with right sibling possible
	if (childIdx < parent.getSize() + 1) {
		// Get right sibling
		BTreePage sibling(pageCache.getPage(parent.getAddress(childIdx + 1)));
		if (sibling.getSize() < 2 * ORDER) {
			// Compensation will happen
			doCompensate(page, parent, sibling, childIdx);
			// Save all the changes done to pages
			pageCache.setPage(page.getParent(), parent);
			pageCache.setPage(pageNum, page);
			pageCache.setPage(parent.getAddress(childIdx + 1), sibling);
			return ReturnValue::OK;
		}
	}

	// Compensation was not possible since no sibling had num records < 2*ORDER
	return ReturnValue::NOT_POSSIBLE;
}

void BTree::doCompensate(BTreePage& left, BTreePage& parent, BTreePage& right, int32_t idxParent)
{
	// All records that are used in compensation
	std::vector<BTreeRecord>records;
	// All ptr that are relevant
	std::vector<int64_t>children;

	// Add all records and pointers before this records from left child
	for (int32_t i = 0; i < left.getSize(); ++i) {
		records.push_back(left.getRecord(i));
		children.push_back(left.getAddress(i));
	}
	// Add last ptr from left child
	children.push_back(left.getAddress(left.getSize()));
	// Add to records record from parent
	records.push_back(parent.getRecord(idxParent));
	// Add all records and pointers before this records from right child
	for (int32_t i = 0; i < right.getSize(); ++i) {
		records.push_back(right.getRecord(i));
		children.push_back(right.getAddress(i));
	}
	// Add last ptr from right child
	children.push_back(right.getAddress(right.getSize()));
	// Idx of record that goes to the parent
	int32_t midIdx = records.size() / 2;
	
	// Check if left was full and if it was midIdx cannot be the size of left
	if ((midIdx == 2*ORDER) && (left.getSize() == 2 * ORDER)) {
		// Decrement midIdx so left will not be full again
		--midIdx;
	}

	// All records before this idx go to left child
	for (int32_t i = 0; i < midIdx; ++i) {
		left.setRecord(i, records[i]);
		left.setAddress(i, children[i]);
	}
	// Set last ptr in left child
	left.setAddress(midIdx, children[midIdx]);
	// Set middle record in parent
	parent.setRecord(idxParent, records[midIdx]);
	// All records after midIdx go to right child
	for (int32_t i = midIdx + 1, k=0; i < records.size(); ++i, ++k) {
		right.setRecord(k, records[i]);
		right.setAddress(k, children[i]);
	}
	// Set last ptr in right child
	right.setAddress(records.size() - midIdx - 1, children[records.size()]);
	// Set size of left child
	left.setSize(midIdx);
	// Set size of right child
	right.setSize(records.size() - midIdx - 1);
}

void BTree::split(int64_t pageNum, const BTreeRecord& record)
{
	int64_t newPageNum = pageCache.getNewPageNumber();
	BTreePage oldPage(pageCache.getPage(pageNum));
	BTreePage parent(pageCache.getPage(oldPage.getParent()));
	BTreePage newPage(pageCache.getPage(newPageNum));
	
	newPage.setParent(oldPage.getParent());


	for (int i = 0; i < oldPage.getSize(); ++i) {

	}
}

ReturnValue BTree::search(BTreeRecord& record, int64_t pageNum, int64_t* pageNumEnd) {
	if (pageNum == NIL) {
		return ReturnValue::NOT_FOUND;
	}
	BTreePage page = pageCache.getPage(pageNum);
	int32_t idx_bisect = binarySearch(page, record.key);
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

void BTree::insertIntoPage(int64_t pageNum, const BTreeRecord& record) {
	// Try to simply insert record to the page
	if (simpleInsertIntoPage(pageNum, record) == ReturnValue::OK) {
		// Return if it was successful
		return;
	}
	// There is an overflow

	// Try to compensate
	if (compensate(pageNum) == ReturnValue::OK) {
		// If it was possible
		simpleInsertIntoPage(pageNum, record);

		// Return if this was successful
		return;
	}
	// Compensation not possible do a split
	split(pageNum, record);

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
	// Try simple insert, try compensation and if neccessary do a split
	insertIntoPage(pageNumEnd, record);
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