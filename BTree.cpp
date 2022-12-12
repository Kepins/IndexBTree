#include "BTree.h"

BTree::BTree(const std::string& filePath, int32_t page_size, int32_t order, int32_t cache_size) :
	ORDER(order),
	pageCache(filePath, cache_size, page_size, ORDER * 2)
{
}

BTree::~BTree()
{
}
