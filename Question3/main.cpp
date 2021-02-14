#include <iostream>
#include <fstream>
#include <string.h>
#include <functional>
#include <math.h>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;
struct Record {
    streampos recordStart;
    streampos recordEnd;
    size_t key;
};

struct Block {
    int size = 0;
    vector<Record> records;

    streampos blockStart = NULL;
    streampos blockEnd = NULL;
    streampos overflowStart = NULL;
    struct Block* overflow = nullptr;
};

//Hash index abstraction
class HashIndex {

public:
    HashIndex() {
        n = 2;
        i = 1;
        blocks.resize(n);
    }
    //Add entry to hash table and record file
    //Record file will be formatted:
    //<hash index>|<record csv1>|<record csv2>...\n
    void addEntry(string id, string data) {

        //Produce valid key
        std::hash<string> idHash;
        size_t keyFull = idHash(id); 
        size_t key = keyFull & ((1 << i) - 1);
        if(key >= n) {
            key ^= (1 << (i - 1));
        }
        Record record{NULL, NULL, keyFull};
        addRecord(key, &(blocks[key]), data, &record);
        float filled = percentFilled();


        if(filled > 0.8) {
            
            n++;
            i = (int)ceil(log2((double) n));

            blocks.resize(n);

            int splitIdx = (n - 1) ^ (1 << (i - 1));
            int newIdx = n - 1;


            
            Block* currentBlock = &blocks[splitIdx];
            //Recirculate records from split block and overflow blocks back into the new block and split block
            do {
                vector<Record> splitRecords = currentBlock->records;
                vector<string> splitRecordsData;
                for(Record r : splitRecords) {
                    splitRecordsData.push_back(readRecord(r));
                }
                clearBlock(currentBlock);
                currentBlock->records.clear();

                for(int ri = 0; ri < splitRecords.size(); ri++) {
                    size_t key = splitRecords[ri].key & ((1 << i) - 1);

                    //eraseRecord(r);
                    addRecord(key, &blocks[key], splitRecordsData[ri], &splitRecords[ri]);

                }
                currentBlock = currentBlock->overflow;
            }while(currentBlock != nullptr);               
        }
    }
    void findEntry(string id) {
        // cout << id << endl;

        string line;
        ifstream empIndex;
        empIndex.open("EmployeesIndex");
        if (!empIndex.is_open()){
            cout << "error, no file EmployeesIndex found" << endl;
            return;
        }

        // count n from hash index
        int n = 0;
        while(getline(empIndex, line)){
            n = max(stoi(line.substr(0, line.find("|"))), n);
        }
        
        int i = (int)ceil(log2((double) n));

        // obtain bucket key from hash.
        std::hash<string> idHash;
        size_t keyFull = idHash(id);
        size_t key = keyFull & ((1 << i) - 1);
        if(key >= n) {
            key ^= (1 << (i - 1));
        }

        empIndex.close();
        empIndex.open("EmployeesIndex");
        while(getline(empIndex, line)) {
            int k = stoi(line.substr(0, line.find("|")));
            if(k == key) {
                break;
            }
        }
        
        string record = "";
        while(record == "") {
            record = findInBlock(line, id);
            if(record == "") {
                if(line.find("`") == string::npos) {
                    return;
                }
                int overflowOffset = stoi(line.substr(line.find("`") + 1));
                empIndex.seekg(overflowOffset);
                getline(empIndex, line);                
            }
        }
        cout << record;

        empIndex.close();
    }


private:
    //4096 bytes plus a maximum of 228 records with 3 commas and 1 "|" separator plus a newline
    const int BLOCK_SIZE = 4096 + 228 * 4;
    const int OVERFLOW_OFFSET = BLOCK_SIZE - 30;
    int n;
    int i;
    // ofstream employeesIndex {"EmployeesIndex"};
    vector<Block> blocks;

    string findInBlock(string block, string id) {
        stringstream b(block);
        string record;
        getline(b, record, '|');
        while(getline(b, record, '|')) {
            if(record.find('`') != string::npos) {
                record = record.substr(0, record.find('`'));
            }
            if(record.substr(0, record.find(',')) == id) {
                return record;
            }
        }
        return "";
    }

    int blockSize(Block b) {
        int size = b.size;
        while(b.overflow != nullptr) {
            b = *b.overflow;
            size += b.size;
        } 
        return size;
    }
    void clearBlock(Block* block) {
        fstream file {"EmployeesIndex"};
        file.seekg(block->blockStart);
        file << '\0';

        block->size = 0;
        block->blockEnd = block->blockStart; 
 
    }

    float percentFilled() {
        float sum = 0.0;
        for(int j = 0; j < n; j++) {
            
            sum += (float)blockSize(blocks[j]) / 4096.0;
            
        }
        float average = sum / n;
        return average;
    }

    string readRecord(Record record) {
        fstream file {"EmployeesIndex"};
        file.seekg(record.recordStart);
        string data;
        while(file.tellg() != record.recordEnd) {
            data += (char)file.get();
        }
        return data;
    }
    
    void addBlock(Block* block, Block* overflowFrom, size_t key) {
        fstream file {"EmployeesIndex"};
         
        file.seekg(0, file.end);

        if(overflowFrom != nullptr) {
            overflowFrom->overflow = block;
            overflowFrom->overflowStart = file.tellg();

            //Write overflow signifier
            file.seekg(overflowFrom->blockEnd);
            file << '`' << overflowFrom->overflowStart;
            file.seekg(0, file.end);
        }



        file << key;
        block->blockStart = file.tellg();
        block->blockEnd = block->blockStart;

        for(int j = 0; j < BLOCK_SIZE; j++) {
            //Pad rest of block
            file.put('\0');
        }        
        file << "\n";
    }

    void addRecord(size_t key, Block* block, string recordData, Record* record) {
        fstream file {"EmployeesIndex", fstream::in | fstream::out};
        
        if(block->blockStart == NULL) {
            addBlock(block, nullptr, key);
        }

        while(block->size + recordData.size() - 3 > 4096) {
            if(block->overflow != nullptr) {
                block = block->overflow;
            }
            else {
                Block* overflowBlock = new Block;
                addBlock(overflowBlock, block, key);
                block = overflowBlock;

            }
        }
        //Go to end of block
        file.seekg(block->blockEnd);

        //Write record data and record its location
        file << "|";
        record->recordStart = file.tellg();
        file << recordData;
        record->recordEnd = file.tellg();
        
        //Update end of block
        block->records.push_back(*record);
        block->size += recordData.size() - 3;
        block->blockEnd = file.tellg();

        //If there is overflow rewrite the pointer
        if(block->overflowStart != NULL) {
            file << "`" << block->overflowStart;
        }
        
        //Signify end of block as one after the end of block so the marker is overwritten next time.
        file << '\0';

    }

};
void printUsage() {
    cout << "Usage: {-C | -L <id>}" << std::endl;
}
int main(int argc, char* argv[]) {
    if(argc < 2) {
        printUsage();
    }
    else {
        if(strcmp(argv[1], "-C") == 0) {
            ifstream employees {"Employee.csv"};


            string entry;
            HashIndex index;
            while(getline(employees, entry)) {
                string id = entry.substr(0, entry.find(","));
                index.addEntry(id, entry);
            }
            cout << "done";

        }
        else if(strcmp(argv[1], "-L") == 0 && argc > 2) {
            // cout << argv[2] << endl;
            HashIndex index;
            index.findEntry(argv[2]);            
        }
        else {
            printUsage();
        }
    }


}
