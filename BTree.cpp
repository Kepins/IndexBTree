#include "BTree.h"

BTree::BTree(const std::string& filePath, int32_t page_size, int32_t order, int32_t cache_size) :
	ORDER(order),
	pageCache(filePath, cache_size, page_size, ORDER * 2)
{
	//char buffer[4096];
	//pageCache.getNewPageNumber();
	//pageCache.getNewPageNumber();
	//pageCache.getNewPageNumber();
	//pageCache.getNewPageNumber();

	//BTreeRecord rec(321, 3);

	//BTreePage page0 = pageCache.getPage(0);
	//page0.setAddress(0, 1);
	//page0.setRecord(13, rec);
	//page0.setAddress(1, 12);
	//pageCache.setPage(1, page0);
	//pageCache.setPage(2, page0);
	//pageCache.setPage(3, page0);
	//BTreePage page1 = pageCache.getPage(1);
	//pageCache.setPage(0, page1);

	//pageCache.flushPages();

	//pageCache.returnPage(0);
	//pageCache.returnPage(1);
	//pageCache.returnPage(2);
	//pageCache.returnPage(3);
	//pageCache.get_counter_reads();
	//pageCache.get_counter_writes();
}

BTree::~BTree()
{
}
