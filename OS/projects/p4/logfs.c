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

#define MAX_SIZE 8192

char queue[MAX_SIZE];
int queue_size = 0;
int batch_size = 4096;
int count = 1;
int remaining = 0;

struct logfs
{
    pthread_mutex_t mutex;
    struct device *device;
    pthread_cond_t cond;
    pthread_t consumer;
    int lastPrintedIndex;
};

void *threadFunction(void *arg)
{
    struct logfs *logfs = (struct logfs *)arg;
    while (1)
    {
        pthread_mutex_lock(&logfs->mutex);
        while (queue_size == 0)
        {
            pthread_cond_wait(&logfs->cond, &logfs->mutex);
        }
        printf("Wait finished\n");
        char *buf;
        char *writeData;
        buf = (char *)malloc(batch_size);
        for (int i = logfs->lastPrintedIndex; i < MIN(logfs->lastPrintedIndex + batch_size, queue_size); i++)
        {
            buf[i - logfs->lastPrintedIndex] = queue[i];
        }
        writeData = (char *)malloc(batch_size);
        if (writeData == NULL)
        {
            // Handle memory allocation failure
            return NULL;
        }
        const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
        const int alphabetSize = sizeof(alphabet) - 1; // Exclude null terminator

        for (int i = 0; i < batch_size; ++i)
        {
            writeData[i] = alphabet[i % alphabetSize];
        }
        printf("Begin Writing\n");
        device_write(logfs->device, (void *)writeData, logfs->lastPrintedIndex, strlen(writeData));
        printf("End Writing\n");
        count++;
        logfs->lastPrintedIndex = logfs->lastPrintedIndex + batch_size;
        pthread_mutex_unlock(&logfs->mutex);
    }
    return 0;
}

struct logfs *logfs_open(const char *pathname)
{
    struct logfs *logfs = (struct logfs *)malloc(sizeof(struct logfs));
    logfs->device = device_open(pathname);
    pthread_mutex_init(&logfs->mutex, NULL);
    pthread_cond_init(&logfs->cond, NULL);
    pthread_create(&logfs->consumer, NULL, threadFunction, logfs);
    printf("Thread Created\n");
    logfs->lastPrintedIndex = 0;
    return logfs;
}

void logfs_close(struct logfs *logfs)
{

    printf("Last Printed Index = %d and Queue size %d\n", logfs->lastPrintedIndex, queue_size);
    for (int i = 0; i < queue_size; i++)
    {
        printf("%c", queue[i]);
    }
    printf("\n");
    pthread_join(logfs->consumer, NULL);
    pthread_cond_destroy(&logfs->cond);
    pthread_mutex_destroy(&logfs->mutex);
    free(logfs);
}

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len)
{
    // pthread_mutex_lock(&logfs->mutex);
    // remaining = 1;
    // pthread_cond_signal(&logfs->cond);
    // pthread_mutex_unlock(&logfs->mutex);
    int someStupidVar = (int)off + (int)len + (int)strlen(buf) + (int)(logfs->lastPrintedIndex);
    return 0 * someStupidVar;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len)
{
    pthread_mutex_lock(&logfs->mutex);
    for (int i = 0; i < (int)len; i++)
    {
        printf("Appending %c\n", ((char *)buf)[i]);
        queue[queue_size] = ((char *)buf)[i];
        queue_size++;
        // if (queue_size % batch_size == 0)
        // {
        //     printf("----------Signal\n");
        // }
    }
    pthread_cond_signal(&logfs->cond);
    // printf("buffer %s\n",(char*)buf);
    pthread_mutex_unlock(&logfs->mutex);
    return 0;
}
