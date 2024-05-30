#include "../include/producer.h"

void *producer(void *param)
{
    EnqueueRequest *req = (EnqueueRequest *)param;
    if (!req)
    {
        fprintf(stderr, "Producer received a null request.\n");
        return NULL; // Early return if req is NULL
    }

    if (!req->msg.data)
    {
        fprintf(stderr, "Producer received a request with null data.\n");
        free(req);   // Clean up request
        return NULL; // Early return if data is NULL
    }

    debug(__FILE__, __LINE__, "Produced: %s", (char *)req->msg.data);
    enqueue(req->queue, req->msg);
    free(req);
    return NULL;
}
