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
struct functions
{
    int fd, bind, connect, read, write;
};
#define SOCKET_NAME "/tmp/lab4"
bool isRunning = true;
char buffer[256];

int main()
{
    struct functions client;

    struct sockaddr_un serv_addr;
    int serv_size = sizeof(serv_addr);

    client.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client.fd < 0)
    {
        error_msg("Error: Socket was not created.\n")
    }
    else
    {
        printf("Socket created...\n");
    }

    bzero((char *)&serv_addr, serv_size);
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, SOCKET_NAME, sizeof(client_addr.sun_path) - 1);
    unlink(SOCKET_NAME);

    client.bind = bind(client.fd, (struct sockaddr *)&serv_addr, serv_size);
    if (client.bind < 0)
    {
        error_msg("Error: Client Socket did not bind\n");
    }
    else
    {
        printf("Client Binded!\n");
    }

    // connect
    client.connect = connect(client.fd, (struct sockaddr *)&serv_addr, serv_size);
    if (client.connect < 0)
    {
        error_msg("Client couldn't connect to server!\n")
    }
    else
    {
        printf('Client connected!\n')
    }

    while (isRunning){
    	memset(buffer, '\0', sizeof(buffer));
    	client.read = read(client.fd, buffer, sizeof(buffer) - 1);
        if (client.read < 0)
        {
            error_msg("Eroor: Client-read failed!\n")
        }
    	
        actions(buffer);
        
        client.write = write(client.fd, buffer, strlen(buffer));
        if (client.write < 0)
        {
            error_msg("Error: Client-Write failed!\n");
        }
        
    }

    close(client.fd);

    return 0;
}

void error_msg(const char *msg)
{
    perror(msg);
    exit(1);
}

void actions(char *command)
{
    bzero(buffer, sizeof(buffer));
    switch (command){
    case "Pid":
        cout << "A request for the client's pid has been received" << endl;
        char const *pid = to_string(getpid()).c_str();
        strncpy(buffer, "This client has pid: ");
        strcat(buffer, pid);
    	break;

    case "Sleep":
        cout << "This client is going to sleep for 5 seconds" << endl;
        sleep(5);
        break;

    case "Done":
        cout << "This client is quitting" << endl;
        isRunning = false;
        break;
    }
}
