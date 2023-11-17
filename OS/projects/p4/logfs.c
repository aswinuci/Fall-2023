#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "device.h"
#include "logfs.h"

#define MAX_QUEUE_SIZE 1024

struct logfs {
    struct device *device;
    void *queue[MAX_QUEUE_SIZE];
    int array_index; // Array index pointer
    pthread_mutex_t mutex;
    const void *buf; // Pointer to data to be appended
    uint64_t buf_len; // Length of the data to be appended
};

void print_queue_elements(struct logfs *logfs) {
    printf("Queue elements: ");
    for (int i = 0; i < logfs->array_index; i++) {
        const char *element = (const char *)(logfs->queue[i]);
        printf("%c ", *element);
    }
    printf("\n");
}

void* append_thread(void *args) {
    struct logfs *logfs = (struct logfs *)args;
    
    while (1) {
        pthread_mutex_lock(&logfs->mutex);
        const char *buf = (const char *)logfs->buf;
        uint64_t len = logfs->buf_len;
        
        for (uint64_t i = 0; i < len && logfs->array_index < MAX_QUEUE_SIZE; i++) {
            logfs->queue[logfs->array_index] = (void *)&buf[i];
            logfs->array_index++;

            // Check if array_index is divisible by 10
            if (logfs->array_index % 10 == 0) {
                print_queue_elements(logfs);
            }
        }
        pthread_mutex_unlock(&logfs->mutex);
    }
    return NULL;
}

void* monitor_index(void *args) {
    struct logfs *logfs = (struct logfs *)args;

    while (1) {
        pthread_mutex_lock(&logfs->mutex);
        if (logfs->array_index % 10 == 0) {
            print_queue_elements(logfs);
        }
        pthread_mutex_unlock(&logfs->mutex);
    }
    return NULL;
}

struct logfs *logfs_open(const char *pathname) {
    struct logfs *new_logfs = (struct logfs *)malloc(sizeof(struct logfs));
    if (new_logfs == NULL) {
        return NULL;
    }
    
    new_logfs->device = device_open(pathname);
    memset(new_logfs->queue, 0, MAX_QUEUE_SIZE * sizeof(void *));
    new_logfs->array_index = 0;
    pthread_mutex_init(&new_logfs->mutex, NULL);
    new_logfs->buf = NULL;
    new_logfs->buf_len = 0;

    pthread_t append_tid, monitor_tid;
    pthread_create(&append_tid, NULL, append_thread, (void *)new_logfs);
    pthread_create(&monitor_tid, NULL, monitor_index, (void *)new_logfs);

    return new_logfs;
}

void logfs_close(struct logfs *logfs){
    device_close(logfs->device);
    free(logfs);
}

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len){
    return 0;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len){
    pthread_mutex_lock(&logfs->mutex);
    logfs->buf = buf;
    logfs->buf_len = len;
    pthread_mutex_unlock(&logfs->mutex);
    return 0;
}
