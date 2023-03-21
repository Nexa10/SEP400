// Server.cpp 
//
// 01-Mar-23  Yiyuan Dong         Created.
//
#include <errno.h>
#include <netinet/ip.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h> 
#include <string.h>    
#include <signal.h>     
#include <unistd.h>
#include <pthread.h>
<<<<<<< HEAD
#include <curses.h> 
=======
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
#include <iostream>
using namespace std;  

void sigHandler(int sig);
void *recvFunc(void* arg);
void error_msg(const char* msg);

pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

<<<<<<< HEAD
int BUF_LEN = 1024;
char buf[BUF_LEN];
bool isRunning = true;

const char logFile[] = "logFile.txt";
const char LOCALHOST[] = "127.0.0.1";
const int PORT = 1153;

int main(int argc, char const *argv[]){

    int fd, client, response, level, len, file_fd;
    struct sockaddr_in addr;
    struct sockaddr_in destaddr;
    int dest_len = sizeof(destaddr);
=======
//shared resources
bool isRunning = true;
const int BUF_LEN = 1024;
char buf[BUF_LEN];
const char* logFile = "logFile.txt";
int file_fd;
struct sockaddr_in destaddr;


int main(int argc, char const *argv[]){

    const char LOCALHOST[] = "127.0.0.1";
    const int PORT = 1153;
    int fd, ret, response, level, len;
    struct sockaddr_in addr;
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
    pthread_t recv_thread;

    struct sigaction sa;
    sa.sa_handler = &sigHandler;
<<<<<<< HEAD
    sa.sa_flag = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    fd = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, 0);
=======
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
    if(fd < 0){
        error_msg("ERROR: Socket()");
    }

    bzero((char *)&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);
<<<<<<< HEAD
    addr.sin_port = htons(atoi(PORT));

    if((bind(fd, (struct sockaddr_in*)&addr, sizeof(addr))) < 0){
=======
    addr.sin_port = htons(PORT);

    if((bind(fd, (struct sockaddr*)&addr, sizeof(addr))) < 0){
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
        error_msg("ERROR: Bind()");
    }

    pthread_create(&recv_thread, NULL, recvFunc, &fd);
     
   while(isRunning){
<<<<<<< HEAD
        cout << "1. Set the log level\n2. Dump log file\n0. Shut down\n$: ";
        cin >> response;

        switch(response){
            case 1:
                cout << "What Level? (0-Debug, 1-Warning, 2-Error, 3-Critical)\n $: "
                cin >> level;
                pthread_mutex_lock(lock_x);
                memset(buf, 0, BUF_LEN);
                len=sprintf(buf, "Set Log Level=%d", level)+1;
                sendto(fd, buf, len, 0, (struct sockaddr *)&destaddr, dest_len);
                pthread_mutex_unlock(lock_x);
                break;
            case 2:
                pthread_mutex_lock(lock_x);
                file_fd = open(logFile, O_RDONLY | O_CREAT);
=======
   	cout << "---------------------------------------------------------" << endl;
        cout << "1. Set the log level\n2. Dump log file\n0. Shut down\n $: ";
        cin >> response;
	
	if(response == 0) response = 3; //case 0 is also default.
        switch(response){
            case 1:	
                cout << "\nWhat Level? (0-Debug, 1-Warning, 2-Error, 3-Critical)\n $: ";
                cin >> level;
                cout << "Setting Level..." << endl;
                pthread_mutex_lock(&lock_x);
                memset(buf, 0, BUF_LEN);
                len=sprintf(buf, "Set Log Level=%d", level)+1;
                sendto(fd, buf, len, 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
                pthread_mutex_unlock(&lock_x);
                break;
            case 2:
                pthread_mutex_lock(&lock_x);
                file_fd = open(logFile, O_RDONLY | O_CREAT, 0777);
           
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
                if(file_fd < 0) error_msg("ERROR: file open");

                bzero(buf, BUF_LEN);
                read(file_fd, buf, BUF_LEN);
                cout << buf << endl;
<<<<<<< HEAD
                pthread_mutex_unlock(lock_x);
                cout << "Press any key to continue: ";
                system("read -s -N 1"); //reads any key from the keyboard
                break;
            case 0:
                cout << "Shutting Down..." << endl;
                sleep(2);
                isRunning = false
            default:
                cout << "Option not found";
        }
   }

   pthread_join(recvThread, NULL);
   close(client);
=======
                pthread_mutex_unlock(&lock_x);
              
                cout << "Press any key to continue: ";
                char c;
                cin >> c;
                break;
            case 3:
                cout << "\nShutting Down..." << endl;
                isRunning = false;
            default:
            	cout << " ";
            
        }
   }
   pthread_join(recv_thread, NULL);
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
   close(file_fd);
   close(fd);   

    return 0;
}

<<<<<<< HEAD

=======
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
void sigHandler(int sig){
    if(sig == SIGINT) isRunning = false;
}

void *recvFunc(void* arg){
<<<<<<< HEAD
    int * fd = *(int*) arg;
    int file_fd, ret;
=======
    int fd = *(int*) arg;
    int ret;
    // comment: does not work 
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
<<<<<<< HEAD
    
    while(isRunning){
        pthread_mutex_lock(lock_x);
        file_fd = open(logFile, O_WRONLY | O_CREAT | O_APPEND);
        if(file_fd < 0) error_msg("ERROR: file open");

        bzero(buf, sizeof(buf));  

        ret = recv(fd, buf, BUF_LEN, 0);
=======
    // -----------
    socklen_t dest_len = sizeof(destaddr);
    
    while(isRunning){
        pthread_mutex_lock(&lock_x);
        file_fd = open(logFile, O_WRONLY | O_CREAT | O_APPEND, 0777);
        if(file_fd < 0) error_msg("ERROR: file open");

        bzero(buf, BUF_LEN);  
        ret = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr *)&destaddr, &dest_len);
 
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
        if(ret < 0){
            if(errno == EWOULDBLOCK) sleep(1);
            else{
                close(file_fd);
                error_msg("Error: Recv()");
            } 
        }
        else write(file_fd, buf, BUF_LEN);
        
<<<<<<< HEAD
        pthread_mutex_lock(lock_x);
    }
    return pthread_exit(NULL);
=======
        pthread_mutex_unlock(&lock_x);
    }
  
    pthread_exit(NULL);
>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
}

void error_msg(const char* msg){
    perror(msg);
    exit(-1);
}
<<<<<<< HEAD
=======

>>>>>>> e23b45591b7f3870e6e079dbde1d3df215a85c2b
