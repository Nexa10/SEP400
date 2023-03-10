//intfMonitor_solution.cpp - An interface monitor
//
// 13-Jul-20  M. Watler         Created.
//
//Dennis Audu
//148463193
//daudu@myseneca.ca

#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

const int MAXBUF=128;
bool isRunning=false;

//TODO: Declare your signal handler function prototype
void signalHandler(int signum);

int main(int argc, char *argv[])
{
    //TODO: Declare a variable of type struct sigaction
    //      For sigaction, see http://man7.org/linux/man-pages/man2/sigaction.2.html
    struct sigaction sa;
    sa.sa_handler = &signalHandler;
    sa.sa_flags = SA_RESTART;

    char interface[MAXBUF];
    char statPath[MAXBUF];
    const char logfile[]="Network.log";//store network data in Network.log
    int retVal=0;

    //TODO: Register signal handlers for SIGUSR1, SIGUSR2, ctrl-C and ctrl-Z
    //TODO: Ensure there are no errors in registering the handlers
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    strncpy(interface, argv[1], MAXBUF);//The interface has been passed as an argument to intfMonitor
    int fd=open(logfile, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    cout<<"intfMonitor:main: interface:"<<interface<<":  pid:"<<getpid()<<endl;

    //TODO: Wait for SIGUSR1 - the start signal from the parent
    while(!isRunning){
        sleep(0.1);
    }

    while(isRunning) {
        //gather some stats
        int tx_bytes=0;
        int rx_bytes=0;
        int tx_packets=0;
        int rx_packets=0;
	    ifstream infile;
            sprintf(statPath, "/sys/class/net/%s/statistics/tx_bytes", interface);
	    infile.open(statPath);
	    if(infile.is_open()) {
	        infile>>tx_bytes;
	        infile.close();
	    }
            sprintf(statPath, "/sys/class/net/%s/statistics/rx_bytes", interface);
	    infile.open(statPath);
	    if(infile.is_open()) {
	        infile>>rx_bytes;
	        infile.close();
	    }
            sprintf(statPath, "/sys/class/net/%s/statistics/tx_packets", interface);
	    infile.open(statPath);
	    if(infile.is_open()) {
	        infile>>tx_packets;
	        infile.close();
	    }
            sprintf(statPath, "/sys/class/net/%s/statistics/rx_packets", interface);
	    infile.open(statPath);
	    if(infile.is_open()) {
	        infile>>rx_packets;
	        infile.close();
	    }
	    char data[MAXBUF];
	    //write the stats into Network.log
	    int len=sprintf(data, "%s: tx_bytes:%d rx_bytes:%d tx_packets:%d rx_packets: %d\n", interface, tx_bytes, rx_bytes, tx_packets, rx_packets);
	    write(fd, data, len);
	    sleep(1);
    }
    close(fd);

    return 0;
}

//TODO: Create a signal handler that starts your program on SIGUSR1 (sets isRunning to true)

void signalHandler(int signum){
	switch(signum){
		case SIGUSR1:
			cout << "intfMonitor: starting up" <<endl;
			isRunning = true;
			break;
			
		case SIGUSR2:
			cout << "intfMonitor: shutting down" <<endl;
			isRunning = false;
			break;
			
		case SIGINT:
			cout << "intfMonitor: ctrl-C discarded" <<endl;
			break;
			
		case SIGTSTP:
			cout << "intfMonitor: ctrl-Z discarded" <<endl;
			break;
		
		default:
			cout << "intfMonitor: undefined signal" <<endl;
	}
}
