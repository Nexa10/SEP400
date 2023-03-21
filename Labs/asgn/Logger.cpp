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

#define DEBUG_CMD "Set Log Level=<DEBUG>"
#define WARNING_CMD "Set Log Level=<WARNING>"
#define ERROR_CMD "Set Log Level=<ERROR>"
#define CRITICAL_CMD "Set Log Level=<CRITICAL>"


LOG_LEVEL LEVEL = DEBUG;
const int BUF_LEN = 1024;
char buf[BUF_LEN];
struct sockaddr_in addr;
int server, ret;
bool is_running = true;
pthread_mutex_t lock_x;

int InitializeLog() {
    /* -----------------------------------------
     create a non-blocking socket for UDP communications (AF_INET, SOCK_DGRAM). 
     Set the address and port of the server. Create a mutex to protect any shared resources. 
     Start the receive thread and pass the file descriptor to it.
     -----------------------------------------  */
    const char LOCALHOST[] = "127.0.0.1";
    const int PORT = 1153;

    server = socket(AF_INET, SOCK_DGRAM, 0);

    // struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    ret = inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);
    //  TODO: ERROR CHECKING
    ret = bind(server, (struct sockaddr*)&addr, sizeof(addr));

    pthread_mutex_init(&lock_x, NULL);    
    pthread_t tid;
    ret = pthread_create(&tid, NULL, recv_func, &server);

}

void SetLogLevel(LOG_LEVEL level) {
    //  set the filter log level and store in a variable global within Logger.cpp.
    LEVEL = level;
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
    pthread_mutex_lock(&lock_x);
    bool skip = (level < LEVEL);
    pthread_mutex_unlock(&lock_x);

    if(skip) return;
   
    time_t now = time(0); 
    char *dt = ctime(&now); 
    memset(buf, 0, BUF_LEN); 
    char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"}; 
    int len = sprintf(buf, "%s %s %s:%s:%d%s\n", dt, levelStr[level], prog, func, line, message)+1;
    buf[len-1]='\0';
    sendto(server, buf, BUF_LEN, 0, (struct sockaddr*) &addr, sizeof(addr));

    
}

void ExitLog() {
    //  will stop the receive thread via an is_running flag and close the file descriptor.
    is_running = false;
    close(server);
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
    is_running = true;
    //Set a socket timeout of 1 seconds
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    socklen_t addrlen = sizeof(addr);
    memset(&buf, 0, sizeof(buf));
    while (is_running){
        ret = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr *)&addr, &addrlen);
        // TODO: ERROR CHECKING
        if(ret > 0) {
            LOG_LEVEL new_level;
            if(strncmp(DEBUG_CMD, buf, 21) == 0) new_level = DEBUG;
            if(strncmp(WARNING_CMD, buf, 23) == 0) new_level = WARNING;
            if(strncmp(ERROR_CMD, buf, 21) == 0) new_level = ERROR;
            if(strncmp(CRITICAL_CMD, buf, 24) == 0) new_level = CRITICAL;
            pthread_mutex_lock(&lock_x);
            LEVEL = new_level;
            pthread_mutex_unlock(&lock_x);
            memset(&buf, 0, sizeof(buf));
        }
    }
    
}
