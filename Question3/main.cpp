#include <iostream>
#include <fstream>
#include <string.h>
#include <functional>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;
struct Record {
    int size;
    size_t key;
    string record;
};
struct Block {
    vector<Record> data;
};

//Hash index abstraction
class HashIndex {
public:
    HashIndex() {
        n = 2;
        i = 1;
        buckets.resize(n);
    }
    //Add entry to hash table and record file
    //Record file will be formatted:
    //<hash index>|<record csv1>|<record csv2>...\n
    void addEntry(string id, string record) {
        std::hash<string> idHash;
        size_t keyFull = idHash(id); 
        size_t key = keyFull & ((1 << i) - 1);
        if(key >= n) {
            key ^= (1 << (i - 1));
        }

        buckets[key].data.push_back({record.length() - 3, keyFull, record});

        if(percentFilled() > 0.8) {
            
            n++;
            i = (int)ceil(log2((double) n));

            buckets.resize(n);

            int splitIdx = (n - 1) ^ (1 << (i - 1));
            int newIdx = n - 1;
            vector<Record> splitRecord = buckets[splitIdx].data;
            buckets[splitIdx].data.clear();
            for(auto it = splitRecord.begin(); it != splitRecord.end(); ++it) {            
                size_t key = it->key & ((1 << i) - 1);
                buckets[key].data.push_back(*it);

            }
        }



    }

    void findEntry(string) {

    }
    void write(){
        for(int k = 0; k < n; k++){
            employeesIndex << k;
            for(int j = 0; j < buckets[k].data.size(); j++){
                employeesIndex << "|" << buckets[k].data[j].record;
            }
            employeesIndex << endl;
        }
    }
private:
    int n;
    int i;
    ofstream employeesIndex {"EmployeesIndex"};
    vector<Block> buckets;
    int bucketSize(Block block){
        int sum = 0;
        for(int j = 0; j < block.data.size(); j++) {
            sum += block.data[j].size;
        }
        return sum;

    }
    float percentFilled() {
        float sum = 0.0;
        for(int j = 0; j < n; j++) {
            sum += (float)bucketSize(buckets[j]) / 4096.0;
        }
        int average = sum / n;
        return average;
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
                index.addEntry(id, entry.substr(0, entry.find("\n")));
            }
            index.write();
        }
        else if(strcmp(argv[1], "-L") == 0 && argc > 2) {
            //Put code here
        }
        else {
            printUsage();
        }
    }


}
