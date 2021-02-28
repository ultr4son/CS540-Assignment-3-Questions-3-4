
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

int get_eid(std::string e) {
    int eid;
    sscanf(e.c_str(), "%d,", &eid);
    return eid;
}

bool compare_eid(std::string a, std::string b) {
    int eid_a = get_eid(a);
    int eid_b = get_eid(b);
    return eid_a < eid_b;
}

std::string peek_line(std::ifstream& file) {
        std::string tuple;
        int pos = file.tellg();

        getline(file, tuple);
        
        file.seekg(pos);
        return tuple;
}

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
        std::string run_file_name = "emp" + std::to_string(runs_written) + ".csv";
        std::ofstream run_file(run_file_name, std::ofstream::out | std::ofstream::trunc);
        for(int w = 0; w < num_read; w++){
            run_file << records[w] << "\n";
        }
    }

    return runs_written;
}

void populate_buffer(std::vector<ReadBuffer*> files, std::string base, int beg, int end){
    // clear buffer and populate with M-1 files 
    // free memory here before clearing!!!
    files.clear();
    for(int i = beg; i < end; i++){
        files.push_back(new ReadBuffer);
        files[i - beg]->file = new std::ifstream("emp" + std::to_string(i) + ".csv", std::ifstream::in);
    }
}

bool buffer_has_records(std::vector<ReadBuffer*> files){
    return 1;
}

void merge_runs(int runs, std::string base){

    // define file buffer and iterators to keep track of beginning
    // and end of 
    int beg = 0;
    int end = std::max(runs, MAX_BLOCKS - 1);
    std::vector<ReadBuffer*> merge_files;
    populate_buffer(merge_files, base, beg, end);
    
    // while(merge_files.size() > 1){
        // populate_buffer(merge_files, base, beg, end);

        // empty buffer into new runs
        // while(buffer_has_records(merge_files)){

        // }
    // }
}

// General multi-way merge sort
int main(int argc, char** argv){

    // Pass 0
    int emp_runs = to_runs(EMP_FILE, compare_eid);
    // std::cout << emp_runs << std::endl;

    // Pass i
    merge_runs(emp_runs, "emp");

    return 0;
}