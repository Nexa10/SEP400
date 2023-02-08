#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/lab4"
struct functions{int fd, Bind, Listen, Client, Read, Send;};

void error_msg(const char *msg);

int main()
{
    char buffer[256];
    struct sockaddr_un server_addr, client_addr;
    int size_serv = sizeof(server_addr);
    int size_client = sizeof(client_addr);
    struct functions server;

    // create sockets
    server.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server.fd < 0)
    {
        error_msg("Error: Socket was not created.\n");
    }
    else
    {
        printf("Socket created...\n");
    }
    bzero((char *)&server_addr, size_serv);

    // initialize socket struct
    server_addr.sun_family = AF_UNIX;
    // Copy path to sun_path
    strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_NAME);

    // bind socket with portnumber
    server.Bind = bind(fd, (struct sockaddr *)&server_addr, size_serv);
    if (server.Bind < 0){
        error_msg("Error: Server could not bind\n");
    }
        printf("Socket Binded on: %s\n", server_addr.sun_path);


    // Listen
    server.Listen = listen(server.fd, 5);
    if (server.Listen < 0){
        error_msg("Error: Server is not listening!\n")
    }
        printf("Waiting for the client...\n");
        sleep(1);
    

    // accept connection
    server.Client = accept(server.fd, sockaddr_un * client_addr, size_client);
    if (server.Accept < 0){
        error_msg("Error: Server did not connect")
    }
        printf("Server: accept()\n");
	sleep(1);

    // send
    char msg[] = "Hello\n";
    send(server.fd, msg, sizeof(msg), 0);
    prinf("Sending to client: %s....", client_addr.sun_path)

        // Read
        server.Read = read(server.Client, buffer, sizeof(buffer) - 1);
    if (server.Read < 0)
    {
        error_msg("Error: Failed to read");
    }
    memset(buffer, '\0', sizeof(buffer));
    printf("[Client]: %s\n", buffer);

    return 0;
}

void error_msg(const char *msg)
{
    perror(msg);
    exit(1);
}
