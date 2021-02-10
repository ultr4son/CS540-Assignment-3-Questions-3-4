#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

//Hash index abstraction
class HashIndex {
public:
    HashIndex() {
        n = 1;
        i = 1;
        hash = calloc(

    }
    //Add entry to hash table and record file
    //Record file will be formatted:
    //<hash index>|<key>|<record csv1>|<record csv2>...\n
    void addEntry(char id[8], string record) {

    }

    void findEntry(char id[8]) {

    }

private:
    int n;
    int i;

    //Store the size of the bucket here
    int* buckets;
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
            ifstream employees {"Employees.csv"}

            while
            string entry;
            getline(employees, entry);


        }
        else if(strcmp(argv[1], "-L") == 0 && argc > 2) {
            //Put code here
        }
        else {
            printUsage();
        }
    }


}
