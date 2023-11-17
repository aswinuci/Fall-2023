#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"
#include "logfs.h"

#define MAX_QUEUE_SIZE 1024

struct logfs {
    struct device *device;
    void* queue[MAX_QUEUE_SIZE];
    int array_index; // Array index pointer
};

void print_queue(struct logfs *logfs) {
    printf("Queue elements: ");
    for (int i = 0; i < logfs->array_index; i++) {
        const char *element = (const char *)(logfs->queue[i]);
        printf("%c ", *element);
    }
    printf("\n");
}


struct logfs *logfs_open(const char *pathname) {
    struct logfs *new_logfs = (struct logfs *)malloc(sizeof(struct logfs));
    if (new_logfs == NULL) {
        return NULL;
    }
    new_logfs->device = device_open(pathname); 
    memset(new_logfs->queue, 0, MAX_QUEUE_SIZE * sizeof(void*));
    new_logfs->array_index = 0;
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
    printf("Initial array index %d\n",logfs->array_index);
    if (logfs == NULL || buf == NULL || len == 0) {
        return -1;
    }

    const char *char_buf = (const char *)buf;
    for (uint64_t i = 0; i < len; i++) {
        if (logfs->array_index < MAX_QUEUE_SIZE) {
            logfs->queue[logfs->array_index] = (void *)&char_buf[i];
            logfs->array_index++;
        } else {
            // Queue is full
            return -1;
        }
    }
    printf("Final array index %d\n",logfs->array_index);
    print_queue(logfs);
    return 0;
}


