
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iostream>

const int BLOCK_SIZE = 4 + 40 + 4 + 8;
const int MAX_BLOCKS = 22;

const std::string EMP_FILE = "Emp.csv";
const std::string SORTED_FILE = "EmpSorted.csv";

struct ReadBuffer {
    std::string line;
    std::ifstream* file;
};

int to_runs(std::string record_file_name, bool (*compar)(std::string, std::string)){
    int runs_written;

    std::ifstream record_file(EMP_FILE, std::ifstream::in);

    for(runs_written = 0; record_file; runs_written++) {
        int num_read = 0;

        std::vector<std::string> records;
        std::string line;
        while (num_read < MAX_BLOCKS && getline(record_file, line)){
            records.push_back(line);
            num_read++;
        }
        
        // sort the M records
        sort(records.begin(), records.end(), compar);

        // write out sorted runs
        std::string run_file_name = "emp0" + std::to_string(runs_written) + ".csv";
        std::ofstream run_file(run_file_name, std::ofstream::out | std::ofstream::trunc);
        for(int w = 0; w < num_read; w++){
            run_file << records[w] << "\n";
        }
    }

    return runs_written;
}

bool compare_emp(std::string a, std::string b) {
    int eid_a;
    sscanf(a.c_str(), "%d,", &eid_a);

    int eid_b;
    sscanf(b.c_str(), "%d,", &eid_b);

    return eid_a < eid_b;
}

void merge(int runs){

}

// General multi-way merge sort
int main(int argc, char** argv){

    // Pass 0
    int emp_runs = to_runs(EMP_FILE, compare_emp);
    // std::cout << emp_runs << std::endl;

    // Pass i
    merge(emp_runs);

    return 0;
}