#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"

int NUM_MEMS = 512;

int main(int argc, char **argv){

    /*
    ** Parse command line options
    */
    if(argc != 3){
        fprintf(stderr, "Usage: %s <processors> <u|n>\n", argv[0]);
        exit(1);
    }
    int procs = atoi(argv[1]);
    if(procs < 1){
        fprintf(stderr, "The number of processors must be a positive "
                "integer.\n");
        exit(1);
    }
    char dist = argv[2][0];
    if(dist != 'u' && dist != 'n'){
        fprintf(stderr, "The distribution must be 'u' or 'n' (uniform or "
                "normal).\n");
        exit(1);
    }

    /*
    ** Init seed for random numbers
    */
    srand(time(0));

    /*
    ** Run simulation
    */
    double avg_access_time[NUM_MEMS];
    simulate(avg_access_time, NUM_MEMS, procs, dist);

    /*
    ** Print results
    */
   FILE *file = fopen("output.txt", "a");

    if (file == NULL) {
        perror("Error opening the file");
        return 1;
    }
    fprintf(file,"\n------------- Printing output distribution with %d proc and %c distribution--------------------\n",procs,dist);
    for(int i=0; i<NUM_MEMS; i++){
        fprintf(file,"The average access time for memory %d with %d processors and %c distribution is is %.4f\n",i+1, procs,dist, avg_access_time[i]);
    }
    fprintf(file,"\n---------------------------------End of output------------------------------------------------\n");
    return 0;
}
