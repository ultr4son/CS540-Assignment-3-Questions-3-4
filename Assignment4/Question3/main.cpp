
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <sstream>

//Both records have the same block size
const int BLOCK_SIZE = 4 + 40 + 8 + 4;
const int MAX_BLOCKS = 22;

const std::string EMP_FILE = "Emp.csv";
const std::string DEPT_FILE = "Dept.csv";
const std::string JOIN_FILE = "Join.csv";

int compare_employee(const void* a, const void* b) {
    std::string emp_a = *(std::string*)(a);
    std::string emp_b = *(std::string*)(b);
    
    int eid_a;
    sscanf(emp_a.c_str(), "%d,", &eid_a);
    
    int eid_b;
    sscanf(emp_b.c_str(), "%d,", &eid_b);

    return eid_a - eid_b;
}

int compare_dept(const void* a, const void* b) {
    std::string dept_a = *(std::string*)(a);
    std::string dept_b = *(std::string*)(b);
    
    int did;
    std::string dname;
    double budget;

    int managerid_a;
    sscanf(dept_a.c_str(), "%d,%s,%f,%d", &did, &dname, &budget, &managerid_a);
    
    int managerid_b;
    sscanf(dept_b.c_str(), "%d,%s,%f,%d", &did, &dname, &budget, &managerid_b);

    return managerid_a - managerid_b;
}

int compare_dept_employee(const void* deptp, const void* empp) {
    std::string dept = *(std::string*)(deptp);
    std::string emp = *(std::string*)(empp);

    int eid;
    sscanf(emp.c_str(), "%d,", &eid);
    
    int did;
    std::string dname;
    double budget;

    int managerid;
    sscanf(dept.c_str(), "%d,%s,%f,%d", &did, &dname, &budget, &managerid);

    return managerid - eid;


}



/*
 * Convert the record into a number of sorted runs labeled <file_base><n> based on the function compar.
 *  
 */
int to_runs(std::string record_file_name, int (*compar)(const void*, const void*), std::string file_base) {
    int runs_written;

    std::ifstream record_file(record_file_name, std::ifstream::in);

    //Read, sort, and write runs
    for(runs_written = 0; ; runs_written++) {
        int num_read;
        std::string records[MAX_BLOCKS]; 
        for(num_read = 0; num_read < MAX_BLOCKS || record_file.eof(); num_read++) {
            getline(record_file, records[num_read]);
        }

        qsort(records, num_read, BLOCK_SIZE + 1, compar);

        std::string run_file_name = file_base + std::to_string(num_read) + ".csv";
        std::ofstream run_file(run_file_name, std::ofstream::out);
        for(int w = 0; w < num_read; w++) {
            run_file << records[w] << "\n";
        }   

    }
    return runs_written;

}



/*
 *
 * Join a_runs and b_runs from a and b, sequentially. 
 * 
 */
void join_runs(std::string a_base, std::string b_base, int a_runs, int b_runs, int (*compar)(const void*, const void*)) {

    
    std::ofstream join_file(JOIN_FILE, std::ofstream::out);
    for(int r = 0; r < std::max(a_runs, b_runs); r++) {

        std::ifstream a_file(a_base + std::to_string(r) + ".csv", std::ifstream::in);
        std::ifstream b_file(b_base + std::to_string(r) + ".csv", std::ifstream::in);

        std::string a_record;
        std::string b_record;

        getline(a_file, a_record);
        getline(b_file, b_record);
        
        while(!a_file.eof() && !b_file.eof()) {

            int comparison = compar(static_cast<void*>(&a_record), static_cast<void*>(&b_record));
            if(comparison == 0) {
                join_file << a_record << b_record << "\n";
                getline(a_file, a_record);
                getline(b_file, b_record);

            }
            else if(comparison > 0) {
                getline(a_file, a_record);
            }
            else {
                getline(b_file, b_record);
            }
        }


    }
    
}

int main(int argc, char** argv) {


    int emp_runs = to_runs(EMP_FILE, compare_employee, "emp");
    int dept_runs = to_runs(DEPT_FILE, compare_dept, "dept");

    join_runs( "dept", "emp", dept_runs, emp_runs, compare_dept_employee);

}