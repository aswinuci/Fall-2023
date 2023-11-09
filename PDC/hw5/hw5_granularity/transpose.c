/**
 ** SUBMIT ONLY THIS FILE
 ** NAME: .......
 ** UCI ID: .......
 **
 ** Use only standard libraries.
 ** See description in header file.
 **/

#include "util_common.h"
#include <pthread.h>
#include "transpose.h" //implementing

#ifdef DEBUG
#include <stdio.h> //remove if not using.
#endif



typedef struct {
    Mat *mat;
    unsigned int grain;
    unsigned int start;
    unsigned int end;
} TransposeArgs;

void *transpose_worker(void *arg) {
    TransposeArgs *args = (TransposeArgs *) arg;
    Mat *mat = args->mat;
    unsigned int grain = args->grain;
    unsigned int start = args->start;
    unsigned int end = args->end;

    for (unsigned int i = start; i < end; i += grain) {
        for (unsigned int j = i + 1; j < mat->n; j++) {
            double temp = mat->ptr[i * mat->n + j];
            mat->ptr[i * mat->n + j] = mat->ptr[j * mat->n + i];
            mat->ptr[j * mat->n + i] = temp;
        }
    }

    return NULL;
}

void mat_squaretransp_sequential(Mat *mat){
    
    // YOUR CODE GOES HERE
    for (unsigned int i = 0; i < mat->m; i++) {
        for (unsigned int j = i + 1; j < mat->n; j++) {
            double temp = mat->ptr[i * mat->n + j];
            mat->ptr[i * mat->n + j] = mat->ptr[j * mat->n + i];
            mat->ptr[j * mat->n + i] = temp;
        }
    }
}

void mat_squaretransp_parallel(Mat *mat, unsigned int grain, unsigned int thr){
    // Implement transpose of a matrix in parallel using threads , pthread
    pthread_t threads[thr];
    TransposeArgs args[thr];

    for (unsigned int t = 0; t < thr; t++) {
        args[t].mat = mat;
        args[t].grain = grain;
        args[t].start = t * (mat->m / thr);
        args[t].end = (t + 1) * (mat->m / thr);
        pthread_create(&threads[t], NULL, transpose_worker, &args[t]);
    }

    for (unsigned int t = 0; t < thr; t++) {
        pthread_join(threads[t], NULL);
    }
}
