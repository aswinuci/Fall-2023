/* SUBMIT ONLY THIS FILE */
/* NAME: Aswin */
/* UCI ID: saswin */

#include <pthread.h>
#include "util_common.h"
#include<stdio.h>
#include<math.h>
#include "primes.h"//implementing

unsigned int upperLimit =0;


int isPrime(int val){
    if (val <= 1) return 0;
    if (val <= 3) return 1;
    if (val % 2 == 0 || val % 3 == 0) return 0;
    for(int i=5;i<=sqrt(val);i++){
        if(val%i==0)return 0;
    }
    return 1;
}

unsigned int shared_counter = 1; // Shared counter for prime number checks
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the shared counter

void* find_primes(void* primes_list) {
    while (1) {
        unsigned int current;
        pthread_mutex_lock(&counter_mutex);
        current = shared_counter;
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);

        if (current > upperLimit) break;

        if (isPrime(current)) {
            carr_d_push(primes_list,current);
        }
    }
    return NULL;
}


void primes_sequential(carr_d_t *primes_list, unsigned int max){
    
    // YOUR CODE GOES HERE
    upperLimit = max;
    for(unsigned int i=2;i<upperLimit;i++){
        if(isPrime(i))carr_d_push(primes_list, i);
    }
    printf("Sequential\n");
    return;
}

void primes_parallel(carr_d_t *primes_list, unsigned int max, \
                     unsigned int numThreads){
        
    // YOUR CODE GOES HERE 
    upperLimit = max;
    printf("Parallel\n");
    pthread_t threads[numThreads];

    for (unsigned int i = 0; i < numThreads; i++) {
        if (pthread_create(&threads[i], NULL, find_primes, primes_list) != 0) {
            perror("pthread_create");
            return;
        }
    }

    for (unsigned int i = 0; i < numThreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return;
        }
    }
}