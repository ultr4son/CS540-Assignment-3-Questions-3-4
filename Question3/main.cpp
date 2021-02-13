#include <iostream>
#include <fstream>
#include <string.h>
#include <functional>
#include <math.h>
#include <vector>
#include <algorithm>

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
    streampos dataStart = NULL;
    streampos blockEnd = NULL;
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
    void addEntry(string id, string record) {

        //Produce valid key
        std::hash<string> idHash;
        size_t keyFull = idHash(id); 
        size_t key = keyFull & ((1 << i) - 1);
        if(key >= n) {
            key ^= (1 << (i - 1));
        }

        addRecord(key, &(blocks[key]), record, new Record {keyFull});
        float filled = percentFilled();


        if(filled > 0.8) {
            
            n++;
            i = (int)ceil(log2((double) n));

            blocks.resize(n);

            int splitIdx = (n - 1) ^ (1 << (i - 1));
            int newIdx = n - 1;

            
            vector<Record> splitRecords = getRecords(blocks[splitIdx]);
            splitInit(&blocks[splitIdx]);
            blocks[splitIdx].records.clear();

            
            for(Record r : splitRecords) {
                size_t key = r.key & ((1 << i) - 1);

                string recordData = readRecord(r);
                eraseRecord(r);
                addRecord(key, &blocks[key], recordData, &r);

            }
            

            addBlock(&blocks[newIdx], nullptr, newIdx);
            
            for(Record r : blocks[splitIdx].records) {
                eraseRecord(r);
                addRecord(splitIdx, &blocks[splitIdx], readRecord(r), &r);
            }
            for(Record r: blocks[newIdx].records) {
                eraseRecord(r);
                addRecord(newIdx, &blocks[newIdx], readRecord(r), &r);
            }
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
            n++;
            // cout << line << endl;
        }
        empIndex.close();   
        
        int i = (int)ceil(log2((double) n));

        // obtain bucket key from hash.
        std::hash<string> idHash;
        size_t keyFull = idHash(id);
        size_t key = keyFull & ((1 << i) - 1);
        if(key >= n) {
            key ^= (1 << (i - 1));
        }
        // cout << key << endl;

        empIndex.open("EmployeesIndex");
        while(getline(empIndex, line)){
            // loop until correct bucket is found
            if(key == 0){
                //erase bucket number and delimiter (2 char long)
                line.erase(0, 2);
                
                // check the bucket
                while (!line.empty()) {
                    // find end of id
                    auto end = line.find(",");
                    if (id.compare(line.substr(0, end)) == 0){

                        //report tuple
                        cout << "id:   " << (line.substr(0, end)) << endl;
                        line.erase(0, end + 1);

                        end = line.find(",");
                        cout << "name: " << (line.substr(0, end)) << endl;
                        line.erase(0, end + 1);

                        end = line.find(",");
                        cout << "bio:  " << (line.substr(0, end)) << endl;
                        line.erase(0, end + 1);

                        end = line.find("|");
                        cout << "manager-id: " << (line.substr(0, end)) << endl;
                        line.erase(0, end);

                        return;
                    }
                    else{
                        //skip current line
                        line.erase(0, line.find("|") + 1);
                    }
                }
            }
            key--;
        }

        empIndex.close();
    }


private:
    //4096 bytes plus a maximum of 228 records with 3 commas and 1 "|" separator plus a newline
    const int BLOCK_SIZE = 4096 + 228 * 4;
    int n;
    int i;
    // ofstream employeesIndex {"EmployeesIndex"};
    vector<Block> blocks;
    vector<Block*> freeBlocks;

    int blockSize(Block b) {
        int size = b.size;
        while(b.overflow != nullptr) {
            b = *b.overflow;
            size += b.size;
        } 
        return size;
    }
    void splitInit(Block* block) {
        block->size = 0;
        block->blockEnd = block->dataStart; 
        clearOverflow(block);
    }
    void clearOverflow(Block* block) {
        if(block->overflow != nullptr) {
            clearOverflow(block->overflow);
            eraseBlock(block->overflow);
            delete block->overflow;
            block->overflow = nullptr;
        }
    }
    void eraseBlock(Block* block) {
        fstream file {"EmployeesIndex"};
        file.seekg(block->blockStart);
        while(file.tellg() != block->blockEnd) {
            file.put('\0');
        }

    }
    void freeOverflow(Block* block) {
        freeBlocks.push_back(block);
        eraseBlock(block);
        block->blockEnd = block->dataStart = block->blockStart;

    }
    float percentFilled() {
        float sum = 0.0;
        for(int j = 0; j < n; j++) {
            
            sum += (float)blockSize(blocks[j]) / 4096.0;
            
        }
        float average = sum / n;
        return average;
    }
    vector<Record> getRecords(Block block) {
        vector<Record> records = block.records;
        while(block.overflow != nullptr) {
            block = *block.overflow;
            records.insert(records.end(), block.records.begin(), block.records.end());
        }
        return records;
    }
    void eraseRecord(Record record) {
        fstream file {"EmployeesIndex"};
        //Include record delimiter for deletion
        file.seekg(record.recordStart - 1);
        while(file.tellg() != record.recordEnd) {
            file.put('\0');
        }
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
        

        if(overflowFrom != nullptr) {
            overflowFrom->overflow = block;
            file.seekg(overflowFrom->blockEnd);
            
            file << "!" << endOfFile();
            
        }

        file.seekg(0, file.end);

        block->blockStart = file.tellg();
        file << key;


        block->dataStart = file.tellg();
        block->blockEnd = block->dataStart;

        //Allocate block
        for(int j = 0; j < BLOCK_SIZE; j++) {
            file.put('\0');
        }
        file << "\n";
    }
    streampos endOfFile(){
        fstream file {"EmployeesIndex"};
        file.seekg(0, file.end);
        return file.tellg();
    }
    void addRecord(size_t key, Block* block, string recordData, Record* record) {
        fstream file {"EmployeesIndex"};
        
        
        while(block->overflow != nullptr) {
            block = block->overflow;
        }
        if(block->dataStart == NULL) {
            addBlock(block, nullptr, key);
        }

        if(block->size + recordData.size() - 3 > 4096) {
            Block* overflowBlock = new Block;
            addBlock(overflowBlock, block, key);
            block = overflowBlock;
        }
        //Go to end of block
        file.seekg(block->blockEnd);

        //Write record data and record its location
        file << "|";
        record->recordStart = file.tellg();
        file << recordData;
        record->recordEnd = file.tellg();

        //Update end of block
        block->blockEnd = file.tellg();
        block->records.push_back(*record);
        block->size += recordData.size() - 3;
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
            fstream file;

            file.open("EmployeesIndex", ios::trunc);
            file.close();
            
            string entry;
            HashIndex index;
            while(getline(employees, entry)) {
                string id = entry.substr(0, entry.find(","));
                index.addEntry(id, entry);
            }
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
