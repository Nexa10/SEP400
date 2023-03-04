#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <queue>
using namespace std;

#define MAX_CLIENTS 3
const char IP_ADDR[] = "127.0.0.1"; 
bool is_running = true;

queue<string> message;
pthread_t client_threads [MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sigHandler(int sig);
void* readThread(void* arg);
void errorMsg(const char* msg);
void printQueue(queue<string> str);

int main(int argc, char* argv[]){
    int fd;
    int num_client = 0;
    int cl1, cl2, cl3;
    int clients[] = {cl1, cl2, cl3};

    struct sockaddr_in addr;
    struct sockaddr_in destaddr;
    int clilen = sizeof(destaddr);

    struct sigaction sa;
    sa.sa_handler = &sigHandler;
    sa.sa_flags = SA_RESTART;

    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(fd < 0){
        errorMsg("Error: Socket failed");
    }

    bzero((char *)&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[1]));

    //bind
    if((bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0){
        errorMsg("Error: Bind failed");
    }

    //listen
    if(listen(fd, 3) < 0){
        close(fd);
        errorMsg("Error: listen failed");
    }

    while(is_running){
        //accept
        while(num_client < MAX_CLIENTS){
            clients[num_client] = accept(fd, (struct sockaddr *)&destaddr, (socklen_t*)&clilen);
            if(clients[num_client] < 0){
                if(errno == EWOULDBLOCK){
                    cout << "Waiting..." << endl;
                    sleep(1);
                }
                else{
                    errorMsg("Error: Accept");
                }
            }
            else{
                pthread_create(&client_threads[num_client], NULL, readThread, &clients[num_client]);
                num_client  += 1;
            }
        }

        pthread_mutex_lock(&mutex);
        printQueue(message);
        if(!message.empty()) while(!message.empty()) message.pop();
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
    //send quit

    for(int i=0; i<MAX_CLIENTS; i++) {
        send(clients[i], "Quit\n", 4, 0);
        pthread_join( client_threads[i], NULL);
        close(clients[i]);
    }
 

    close(fd);
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
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    bzero(buf, sizeof(buf));
    int rd = read(fd, buf, sizeof(buf)-1);
    if(rd < 0){
        errorMsg("Error: Read failed");
    }
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
