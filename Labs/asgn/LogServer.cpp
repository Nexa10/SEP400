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
#include <curses.h> 
#include <iostream>
using namespace std;  

void sigHandler(int sig);
void *recvFunc(void* arg);
void error_msg(const char* msg);

pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

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
    pthread_t recv_thread;

    struct sigaction sa;
    sa.sa_handler = &sigHandler;
    sa.sa_flag = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    fd = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, 0);
    if(fd < 0){
        error_msg("ERROR: Socket()");
    }

    bzero((char *)&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);
    addr.sin_port = htons(atoi(PORT));

    if((bind(fd, (struct sockaddr_in*)&addr, sizeof(addr))) < 0){
        error_msg("ERROR: Bind()");
    }

    pthread_create(&recv_thread, NULL, recvFunc, &fd);
     
   while(isRunning){
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
                if(file_fd < 0) error_msg("ERROR: file open");

                bzero(buf, BUF_LEN);
                read(file_fd, buf, BUF_LEN);
                cout << buf << endl;
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
   close(file_fd);
   close(fd);   

    return 0;
}


void sigHandler(int sig){
    if(sig == SIGINT) isRunning = false;
}

void *recvFunc(void* arg){
    int * fd = *(int*) arg;
    int file_fd, ret;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    while(isRunning){
        pthread_mutex_lock(lock_x);
        file_fd = open(logFile, O_WRONLY | O_CREAT | O_APPEND);
        if(file_fd < 0) error_msg("ERROR: file open");

        bzero(buf, sizeof(buf));  

        ret = recv(fd, buf, BUF_LEN, 0);
        if(ret < 0){
            if(errno == EWOULDBLOCK) sleep(1);
            else{
                close(file_fd);
                error_msg("Error: Recv()");
            } 
        }
        else write(file_fd, buf, BUF_LEN);
        
        pthread_mutex_lock(lock_x);
    }
    return pthread_exit(NULL);
}

void error_msg(const char* msg){
    perror(msg);
    exit(-1);
}
