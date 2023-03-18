//Dennis Audu
//148463193
//daudu@myseneca.ca

#include <errno.h> 
#include <iostream> 
#include <queue>
#include <signal.h> 
#include <string.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h>
#include <pthread.h>
#include "client.h"

using namespace std;

bool is_running = true;
queue<Message> message;
pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

void* recvFunc(void* arg);
void sigHandler(int sig);

int main(){
	int msgid;
	key_t key;
	pthread_t tid;
	Message send_Msg;
	struct sigaction sa;

	sa.sa_handler = sigHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	key = ftok("serverclient", 65);
	msgid = msgget(key, 0666 | IPC_CREAT); 

	if((pthread_create(&tid, NULL, recvFunc, &msgid)) < 0){
		is_running = false;
		cout<<strerror(errno)<<endl;
		return -1;
	}
	bool received;
	while(is_running){
		received = false;
		while(message.size() > 0){
			pthread_mutex_lock(&lock_x);
			send_Msg = message.front();
			message.pop();
			pthread_mutex_unlock(&lock_x);
			received = true;
		}
		
		if(received == true){
			send_Msg.mtype = send_Msg.msgBuf.dest;
			msgsnd(msgid, &send_Msg, sizeof(send_Msg), 0);
			cout << "client " << send_Msg.msgBuf.source << " -> " << send_Msg.msgBuf.dest << endl;
			sleep(1);
		}
	}
	
	Message msg;
	for(long i = 0; i < 3; i++){
		msg.mtype = i+1;
		sprintf(msg.msgBuf.buf, "Quit");
		msgsnd(msgid, &msg, sizeof(msg), 0);
	}

	pthread_join(tid, NULL);
	msgctl(msgid, IPC_RMID, NULL); 
	return 0;
}

void* recvFunc(void* arg){
	int msgid = *(int *)arg;
	int ret;
	while(is_running){
		Message recv_Msg;
		ret = msgrcv(msgid, &recv_Msg, sizeof(recv_Msg), 4, IPC_NOWAIT);
		if(ret < 0) {
            			
			sleep(1);
		} 
		else{
			pthread_mutex_lock(&lock_x);
			message.push(recv_Msg);	
			pthread_mutex_unlock(&lock_x);
		}
	}
	pthread_exit(NULL);
}

void sigHandler(int sig){
	if(sig == SIGINT) is_running = false;
}
