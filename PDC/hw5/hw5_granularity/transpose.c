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
    unsigned int grains;
    unsigned int lastIndexTransposed;
} ThreadData;

pthread_mutex_t mutex;

void *transposeMatrix(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    unsigned int n = data->mat->n;
    unsigned int grains = data->grains;
    unsigned int exchange_index = 0;
    unsigned int exchg_end_index = 0;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        exchange_index = data->lastIndexTransposed + 1;
        if (exchg_end_index >= data->numElements - 1)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        exchg_end_index = exchange_index + grains - 1;
        if (exchg_end_index >= data->numElements)
        {
            exchg_end_index = data->numElements - 1;
        }
        data->lastIndexTransposed = exchg_end_index;
        pthread_mutex_unlock(&mutex);
        while (exchange_index <= exchg_end_index)
        {
            int row = n - 2 - floor(sqrt(-8 * exchange_index + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int col = exchange_index + row + 1 - n * (n - 1) / 2 + (n - row) * ((n - row) - 1) / 2;
            int temp = data->mat->ptr[row * n + col];
            data->mat->ptr[row * n + col] = data->mat->ptr[col * n + row];
            data->mat->ptr[col * n + row] = temp;
            exchange_index++;
        }
    }
    pthread_exit(NULL);
}

ThreadData initialize_thread(Mat *mat, unsigned int grains, pthread_mutex_t *mutex)
{
    ThreadData data;
    data.mat = mat;
    unsigned int n = mat->n;
    data.numElements = ((n * n) - n) / 2;
    data.grains = grains;
    data.lastIndexTransposed = -1;
    pthread_mutex_init(mutex, NULL);
    return data;
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
    pthread_t threads[thr];
    ThreadData data = initialize_thread(mat, grain, &mutex);
    for (unsigned int i = 0; i < thr; i++)
    {
        int creation_rc = pthread_create(&threads[i], NULL, &transposeMatrix, (void *)&data);
        if (creation_rc)
        {
            printf("Error creating thread\n");
            exit(-1);
        }
    }

    for (unsigned int i = 0; i < thr; i++)
    {
        pthread_join(threads[i], NULL);
    }
}