#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;

int main() {
    const char* socket_file = "/tmp/lab4";
    struct sockaddr_un address;
    char command[1024];

    memset(&address, 0, sizeof(address));

    // Create a socket
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    // Connect to the server
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_file, sizeof(address.sun_path) - 1);
    if (connect(client_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("connect");
        exit(1);
    }

    while(true){
        memset(&command, 0, sizeof(command));
        // Receive a command from the server
        int rd = read(client_socket, command, sizeof(command) - 1);
        if (rd == -1) {
            perror("recv");
            exit(1);
        }

        // Handle the "Pid" command
        if (strncmp(command, "Pid", 3) == 0) {
        	cout << "A request for the client's pid has been recieved" << endl;
            memset(&command, 0, sizeof(command)); 
            int pid = getpid();
            snprintf(command, sizeof(command), "This client has pid %d", pid);
            if (send(client_socket, command, strlen(command), 0) == -1) {
                perror("send");
                exit(1);
            }
        }
        // Handle the "Sleep" command
        else if (strncmp(command, "Sleep", 5) == 0) {
            cout << "This client is going to sleep for 5 seconds" << endl;
            sleep(5);
            if (send(client_socket, "Done", 4, 0) == -1) {
                perror("send");
                exit(1);
            }
        }
        // Handle the "Quit" command
        else if (strncmp(command, "Done", 4) == 0) {
            cout << "This Client is quitting" << endl;
            break;
        }
        // Handle an unknown command
        else{
            cerr << "Unknown command: " << command << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    // Clean up
    close(client_socket);
    return 0;
}
