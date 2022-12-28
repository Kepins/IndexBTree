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

    bool end = false;
    char command;
    while (!end && *input>>command) {
        switch (command) {
            case 'i': {
                int64_t key;
                Record record;
                *input >> key;
                int64_t addr = recordsManager.insertRecord(record);
                if (btree.insert(BTreeRecord(key, addr))==ReturnValue::OK) {
                    std::cout << "Record("<< key<<") inserted successfully\n";
                }
                else {
                    std::cout << "Record(" << key << ") already exists!\n";
                }
            }break;
            case 'l': {
                int64_t key;
                Record record;
                *input >> key;
                for (int i = 0; i < record.numElements; i++) {
                    *input >> record.content[i];
                }
                int64_t addr = recordsManager.insertRecord(record);
                if (btree.insert(BTreeRecord(key, addr)) == ReturnValue::OK) {
                    std::cout << "Record(" << key << ") inserted successfully\n";
                }
                else {
                    std::cout << "Record(" << key << ") already exists!\n";
                }
            }break;
            case 's': {
                int64_t key;
                *input >> key;
                BTreeRecord bTrecord(key, 0);
                if (btree.search(bTrecord) == ReturnValue::OK) {
                    Record record = recordsManager.getRecord(bTrecord.address);
                    std::cout << "Record(" << key << "): ";
                    for (int i = 0; i < record.numElements; i++) {
                        std::cout << record.content[i] << " ";
                    }
                    std::cout << "\n";
                }
                else {
                    std::cout << "Record(" << key << ") not found\n";
                }
            }break;
            case 'd': {

            }break;
            case 'u': {

            }break;
            case 'p': {
                btree.print(std::cout);
            }break;
            case 'q': {
                std::cout << "Reading ended\n";
                end = true;
            }break;
            default: {
                std::cout << "Bad command!\n";
            }break;
        }
    }


    if (mode == 1) {
        delete input;
    }

    int64_t writes = btree.getCounterWritesIfFlushed() + recordsManager.getCounterWrites() + recordsManager.getHowManyDirty();
    int64_t reads = btree.getCounterReads() + recordsManager.getCounterReads();
    std::cout << "All writes: " << writes << "\n";
    std::cout << "All reads: " << reads << "\n";


    /*
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
    */



    return 0;
}
