// client.h - Header file for shared memory
//
// 04-Aug-21  M. Watler         Created.

// Dennis Audu
// 148463193
// daudu@myseneca.ca

#ifndef CLIENT_H
#define CLIENT_H

//TODO: A memory name and a semaphore name along with semaphore permissions
const char semName[] = "sharedMsg";
const char menName[] = "sharedMem";
const mode_t semPerms=(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
int proj_id = 65;
const int BUF_LEN=1024;
const int NUM_MESSAGES=30;

struct Memory {
    int            packet_no;
    unsigned short srcClientNo;
    unsigned short destClientNo;
    char           message[BUF_LEN];
};

void *recv_func(void *arg);
#endif//CLIENT_H
