#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

int main() {
    const char* socket_file = "/tmp/lab4";

    // Create a socket
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::perror("socket");
        std::exit(EXIT_FAILURE);
    }

    // Connect to the server
    struct sockaddr_un address;
    std::memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    std::strncpy(address.sun_path, socket_file, sizeof(address.sun_path) - 1);
    if (connect(client_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        std::perror("connect");
        std::exit(EXIT_FAILURE);
    }

    // Loop forever
    while (true) {
        // Receive a command from the server
        char command[1024];
        ssize_t num_bytes = recv(client_socket, command, sizeof(command) - 1, 0);
        if (num_bytes == -1) {
            std::perror("recv");
            std::exit(EXIT_FAILURE);
        }
        command[num_bytes] = '\0';

        // Handle the "Pid" command
        if (std::strcmp(command, "Pid") == 0) {
            int pid = getpid();
            char pid_string[1024];
            std::snprintf(pid_string, sizeof(pid_string), "This client has pid %d", pid);
            if (send(client_socket, pid_string, std::strlen(pid_string), 0) == -1) {
                std::perror("send");
                std::exit(EXIT_FAILURE);
            }
        }

        // Handle the "Sleep" command
        else if (std::strcmp(command, "Sleep") == 0) {
            std::cout << "Sleeping for 5 seconds..." << std::endl;
            std::sleep(5);
            if (send(client_socket, "Done", 4, 0) == -1) {
                std::perror("send");
                std::exit(EXIT_FAILURE);
            }
        }

        // Handle the "Quit" command
        else if (std::strcmp(command, "Quit") == 0) {
            break;
        }

        // Handle an unknown command
        else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Clean up
    close(client_socket);

    return EXIT_SUCCESS;
}
