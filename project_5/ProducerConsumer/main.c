/**
 * Producer-consumer solution using semaphores.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "buffer.h"

typedef struct
{
    int id;
}
thread_arg;

// protects calls to rand()
static pthread_mutex_t rand_mutex = PTHREAD_MUTEX_INITIALIZER;

// indicates whether worker threads should keep running
static volatile bool running = true;

// generate a random number in the range [0, limit)
static int random_number(int limit)
{
    int value;

    pthread_mutex_lock(&rand_mutex);
    value = rand() % limit;
    pthread_mutex_unlock(&rand_mutex);

    return value;
}

void *producer(void *param)
{
    thread_arg *arg;
    buffer_item item;

    arg = (thread_arg *)param;

    while (running) {
        sleep((unsigned int)(random_number(3) + 1));

        if (!running) {
            break;
        }

        item = random_number(1000);

        if (insert_item(item) != 0) {
            fprintf(stderr, "producer %d failed to insert item\n", arg->id);
        } else {
            printf("producer %d produced %d\n", arg->id, item);
        }
    }

    return NULL;
}

void *consumer(void *param)
{
    thread_arg *arg;
    buffer_item item;

    arg = (thread_arg *)param;

    while (running) {
        sleep((unsigned int)(random_number(3) + 1));

        if (!running) {
            break;
        }

        if (remove_item(&item) != 0) {
            fprintf(stderr, "consumer %d failed to remove item\n", arg->id);
        } else {
            printf("consumer %d consumed %d\n", arg->id, item);
        }
    }

    return NULL;
}

// validate a positive integer command line argument
static int parse_argument(char *text, char *name)
{
    int value;

    value = atoi(text);
    if (value <= 0) {
        fprintf(stderr, "%s must be a positive integer\n", name);
        exit(1);
    }

    return value;
}

int main(int argc, char *argv[])
{
    int sleep_time;
    int producer_count;
    int consumer_count;
    pthread_t *producer_threads;
    pthread_t *consumer_threads;
    thread_arg *producer_args;
    thread_arg *consumer_args;
    int i;

    if (argc != 4) {
        fprintf(stderr, "usage: %s <sleep time> <producers> <consumers>\n", argv[0]);
        return 1;
    }

    sleep_time = parse_argument(argv[1], "sleep time");
    producer_count = parse_argument(argv[2], "number of producers");
    consumer_count = parse_argument(argv[3], "number of consumers");

    srand((unsigned int)time(NULL));

    if (buffer_init() != 0) {
        fprintf(stderr, "failed to initialize buffer\n");
        return 1;
    }

    producer_threads = malloc(sizeof(pthread_t) * producer_count);
    consumer_threads = malloc(sizeof(pthread_t) * consumer_count);
    producer_args = malloc(sizeof(thread_arg) * producer_count);
    consumer_args = malloc(sizeof(thread_arg) * consumer_count);

    if (producer_threads == NULL || consumer_threads == NULL ||
        producer_args == NULL || consumer_args == NULL) {
        fprintf(stderr, "failed to allocate thread resources\n");
        free(producer_threads);
        free(consumer_threads);
        free(producer_args);
        free(consumer_args);
        buffer_shutdown();
        return 1;
    }

    for (i = 0; i < producer_count; i++) {
        producer_args[i].id = i + 1;
        pthread_create(&producer_threads[i], NULL, producer, &producer_args[i]);
    }

    for (i = 0; i < consumer_count; i++) {
        consumer_args[i].id = i + 1;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_args[i]);
    }

    sleep((unsigned int)sleep_time);

    running = false;

    for (i = 0; i < producer_count; i++) {
        pthread_cancel(producer_threads[i]);
    }

    for (i = 0; i < consumer_count; i++) {
        pthread_cancel(consumer_threads[i]);
    }

    for (i = 0; i < producer_count; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    for (i = 0; i < consumer_count; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    free(producer_threads);
    free(consumer_threads);
    free(producer_args);
    free(consumer_args);

    buffer_shutdown();

    return 0;
}
