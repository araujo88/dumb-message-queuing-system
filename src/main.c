#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include "../include/queue.h"
#include "../include/helpers.h"
#include "../include/threadpool.h"
#include "../include/constants.h"
#include "../include/producer.h"
#include "../include/consumer.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "error: please provide port number as arg\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    Queue q;
    initQueue(&q, MAX_CAPACITY);
    thread_pool_t *producer_pool = thread_pool_create(4, 4); // 4 threads, 4 queue size.
    thread_pool_t *consumer_pool = thread_pool_create(4, 4); // 4 threads, 4 queue size.

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    debug(__FILE__, __LINE__, "Creating socket ...");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
    debug(__FILE__, __LINE__, "Binding socket ...");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for clients
    debug(__FILE__, __LINE__, "Listening ...");
    if (listen(server_fd, 8) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int client_fd;
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            continue; // Continue accepting new connections if an accept call fails
        }
        debug(__FILE__, __LINE__, "Connection accepted");

        while (1)
        {
            char buffer[1024] = {0};
            ssize_t read_return = read(client_fd, buffer, sizeof(buffer));
            if (read_return <= 0)
            {
                if (read_return == 0)
                {
                    debug(__FILE__, __LINE__, "Client disconnected");
                }
                else
                {
                    perror("read");
                }
                break; // Exit inner loop to close this client connection
            }
            clean_string(buffer, '\n');
            debug(__FILE__, __LINE__, "Message received: %s", buffer);

            if (strncmp_s(buffer, PUSH, strlen(PUSH)) == 0)
            {
                EnqueueRequest *req = malloc(sizeof(EnqueueRequest));
                if (req == NULL)
                {
                    fprintf(stderr, "Failed to allocate memory for EnqueueRequest.\n");
                    continue; // Skip this iteration if memory allocation fails
                }
                req->queue = &q;
                req->msg.size = strlen(buffer) - strlen(PUSH);
                req->msg.data = strdup(buffer + strlen(PUSH)); // Duplicate the string part of the message
                thread_pool_add_task(producer_pool, producer, (void *)req);
                debug(__FILE__, __LINE__, "Task to create message added to producer pool.");
            }
            else if (strncmp_s(buffer, PULL, strlen(PULL)) == 0)
            {
                DequeueRequest *req = malloc(sizeof(DequeueRequest));
                if (req == NULL)
                {
                    fprintf(stderr, "Failed to allocate memory for DequeueRequest.\n");
                    continue; // Skip this iteration if memory allocation fails
                }
                req->queue = &q;
                req->client_fd = &client_fd;
                thread_pool_add_task(consumer_pool, consumer, (void *)req);
                debug(__FILE__, __LINE__, "Task to retrieve message added to consumer pool.");
            }
            else if (strncmp_s(buffer, SHOW, strlen(SHOW)) == 0)
            {
                showQueue(&q);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    thread_pool_cleanup(producer_pool);
    thread_pool_cleanup(consumer_pool);
    destroyQueue(&q);

    return 0;
}
