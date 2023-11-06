#include <stdio.h>
#include "util.h"
#include <pthread.h>


typedef struct {
    int thread_id;
    int num_threads;
    Mat* A;
    Mat* B;
    Mat* C;
} ThreadArgs;



void* mat_multiply_parallel_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    for (int i = args->thread_id; i < args->A->n; i += args->num_threads) {
        for (int j = 0; j < args->B->m; j++) {
            for (int k = 0; k < args->C->n; k++) {
                args->C->ptr[i * args->C->n + j] += args->A->ptr[i * args->A->n + k] * args->B->ptr[k * args->B->n + j];
            }
        }
    }

    pthread_exit(NULL);
}

void mat_multiply(Mat* A, Mat* B, Mat* C, unsigned int threads) {
    // Initialize C->m and C->n here if necessary.

    pthread_t thread_handles[threads];
    ThreadArgs thread_args[threads];

    for (int i = 0; i < threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].num_threads = threads;
        thread_args[i].A = A;
        thread_args[i].B = B;
        thread_args[i].C = C;
        pthread_create(&thread_handles[i], NULL, mat_multiply_parallel_thread, &thread_args[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_handles[i], NULL);
    }
}
