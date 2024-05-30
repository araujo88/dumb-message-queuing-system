#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // close socket
#include <errno.h>   // error codes
#include <stdbool.h> // boolean types
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <signal.h> // for interrupt signal handler
#include <sys/select.h>

#define BUFFER_SIZE 65536

int client_socket; // Client socket global variable for signal handler

void check_socket(int socket)
{
    if (socket < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    puts("Socket created successfully!");
}

void check_connection(int socket, struct sockaddr_in address)
{
    if (connect(socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }
    puts("Connected successfully!");
}

void send_message(int socket)
{
    char message[BUFFER_SIZE] = {0};
    printf("dmqs$ ");
    fgets(message, BUFFER_SIZE, stdin);
    send(socket, message, strlen(message), 0); // sends message
}

void recv_message(int socket_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    struct timeval tv;
    tv.tv_sec = 5;  // Set timeout (seconds)
    tv.tv_usec = 0; // Set timeout (microseconds)

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket_fd, &readfds);

    // Check if there is something to read within 5 seconds
    int activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);

    if (activity < 0)
    {
        perror("select error");
    }
    else if (activity == 0)
    {
        printf("recv timeout occurred. No data received.\n");
    }
    else
    {
        if (FD_ISSET(socket_fd, &readfds))
        {
            int num_bytes = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
            if (num_bytes > 0)
            {
                printf("%s\n", buffer);
            }
            else if (num_bytes == 0)
            {
                printf("Server closed the connection.\n");
                return; // Exit the function if server closed the connection
            }
            else
            {
                perror("recv failed");
                return; // Exit the function if an error occurred
            }
        }
    }
}

void handle_signal(int sig)
{
    printf("\nCaught interrupt signal %d\n", sig);
    if (close(client_socket) == 0)
    {
        puts("Socket closed successfully!");
        exit(EXIT_SUCCESS);
    }
    else
    {
        perror("An error occurred while closing the socket");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error - Usage: %s <IP address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    printf("IP address: %s - port: %d\n", ip, port);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    check_socket(client_socket);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    check_connection(client_socket, server_address);

    signal(SIGINT, handle_signal);

    while (true)
    {
        send_message(client_socket);
        recv_message(client_socket);
    }

    return 0;
}
