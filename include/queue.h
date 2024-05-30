#pragma once
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char *data;  // Pointer to the message data
    size_t size; // Size of the message
} Message;

typedef struct Node
{
    Message message;
    struct Node *next;
} Node;

typedef struct
{
    Node *front; // Pointer to the front of the queue
    Node *rear;  // Pointer to the rear of the queue
    int count;   // Number of items in the queue
    unsigned int capacity;
    pthread_mutex_t lock;
    sem_t semEmpty;
    sem_t semFull;
} Queue;

void initQueue(Queue *q, unsigned int buffer_size);
void enqueue(Queue *q, Message msg);
Message dequeue(Queue *q);
Message peek(Queue *q);
void showQueue(Queue *q);
void destroyQueue(Queue *q);
