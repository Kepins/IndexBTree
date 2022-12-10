#include <iostream>
#include "BTreePage.h"

const int PAGE_SIZE = 4096;
const int TREE_ORDER = 2;

int main()
{
    BTreePage page(PAGE_SIZE, TREE_ORDER*2);
    page.setParent(0x30012);
    page.setAddress(0, 0x2b3a);
    page.setRecord(0, BTreeRecord(23, 0x11));
    page.setAddress(1, 0x3a01);
    page.setRecord(1, BTreeRecord(25, 0x99));
    page.setAddress(2, 0x403a1);
    page.setSize(2);

    page.getAddress(2);
    page.getSize();
    page.getParent();
    page.getRecord(1);

    return 0;
}
