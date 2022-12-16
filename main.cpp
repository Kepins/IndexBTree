#include <iostream>
#include <string>

#include "BTree.h"


const int PAGE_SIZE = 4096;
const int TREE_ORDER = 2;
const std::string FILES_DIR = "diskFiles\\";

int main()
{
    BTree btree(FILES_DIR + "btree", PAGE_SIZE, TREE_ORDER, 3);
    return 0;
}
