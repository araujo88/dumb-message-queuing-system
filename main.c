#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

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
    unsigned int buffer_size;
    pthread_mutex_t lock;
    sem_t semEmpty;
    sem_t semFull;
} Queue;

void initQueue(Queue *q, unsigned int buffer_size);
void enqueue(Queue *q, Message msg);
Message dequeue(Queue *q);
void destroyQueue(Queue *q);
void *producer(void *param);
void *consumer(void *param);
int randint(int min_num, int max_num);

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
    pthread_mutex_lock(&q->lock);
    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL)
    {
        pthread_mutex_unlock(&q->lock);
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
}

Message dequeue(Queue *q)
{
    pthread_mutex_lock(&q->lock);
    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return (Message){NULL, 0}; // Return an empty message if queue is empty
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
    return msg;
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

void *producer(void *param)
{
    Queue *q = (Queue *)param;
    for (int i = 0; i < 20; i++)
    {
        sem_wait(&q->semEmpty);
        char *data = malloc(20);
        sprintf(data, "Message %d", i);
        Message msg = {data, strlen(data)};
        printf("Produced: %s\n", (char *)msg.data);
        enqueue(q, msg);
        sleep(randint(1, 2));
        sem_post(&q->semFull);
    }
    return NULL;
}

void *consumer(void *param)
{
    Queue *q = (Queue *)param;
    for (int i = 0; i < 20; i++)
    {
        sem_wait(&q->semFull);
        Message msg = dequeue(q);
        if (msg.data)
        {
            printf("Consumed: %s\n", (char *)msg.data);
            free(msg.data);
        }
        sleep(randint(5, 10));
        sem_post(&q->semEmpty);
    }
    return NULL;
}

int randint(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    }
    else
    {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

int main()
{
    srand(time(NULL));
    Queue q;
    initQueue(&q, 10);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, producer, &q);
    pthread_create(&t2, NULL, consumer, &q);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    destroyQueue(&q);

    return 0;
}
