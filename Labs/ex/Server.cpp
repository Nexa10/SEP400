#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;

int main()
{
    char socket_file[] = "/tmp/lab4";
    char buf[256];
    struct sockaddr_un server;

    memset(&server, 0, sizeof(server));
    // Create a socket
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1){
        perror("socket");
        exit(1);
    }
 
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, socket_file, sizeof(server.sun_path) - 1);
    unlink(socket_file);

    // Bind the socket to a file
    if (bind(server, (struct sockaddr *)&server, sizeof(server)) == -1){
        perror("bind");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(fd, 5) == -1){
        perror("listen");
        exit(1);
    }

    // Wait for a client to connect
    cout << "Waiting for client connection..." << endl;
    int cl = accept(fd, nullptr, nullptr);
    if (cl == -1){
        perror("accept");
        exit(1);
    }
    else{
        cout << "Client connected." << endl;
        cout << "Server: accept()" << endl;
    }
    
    char cmd [][64] = {"Pid", "sleep", "done"};
    for (int i = 0; i < sizeof(cmd); i++){
        // Send the "Pid" command to the client
        if (send(cl, cmd[i], sizeof(cmd), 0) == -1){
            perror("send");
            unlink(socket_file);
            close(fd);
            exit(1);
        }
        else{
            cout << "Request Sent" << endl;
        }

        memset(&buf, 0, sizeof(buf));
        if(read(cl, buf, sizeof(buf)-1) == -1){
            perror("send");
            unlink(socket_file);
            close(fd);
            exit(1);
        }
        
        if(strncmp(cmd[i], "Pid", 3) == 0){
            cout << "Server: " << buf << endl;
        }
    }
    
    unlink(socket_file);
    // Close the connection to the client
    close(cl);
    // Clean up
    close(fd);

    return 0;
}
