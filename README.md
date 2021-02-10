# CS540-Assignment-3-Questions-3-4
Programs for questions 3-4 on CS 540 Assignment 3.

# Question 3:
Assume that we have a relation Employee(id, name, bio, manager-id). 
The values of id and manager-id are character strings with fixed size of 8 bytes. The values of name and bio are character strings and take at most 200 and 500 bytes. Note that as opposed to the values of id and manager-id, the sizes of the values of name (bio) are not fixed and are between 1 to 200 (500) bytes. The size of each block is 4096 bytes (4KB). The size of each record is less than the size of a block. 
Write a C++ program that reads an input Employee relation and builds a linear hash index for the relation using attribute id.  You may use a hash function of your choice. Your program must increment the value of n if the average number of records per each block exceeds 80% of the block capacity. The input relation is stored in a CSV file, i.e., each tuple is in a separate line and fields of each record are separated by commas. The program must also support looking up a tuple using its id.
* The program must accept switch C for index creation mode and L for lookup in its command line. The switch L is succeeded by the value of input id.
* Your program must assume that the input CSV file is in the current working directory, i.e.,the one from which your program is running, and its name is Employee.csv
* The program must store the indexed relation in a file with the name EmployeeIndex on the current working directory.
* Your program must run on hadoop-master.engr.oregonstate.edu. Submissions should alsoinclude the g++ command (including arguments) that was used to compile the program.Each student has an account on the hadoop-master.engr.oregonstate.eduserver, which is a Linux machine.  You can use the following bash command to connect to it:

  > ssh your_onid_username@hadoop-master.engr.oregonstate.edu

  It will prompt you for your ONID password.  You will need to be connected to the VPN inorder to access the server.

* You can use the following commands to compile and run C++ code:
  > g++ main.cpp -o main.out > main.out
  
# Question 4:
Using the relations and data described in question (3), write a C++ program that reads aninput Employee relation and builds an R-Tree index for the relation using attributes id and name. The input relation is stored in a CSV file, i.e., each tuple is in a separate line and fields of eachrecord are separated by commas.
* The program must accept switch C for index creation in its command line.
* Your program must assume that the input CSV file is in the current working directory, i.e.,the one from which your program is running, and its name is Employee.csv
* The program must store the index relation in a file with the name EmployeeRTree on the current working directory.
* Your program must run on hadoop-master.engr.oregonstate.edu. Submissions should also include the g++ command (including arguments) that was used to compile the program. Each student has an account on the hadoop-master.engr.oregonstate.eduserver, which is a Linux machine.  You can use the following bash command to connect to it:
  > ssh your_onid_username@hadoop-master.engr.oregonstate.edu
  It will prompt you for your ONID password. You will need to be connected to the VPN in order to access the server.
* You can use the following commands to compile and run C++ code:\
  > g++ main.cpp -o main.out> main.ou
