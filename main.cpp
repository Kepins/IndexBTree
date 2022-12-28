#include <iostream>
#include <fstream>
#include <string>

#include "BTree.h"
#include "RecordManager.h"

const int NUM_ELEM_IN_RECORD = 10;

int main(int argc, char* argv[])
{
    if (argc != 6) {
        std::cerr << "Bad arguments!";
        return 1;
    }

    int PAGE_SIZE = std::stoi(argv[1]);
    int TREE_ORDER = std::stoi(argv[2]);
    std::string FILES_DIR = argv[3];
    int TREE_CACHE = std::stoi(argv[4]);
    int RECORDS_CACHE = std::stoi(argv[5]);

    std::string fileName="";
    int mode;
    std::istream* input;

    std::cout << "Choose mode: \n"
        << "0. Interactive\n"
        << "1. From txt file\n";
    
    std::cin >> mode;
    input = &std::cin;
    if (mode == 1) {
        std::cin >> fileName;
        input = new std::ifstream(fileName);
        if (!input->good()) {
            delete input;
            std::cerr << "Error while opening file!\n";
            return 1;
        }
    }

    BTree btree(FILES_DIR + "btree", PAGE_SIZE, TREE_ORDER, TREE_CACHE);
    RecordManager recordsManager(FILES_DIR + "records", RECORDS_CACHE, PAGE_SIZE);

    char command;
    while (*input>>command) {
        std::cout << command << std::endl;
        switch (command) {
            case 'i': {
                int64_t key;
                Record record;
                *input >> key;
                int64_t addr = recordsManager.insertRecord(record);
                btree.insert(BTreeRecord(key, addr));
            }break;
            case 's': {

            }break;
            case 'd': {

            }break;
            case 'u': {

            }break;
            case 'p': {
                btree.print(std::cout);
            }break;
            case 'q': {
                break;
            }break;
            default: {
                break;
            }break;
        }
    }


    if (mode == 1) {
        delete input;
    }




    /*BTree btree(FILES_DIR + "btree", PAGE_SIZE, TREE_ORDER, 3);
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
    btree.getCounterAllOpIfFlushed();*/



    return 0;
}
