#include <iostream>
#include <string>

#include "BTree.h"


const int PAGE_SIZE = 4096;
const int TREE_ORDER = 2;
const std::string FILES_DIR = "diskFiles\\";

int main()
{
    BTree btree(FILES_DIR + "btree", PAGE_SIZE, TREE_ORDER, 3);
    btree.insert(BTreeRecord(100, 0x00000032));
    btree.insert(BTreeRecord(80, 0x00000033));
    btree.insert(BTreeRecord(120, 0x00001042));
    btree.insert(BTreeRecord(1256, 0x00000000));
    btree.insert(BTreeRecord(1251, 0x00000100));
    BTreeRecord find(1256, 0);
    btree.search(find);
    find = BTreeRecord(120, 0);
    btree.search(find);
    find = BTreeRecord(80, 0);
    btree.search(find);
    find = BTreeRecord(99, 0);
    btree.search(find);
    find = BTreeRecord(100, 0);
    btree.search(find);
    find = BTreeRecord(1251, 0);
    btree.search(find);

    btree.getCounterWrites();
    btree.getCounterWritesIfFlushed();
    btree.getCounterReads();
    btree.getCounterAllOp();
    btree.getCounterAllOpIfFlushed();
    btree.print(std::cout);
    btree.getCounterWrites();
    btree.getCounterWritesIfFlushed();
    btree.getCounterReads();
    btree.getCounterAllOp();
    btree.getCounterAllOpIfFlushed();
    return 0;
}
