/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.c
 */

#include <pthread.h>
#include <inttypes.h>
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

struct logfs
{
    struct device *device;
    char *queue;
    pthread_t consumer;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    uint64_t utilized;
    uint64_t head;
    uint64_t tail;
    uint64_t queueUtilized;
    int flushStart;
    int flushComplete;
    int terminate;
};

void flush(struct logfs *logfs)
{
    uint64_t deviceIndexStart = (logfs->utilized / device_block(logfs->device)) * device_block(logfs->device);
    uint64_t queueEnd = logfs->tail / device_block(logfs->device) * device_block(logfs->device) + device_block(logfs->device);
    device_write(logfs->device, logfs->queue + logfs->head, deviceIndexStart, queueEnd - logfs->head);
    logfs->utilized += logfs->tail - logfs->head;
    logfs->head = queueEnd - device_block(logfs->device);
    logfs->queueUtilized = 0;
    printf("flushed\n");
}

void *threadFunction(void *arg)
{
    struct logfs *logfs = arg;
    while (1)
    {
        while (logfs->queueUtilized == 0)
        {
            pthread_cond_wait(&logfs->cond, &logfs->mutex);
            if (logfs->terminate == 1)
            {
                return NULL;
            }
        }

        if (logfs->flushStart == 1)
        {
            // Flush logic
            pthread_mutex_lock(&logfs->mutex);
            flush(logfs);
            logfs->flushStart = 0;
            logfs->flushComplete = 1;
            pthread_cond_signal(&logfs->cond);
            pthread_mutex_unlock(&logfs->mutex);
        }
        while (logfs->tail - logfs->head > device_block(logfs->device))
        {
            pthread_mutex_lock(&logfs->mutex);
            device_write(logfs->device, logfs->queue + logfs->head, logfs->utilized, device_block(logfs->device));
            logfs->queueUtilized -= device_block(logfs->device);
            printf("Queue Utilized in thread: %lu\n", logfs->queueUtilized);
            logfs->head += device_block(logfs->device);
            logfs->utilized += device_block(logfs->device);
            pthread_mutex_unlock(&logfs->mutex);
        }
    }
    return NULL;
}

struct logfs *logfs_open(const char *pathname)
{
    struct logfs *logfs = malloc(sizeof(struct logfs));
    logfs->device = device_open(pathname);
    logfs->queue = malloc(sizeof(char) * device_block(logfs->device) * WCACHE_BLOCKS);
    pthread_create(&logfs->consumer, NULL, threadFunction, logfs);
    pthread_mutex_init(&logfs->mutex, NULL);
    pthread_cond_init(&logfs->cond, NULL);
    logfs->queueUtilized = 0;
    logfs->head = 0;
    logfs->tail = 0;
    logfs->flushStart = 0;
    logfs->flushComplete = 0;
    logfs->utilized = 0;
    logfs->terminate = 0;
    return logfs;
}

void logfs_close(struct logfs *logfs)
{
    pthread_mutex_lock(&logfs->mutex);
    logfs->terminate = 1;
    pthread_cond_signal(&logfs->cond);
    pthread_mutex_unlock(&logfs->mutex);
    pthread_join(logfs->consumer, NULL);
    pthread_mutex_destroy(&logfs->mutex);
    pthread_cond_destroy(&logfs->cond);
    free(logfs->queue);
    device_close(logfs->device);
    free(logfs);
}

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len)
{
    // Flush Start =1
    pthread_mutex_lock(&logfs->mutex);
    logfs->flushStart = 1;
    pthread_cond_signal(&logfs->cond);
    pthread_mutex_unlock(&logfs->mutex);
    while (logfs->flushComplete == 0)
    {
        pthread_cond_wait(&logfs->cond, &logfs->mutex);
        printf("Queue Utilized in read: %lu\n", logfs->queueUtilized);
    }
    char *temp = malloc(sizeof(char) * device_block(logfs->device));
    device_read(logfs->device, temp, off / device_block(logfs->device) * device_block(logfs->device), device_block(logfs->device));
    memcpy(buf, temp + off % device_block(logfs->device), len);
    printf("Buffer: %s\n", (char *)buf);
    return 0;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len)
{
    if (len > device_block(logfs->device) * WCACHE_BLOCKS)
    {
        TRACE("logfs_append: not enough space\n");
        return -1;
    }

    // if (logfs->tail + len > device_block(logfs->device) * WCACHE_BLOCKS)
    // {
    //     // Flush loigic
    //     logfs->flushStart = 1;
    //     pthread_cond_signal(&logfs->cond);
    //     while (logfs->flushComplete == 0)
    //     {
    //         pthread_cond_wait(&logfs->cond, &logfs->mutex);
    //     }
    // }

    pthread_mutex_lock(&logfs->mutex);
    memcpy(logfs->queue + logfs->tail, buf, len);
    logfs->tail += len;
    logfs->queueUtilized += logfs->tail - logfs->head;
    pthread_cond_signal(&logfs->cond);
    pthread_mutex_unlock(&logfs->mutex);
    return 0;
}