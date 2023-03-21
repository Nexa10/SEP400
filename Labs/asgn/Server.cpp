// Server.cpp 
//
// 01-Mar-23  Yiyuan Dong         Created.
//

//--- includes ---------------------------
#include <netinet/ip.h>     
#include <arpa/inet.h>      
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>         
#include <unistd.h>         
#include <errno.h>
#include <fstream>
using namespace std;  

/*------------ Declarations --------------*/
//  Signal Handler
void sigHandler(int sig);
//  Thread Receive function
void *recv_func(void *arg);

//  Helper function
int getInteger();
void check(int ret);
void setupSigHandler();

//  Global variables
const char LOCALHOST[] = "127.0.0.1";
const int PORT = 1153;
bool isRunning = true;
const int BUF_LEN=4096;

//Shared Resources
const char* logFile = "logFile.txt";
struct sockaddr_in cl_addr;

//Mutex
pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    //Initializes Sigaction
    setupSigHandler();
    
    int rc, client,response;
    pthread_t tid;
    char buf[BUF_LEN];

    struct sockaddr_in sv_addr;

    memset(&sv_addr, 0, sizeof(sv_addr));
    memset(&cl_addr, 0, sizeof(cl_addr));

    int server = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    check(server);

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(PORT);
    rc = inet_pton(AF_INET, LOCALHOST, &sv_addr.sin_addr);
    rc = bind(server, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    check(rc);
    
    rc = pthread_create(&tid, NULL, recv_func, &server);
    check(rc);

    while(isRunning){
        memset(buf, 0, BUF_LEN);
        cout << "1. Set the log level\n2. Dump log file\n0. Shut down\n: ";
        response = getInteger();

        switch(response){
            case 0:
                isRunning = false;
                break;

            case 1:
                cout << "What Level? (0-Debug, 1-Warning, 2-Error, 3-Critical): ";
                int level;
                do {
                    level = getInteger();
                    if (level > 3 || level < 0) {
                        cout << "Invalid option. Try again (0-Debug, 1-Warning, 2-Error, 3-Critical): ";
                    } else {
                        rc=sprintf(buf, "Set Log Level=%d", level)+1;
                        sendto(server, buf, rc, 0, (struct sockaddr *)&cl_addr, sizeof(cl_addr));   
                    }
                } while(level>3 || level < 0);
                break;

            case 2: {
                ifstream file(logFile); 
                string line;
                pthread_mutex_lock(&lock_x);
                while (getline(file, line)) {
                    cout << line << endl;
                }
                pthread_mutex_unlock(&lock_x);
                file.close();
                
                cout << endl << "Press any key to continue: ";
                getchar();
                break;
            }
            default:
                cout << "Invalid option." << endl;
                break;
        }
    }
    pthread_join(tid, NULL);
    close(server);

    return 0;
}

void *recv_func(void *arg) {
    int server = *(int *)arg;
    int msg_len, log_fd;
    socklen_t addr_len = sizeof(cl_addr);
    char tempBuf[BUF_LEN];

    log_fd = open(logFile, O_WRONLY | O_CREAT | O_APPEND, 0777);
    check(log_fd);
    
    while (isRunning){
        memset(tempBuf, 0, BUF_LEN);
        msg_len = recvfrom(server, tempBuf, BUF_LEN, 0, (struct sockaddr *)&cl_addr, &addr_len);
        if(msg_len < 0){
            sleep(3);
        }   
        else {
            pthread_mutex_lock(&lock_x);
            check(write(log_fd, tempBuf, msg_len));
            pthread_mutex_unlock(&lock_x);
        }
    }
    close(log_fd);
    pthread_exit(NULL);
}

void sigHandler(int sig){
    if(sig == SIGINT) isRunning = false;
}

// Helper Functions:
int getInteger() {
    int num;

    while(!(std::cin >> num)) {     
        std::cout << "Invalid number. Please enter an INTEGER: ";    
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }  
    cin.clear();
    cin.ignore(1000, '\n');
    
    return num;    
}

void check(int ret){
    if (ret < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) sleep(1);
        else{
            cerr << strerror(errno) << endl;
            exit(ret);
        }    
    }
}

void setupSigHandler(){
    struct sigaction sa;
    sa.sa_handler = &sigHandler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
}