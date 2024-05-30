#pragma once
#include "queue.h"
#include "helpers.h"

typedef struct
{
    int *client_fd;
    Queue *queue;
    Message msg;
} DequeueRequest;

void *consumer(void *param);
