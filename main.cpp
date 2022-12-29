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
            // Insert
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
            // Long insert
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
            // Search for record
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
            // Delete
            case 'd': {
                int64_t key;
                *input >> key;
                BTreeRecord bTrecord(key, 0);
                if (btree.remove(bTrecord) == ReturnValue::OK) {
                    recordsManager.deleteRecord(bTrecord.address);
                    std::cout << "Record(" << key << ") removed successfully\n";
                 }
                else {
                    std::cout<< "Record(" << key << ") not found\n";
                }
            }break;
            // Update
            case 'u': {
                int64_t key;
                *input >> key;
                
                BTreeRecord bTrecord(key, 0);
                if (btree.search(bTrecord) == ReturnValue::OK) {
                    Record record;
                    std::cout << "New values: ";
                    for (int i = 0; i < record.numElements; i++) {
                        *input >> record.content[i];
                    }
                    recordsManager.updateRecord(bTrecord.address, record);
                    std::cout << "Record(" << key << ") updated ";
                    std::cout << "\n";
                }
                else {
                    std::cout << "Record(" << key << ") not found\n";
                }
            }break;
            // Read sequential
            case 'r': {
                btree.startSequentialRead();
                BTreeRecord bTrecord(0,0);
                std::cout << "---- Sequential read ----\n";
                while (btree.getNextRecord(bTrecord) == ReturnValue::OK) {
                    Record record = recordsManager.getRecord(bTrecord.address);
                    std::cout << "Record(" << bTrecord.key << "): ";
                    for (int i = 0; i < record.numElements; i++) {
                        std::cout << record.content[i] << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << "---- End read ----\n";
            }break;
            // Print tree
            case 'p': {
                btree.print(std::cout);
            }break;
            // Get info
            case '?': {
                int64_t writes = btree.getCounterWrites() + recordsManager.getCounterWrites();
                int64_t writesFlush = btree.getCounterWritesIfFlushed() + recordsManager.getCounterWrites() + recordsManager.getHowManyDirty();
                int64_t reads = btree.getCounterReads() + recordsManager.getCounterReads();
                std::cout << "----INFO----\n";
                std::cout << "Height: " << btree.getHeight() << "\n";
                std::cout << "Reads: " << reads << "\n";
                std::cout << "Writes: " << writes << "\n";
                std::cout << "Writes if flushed: " << writesFlush << "\n";
            }break;
            // End loop
            case 'q': {
                std::cout << "Reading ended\n";
                end = true;
            }break;
            // Command not recognized
            default: {
                std::cout << "Bad command(use 'q' to exit)!\n";
            }break;
        }
    }

    // Reading from file
    if (mode == 1) {
        ((std::ifstream*)input)->close();
        delete input;
    }

    int64_t writes = btree.getCounterWritesIfFlushed() + recordsManager.getCounterWrites() + recordsManager.getHowManyDirty();
    int64_t reads = btree.getCounterReads() + recordsManager.getCounterReads();
    std::cout << "All writes: " << writes << "\n";
    std::cout << "All reads: " << reads << "\n";

    return 0;
}
