
#include "pidUtil.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main()
{

    // GetAllPids takes a vector to return the pids
    // You need a vector of int: vector <int> "whatever you wanna call it"
    // You need a string to store pids in the vector, you can call the string whatever
    // You need an int to do Q4 & Q5 (instantiated the int var to zero)

    // Q1: Get all names
    // pass the vector name into GetAllPids()
    vector<int> Pids;
    GetAllPids(Pids);

    // Q2: Get all pids
    // 1. Iterate through the vector created, 2. then pass the index (i),
    // and the string variable into GetNameByPid()
    cout << "Pidlist" << endl;
    string Name;
    for (int i = 0; Pids[i] < Pids.size(); i++)
    {
        GetNameByPid(Pids[i], Name);
        cout << i << "    " << Name << endl
             << endl;
    }

    /*for(auto i: ("vector name")){
        implement 2. in here (don't forget to print out the name)
    }*/

    // q3: Set Pid to 1
    // pass "1" and the string variable into GetNameByPid();
    // print the string variable
    int Pid = 1;
    GetNameByPid(Pid, Name);
    cout << "Returning process name for process id 1: " << Name << endl;

    // Q4: Get Pid of Lab1
    // pass "Lab1" and the int variable into GetPidByName();
    // print the int variable
    int pidlab1;
    string Lab1;
    GetPidByName(Lab1, pidlab1);
    cout << pidlab1 << endl;

    // Q%: Get Pid of Lab11 -> Err_msg
    // Make a variable of type ErrStatus (ErrStatus is the enum in the header file)
    // Instantiate your variable with GetPidByName() - passing "Lab11", and your int variable
    //  i.e. ErrStatus var = func
    // the print out the error message by passing the ErrStatus var into GetErrorMsg()
    string Lab11;
    ErrStatus Puka;
    GetPidByName(Lab11, pidlab1);

    return 0;
}
