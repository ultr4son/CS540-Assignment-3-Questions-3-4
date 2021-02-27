
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>

//Both records have the same block size
const int BLOCK_SIZE = 4 + 40 + 8 + 4;
const int MAX_BLOCKS = 22;

const std::string EMP_FILE = "Emp.csv";
const std::string DEPT_FILE = "Dept.csv";
const std::string JOIN_FILE = "Join.csv";

struct ReadBuffer {
    std::string line;
    std::ifstream* file;
};

int get_managerid(std::string d) {
    std::stringstream ds(d);
    std::string managerid;
    for(int i = 0; i < 4; i++) {
        getline(ds, managerid, ',');
    }
    return std::stoi(managerid);
}

int get_eid(std::string e) {
    int eid;
    sscanf(e.c_str(), "%d,", &eid);
    return eid;
}

bool compare_employee(std::string a, std::string b) {
    

    int eid_a;
    sscanf(a.c_str(), "%d,", &eid_a);
    
    int eid_b;
    sscanf(b.c_str(), "%d,", &eid_b);

    return eid_a < eid_b;
}

bool compare_dept(std::string a, std::string b) {
    //TFW you want to split a comma separated string in c++    
    std::stringstream as(a);
    std::stringstream bs(b);

    std::string managerid_a;
    std::string managerid_b;
    for(int i = 0; i < 4; i++) {
        getline(as, managerid_a, ',');
        getline(bs, managerid_b, ',');
    }

    return std::stoi(managerid_a) < std::stoi(managerid_b);
}

int compare_dept_employee(std::string dept, std::string emp) {

    int eid;
    sscanf(emp.c_str(), "%d,", &eid);
    
    std::stringstream as(dept);
    std::string managerid;
    for(int i = 0; i < 4; i++) {
        getline(as, managerid, ',');
    }


    return std::stoi(managerid) - eid;


}



/*
 * Convert the record into a number of sorted runs labeled <file_base><n> based on the function compar.
 *  
 */
int to_runs(std::string record_file_name, bool (*compar)(std::string, std::string), std::string file_base) {
    int runs_written;

    std::ifstream record_file(record_file_name, std::ifstream::in);

    //Read, sort, and write runs
    for(runs_written = 0; record_file; runs_written++) {
        int num_read = 0;

        std::vector<std::string> records;
        std::string line;
        while(getline(record_file, line) && num_read < MAX_BLOCKS){
            records.push_back(line);
            num_read++;                        

        }

        sort(records.begin(), records.end(), compar);

        std::string run_file_name = file_base + std::to_string(runs_written) + ".csv";
        std::ofstream run_file(run_file_name, std::ofstream::out | std::ofstream::trunc);
        for(int w = 0; w < num_read; w++) {
            run_file << records[w] << "\n";
        }   

    }
    return runs_written;

}



std::vector<ReadBuffer> read_all(std::vector<std::ifstream*> files) {
    std::vector<ReadBuffer> tuples;
    std::string tuple;
    for(auto& file : files) {
        int pos = file->tellg();

        getline(*file, tuple);
        tuples.push_back({tuple, file});

        file->seekg(pos);
    }
    return tuples;
}

std::string peek_line(std::ifstream& file) {
        std::string tuple;
        int pos = file.tellg();

        getline(file, tuple);
        
        file.seekg(pos);
        return tuple;
}

void advance(ReadBuffer& r) {
    std::string temp;
    getline(*r.file, temp);
    
    r.line = peek_line(*r.file);
}

bool buffer_avaliable(std::vector<ReadBuffer> rbs) {
    for(auto& r : rbs) {
        if(r.line != "") {
            return true;
        }
    }
    return false;
}
std::vector<int> readBuffer_as_keys(std::vector<ReadBuffer> rb, int (as_int)(std::string)) {
    std::vector<int> as_key;
    for(int i = 0; i < rb.size(); i++) {
        if(rb[i].line != "") {
            as_key.push_back(as_int(rb[i].line));
        }
    }
    return as_key;
}

/*
 *
 * Join a_runs and b_runs from a and b, sequentially. 
 * 
 */
void join_runs(std::string a_base, std::string b_base, int a_runs, int b_runs, int(a_as_int)(std::string), int (b_as_int)(std::string), bool b_not_unqique) {
    std::ofstream join_file(JOIN_FILE, std::ofstream::out | std::ofstream::trunc);
    
    std::vector<std::ifstream*> a_files;
    std::vector<std::ifstream*> b_files;

    for(int i = 0; i < a_runs; i++) {
        a_files.push_back(new std::ifstream(a_base + std::to_string(i) + ".csv", std::ifstream::in));
    }
    for(int i = 0; i < b_runs; i++) {
        b_files.push_back(new std::ifstream(b_base + std::to_string(i) + ".csv", std::ifstream::in));
    }
    std::vector<ReadBuffer> a_in_buffer = read_all(a_files);
    std::vector<ReadBuffer> b_in_buffer = read_all(b_files);


    while(buffer_avaliable(a_in_buffer) && buffer_avaliable(b_in_buffer)) {
        std::vector<int> a_keys = readBuffer_as_keys(a_in_buffer, a_as_int);
        std::vector<int> b_keys = readBuffer_as_keys(b_in_buffer, b_as_int);

        auto min_a_position = min_element(a_keys.begin(), a_keys.end());
        auto min_b_position = min_element(b_keys.begin(), b_keys.end());

        int a_i = std::distance(a_keys.begin(), min_a_position);
        int b_i = std::distance(b_keys.begin(), min_b_position);

        int min_a = *min_a_position;
        int min_b = *min_b_position;

        if(min_a == min_b) {
            join_file << b_in_buffer[b_i].line << "," << a_in_buffer[a_i].line << "\n";
            if(b_not_unqique) {
                advance(b_in_buffer[b_i]);
            }
            else {
                advance(a_in_buffer[a_i]);
                advance(b_in_buffer[b_i]);
            }


        }
        else if(min_a > min_b) {
            advance(b_in_buffer[b_i]);
        }
        else {
            advance(a_in_buffer[a_i]);
        }







    }

    
}


int main(int argc, char** argv) {


    int emp_runs = to_runs(EMP_FILE, compare_employee, "emp");
    int dept_runs = to_runs(DEPT_FILE, compare_dept, "dept");

    join_runs("emp", "dept", emp_runs, dept_runs, get_eid, get_managerid, true);

}