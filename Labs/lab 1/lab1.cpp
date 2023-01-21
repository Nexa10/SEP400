// Dennis Audu
// 148463193
// daudu@myseneca.ca

#include "pidUtil.h"
#include <string>
#include<vector>

int main(){

	vector <int> pids;
	string name;
	int pid_num;
	
//Get all names
	GetAllPids(pids);
	if (pids.empty() == false){
		cout <<"Q1: ...Pids retrieved "<< endl;
		cout << endl;
	}
	
//Get all pids
	cout <<"Q2: Pid Names "<< endl;
	for(auto i: pids){
		GetNameByPid(i, name);
		cout << i << " "<< name;
		cout << endl;
	}
	cout << endl;
	
//Set Pid to 1
	cout <<"Q3: "<< endl;
	GetNameByPid(1, name);
	cout << "Pid: 1, Name: " << name <<endl;
	cout << endl;
	
//Get Pid of Lab1
	cout <<"Q4: "<< endl;
	name = "Lab1";
	cout << "Lab1: "<<  GetPidByName(name, pid_num) << endl << endl;
	
	
//Get Pid of Lab11 -> Err_msg
	cout <<"Q5: "<< endl;
	ErrStatus msg = GetPidByName("Lab11", pid_num);
	
	cout << "Error Msg: " << GetErrorMsg(msg) << endl;
	cout << endl;

	return 0;
}
