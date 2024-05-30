#include "../include/consumer.h"
#include <netinet/in.h>

void *consumer(void *param)
{
    DequeueRequest *req = (DequeueRequest *)param;
    Message msg = dequeue(req->queue);
    if (msg.data)
    {
        debug(__FILE__, __LINE__, "Consumed: %s", (char *)msg.data);
        send(*req->client_fd, msg.data, strlen(msg.data), 0);
        free(msg.data);
    }
    free(req);
    return NULL;
}
