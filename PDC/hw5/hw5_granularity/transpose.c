#include <pthread.h>
#include <stdio.h>
#include <matrix.h>

struct ThreadData
{
    Mat *mat;
    int id;
    int startRow;
    int startCol;
    int currentRow;
    int currentCol;
    int grains;
};

int numThreads = 0;
int numGrains = 0;

// Take transpose of a square matrix
void mat_squaretransp_sequential(Mat *mat)
{
    int n = mat->n;
    for (int i = 0; i <= n - 1; i++)
    {
        for (int j = i + 1; j <= n - 1; j++)
        {
            double temp = mat->ptr[i * n + j];
            mat->ptr[i * n + j] = mat->ptr[j * n + i];
            mat->ptr[j * n + i] = temp;
        }
    }
}

void *transpose(void *arg)
{
    struct ThreadData *currentThread = (struct ThreadData *)arg;
    int n = currentThread->mat->n;
    while (currentThread->startCol < n)
    {
        while (currentThread->currentRow < n && currentThread->currentCol < n)
        {
            double currentThreadEle = currentThread->mat->ptr[currentThread->currentRow * n + currentThread->currentCol];
            // Swap it
            double temp = currentThreadEle;
            currentThread->mat->ptr[currentThread->currentRow * n + currentThread->currentCol] = currentThread->mat->ptr[currentThread->currentCol * n + currentThread->currentRow];
            currentThread->mat->ptr[currentThread->currentCol * n + currentThread->currentRow] = temp;
            currentThread->currentRow++;
            currentThread->currentCol++;
        }
        currentThread->startCol += numThreads;
        currentThread->currentRow = currentThread->startRow;
        currentThread->currentCol = currentThread->startCol;
    }

    return NULL;
}

void *transpose2(void *arg)
{
    struct ThreadData *currentThread = (struct ThreadData *)arg;
    int n = currentThread->mat->n;
    int grains = currentThread->grains;
    while (grains--)
    {
        double currentThreadEle = currentThread->mat->ptr[currentThread->currentRow * n + currentThread->currentCol];
        printf("Current ele: %f\n", currentThreadEle);
        // Swap it
        double temp = currentThreadEle;
        currentThread->mat->ptr[currentThread->currentRow * n + currentThread->currentCol] = currentThread->mat->ptr[currentThread->currentCol * n + currentThread->currentRow];
        currentThread->mat->ptr[currentThread->currentCol * n + currentThread->currentRow] = temp;
        currentThread->currentRow++;
        currentThread->currentCol++;
    }
    return NULL;
}

void mat_squaretransp_parallel(Mat *mat, unsigned int grains, unsigned int thr)
{
    // Create threads
    pthread_t threads[thr];
    numThreads = thr;
    struct ThreadData threadData[thr];
    for (unsigned int i = 0; i < thr; i++)
    {
        threadData[i].id = i + 1;
        threadData[i].mat = mat;
        threadData[i].grains = grains;
        threadData[i].startRow = 0;
        threadData[i].startCol = i;
        threadData[i].currentRow = threadData[i].startRow;
        threadData[i].currentCol = threadData[i].startCol;
        pthread_create(&threads[i], NULL, transpose, &threadData[i]);
    }
    // Wait for threads to finish
    for (unsigned int i = 0; i < thr; i++)
    {
        pthread_join(threads[i], NULL);
    }
}