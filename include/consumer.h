#pragma once
#include "queue.h"
#include "helpers.h"

typedef struct
{
    Queue *queue;
    Message msg;
} DequeueRequest;

void *consumer(void *param);
