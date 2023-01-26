
#include "pidUtil.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// run it with: g++ -o Lab1 Lab1.cpp libPidUtil.a 
//this makes Lab1 a process, so you print out Q4

int main()
{

    // GetAllPids takes a vector to return the pids
    // You need a vector of int: vector <int> "whatever you wanna call it"
    vector<int> Pids;
    // You need a string to store pids in the vector, you can call the string whatever
    string Name;
    // You need an int to do Q4 & Q5 (instantiated the int var to zero)
    int Pid;

    // Q1: Get all names
    // pass the vector name into GetAllPids()
    GetAllPids(Pids);
    
    // Q2: Get all pids
    // 1. Iterate through the vector created, 2. then pass the index (i),
    // and the string variable into GetNameByPid()
    cout << "Pidlist" << endl;
    
    for (int i = 0; i < Pids.size(); i++)
    {
        GetNameByPid(Pids[i], Name);
        cout << Pids[i] << " " << Name << endl;
    }
    cout << endl;

    // q3: Set Pid to 1
  
    // pass "1" and the string variable into GetNameByPid();
    GetNameByPid(1, Name);
    // print the string variable
    cout << "Returning process name for process id 1: " << Name << endl << endl;

    // Q4: Get Pid of Lab1
    // pass "Lab1" and the int variable (int Pid )into GetPidByName();
    GetPidByName("Lab1", Pid);
    // print the int variable
    cout << "Lab1 pid: " << Pid << endl;

    // Q5: Get Pid of Lab11 -> Err_msg
    
    // Make a variable of type ErrStatus (ErrStatus is the enum in the header file)
    // Instantiate your variable with GetPidByName() - passing "Lab11", and your int variable
    //  i.e. ErrStatus var = func
    cout << endl;
    ErrStatus Puka = GetPidByName("Lab11", Pid);
    
    // the print out the error message by passing the ErrStatus var into GetErrorMsg()
    cout << GetErrorMsg(Puka) << endl;
    


    return 0;
}
