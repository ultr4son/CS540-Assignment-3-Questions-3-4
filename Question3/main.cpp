#include <iostream>
#include <string.h>

void printUsage() {
    std::cout << "Usage: {-C | -L <id>}" << std::endl;
}
int main(int argc, char* argv[]) {
    if(argc < 2) {
        printUsage();
    }
    else {
        if(strcmp(argv[1], "-C") == 0) {
            //Put code here
        }
        else if(strcmp(argv[1], "-L") == 0 && argc > 2) {
            //Put code here
        }
        else {
            printUsage();
        }
    }

}
