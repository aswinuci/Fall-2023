/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

// only include standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h>
#include "queue.h"
#include "simulator.h" // implements

double PI_VAL = 3.14159265358979323846;
#define UNIFORM "UNIFORM"
#define NORMAL "NORMAL"
#define TRESHOLD 0.02

#include <stdbool.h>

const char *getDistribution(char inputDistribution)
{
    if (inputDistribution == 'u')
        return UNIFORM;
    else if (inputDistribution == 'n')
        return NORMAL;
    else
        return "Invalid Input";
}



bool timeDivision(double current,double previous){
    double result = 1 - (double)(current/previous);
    return result < 0 ? result*-1: result;
}

bool checkTerminationCondition(double currentTime, double previousTime){
    return timeDivision(currentTime,previousTime) - TRESHOLD < 0;
}

void simulate(double *avg_access_time,
              int avg_access_time_len,
              int procs,
              char dist)
{
    FILE* file = fopen("log.txt", "w");
    FILE* outputFile = fopen("output.txt","w");

    // Check if the file was opened successfully
    if (file == NULL || outputFile == NULL ) {
        perror("Failed to open the files");
    }
    fprintf(file,"Simulator is called for distribution %s \n", getDistribution(dist));

    int numberOfProcessors = procs;

    // Each processor will try to access memory
    for (int memory = 1; memory <= 512; memory++)
    {
        fprintf(file,"This is for memory module %d\n",memory);
        int numberOfMemoryModules = memory;
        int memoryModules[numberOfMemoryModules];
        // Set all memory modules to -1, so that it can be checked for undefined
        memset(memoryModules,-1,sizeof memoryModules);
        double accessTimeForPreviousCycle = -1,accessTimeForCurrentCycle=0;
        int accessTimes[numberOfProcessors];
        memset(accessTimes,0, sizeof accessTimes);
        bool visited[numberOfProcessors];
        memset(visited,false,sizeof visited);
        Queue queue;
        initialize(&queue);
        // Each processor will get a memory randonly assigned in each cycle
        for (int cycle = 1; cycle < 10000; cycle++)
        {
            fprintf(file,"-------------------- Start Cycle %d ----------------------------\n",cycle);
            // First find if there are any waiting list, if yes , assign memory modules to them.
            int waitListQueueSize = countElements(&queue);
            fprintf(file,"Waitlist queue size is %d\n",waitListQueueSize);
            while (waitListQueueSize--)
            {
                Pair pair;
                pair = dequeue(&queue);
                int processorNumber = pair.first, memoryLocationWantingtoAccess = pair.second;
                if (memoryModules[memoryLocationWantingtoAccess] != -1)
                {
                    enqueue(&queue, processorNumber, memoryLocationWantingtoAccess);
                    fprintf(file,"The processor %d doesnt get memory access for %d and goes into waitlist\n",processorNumber,memoryLocationWantingtoAccess);
                    // Add waitingTimeFor this Processor
                }
                else
                {
                    memoryModules[memoryLocationWantingtoAccess] = processorNumber;
                    accessTimes[processorNumber]++;
                    fprintf(file,"Assigned the waitlist processor %d the memory location %d \n ", processorNumber,memoryLocationWantingtoAccess);
                }
                visited[processorNumber] = true;
            }

            for(int processor=0;processor<numberOfProcessors;processor++){
                if(!visited[processor]){
                    // Get a memory based on the distribution
                    const char* distribution = getDistribution(dist);
                    int memoryAssignedToProcessor=0;
                    if(distribution == UNIFORM){
                        memoryAssignedToProcessor = rand_uniform(numberOfMemoryModules);
                    }
                    else{
                        memoryAssignedToProcessor = rand_normal_wrap(numberOfMemoryModules/2, 5, numberOfMemoryModules);
                    }

                    fprintf(file,"Memory assigned to processor %d is %d\n",processor,memoryAssignedToProcessor);

                    if(memoryModules[memoryAssignedToProcessor]!=-1){
                        fprintf(file,"Memory module requsted for processor %d already occupied by processor %d\n",processor,memoryModules[memoryAssignedToProcessor]);
                        enqueue(&queue,processor,memoryAssignedToProcessor);
                        fprintf(file,"Processor %d added to waitlist queue\n",processor);
                    }
                    else{
                        memoryModules[memoryAssignedToProcessor] = processor;
                        accessTimes[processor]++;
                        fprintf(file,"Processor %d has been allocated memory %d\n",processor,memoryAssignedToProcessor);
                    }
                }
            }

            // Compute tc for the cycle 
            int processorsHavingAccessToMemoryLocation = 0;
            double sum = 0.0;
            // Print Processor array 
            fprintf(file,"Printing Processor Access array\n");
            for(int i=0;i<numberOfProcessors;i++){
                fprintf(file,"%d : %d/%d, ",i,accessTimes[i],cycle);
            }
            fprintf(file,"\n");
            for(int processor = 0; processor< numberOfProcessors; processor++ ){
                if(accessTimes[processor]==0)continue;
                sum+= (double)(cycle/accessTimes[processor]);
                processorsHavingAccessToMemoryLocation++;
            }
            fprintf(file,"Sum of all access time = %f and no of processors = %d \n",sum,processorsHavingAccessToMemoryLocation);
            sum=(double)(sum/(double)processorsHavingAccessToMemoryLocation);

            accessTimeForCurrentCycle = sum;
            
            fprintf(file,"Access time for current cycle is %f\n",accessTimeForCurrentCycle);

            if(cycle > 1 && 
            checkTerminationCondition(accessTimeForCurrentCycle,accessTimeForPreviousCycle))
            {
                fprintf(file,"Time Difference of two consecutive cycles is less than 0.02 \n -----------------Exiting the Program ---------------------\n");
                break;
            }

            accessTimeForPreviousCycle = accessTimeForCurrentCycle;
            
            fprintf(file,"----------------- End of cycle %d -------------------\n",cycle);
            memset(memoryModules, -1, sizeof memoryModules);
        }
        // Once memory module is completed , reset the accessTime
        memset(accessTimes,0, sizeof accessTimes);
        // Print the output final time for the memory module 
        fprintf(outputFile,"Memory module %d and time is %f\n",memory,accessTimeForCurrentCycle);
    }
}

int rand_uniform(int max)
{
    return rand() % max;
}

int rand_normal_wrap(int mean, int dev, int max)
{
    static double U, V;
    static int phase = 0;
    double Z;
    if (phase == 0)
    {
        U = (rand() + 1.) / (RAND_MAX + 2.);
        V = rand() / (RAND_MAX + 1.);
        Z = sqrt(-2 * log(U)) * sin(2 * PI_VAL * V);
    }
    else
    {
        Z = sqrt(-2 * log(U)) * cos(2 * PI_VAL * V);
    }
    phase = 1 - phase;
    double res = dev * Z + mean;

    // round result up or down depending on whether
    // it is even or odd. This compensates some bias.
    int res_int;
    // if even, round up. If odd, round down.
    if ((int)res % 2 == 0)
        res_int = (int)(res + 1);
    else
        res_int = (int)(res);

    // wrap result around max
    int res_wrapped = res_int % max;
    // deal with % of a negative number in C
    if (res_wrapped < 0)
        res_wrapped += max;
    return res_wrapped;
}
