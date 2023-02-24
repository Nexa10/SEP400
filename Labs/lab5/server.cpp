#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
using namespace std;

bool is_running;
const int BUF_LEN=4096;

int main(){
    int fd, cl;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCL_STREAM, 0);
    

    return 0;
}