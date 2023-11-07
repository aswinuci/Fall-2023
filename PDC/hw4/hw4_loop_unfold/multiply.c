#include <stdio.h>
#include "util.h"
#include <pthread.h>
#include<stdlib.h>

unsigned int NG;
Mat* AG;
Mat* BG;
Mat* CG;

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
        for (int j = 0; j < NG; j++) {
            double sum = 0.0;
            for (int k = 0; k < NG; k++) {
                sum += AG->ptr[i * AG->n + k] * BT.ptr[j * BT.n + k];
            }
            CG->ptr[i * CG->n + j] = sum;
        }
    }
    
    pthread_exit(NULL);
}


void mat_multiply(Mat* A, Mat* B, Mat* C, unsigned int threads) {
    // Assign matrix values and sizes to global variables
    NG = A->n;
    AG = A;
    BG = B;
    CG = C;
    unsigned int NUM_T = threads;

    // Initialize pthread attribute value
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    pthread_t thread_pool[NUM_T];
    ThreadArgs args_pool[NUM_T];

    // Assign B matrix values to the transposed B temporary matrix
    BT.ptr = (double*)malloc(B->m * B->n * sizeof(double));
    BT.m = B->n;
    BT.n = B->m;
    
    for (unsigned int i = 0; i < B->m; i++) {
        for (unsigned int j = 0; j < B->n; j++) {
            BT.ptr[j * BT.n + i] = B->ptr[i * B->n + j];
        }
    }

    // If the number of matrix rows is smaller than the total number of threads,
    // use the number of matrix rows as the number of work units
    unsigned int n_split = NG < threads ? NG : threads;
    unsigned int n_work = NG < threads ? 1 : NG / threads;

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
