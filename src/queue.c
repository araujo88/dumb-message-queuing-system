#include "../include/queue.h"

void initQueue(Queue *q, unsigned int buffer_size)
{
    q->front = q->rear = NULL;
    q->count = 0;
    q->buffer_size = buffer_size;
    pthread_mutex_init(&q->lock, NULL);
    sem_init(&q->semEmpty, 0, q->buffer_size);
    sem_init(&q->semFull, 0, 0);
}

void enqueue(Queue *q, Message msg)
{
    sem_wait(&q->semEmpty);
    pthread_mutex_lock(&q->lock);

    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Error allocating new node\n");
        pthread_mutex_unlock(&q->lock);
        sem_post(&q->semEmpty);
        return; // Handle allocation failure
    }

    newNode->message = msg;
    newNode->next = NULL;

    if (q->rear == NULL)
    { // If queue is empty
        q->front = q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
    pthread_mutex_unlock(&q->lock);
    sem_post(&q->semFull);
}

Message dequeue(Queue *q)
{
    sem_wait(&q->semFull);
    pthread_mutex_lock(&q->lock);
    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        sem_post(&q->semFull);
        fprintf(stderr, "Queue unexpectedly empty.\n");
        return (Message){NULL, 0};
    }

    Node *temp = q->front;
    Message msg = temp->message;

    q->front = q->front->next;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
    q->count--;
    pthread_mutex_unlock(&q->lock);
    sem_post(&q->semEmpty);
    return msg;
}

Message peek(Queue *q)
{
    pthread_mutex_lock(&q->lock);

    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        fprintf(stderr, "Queue is empty.\n");
        return (Message){NULL, 0}; // Return an empty message to indicate an empty queue
    }

    Message msg = q->front->message;
    pthread_mutex_unlock(&q->lock);

    return msg; // Return a copy of the message at the front of the queue
}

void showQueue(Queue *q)
{
    pthread_mutex_lock(&q->lock); // Lock the queue to ensure thread safety

    if (q->front == NULL)
    {
        printf("The queue is empty.\n");
    }
    else
    {
        Node *current = q->front;
        printf("Queue contents:\n");
        while (current != NULL)
        {
            if (current->message.data != NULL)
            {
                printf("%s\n", (char *)current->message.data); // Assuming message data is a null-terminated string
            }
            else
            {
                printf("NULL\n"); // Handle NULL data appropriately
            }
            current = current->next;
        }
    }

    pthread_mutex_unlock(&q->lock); // Unlock the queue after traversing
}

void destroyQueue(Queue *q)
{
    while (q->front != NULL)
    {
        Node *temp = q->front;
        q->front = q->front->next;
        free(temp->message.data); // Assuming the data was dynamically allocated
        free(temp);
    }
    pthread_mutex_destroy(&q->lock);
    sem_destroy(&q->semEmpty);
    sem_destroy(&q->semFull);
}
