#ifndef BTREE_H
#define BTREE_H

#include "BTreePage.h"

class BTree
{
private:

public:
	BTree(int32_t page_size, int32_t order);
	~BTree();
};

#endif