# DMQS - Dumb Message Queueing System

This project is a basic message queueing system implemented in C. It utilizes threads, semaphores, and mutexes to handle concurrent access to a message queue. The system is capable of adding messages to a queue (producer) and removing messages from the queue (consumer).

## Features

- **Thread-Safe Operations**: Uses mutexes and semaphores to ensure that the queue operations are safe in a multithreaded scenario.
- **Basic Queue Operations**: Supports enqueue, dequeue, and peek operations.
- **Thread Pool Management**: Includes a thread pool implementation to manage tasks efficiently.
- **Debugging Support**: Includes utilities for debugging and error handling.

## Components

### Queue

A FIFO (first in, first out) data structure with operations for enqueue, dequeue, and viewing the queue content.

- **Initialization**: `void initQueue(Queue *q, unsigned int buffer_size)`
- **Enqueue**: `void enqueue(Queue *q, Message msg)`
- **Dequeue**: `Message dequeue(Queue *q)`
- **Peek**: `Message peek(Queue *q)`
- **Show Queue**: `void showQueue(Queue *q)`
- **Destroy Queue**: `void destroyQueue(Queue *q)`

### Producer

Responsible for adding messages to the queue.

- **Producer Function**: `void *producer(void *param)`

### Consumer

Responsible for removing messages from the queue and processing them.

- **Consumer Function**: `void *consumer(void *param)`

### Thread Pool

Manages a pool of threads that can execute any given task.

- **Initialize Queue**: `queue_t *queue_init(int size)`
- **Push Task to Queue**: `void queue_push(queue_t *queue, task_t task)`
- **Pop Task from Queue**: `task_t queue_pop(queue_t *queue)`
- **Worker Function**: `void *thread_pool_worker(void *arg)`
- **Create Thread Pool**: `thread_pool_t *thread_pool_create(int num_threads, int queue_size)`
- **Add Task to Pool**: `void thread_pool_add_task(thread_pool_t *pool, void *(*func)(void *), void *arg)`
- **Clean Up Thread Pool**: `void thread_pool_cleanup(thread_pool_t *pool)`

### Helpers

Utility functions for debugging and miscellaneous operations.

- **Generate Random Integer**: `int randint(int min_num, int max_num)`
- **Clean String**: `void clean_string(char *str, char c)`
- **Debug Function**: `void debug(const char *file, int line, const char *format, ...)`
- **String Compare Safe**: `int strncmp_s(const char *s1, const char *s2, size_t n)`

## Build Instructions

To compile the project, you can use the provided `Makefile`. Here are the primary targets:

- `all`: Compiles the entire project.
- `clean`: Removes the compiled binaries and object files.

### Usage

1. **Compile the Project**: Run `make` to compile the project.

   ```bash
   make
   ```

2. **Clean the Project**: Run `make clean` to remove compiled files.

   ```bash
   make clean
   ```

### Running the Server

To run the server, execute the compiled binary:

```bash
./msg-server <port_number>
```

### Interacting with the Server

Compile the client:

```bash
gcc -Wall -Wextra -Wpedantic client.c -o client
```

Run specifying the IP address of the server and port number:

```bash
./client 127.0.0.1 8888
```

In the client shell, there are only 3 commands available:

- `show`: display the contents of the queue
- `push <data>`: push arbitrary data to the queue
- `pull`: pull data from the queue

## License

This project is licensed under the MIT License.
