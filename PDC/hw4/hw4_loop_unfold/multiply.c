#include <stdio.h>
#include "util.h"
#include <pthread.h>
#include<stdlib.h>

unsigned int N;
Mat* A;
Mat* B;
Mat* C;

// Additional B temporary matrix for faster row-wise access
Mat BT;

// Arguments structure for passing to thread functions
typedef struct {
    int start_row;
    int end_row;
} ThreadArgs;

// Compute the multiplication of matrix rows specified by the arguments
void* compute_row(void* args) {
    ThreadArgs* argument = (ThreadArgs*)args;
    int start_row = argument->start_row;
    int end_row = argument->end_row;

    for (int i = start_row; i <= end_row; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A->ptr[i * A->n + k] * BT.ptr[j * BT.n + k];
            }
            C->ptr[i * C->n + j] = sum;
        }
    }
    
    pthread_exit(NULL);
}


void mat_multiply(Mat* matrix1, Mat* matrix2, Mat* matrix3, unsigned int threads) {
    // Assign matrix values and sizes to global variables
    N = matrix1->n;
    A = matrix1;
    B = matrix2;
    C = matrix3;
    unsigned int NUM_T = threads;

    // Initialize pthread attribute value
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    pthread_t thread_pool[NUM_T];
    ThreadArgs args_pool[NUM_T];

    // Assign matrix2 matrix values to the transposed matrix2 temporary matrix
    BT.ptr = (double*)malloc(matrix2->m * matrix2->n * sizeof(double));
    BT.m = matrix2->n;
    BT.n = matrix2->m;
    
    for (unsigned int i = 0; i < matrix2->m; i++) {
        for (unsigned int j = 0; j < matrix2->n; j++) {
            BT.ptr[j * BT.n + i] = matrix2->ptr[i * matrix2->n + j];
        }
    }

    // If the number of matrix rows is smaller than the total number of threads,
    // use the number of matrix rows as the number of work units
    unsigned int n_split = N < threads ? N : threads;
    unsigned int n_work = N < threads ? 1 : N / threads;

    for (unsigned int i = 0; i < n_split; i++) {
        ThreadArgs args;
        args.start_row = i * n_work;
        args.end_row = args.start_row + n_work - 1;
        args_pool[i] = args;
        pthread_create(&thread_pool[i], &attr, compute_row, (void*)&args_pool[i]);
    }

    for (unsigned int i = 0; i < n_split; i++) {
        pthread_join(thread_pool[i], NULL);
    }

    pthread_attr_destroy(&attr);

    free(BT.ptr);
}
