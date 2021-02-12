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

    void write(){
        ofstream employeesIndex {"EmployeesIndex"};
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
    // ofstream employeesIndex {"EmployeesIndex"};
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
            // cout << argv[2] << endl;
            HashIndex index;
            index.findEntry(argv[2]);            
        }
        else {
            printUsage();
        }
    }


}
