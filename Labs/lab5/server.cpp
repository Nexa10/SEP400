#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <queue>
using namespace std;

#define MAX_CLIENTS 3
const char IP_ADDR[] = "127.0.0.1"; 
bool is_running = true;
queue<string> message;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void sigHandler(int sig);
void* readThread(void* arg);
void errorMsg(const char* msg);
void printQueue(queue<string> str);

int main(int argc, char* argv[]){
    int fd, cl, ret;
    struct sockaddr_in addr;
    struct sockaddr_in destaddr;
    struct sigaction sa;
    sa.sa_handler = &sigHandler;
    sa.sa_flags = SA_RESTART;

    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(fd < 0){
        errorMsg("Error: Socket failed");
    }

    bzero((char *)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    addr.sin_port = htons(atoi(argv[1]));
    // if((ret = inet_pton(AF_INET, IP_ADDR, addr.sin_addr)) == 0){
    //     close(fd);
    //     errorMsg("Error: ret")
    // }

    //bind
    if((bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0){
        errorMsg("Error: Bind failed");
    }

    //listen
    if(listen(fd, 5) < 0){
        close(fd);
        errorMsg("Error: listen failed")
    }

    int num_client = 0;
    pthread_t clientRecv;
    fd_set readfds;  
    while(is_running){
        //accept
        if(num_client < MAX_CLIENTS){
            if(cl = accept(fd, (struct sockaddr *)&destaddr, sizeof(destaddr)) < 0){

            }
            else{
                num_client += 1;
            }
        }

        pthread_mutex_lock(&mutex);
        print(message);
        if(!message.empty()) while(!message.empty()) message.pop();
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    //send quit
    
    pthread_join(&clientRecv, NULL); 
    return 0;
}

void sigHandler(int sig){
    if(sig == SIGINT){
        is_running = false;
    }
}
void* readThread(void* arg){
    int fd = *(int *)arg;
    char buf[4026];
    int rd = read(fd, buf, sizeof(buf)-1); 
    while(is_running){
        pthread_mutex_lock(&mutex);  
        message.push(buf);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void errorMsg(const char* msg){
    perror(msg);
    exit(-1);
}

void printQueue(queue<string> str){
    while(!str.empty()){
        cout << str.front() <<endl;
        str.pop();
    }
}