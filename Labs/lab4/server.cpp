#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
using namespace std;

char SOCKET_NAME[] = "/tmp/lab5";
void error_msg(const char *msg);

int main(int argc, char *argv[])
{
    char buffer[256];
    struct sockaddr_un server_addr;
    int fd, client, rd, wr;
    bool isRunning = true;
    
    // create sockets
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        error_msg("Error: Socket was not created.\n");
    }
 
   cout << "Socket created..." << endl;
  
    bzero(&server_addr, sizeof(server_addr));

    // initialize socket struct
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
  
    unlink(SOCKET_NAME);

    // bind socket with file
    if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        error_msg("Error: Server could not bind\n");
    }
    else cout << "Socket Binded on: " << server_addr.sun_path << endl;
    
    // Listen
	if (listen(fd, 5) < 0){
	error_msg("Error: Server is not listening!\n");
	unlink(SOCKET_NAME);
	close(fd);
	exit(-1);
	}
	else{
		cout << "Client Connected to the server" <<endl;
	}

    // accept connection   
    while(isRunning){
		cout << "Server: accept()" << endl;
	    if (client = accept(fd, NULL, NULL) < 0){
		error_msg("Error: Server accept()\n");
		unlink(SOCKET_NAME);
		close(fd);
		exit(-1);
	    }
	}
	
	    // send
	    char msg[][64]= {"Pid", "Sleep", "Done"};
	    for (int i = 0; i < 3; i++){
			// Write
	    	if(wr = write(client, msg[i], strlen(msg[i])) < 0){
	    		error_msg("Error: Server-write failed\n");
	    		unlink(SOCKET_NAME);
				close(fd);
	    	}
			else{
				cout << endl << "Sending to client:...." << endl;
			}
			printout(msg[i]);
	    	
	    	sleep(5);

			// Read
			bzero(buffer, sizeof(buffer));
	    	if (rd = read(client, buffer, sizeof(buffer)-1) < 0){
			error_msg("Error: Failed to read");
			unlink(SOCKET_NAME);
			close(fd);
	    	}
			cout << "[Client]: " << buffer << endl;
			
	    }
	        
	cout << "Server Shutting Down" << endl;
	unlink(SOCKET_NAME);
	close(fd);
	close(client);
        
    return 0;
}

void error_msg(const char *msg)
{
    perror(msg);
    exit(1);
}

void printout(const char str[]){
	if (strncmp(str, "Pid", 3) == 0){
            
            cout << "The server requests the client pid" << endl;
        }
        else if(strncmp(str, "Sleep", 5) == 0){

            cout << "The server requests the client to sleep" << endl;
        }
        else if(strncmp(str, "Done", 4) == 0){
            cout << "The server requests the client to quit" << endl;
        } 
}