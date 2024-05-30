#include "../include/consumer.h"

void *consumer(void *param)
{
    DequeueRequest *req = (DequeueRequest *)param;
    Message msg = dequeue(req->queue);
    if (msg.data)
    {
        debug(__FILE__, __LINE__, "Consumed: %s", (char *)msg.data);
        free(msg.data);
    }
    free(req);
    return NULL;
}
