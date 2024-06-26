#pragma once
#include "queue.h"
#include "helpers.h"

typedef struct
{
    Queue *queue;
    Message msg;
} EnqueueRequest;

void *producer(void *param);
