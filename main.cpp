#include <iostream>
#include <string>
#include "BTreePage.h"
#include "DiskFile.h"


const int PAGE_SIZE = 4096;
const int TREE_ORDER = 2;
const std::string FILES_DIR = "diskFiles\\";

int main()
{
    char bytes[8] = { 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x01, 0x00 };

    DiskFile file(FILES_DIR + "diskFile", 2, 8);
    file.write(0x00, bytes, 8);
    file.write(0x7, bytes, 8);

    file.read(0x4, bytes, 8);


    return 0;
}
