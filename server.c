#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

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

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

void clean_string(char *str, char c)
{
    char *pr = str, *pw = str;
    while (*pr)
    {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

void print(const char *string)
{
    printf("%s\n", string);
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    Queue q;
    initQueue(&q, 10);

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    print("Creating socket ...");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
    print("Binding socket ...");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for clients
    print("Listening ...");
    if (listen(server_fd, 8) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        int client_fd;
        pthread_t t;

        // Accept connection from a client
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        print("Connection accepted");

        // Read message from client
        read(client_fd, buffer, BUFFER_SIZE);
        clean_string(buffer, '\n');
        printf("Message received: %s\n", buffer);

        if (strcmp(buffer, "produce") == 0)
        {
            pthread_create(&t, NULL, producer, (void *)&q);
        }
        else if (strcmp(buffer, "consume") == 0)
        {
            pthread_create(&t, NULL, consumer, (void *)&q);
        }

        close(client_fd);
    }

    close(server_fd);

    destroyQueue(&q);

    return 0;
}
