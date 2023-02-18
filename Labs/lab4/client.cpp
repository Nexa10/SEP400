#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void error_msg(const char *msg);
char SOCKET_NAME[] = "/tmp/lab5";

int main(int argc, char *argv[])
{
    int fd, rd;
    char buffer[256];
    bool isRunning = true;
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
        bzero(buffer, sizeof(buffer));
		// Read
        if (rd = read(fd, buffer, sizeof(buffer)) < 0){
            error_msg("Error: Client-read failed!\n");
        }
    	
        bzero(buffer, sizeof(buffer));
        if (strncmp(buffer, "Pid", 3) == 0){
            char const *pid = to_string(getpid()).c_str(); //converts pid to char array
            strncpy(buffer, "This client has pid: ", sizeof(buffer));
            strcat(buffer, pid); //concatenate buffer and pid 
            cout << "sent Pid" << endl;
        }
        else if(strncmp(buffer, "Sleep", 5) == 0){
            printf("This client is going to sleep for 5 seconds\n");
            sleep(5);
            cout << "slept" << endl;
        }
        else if(strncmp(buffer, "Done", 4) == 0){
            printf("This client is quitting\n");
            isRunning = false;
        }      
        
        // Write
        if (write(fd, &buffer, strlen(buffer)-1) < 0){
		    error_msg("Error: Client-Write failed!\n");
		    close(fd);
		    exit(-1);
		}
    }
    
    close(fd);
    return 0;
}

void error_msg(const char *msg)
{
    perror(msg);
    exit(1);
}

