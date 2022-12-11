#include <iostream>
#include <string>

#include "DiskFilePageManager.h"


const int PAGE_SIZE = 4096;
const int TREE_ORDER = 2;
const std::string FILES_DIR = "diskFiles\\";

int main()
{
    char bytes[8] = { 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x01, 0x00 };

    DiskFilePageManager manager(FILES_DIR + "diskFile", 5, 8);

    int64_t page_num = manager.get_new_page_number();
    manager.write_page_content(page_num, bytes);

    return 0;
}
