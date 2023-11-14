/**
 ** SUBMIT ONLY THIS FILE
 ** NAME: .......
 ** UCI ID: .......
 **
 ** Use only standard libraries.
 ** See description in header file.
 **/

#include "util_common.h"
#include "transpose.h" //implementing
#include "pthread.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

typedef struct
{
    Mat *mat;
    unsigned int numElements;
    unsigned int swaps;
    unsigned int lastSwappedIndex;
    pthread_mutex_t *mutex;
} ThreadData;


void *transpose(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    unsigned int n = data->mat->n;
    unsigned int swaps = data->swaps;

    unsigned int swapStart=0;
    unsigned int swapEnd=0;

    while (1)
    {
        pthread_mutex_lock(data->mutex);

        swapStart = data->lastSwappedIndex + 1;
        if (swapEnd >= data->numElements - 1)
        {
            pthread_mutex_unlock(data->mutex);
            break;
        }
        swapEnd = swapStart + swaps - 1;
        if (swapEnd >= data->numElements)
        {
            swapEnd = data->numElements - 1;
        }
        data->lastSwappedIndex = swapEnd;
        pthread_mutex_unlock(data->mutex);

        while (swapStart <= swapEnd)
        {
            int row = n - 2 - floor(sqrt(-8 * swapStart + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int col = swapStart + row + 1 - n * (n - 1) / 2 + (n - row) * ((n - row) - 1) / 2;
            int temp = data->mat->ptr[row * n + col];
            data->mat->ptr[row * n + col] = data->mat->ptr[col * n+ row];
            data->mat->ptr[col * n + row] = temp;
            swapStart++;
        }
    }
    pthread_exit(NULL);
}

ThreadData initialize_thread_data(Mat *mat, unsigned int swaps, pthread_mutex_t *mutex)
{
    ThreadData data;
    data.mat = mat;
    int n = mat->n;
    data.numElements = ((n * n) - n) / 2;
    data.swaps = swaps;
    data.lastSwappedIndex = -1;
    pthread_mutex_init(mutex, NULL);
    data.mutex = mutex;
    return data;
}

void create_thread(pthread_t *worker, ThreadData *data)
{
    int creation_rc = pthread_create(worker, NULL, &transpose, (void *)data);
    if (creation_rc)
    {
        printf("Error creating thread\n");
        exit(-1);
    }
}

// Take transpose of a square matrix
void mat_squaretransp_sequential(Mat *mat)
{
    int n = mat->n;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double temp = mat->ptr[i * n + j];
            mat->ptr[i * n + j] = mat->ptr[j * n + i];
            mat->ptr[j * n + i] = temp;
        }
    }
}

void mat_squaretransp_parallel(Mat *mat, unsigned int grain, unsigned int thr)
{
    pthread_mutex_t mutex;
    ThreadData data = initialize_thread_data(mat, grain, &mutex);
    pthread_t threads[thr];
    for (unsigned int i = 0; i < thr; ++i)
    {
        create_thread(&threads[i], &data);
    }
    for (unsigned int i = 0; i < thr; ++i)
    {
        pthread_join(threads[i], NULL);
    }
}