#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

struct workersLog{
    int entries = 0;
    double amount = 0;
    int hours = 0;
};

int selection;

int main(){
    
    bool isRunning = true;
    workersLog* addr;
    memset(&addr, 0, sizeof(addr));

    char logfile[] = "WorkData.bin";
    const int MEM_SIZE = 64;
    struct stat sb;

    int openFlags = O_RDWR | O_CREAT;
    mode_t filePerms =  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    int fd = open(logfile, openFlags, filePerms);
    if(fd == -1) {
        cout<<"Open()" << endl;
        cout<<strerror(errno)<<endl;
        exit(-1);
    }

    if(stat(logfile, &sb) == -1){
        char buf[MEM_SIZE];
        memset(buf,0,MEM_SIZE);
        write(fd, buf, MEM_SIZE);
    }
    
    addr = (workersLog *)mmap(NULL, MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
	    cout<<"mmap()"<<endl;
        close(fd);
        exit(-1);
    }


    while(isRunning){
        system("clear");
        cout << "WORK PACKAGE" << endl;
        printf("\nEntries: %d\nHours: %d\nBalance: $%.2f\n\n", addr->entries, addr->hours, addr->amount);
        printf("Make a selection: \n1. Log hours.\n2. Add Expense.\n3. Add Deposit.\n4. Refresh.\n5. Clear Data.\n0. Quit.\n\n");
        cout << "Selection: ";
        cin >> selection;
        double amt = 0;

        switch (selection){
            case 1:
                cout << "Enter number of hours: ";
                cin >> amt;
                addr->hours += amt;
                addr->entries ++;
                msync(addr, sb.st_size, MS_SYNC);
                break;
            
            case 2:
                cout << "Enter Expense: $";
                cin >> amt;

                if(amt > addr->amount){
                    cout << "Insufficient Balance\n";
                    sleep(2);
                }else{
                    addr->amount -= amt;
                    msync(addr, sb.st_size, MS_SYNC);
                }
                break;

            case 3:
                cout << "Enter Deposit: $";
                cin >> amt;

                addr->amount += amt;
                msync(addr, sb.st_size, MS_SYNC);
                break;

            case 4:
                msync(addr, sb.st_size, MS_SYNC);
                break;

            case 5:
                addr->amount = 0;
                addr->entries = 0;
                addr->hours = 0;
                msync(addr, sb.st_size, MS_SYNC);
                break;

            case 0:
                isRunning = false;
                break;

            default:
                cout << "Invalid Option" << endl;
        }
    }
    cout << "Exiting." << endl;

    return 0;
}
