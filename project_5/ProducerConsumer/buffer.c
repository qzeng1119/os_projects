/**
 * Implementation of the bounded buffer.
 */

#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

// the buffer
static buffer_item buffer[BUFFER_SIZE];

// circular queue positions
static int insert_index = 0;
static int remove_index = 0;

// counts empty and full slots
static sem_t empty;
static sem_t full;

// protects access to the buffer
static pthread_mutex_t buffer_mutex;

// initialize the buffer
int buffer_init(void)
{
    insert_index = 0;
    remove_index = 0;

    if (sem_init(&empty, 0, BUFFER_SIZE) != 0) {
        return -1;
    }

    if (sem_init(&full, 0, 0) != 0) {
        sem_destroy(&empty);
        return -1;
    }

    if (pthread_mutex_init(&buffer_mutex, NULL) != 0) {
        sem_destroy(&empty);
        sem_destroy(&full);
        return -1;
    }

    return 0;
}

// destroy the buffer resources
void buffer_shutdown(void)
{
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&buffer_mutex);
}

// insert item into buffer
// return 0 if successful, otherwise
// return -1 indicating an error condition
int insert_item(buffer_item item)
{
    if (sem_wait(&empty) != 0) {
        return -1;
    }

    pthread_mutex_lock(&buffer_mutex);

    buffer[insert_index] = item;
    insert_index = (insert_index + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&buffer_mutex);

    sem_post(&full);

    return 0;
}

// remove an object from buffer
// placing it in item
// return 0 if successful, otherwise
// return -1 indicating an error condition
int remove_item(buffer_item *item)
{
    if (sem_wait(&full) != 0) {
        return -1;
    }

    pthread_mutex_lock(&buffer_mutex);

    *item = buffer[remove_index];
    remove_index = (remove_index + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&buffer_mutex);

    sem_post(&empty);

    return 0;
}
