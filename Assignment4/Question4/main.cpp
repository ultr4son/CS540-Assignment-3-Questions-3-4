
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
const std::string EMP_BASE = "emp";
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

std::string peek_line(std::ifstream & file) {
    std::string tuple;
    int pos = file.tellg();

    getline(file, tuple);
        
    file.seekg(pos);
    return tuple;
}

void advance(ReadBuffer & r) {
    std::string temp;
    getline(*r.file, temp);
    
    r.line = peek_line(*r.file);
}

void first_line(ReadBuffer & r){
    r.line = peek_line(*r.file);
}

bool buffer_avaliable(std::vector<ReadBuffer> rbs) {
    // for(auto& r : rbs) {
    // std::cout << rbs.size() << std::endl;
    for(int i = 0; i < rbs.size(); i++){
        
        if(rbs[i].line != "") {
            // std::cout << rbs[i].line << std::endl;    
            return true;
        }
    }
    return false;
}

std::string construct_filename(std::string base, int run){
    return std::string(base + std::to_string(run) + ".csv");
}

int to_runs(std::string record_file_name, bool (*compar)(std::string, std::string), std::string base){
    int runs_written;

    std::ifstream record_file(record_file_name, std::ifstream::in);

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
        std::string run_file_name = construct_filename(base, runs_written);
        std::ofstream run_file(run_file_name, std::ofstream::out | std::ofstream::trunc);
        for(int w = 0; w < num_read; w++){
            run_file << records[w] << "\n";
        }
        run_file.close();
    }

    return runs_written;
}

std::string get_min_record(std::vector<ReadBuffer> & files){
    int min_eid = std::numeric_limits<int>::max();
    int min_idx = 0;
    for(int i = 0; i < files.size(); i++){
        // look at all strings, compare based on eid
        int temp_eid = std::numeric_limits<int>::max();
        if(files[i].line != ""){
            temp_eid = get_eid(files[i].line);
        }
        // std::cout << temp_eid << std::endl;
        
        if(temp_eid < min_eid){
            min_eid = temp_eid;
            min_idx = i;
        }
    }

    std::string min_record = files[min_idx].line;
    advance(files[min_idx]);

    return min_record;
}

int find_buffer_end(std::string base, int beg, int end){
    int offset = beg;
    for(int i = beg; i < end; i++){
        if(FILE *file = fopen(construct_filename(base, i).c_str(), "r")) {
            fclose(file);
            offset++;
        }
    }
    return offset;
}

void populate_buffer(std::vector<ReadBuffer> & files, std::string base, int beg, int end){
    // clear buffer and populate with M-1 files 
    // free memory here before clearing!!!
    files.clear();
    files.resize(end - beg);
    for(int i = 0; i < end - beg; i++){
        files[i].file = new std::ifstream(construct_filename(base, beg + i), std::ifstream::in);
        first_line(files[i]);
    }
}

void print_buffer(std::vector<ReadBuffer> & files){
    std::cout << "first element in each buffer block" << std::endl;

    for(int i = 0; i < files.size(); i++){
        std::cout << "buffer file " << i  <<  ": " << files[i].line << std::endl;
    }
}

void merge_runs(int runs, std::string base){

    // define file buffer and iterators to keep track of beginning
    // and end of our temp files to populate the buffer
    int beg = 0;
    int end = std::min(runs, MAX_BLOCKS - 1);


    std::vector<ReadBuffer> merge_files;
    // populate_buffer(merge_files, base, beg, end);

    // while(merge_files.size() > 1){
    while(true){
        // std::cout << merge_files.size() << std::endl;

        end = find_buffer_end(base, beg, beg + MAX_BLOCKS - 1);
        populate_buffer(merge_files, base, beg, end);
        // print_buffer(merge_files);
        std::cout << "beg: " << beg << " end: " << end << std::endl;

        if(merge_files.size() == 0){
            break;
        }

        // empty buffer into new run
        std::string run_file_name = construct_filename(base, runs);
        std::ofstream run_file(run_file_name, std::ofstream::out | std::ofstream::trunc);


        while(buffer_avaliable(merge_files)){
            // print_buffer(merge_files);
            run_file << get_min_record(merge_files) << "\n";
            
            // std::cout << "record selected: " << get_min_record(merge_files) << "\n\n";
            
        }
        
        run_file.close();

        // adjust counters
        beg = end + 1;
        runs++;
    }
    runs--;
    std::cout << "total runs: " << runs << std::endl;

}

void clean_temp_csv(){
    char cmd[20];
    strcpy(cmd, "rm emp*.csv");
    system(cmd);
}

// General multi-way merge sort
int main(int argc, char** argv){

    // clean files
    clean_temp_csv();

    // Pass 0
    int emp_runs = to_runs(EMP_FILE, compare_eid, EMP_BASE);
    // int emp_runs = to_runs("EmpSmall.csv", compare_eid, EMP_BASE);
    std::cout << "num of level-0 runs: " << emp_runs << std::endl;

    // Pass i
    merge_runs(emp_runs, EMP_BASE);

    // clean files and final output

    return 0;
}