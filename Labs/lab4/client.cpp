#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void error_msg(const char *msg);
void actions(char *command);

char SOCKET_NAME[] = "/tmp/lab5";
char buffer[256] = "Client Nexa";
bool isRunning = true, ifSend = true;

int main(int argc, char *argv[])
{
    int fd, rd;
    struct sockaddr_un server_addr;
    
    memset(&server_addr, 0, sizeof(server_addr));
  
    //create
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0){
        error_msg("Error: Socket was not created.\n");
        exit(-1);
    }
    cout << "Socket created..." << endl;

    
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
  
    // connect
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        error_msg("Client couldn't connect to server!\n");
        close(fd);
        exit(-1);
    }
   	
    while (isRunning){
    cout << "Client connected!" << endl;
    
    	if (write(fd, &buffer, strlen(buffer)-1) < 0){
		    error_msg("Error: Client-Write failed!\n");
		    close(fd);
		    exit(-1);
		}
		
        if (rd = read(fd, buffer, sizeof(buffer)) < 0)
        {
            error_msg("Error: Client-read failed!\n");
        }
    	
        actions(buffer);
        
        //if(ifSend){ //do not send if asked to sleep or quit
		
        //}
        //bzero(buffer, sizeof(buffer));
    }
    
    close(fd);
    
    return 0;
}

void error_msg(const char *msg)
{
    perror(msg);
    exit(1);
}

void actions(char *msg){
       
   if (strncmp(msg, "Pid", 3) == 0){
   	bzero(buffer, sizeof(buffer));
   	char const *pid = to_string(getpid()).c_str(); //converts pid to char array
   	strncpy(buffer, "This client has pid: ", sizeof(buffer));
        strcat(buffer, pid); //concatenate buffer and pid 
        cout << "sent Pid" << endl;
        }
        
   else if(strncmp(msg, "Sleep", 5) == 0){
       	printf("This client is going to sleep for 5 seconds\n");
        sleep(5);
        ifSend = false;
        cout << "slept" << endl;
        }
        
   else if(strncmp(msg, "Done", 4) == 0){
       printf("This client is quitting\n");
       isRunning = false;
      }
        
   else{
         cout << "None" << endl;
    }
}
