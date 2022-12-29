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

void BTree::printPage(std::ostream& os, const BTreePage& page, int64_t selfPageNum)
{
	os << "<" << selfPageNum <<">{"<< page.getParent() <<"}[-";
	for (int32_t i = 0; i < page.getSize(); i++) {
		int64_t addr = page.getAddress(i);
		BTreeRecord record = page.getRecord(i);
		if (addr != NIL) {
			os << addr << "- (" << record.key << "," << record.address << ") -";
		}
		else {
			os << "NIL" << "- (" << record.key << "," << record.address << ") -";
		}
	}
	if (page.getAddress(page.getSize()) != NIL) {
		os << page.getAddress(page.getSize()) << "-";
	}
	else {
		os << "NIL" << "-";
	}
	os << "] ";
}

int32_t BTree::binarySearch(const BTreePage& page, int64_t key) {
	int32_t l = 0;
	int32_t r = page.getSize() - 1;
	int32_t idx = (l + r) / 2;

	while (l < r) {
		int64_t keyAtIdx = page.getRecord(idx).key;
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

void BTree::updateChildren(const BTreePage& page, int64_t pageNum)
{
	// Update childern of new page
	for (int32_t i = 0; i <= page.getSize(); i++) {
		if (page.getAddress(i) != NIL) {
			BTreePage child = pageCache.getPage(page.getAddress(i));
			if (child.getParent() != pageNum) {
				child.setParent(pageNum);
				pageCache.setPage(page.getAddress(i), child);
			}
		}
	}
}

ReturnValue BTree::simpleInsertIntoPage(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum)
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

		// Set ptr to page after last record
		page.setAddress(page.getSize() + 1, page.getAddress(page.getSize()));

		// Move everything after one position further
		for (int32_t i = page.getSize(); i > pos; --i) {
			// Write to pos i+1 record from pos i
			page.setRecord(i, page.getRecord(i - 1));
			page.setAddress(i, page.getAddress(i - 1));
		}

		// After moving everything inser record to proper position
		page.setRecord(pos, record);
		page.setAddress(pos, childPageNum);

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

ReturnValue BTree::compensateInsert(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum)
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
			doCompensate(sibling, parent, page, childIdx-1, &record, childPageNum);

			updateChildren(sibling, parent.getAddress(childIdx - 1));
			updateChildren(page, pageNum);

			// Save all the changes done to pages
			pageCache.setPage(page.getParent(), parent);
			pageCache.setPage(pageNum, page);
			pageCache.setPage(parent.getAddress(childIdx - 1), sibling);
			return ReturnValue::OK;
		}
	}
	// Is compensation with right sibling possible
	if (childIdx < parent.getSize()) {
		// Get right sibling
		BTreePage sibling(pageCache.getPage(parent.getAddress(childIdx + 1)));
		if (sibling.getSize() < 2 * ORDER) {
			// Compensation will happen
			doCompensate(page, parent, sibling, childIdx, &record, childPageNum);

			updateChildren(sibling, parent.getAddress(childIdx + 1));
			updateChildren(page, pageNum);

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

void BTree::doCompensate(BTreePage& left, BTreePage& parent, BTreePage& right, int32_t idxParent, const BTreeRecord* record, int64_t childPageNum)
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

	// Does compensation insert new record
	if (record != nullptr) {
		// Find where new record should be in vector
		int32_t idxNew = 0;
		while (idxNew < records.size() && records[idxNew].key < record->key) {
			++idxNew;
		}
		// Insert new record to vector
		records.insert(records.begin() + idxNew, *record);
		// Insert corresponding childPageNum
		children.insert(children.begin() + idxNew, childPageNum);
	}

	// Idx of record that goes to the parent
	int32_t midIdx = records.size() / 2;

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

void BTree::split(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum)
{
	int64_t newPageNum = pageCache.getNewPageNumber();
	BTreePage oldPage(pageCache.getPage(pageNum));
	BTreePage newPage(pageCache.getPage(newPageNum));

	// All records that are used in distribution
	std::vector<BTreeRecord>records;
	// All ptr that are relevant
	std::vector<int64_t>children;

	// Read all records and children to vectors
	for (int32_t i = 0; i < oldPage.getSize(); ++i) {
		records.push_back(oldPage.getRecord(i));
		children.push_back(oldPage.getAddress(i));
	}
	// Add last child
	children.push_back(oldPage.getAddress(oldPage.getSize()));

	// Find where new record should be in vector
	int32_t idxNew = 0;
	while (idxNew < records.size() && records[idxNew].key < record.key) {
		++idxNew;
	}
	// Insert new record to vector
	records.insert(records.begin() + idxNew, record);
	// Insert corresponding childPageNum
	children.insert(children.begin() + idxNew, childPageNum);

	// Idx of record that goes to the parent
	int32_t midIdx = records.size() / 2;

	// Write records < records[midIdx] to new page
	for (int32_t i = 0; i < midIdx; ++i) {
		newPage.setRecord(i, records[i]);
		newPage.setAddress(i, children[i]);
	}
	// Write last child to new page
	newPage.setAddress(midIdx, children[midIdx]);
	// Set proper size in new page
	newPage.setSize(midIdx);

	// Write records > records[midIdx] to old page
	for (int32_t i = midIdx + 1, k = 0; i < records.size(); ++i, ++k) {
		oldPage.setRecord(k, records[i]);
		oldPage.setAddress(k, children[i]);
	}
	// Write last child to old page
	oldPage.setAddress(records.size() - midIdx - 1, children[records.size()]);
	// Set proper size in old page
	oldPage.setSize(records.size() - midIdx - 1);

	// Set parent of new page to the parent of old page
	newPage.setParent(oldPage.getParent());

	updateChildren(newPage, newPageNum);

	// Check if page that is not the root is being split
	if (oldPage.getParent() != NIL) {
		// Old page is not the root

		// Save changes
		pageCache.setPage(pageNum, oldPage);
		pageCache.setPage(newPageNum, newPage);

		// Insert records[midIdx] to parent
		insertIntoPage(oldPage.getParent(), records[midIdx], newPageNum);
		return;
	}
	// Create new root
	root_addr = pageCache.getNewPageNumber();
	BTreePage root_page = pageCache.getPage(root_addr);
	root_page.setParent(NIL);
	root_page.setSize(1);
	root_page.setAddress(0, newPageNum);
	root_page.setRecord(0, records[midIdx]);
	root_page.setAddress(1, pageNum);
	pageCache.setPage(root_addr, root_page);
	oldPage.setParent(root_addr);
	newPage.setParent(root_addr);

	// Save changes
	pageCache.setPage(pageNum, oldPage);
	pageCache.setPage(newPageNum, newPage);

	height++;
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
	if (recordFound.key < record.key) {
		idx_bisect++;
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
		pageNumNext = page.getAddress(idx_bisect);
	}

	if (pageNumNext == NIL) {
		*pageNumEnd = pageNum;
	}

	return search(record, pageNumNext, pageNumEnd);
}

void BTree::insertIntoPage(int64_t pageNum, const BTreeRecord& record, int64_t childPageNum) {
	// Try to simply insert record to the page
	if (simpleInsertIntoPage(pageNum, record, childPageNum) == ReturnValue::OK) {
		// Return if it was successful
		return;
	}
	// There is an overflow

	// Try to compensate
	if (compensateInsert(pageNum, record, childPageNum) == ReturnValue::OK) {

		// Return if this was successful
		return;
	}
	// Compensation not possible do a split
	split(pageNum, record, childPageNum);

}

void BTree::simpleRemoveRecord(BTreePage& page, const BTreeRecord& record)
{
	// Find record in leaf
	int32_t idx_bisect = binarySearch(page, record.key);

	// Move all records after idx_bisect to the left
	for (int32_t i = idx_bisect; i < page.getSize() - 1; ++i)
	{
		page.setRecord(i, page.getRecord(i + 1));
		page.setAddress(i, page.getAddress(i + 1));
	}
	page.setAddress(page.getSize() - 1, page.getAddress(page.getSize()));
	// Set size
	page.setSize(page.getSize() - 1);
}

void BTree::handleUnderflow(int64_t pageNum) {
	// Try compensation
	if (compensateRemove(pageNum) == ReturnValue::OK) {
		// Return success
		return;
	}
	// Do merge
	merge(pageNum);
}

ReturnValue BTree::compensateRemove(int64_t pageNum)
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
		if (sibling.getSize() > ORDER) {
			// Compensation will happen
			doCompensate(sibling, parent, page, childIdx - 1, nullptr, 0);

			updateChildren(sibling, parent.getAddress(childIdx - 1));
			updateChildren(page, pageNum);

			// Save all the changes done to pages
			pageCache.setPage(page.getParent(), parent);
			pageCache.setPage(pageNum, page);
			pageCache.setPage(parent.getAddress(childIdx - 1), sibling);
			return ReturnValue::OK;
		}
	}
	// Is compensation with right sibling possible
	if (childIdx < parent.getSize()) {
		// Get right sibling
		BTreePage sibling(pageCache.getPage(parent.getAddress(childIdx + 1)));
		if (sibling.getSize() > ORDER) {
			// Compensation will happen
			doCompensate(page, parent, sibling, childIdx, nullptr, 0);

			updateChildren(sibling, parent.getAddress(childIdx + 1));
			updateChildren(page, pageNum);

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

void BTree::merge(int64_t pageNum) {
	// Page where underflow happened
	BTreePage underflown = pageCache.getPage(pageNum);
	// Parent
	BTreePage parent = pageCache.getPage(underflown.getParent());

	// Where page is in parent list
	int32_t siblingIdx;
	int32_t childIdx = 0;
	while (parent.getAddress(childIdx) != pageNum) {
		++childIdx;
	}
	// Use sibling from the left
	if (childIdx > 0) {
		siblingIdx = childIdx - 1;
	}
	// Use sibling from the right
	else {
		siblingIdx = childIdx + 1;
	}
	// Read sibling page
	int64_t siblingPageNum = parent.getAddress(siblingIdx);
	BTreePage sibling = pageCache.getPage(siblingPageNum);

	int32_t leftIdx;
	BTreePage* left;
	BTreePage* right;

	if (childIdx < siblingIdx) {
		left = &underflown;
		right = &sibling;
		leftIdx = childIdx;
	}
	else {
		left = &sibling;
		right = &underflown;
		leftIdx = siblingIdx;
	}
	BTreeRecord fromParent = parent.getRecord(leftIdx);
	int64_t leftPage = parent.getAddress(leftIdx);
	int64_t rightPage = parent.getAddress(leftIdx + 1);

	std::vector<BTreeRecord> records;
	std::vector<int64_t> children;

	for (int32_t i = 0; i < left->getSize(); i++) {
		records.push_back(left->getRecord(i));
		children.push_back(left->getAddress(i));
	}
	children.push_back(left->getAddress(left->getSize()));

	records.push_back(fromParent);
	for (int32_t i = 0; i < right->getSize(); i++) {
		records.push_back(right->getRecord(i));
		children.push_back(right->getAddress(i));
	}
	children.push_back(right->getAddress(right->getSize()));

	for (int32_t i = 0; i < records.size(); i++) {
		right->setRecord(i, records[i]);
		right->setAddress(i, children[i]);
	}
	right->setAddress(records.size(), children[records.size()]);
	right->setSize(records.size());

	updateChildren(*right, rightPage);

	// Removes record and pointer to leftPage
	simpleRemoveRecord(parent, fromParent);

	// Dispose leftPage
	pageCache.returnPage(leftPage);

	if (right->getParent() == root_addr) {
		if (parent.getSize() == 0) {
			right->setParent(NIL);
			// Return current root
			pageCache.returnPage(root_addr);
			// Change root
			root_addr = rightPage;
			// Changer height
			--height;
			// Save rightPage
			pageCache.setPage(rightPage, *right);

			return;
		}
		else {
			// Save rightPage and parent
			pageCache.setPage(rightPage, *right);
			pageCache.setPage(right->getParent(), parent);
		}
	}
	else {
		// Save rightPage and parent
		pageCache.setPage(rightPage, *right);
		pageCache.setPage(right->getParent(), parent);
		if (parent.getSize() < ORDER) {
			handleUnderflow(right->getParent());
		}
	}
}

void BTree::print(std::ostream& os, std::list<int64_t>& printQueue, int64_t* pageNumNewline)
{
	// End recursion if queue is empty
	if (printQueue.empty()) {
		os << "\n";
		return;
	}
	// What page to print now
	int64_t pageNum = printQueue.front();
	// Remove from queue
	printQueue.pop_front();
	// Read page without effecting counters
	BTreePage page = pageCache.getPageExcludeAccesses(pageNum);
	// Check if depth changed
	if (*pageNumNewline == pageNum) {
		// Do not print the newline if we are in root
		if (pageNum != root_addr) {
			// Print newline
			os << "\n";
		}
		// Indicate when the next depth change happen
		*pageNumNewline = page.getAddress(0);
	}
	// Print this page
	printPage(os, page, pageNum);
	// Add children to queue
	for (int32_t i = 0; i <= page.getSize(); i++) {
		if (page.getAddress(i) == NIL) {
			// Break if page is leaf
			break;
		}
		// Add new pageNum to queue
		printQueue.push_back(page.getAddress(i));
	}
	// Print next element from queue
	print(os, printQueue, pageNumNewline);
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
	insertIntoPage(pageNumEnd, record, NIL);
	return ReturnValue::OK;
}

ReturnValue BTree::search(BTreeRecord& record) {
	// Not used
	int64_t pageNumEnd;
	// Search from root
	return search(record, this->root_addr, &pageNumEnd);
}

ReturnValue BTree::remove(BTreeRecord& record) {
	// Page where search ends
	int64_t pageWhereFound;
	// Find record with key in the tree
	if (search(record, root_addr, &pageWhereFound) == ReturnValue::NOT_FOUND) {
		// Return not found if record with such key does not exist
		return ReturnValue::NOT_FOUND;
	}
	// Page where search ended
	BTreePage pageFound = pageCache.getPage(pageWhereFound);
	// Check if record is in the leaf
	if (pageFound.getAddress(0) != NIL) {
		// Not in leaf

		// Search for biggest record on the left subtree
		int32_t idx_bisect = binarySearch(pageFound, record.key);

		// Root of left subtree
		int64_t leftChild = pageFound.getAddress(idx_bisect);
		
		// Leaf that has bigges records in subtree
		int64_t leafWithBiggestRecordSubtree;
		BTreeRecord temp(INT64_MAX, 0);
		search(temp, leftChild, &leafWithBiggestRecordSubtree);

		// Read leaf
		BTreePage leaf = pageCache.getPage(leafWithBiggestRecordSubtree);

		// Last record in Leaf
		BTreeRecord biggestInLeaf = leaf.getRecord(leaf.getSize() - 1);
		
		// Swap records
		pageFound.setRecord(idx_bisect, biggestInLeaf);
		leaf.setRecord(leaf.getSize() - 1, record);

		// Save page content
		pageCache.setPage(pageWhereFound, pageFound);

		// Swap page to leaf
		pageWhereFound = leafWithBiggestRecordSubtree;
		pageFound = leaf;
	}
	// Record is now in leaf

	// Remove record from page
	simpleRemoveRecord(pageFound, record);

	if (root_addr == pageWhereFound) {
		if (pageFound.getSize() == 0) {
			pageCache.returnPage(root_addr);
			root_addr = NIL;
			height = 0;
		}
		else {
			// Save page
			pageCache.setPage(pageWhereFound, pageFound);
		}
		return ReturnValue::OK;
	}

	// Save page
	pageCache.setPage(pageWhereFound, pageFound);

	// Check if we can just end deletion
	if (pageFound.getSize() >= ORDER) {
		// Return success
		return ReturnValue::OK;
	}
	// Try compenstation and then do merges
	handleUnderflow(pageWhereFound);

	// Deleted successfully
	return ReturnValue::OK;
}

void BTree::startSequentialRead() {
	// No tree exsits
	if (root_addr == NIL) {
		// End sequential read
		seqCurrPage = NIL;
		return;
	}
	// Record with the smalles possible key
	BTreeRecord record(INT64_MIN, 0);
	// Search from root
	search(record, this->root_addr, &seqCurrPage);
	// Index on that page at which to start reading
	seqCurrIdx = 0;
}


void BTree::setNextSeqRead(int64_t parentPage, int64_t childPageNum)
{
	if (parentPage == NIL) {
		// End read
		seqCurrPage = NIL;
		return;
	}
	BTreePage parent = pageCache.getPage(parentPage);

	int32_t childIdx = 0;
	while (parent.getAddress(childIdx) != childPageNum) {
		++childIdx;
	}
	if (childIdx == parent.getSize()) {
		setNextSeqRead(parent.getParent(), parentPage);
	}
	else {
		seqCurrPage = parentPage;
		seqCurrIdx = childIdx;
	}
}

ReturnValue BTree::getNextRecord(BTreeRecord& record)
{
	if (seqCurrPage == NIL) {
		return ReturnValue::READ_ENDED;
	}
	BTreePage page = pageCache.getPage(seqCurrPage);
	
	record = page.getRecord(seqCurrIdx);
	seqCurrIdx++;
	
	int64_t child = page.getAddress(seqCurrIdx);
	if (child != NIL) {
		BTreeRecord temp(INT64_MIN,0);
		seqCurrIdx = 0;
		search(temp, child, &seqCurrPage);
	}
	else if (seqCurrIdx == page.getSize()) {
		setNextSeqRead(page.getParent(), seqCurrPage);
	}

	return ReturnValue::OK;
}

void BTree::print(std::ostream& os) {
	if (root_addr == NIL) {
		os << "------- B-tree empty -------\n";
		return;
	}

	// Create queue
	std::list<int64_t> printQueue;
	// Point what pageNum needs to insert newline
	int64_t pageNumNewline = root_addr;
	// Add root to queue
	printQueue.push_back(root_addr);
	// Print header
	os << "------- B-tree structure -------\n";
	// Print recursively
	print(os, printQueue, &pageNumNewline);
}