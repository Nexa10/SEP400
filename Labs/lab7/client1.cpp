// client1.cpp - An exercise with named semaphores and shared memory
//
// 04-Aug-21  M. Watler         Created.

// Dennis Audu
// 148463193
// daudu@myseneca.ca

#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "client.h"
using namespace std;

const int CLIENT_NO=1;
bool is_running=true;

static void sigHandler(int sig)
{
    switch(sig) {
        case SIGINT:
            is_running=false;
	    break;
    }
}

int main(void) {
    key_t          ShmKey;
    int            ShmID;
    struct Memory  *ShmPTR;
    sem_t *sem_id;

    //Intercept ctrl-C for controlled shutdown
    struct sigaction action;
    action.sa_handler = sigHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    //TODO: Generate the key here
    ShmKey = ftok(menName, proj_id);

    //TODO: Create or get the shared memory id
    ShmID = shmget(ShmKey, sizeof(Memory), IPC_CREAT | 0666);

    //TODO: Attach a pointer to the shared memory
    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);

    //TODO:initialize named semaphore, can be used between processes
    sem_id = sem_open(semName, O_CREAT, semPerms, 0);
    
    cout << "[client " << CLIENT_NO << "] waiting.." << endl;
    for(int i=0; i<NUM_MESSAGES && is_running; ++i) {
        //TODO: Synchronize processes with semaphores
        sem_wait(sem_id);
	if(ShmPTR->destClientNo==CLIENT_NO) {
            cout<<"Client "<<CLIENT_NO<<" has received a message from client "<<ShmPTR->srcClientNo<<":"<<endl;
	    cout<<ShmPTR->message<<endl;
	    //Send a message to client 2 or 3
	    ShmPTR->srcClientNo=CLIENT_NO;
	    ShmPTR->destClientNo=2+i%2;//send a message to client 2 or 3
	    memset(ShmPTR->message, 0, BUF_LEN);
	    sprintf(ShmPTR->message, "This is message %d from client %d\n", i+1, CLIENT_NO);
        }
        sem_post(sem_id);
        sleep(1);
    }
    
    sem_close(sem_id);
    sem_unlink(semName);

    shmdt((void *)ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);

    cout<<"client1: DONE"<<endl;

    return 0;
}
