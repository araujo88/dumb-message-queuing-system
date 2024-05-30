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

#define BUFFER_SIZE 1024

void *producer(void *param);
void *consumer(void *param);

void *producer(void *param)
{
    Queue *q = (Queue *)param;
    char *data = malloc(20);
    sprintf(data, "Message %d", randint(0, 69));
    Message msg = {data, strlen(data)};
    printf("Produced: %s\n", (char *)msg.data);
    enqueue(q, msg);
    return NULL;
}

void *consumer(void *param)
{
    Queue *q = (Queue *)param;
    Message msg = dequeue(q);
    if (msg.data)
    {
        printf("Consumed: %s\n", (char *)msg.data);
        free(msg.data);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "error: please provide port number as arg\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    Queue q;
    initQueue(&q, 10);
    thread_pool_t *producer_pool = thread_pool_create(4, 4); // 4 threads, 4 queue size.
    thread_pool_t *consumer_pool = thread_pool_create(4, 4); // 4 threads, 4 queue size.

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    debug("Creating socket ...");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
    debug("Binding socket ...");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for clients
    debug("Listening ...");
    if (listen(server_fd, 8) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        int client_fd;

        // Accept connection from a client
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        debug("Connection accepted");

        // Read message from client
        read(client_fd, buffer, BUFFER_SIZE);
        clean_string(buffer, '\n');
        printf("Message received: %s\n", buffer);

        if (strcmp(buffer, "produce") == 0)
        {
            thread_pool_add_task(producer_pool, producer, (void *)&q);
        }
        else if (strcmp(buffer, "consume") == 0)
        {
            thread_pool_add_task(consumer_pool, consumer, (void *)&q);
        }

        close(client_fd);
    }

    close(server_fd);
    thread_pool_cleanup(producer_pool);
    thread_pool_cleanup(consumer_pool);
    destroyQueue(&q);

    return 0;
}
