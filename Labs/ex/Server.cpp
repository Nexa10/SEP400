#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

int main()
{
    const char *socket_file = "/tmp/lab4";

    // Create a socket
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::perror("socket");
        std::exit(EXIT_FAILURE);
    }

    // Bind the socket to a file
    struct sockaddr_un address;
    std::memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    std::strncpy(address.sun_path, socket_file, sizeof(address.sun_path) - 1);
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        std::perror("bind");
        std::exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == -1)
    {
        std::perror("listen");
        std::exit(EXIT_FAILURE);
    }

    // Wait for a client to connect
    std::cout << "Waiting for client connection..." << std::endl;
    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1)
    {
        std::perror("accept");
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Client connected." << std::endl;

    // Send the "Pid" command to the client
    if (send(client_socket, "Pid", 3, 0) == -1)
    {
        std::perror("send");
        std::exit(EXIT_FAILURE);
    }

    // Receive the response from the client
    char pid_response[1024];
    ssize_t num_bytes = recv(client_socket, pid_response, sizeof(pid_response) - 1, 0);
    if (num_bytes == -1)
    {
        std::perror("recv");
        std::exit(EXIT_FAILURE);
    }
    pid_response[num_bytes] = '\0';
    std::cout << pid_response << std::endl;

    // Send the "Sleep" command to the client
    if (send(client_socket, "Sleep", 5, 0) == -1)
    {
        std::perror("send");
        std::exit(EXIT_FAILURE);
    }

    // Wait for the client to finish sleeping
    char done_response[1024];
    while (true)
    {
        num_bytes = recv(client_socket, done_response, sizeof(done_response) - 1, 0);
        if (num_bytes == -1)
        {
            std::perror("recv");
            std::exit(EXIT_FAILURE);
        }
        done_response[num_bytes] = '\0';
        if (std::strcmp(done_response, "Done") == 0)
        {
            break;
        }
    }

    // Send the "Quit" command to the client
    if (send(client_socket, "Quit", 4, 0) == -1)
    {
        std::perror("send");
        std::exit(EXIT_FAILURE);
    }

    // Close the connection to the client
    close(client_socket);

    // Clean up
    close(server_socket);
    std::remove(socket_file);

    return EXIT_SUCCESS;
}
