/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

typedef struct node
{
    task work;
    struct node *next;
}
task_node;

// linked-list work queue
static task_node *queue_head = NULL;
static task_node *queue_tail = NULL;

// protects access to the queue
static pthread_mutex_t queue_mutex;

// counts how many tasks are available
static sem_t task_available;

// worker threads
static pthread_t bees[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    task_node *new_node = malloc(sizeof(task_node));

    if (new_node == NULL) {
        return 1;
    }

    new_node->work = t;
    new_node->next = NULL;

    pthread_mutex_lock(&queue_mutex);

    if (queue_tail == NULL) {
        queue_head = new_node;
        queue_tail = new_node;
    } else {
        queue_tail->next = new_node;
        queue_tail = new_node;
    }

    pthread_mutex_unlock(&queue_mutex);

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    task empty_task;
    task_node *old_head;
    task dequeued_task;

    empty_task.function = NULL;
    empty_task.data = NULL;

    pthread_mutex_lock(&queue_mutex);

    if (queue_head == NULL) {
        pthread_mutex_unlock(&queue_mutex);
        return empty_task;
    }

    old_head = queue_head;
    dequeued_task = old_head->work;
    queue_head = old_head->next;

    if (queue_head == NULL) {
        queue_tail = NULL;
    }

    pthread_mutex_unlock(&queue_mutex);

    free(old_head);

    return dequeued_task;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    task next_task;

    (void)param;

    while (1) {
        sem_wait(&task_available);

        next_task = dequeue();
        if (next_task.function != NULL) {
            execute(next_task.function, next_task.data);
        }
    }

    return NULL;
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task new_task;
    int status;

    new_task.function = somefunction;
    new_task.data = p;

    status = enqueue(new_task);
    if (status != 0) {
        return 1;
    }

    sem_post(&task_available);

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
    int i;

    pthread_mutex_init(&queue_mutex, NULL);
    sem_init(&task_available, 0, 0);

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&bees[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    int i;
    task_node *current;
    task_node *next;

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(bees[i]);
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(bees[i], NULL);
    }

    pthread_mutex_lock(&queue_mutex);
    current = queue_head;
    queue_head = NULL;
    queue_tail = NULL;
    pthread_mutex_unlock(&queue_mutex);

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    sem_destroy(&task_available);
    pthread_mutex_destroy(&queue_mutex);
}
