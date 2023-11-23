/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.c
 */

#include <pthread.h>
#include "device.h"
#include "logfs.h"
#include <unistd.h>
#include "system.h"

#define WCACHE_BLOCKS 32
#define RCACHE_BLOCKS 256

/**
 * Needs:
 *   pthread_create()
 *   pthread_join()
 *   pthread_mutex_init()
 *   pthread_mutex_destroy()
 *   pthread_mutex_lock()
 *   pthread_mutex_unlock()
 *   pthread_cond_init()
 *   pthread_cond_destroy()
 *   pthread_cond_wait()
 *   pthread_cond_signal()
 */

/* research the above Needed API and design accordingly */


#define BLOCK_SIZE 4096
int remaining = 0;

struct logfs
{
    pthread_mutex_t mutex;
    struct device *device;
    pthread_cond_t cond;
    pthread_t consumer;
    char* queue;
    int utilized;
    int head;
    int tail;
    int terminate;
};

void *threadFunction(void *arg)
{
    struct logfs *logfs = (struct logfs *)arg;
    while(1){
        if(logfs->terminate==1)break;

        while(logfs->tail - logfs->head < BLOCK_SIZE && logfs->terminate==0){
            pthread_cond_wait(&logfs->cond, &logfs->mutex);
        }
        printf("Writer Thread Begins\n");
        if(logfs->tail>0 && logfs->tail - logfs->head >= BLOCK_SIZE){
            printf("Inside the write loop and data len to be written=%d\n",logfs->tail - logfs->head);
            char* buf;
            buf = (char*)malloc(sizeof(char)*BLOCK_SIZE);
            memcpy(buf, logfs->queue + logfs->head, BLOCK_SIZE);
            logfs->head += BLOCK_SIZE;
            device_write(logfs->device, buf, logfs->utilized, BLOCK_SIZE);
            us_sleep(1000000);
            printf("Data Written\n");
            logfs->utilized += BLOCK_SIZE;
        }
    }
    return 0;
}

struct logfs *logfs_open(const char *pathname)
{
    struct logfs *logfs = (struct logfs *)malloc(sizeof(struct logfs));
    logfs->queue = (char *)malloc(sizeof(char) * WCACHE_BLOCKS * BLOCK_SIZE);
    logfs->device = device_open(pathname);
    pthread_mutex_init(&logfs->mutex, NULL);
    pthread_cond_init(&logfs->cond, NULL);
    pthread_create(&logfs->consumer, NULL, threadFunction, logfs);
    printf("Thread Created\n");
    logfs->terminate = 0;
    logfs->utilized = 0;
    logfs->head = 0;
    logfs->tail = 0;
    return logfs;
}

void logfs_close(struct logfs *logfs)
{
    logfs->terminate = 1;
    pthread_cond_signal(&logfs->cond);
    printf("File Data Written = %d and Queue size %d\n", logfs->utilized, logfs->tail);
    pthread_join(logfs->consumer, NULL);
    pthread_cond_destroy(&logfs->cond);
    pthread_mutex_destroy(&logfs->mutex);
    free(logfs);
}

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len)
{
    pthread_mutex_lock(&logfs->mutex);
    remaining = 1;
    device_read(logfs->device, buf, 0, BLOCK_SIZE);
    printf("Buffer read from disk:  %s\n",(char*)buf);
    pthread_cond_signal(&logfs->cond);
    pthread_mutex_unlock(&logfs->mutex);
    return 0;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len)
{
    pthread_mutex_lock(&logfs->mutex);
    memcpy(logfs->queue + logfs->tail, buf, len);
    logfs->tail += len;
    printf("Appending done , now Signalling---------->\n");
    pthread_cond_signal(&logfs->cond);
    us_sleep(1000000);
    pthread_mutex_unlock(&logfs->mutex);
    return 0;
}
