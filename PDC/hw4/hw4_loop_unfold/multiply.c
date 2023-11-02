#include <stdio.h> //remove if not using.
#include "util.h"//implementing

// Functions defined with the modifier 'static' are only visible to other
// functions in this file. They cannot be called from outside (for example,
// from main.c). Use them to organize your code. Remember that in C, you cannot
// use a function until you declare it, so declare all your utility functions
// above the point where you use them.
//
// Maintain the mat_multiply function as lean as possible because we are
// measuring their speed. Unless you are debugging, do not print anything on
// them, that consumes precious time.

void mat_multiply(Mat *A, Mat *B, Mat *C, unsigned int threads){
    // Put your code here.
    // Remember to set the correct values for C->m, and C->n after doing the
    // multiplication (this matters if the matrices are not square)
    for(int i=0; i < A->n; i++){
        for(int j=0; j< B->m; j++){
            for(int k=0;k<C->n;k++){
               C->ptr[i*C->n+j] += A->ptr[i*A->n+k] * B->ptr[k*B->n+j];
            }
        }
    }
    return;
}
