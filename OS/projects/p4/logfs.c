#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "device.h"
#include "logfs.h"

#define MAX_QUEUE_SIZE 1024
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_t producer_thread, consumer_thread;
int array_index=0;

struct logfs {
    struct device *device;
    char *queue[MAX_QUEUE_SIZE];
    const void *buf;
    uint64_t buf_len;
};

void *consumer(void *arg) {
    printf("Consumed called with array_index: %d\n", array_index);
    struct logfs *logfs = (struct logfs *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (array_index > 0) {
            printf("Consumed array: ");
            int start = 0;
            for (int i = start; i < array_index; ++i) {
                printf("%c ", *logfs->queue[i]);
                // free(logfs->queue[i]); // Freeing memory after consumption
                // logfs->queue[i] = NULL;
            }
            printf("\n");

            array_index = 0; // Reset the array index after consuming
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}


int producer(struct logfs *logfs, void* data, uint64_t len){
    char* input_data = (char*)data;
    int data_length = (int)len;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < data_length; ++i) {
        logfs->queue[array_index] = (char*)malloc(sizeof(char));
        *logfs->queue[array_index] = input_data[i];
        printf("Produced: %c at index %d\n", *logfs->queue[array_index], array_index);
        array_index++;

        if (array_index % 5 == 0) {
            pthread_cond_signal(&condition);
            us_sleep(1000000);
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len){
    return producer(logfs, (void *)buf, len);
}

struct logfs *logfs_open(const char *pathname) {
    struct logfs *new_logfs = (struct logfs *)malloc(sizeof(struct logfs));
    if (new_logfs == NULL) {
        return NULL;
    }
    
    new_logfs->device = device_open(pathname);
    memset(new_logfs->queue, 0, MAX_QUEUE_SIZE * sizeof(void *));
    new_logfs->buf = NULL;
    new_logfs->buf_len = 0;

    pthread_create(&producer_thread, NULL, (void *(*)(void *))producer, new_logfs); // Creating the producer thread
    pthread_create(&consumer_thread, NULL, consumer, new_logfs); // Creating the consumer thread

    return new_logfs;
}

void logfs_close(struct logfs *logfs){
    device_close(logfs->device);
    free(logfs);
}

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len){
    return 0;
}
