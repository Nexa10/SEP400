// Logger.cpp
//
// 01-Mar-23  Yiyuan Dong         Created.
//

#include "Logger.h"
#include <errno.h>
#include <netinet/ip.h> //  socket  #include <sys/socket.h> +  #include <netinet/in.h>
#include <arpa/inet.h>  //  inet_pton()
#include <string.h>     //  memset()
#include <pthread.h>
#include <time.h>       //  time_t()
#include <stdio.h>      //  sprintf()
#include <unistd.h>     //  close()
#include <iostream>

using namespace std;

#define DEBUG_CMD "Set Log Level=0"
#define WARNING_CMD "Set Log Level=1"
#define ERROR_CMD "Set Log Level=2"
#define CRITICAL_CMD "Set Log Level=3"

void *recv_func(void *arg);

LOG_LEVEL filter;
// LOG_LEVEL LEVEL = ERROR;
int test = 1;
const int BUF_LEN = 1024;
char buf[BUF_LEN];
struct sockaddr_in myaddr;
struct sockaddr_in servaddr;
int fd, ret;
bool is_running = true;
pthread_mutex_t lock_x;

int InitializeLog() {
    /* -----------------------------------------
     create a non-blocking socket for UDP communications (AF_INET, SOCK_DGRAM). 
     Set the address and port of the server. Create a mutex to protect any shared resources. 
     Start the receive thread and pass the file descriptor to it.
     -----------------------------------------  */
    // TODO: CLIENT BINDS ITSELF TO A DIFFERENT ADDRESS AND PORT
    
    const int SERV_PORT = 1153;
    const char SERV_ADDR[] = "127.0.0.1";
    const int PORT = 1154;

    fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);

    cout << "[DEBUG]" <<"Initializing log...."<< endl;

    // struct sockaddr_in addr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    // ret = inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);
    //  TODO: ERROR CHECKING
    ret = bind(fd, (struct sockaddr*)&myaddr, sizeof(myaddr));

    cout << "[DEBUG]" <<"Socket created, fd: " << fd << endl;

    //  server addr 
     memset((char *)&servaddr, 0, sizeof(servaddr));
         servaddr.sin_family = AF_INET;
    ret = inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);
    if(ret==0) {
        cout<<"No such address"<<endl;
	cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
    }
    servaddr.sin_port = htons(SERV_PORT);
    // ------------------------------

    pthread_mutex_init(&lock_x, NULL);    
    pthread_t tid = 0;
    ret = pthread_create(&tid, NULL, recv_func, &fd);
        if(ret!=0) {
        cout<<"Cannot create receive thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
    }
    cout << "[DEBUG]" <<"Thread created: " << tid << " returned " << ret <<endl;
    filter = ERROR;
    return 0;

}

void SetLogLevel(LOG_LEVEL level) {
    //  set the filter log level and store in a variable global within Logger.cpp.
    cout << "[### level set ###]" <<"Setting Log level"<< endl;
    pthread_mutex_lock(&lock_x);
    filter = level;
    pthread_mutex_unlock(&lock_x);
}

void Log(LOG_LEVEL level, const char *prog, const char *func, int line,
         const char *message) {
    /* -----------------------------------------
     compare the severity of the log to the filter log severity. The log will be thrown away if its severity is lower than the filter log severity.
     create a timestamp to be added to the log message. Code for creating the log message will look something like: 
        time_t now = time(0); 
        char *dt = ctime(&now); 
        memset(buf, 0, BUF_LEN); 
        char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"}; 
        len = sprintf(buf, "%s %s %s:%s:%d%s\n", dt, levelStr[level], file, func, line, message)+1;
        buf[len-1]='\0';
     apply mutexing to any shared resources used within the Log() function.
     The message will be sent to the server via UDP sendto().
    ----------------------------------------- */
    // cout << "[DEBUG]" <<"Log() filter level: "<< LEVEL <<" filter: " << filter <<  endl;
    pthread_mutex_lock(&lock_x);
    bool skip = (level < filter);
    // cout << "[DEBUG]" <<"Skip? " << skip << "level: " << level << " set: " << filter <<endl;
    pthread_mutex_unlock(&lock_x);

    if(skip) return;
    // cout << "[DEBUG]" <<"Sending log...."<< endl;
    time_t now = time(0); 
    char *dt = ctime(&now); 
    char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"}; 
    pthread_mutex_lock(&lock_x);
    memset(buf, 0, BUF_LEN); 
    int len = sprintf(buf, "%s %s %s:%s:%d%s\n", dt, levelStr[level], prog, func, line, message)+1;
    // int len = sprintf(buf, "test messafe")+1;
    buf[len-1]='\0';
    len = sendto(fd, buf, len, 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
    // cout << "[DEBUG]" <<"Message sent to server. bytes: "<<  len <<endl;
    pthread_mutex_unlock(&lock_x);
    // cout << "[DEBUG]" <<"Message sent to server. "<< endl;
    cout << "[DEBUG]" << buf << endl;
    
}

void ExitLog() {
    //  will stop the receive thread via an is_running flag and close the file descriptor.
    cout << "[DEBUG]" <<"Exitlog()"<< endl;
    is_running = false;
    close(fd);
}

void *recv_func(void *arg) {
    /* -----------------------------------------
    The receive thread is waiting for any commands from the server. 
    So far there is only one command from the server: "Set Log Level=<level>". 
    The receive thread will:
        accept the file descriptor as an argument. 
        run in an endless loop via an is_running flag. 
        apply mutexing to any shared resources used within the recvfrom() function. 
        ensure the recvfrom() function is non-blocking with a sleep of 1 second if nothing is received. 
        act on the command "Set Log Level=<level>" from the server to overwrite the filter log severity.
    ----------------------------------------- */
    int fd = *(int *)arg;
    cout << "[DEBUG]" <<"Recv(), fd: " << fd << endl;
    is_running = true;
    //Set a socket timeout of 1 seconds
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    // socklen_t addrlen = sizeof(myaddr);
    struct sockaddr_in recvaddr;
    socklen_t addrlen = sizeof(recvaddr);
    while (is_running){
        // Lock buff
        pthread_mutex_lock(&lock_x);
        memset(&buf, 0, sizeof(buf));
        // cout << "[DEBUG]" <<"Reading message...."<< endl;
        ret = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr *)&recvaddr, &addrlen);
        // cout << "[DEBUG]" <<"Received " << ret << " bytes." << buf <<endl;
        pthread_mutex_unlock(&lock_x);
        // TODO: ERROR CHECKING
        if(ret > 0) {
            LOG_LEVEL new_level = filter;
            if(strncmp(DEBUG_CMD, buf, 21) == 0) new_level = DEBUG;
            if(strncmp(WARNING_CMD, buf, 23) == 0) new_level = WARNING;
            if(strncmp(ERROR_CMD, buf, 21) == 0) new_level = ERROR;
            if(strncmp(CRITICAL_CMD, buf, 24) == 0) new_level = CRITICAL;

            if(new_level != filter) {
                pthread_mutex_lock(&lock_x);
                filter = new_level;
                cout << "[###### DEBUG ######]" <<"New level set to " << new_level <<endl;
                pthread_mutex_unlock(&lock_x);
            }
        }

        // cout << "[DEBUG]" << "Trying to read again" <<endl;
        sleep(1);
    }
    pthread_exit(NULL);
    cout << "[DEBUG]" <<"Closing thread " <<endl;
}
