#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LMIN 10
#define LMAX 1000
#define DMIN 100
#define DMAX 1000
#define MAX_CYCLES 2 * DMIN * DMAX

int processorLoadUnits[DMAX];
int processorTimeUnits[DMAX];

struct hashTable
{
    int index;
    int value[DMAX];
} hashTable[DMAX];

void debug(int processorArray[], int processorArraySize)
{
    for (int i = 0; i < processorArraySize; i++)
    {
        printf("%d ", processorArray[i]);
    }
    printf("\n");
}

void debugHashTable()
{
    printf("Hash Table\n");
    for (int i = 0; i < DMAX; i++)
    {
        if (hashTable[i].index > 0)
        {
            printf("%d: ", i);
            for (int j = 0; j < hashTable[i].index; j++)
            {
                printf("%d ", hashTable[i].value[j]);
            }
            printf("\n");
        }
    }
}

int *findNeighborsLoadUnits(int processorArraySize, int index)
{
    int *ans = malloc(2 * sizeof(int));
    int leftIndex = index == 0 ? processorArraySize - 1 : (index - 1) % processorArraySize;
    int rightIndex = (index + 1) % processorArraySize;
    ans[0] = processorLoadUnits[leftIndex];
    ans[1] = processorLoadUnits[rightIndex];
    return ans;
}

void assignProcessorLoadUnits(int processorArraySize)
{
    for (int i = 0; i < processorArraySize; i++)
    {
        processorLoadUnits[i] = rand() % (LMAX - LMIN) + LMIN;
    }
}

void assignProcessorTimeUnits(int processorArraySize)
{
    for (int i = 0; i < processorArraySize; i++)
    {
        processorTimeUnits[i] = rand() % (DMAX - DMIN) + DMIN;
    }
}

void updateHashTable(int processorArraySize)
{
    memset(hashTable, 0, sizeof(hashTable));
    for (int i = 0; i < processorArraySize; i++)
    {
        int timeIndex = processorTimeUnits[i];
        hashTable[timeIndex].value[hashTable[timeIndex].index] = i;
        hashTable[timeIndex].index++;
    }
}

int checkbalanced(int processorArraySize)
{
    for (int i = 1; i < processorArraySize; i++)
    {
        if (abs(processorLoadUnits[i] - processorLoadUnits[i - 1]) > 1)
        {
            return 0;
        }
    }
    printf("Balanced\n");
    return 1;
}

int simulate(int processorArraySize)
{
    int timeTaken = 0;
    while (timeTaken < MAX_CYCLES)
    {
        // Find processors that share's load at that time
        int *processors = hashTable[timeTaken % DMAX].value;
        int numberOfProcessors = hashTable[timeTaken % DMAX].index;
        if (numberOfProcessors == 0)
        {
            timeTaken++;
            continue;
        }
        // Get the load units for each processor
        for (int i = 0; i < numberOfProcessors; i++)
        {
            int currentProcessor = processors[i];
            int *neighborsLoadUnits = findNeighborsLoadUnits(processorArraySize, currentProcessor);
            int currentProcessorLoadUnits = processorLoadUnits[currentProcessor];
            int leftNeighborLoadUnits = neighborsLoadUnits[0];
            int rightNeighborLoadUnits = neighborsLoadUnits[1];
            free(neighborsLoadUnits);
            /*
            Case 1 : when left and right neighbors have more load units than current processor
            Case 2 : when left and right neighbors have less load units than current processor
            Case 3 : when left neighbor has more load units than current processor and right neighbor has less load units than current processor
            Case 4 : when left neighbor has less load units than current processor and right neighbor has more load units than current processor
             */
            if (leftNeighborLoadUnits > currentProcessorLoadUnits && rightNeighborLoadUnits > currentProcessorLoadUnits)
            {
                timeTaken++;
                continue;
            }
            else if (leftNeighborLoadUnits <= currentProcessorLoadUnits && rightNeighborLoadUnits <= currentProcessorLoadUnits)
            {
                int leftNeighborIndex = (currentProcessor - 1) % processorArraySize;
                int rightNeighborIndex = (currentProcessor + 1) % processorArraySize;
                int avgLoadUnits = (leftNeighborLoadUnits + currentProcessorLoadUnits + rightNeighborLoadUnits) / 3;
                processorLoadUnits[currentProcessor] = avgLoadUnits;
                processorLoadUnits[leftNeighborIndex] = avgLoadUnits;
                processorLoadUnits[rightNeighborIndex] = avgLoadUnits;
            }
            else if (leftNeighborLoadUnits > currentProcessorLoadUnits && rightNeighborLoadUnits <= currentProcessorLoadUnits)
            {
                int rightNeighborIndex = (currentProcessor + 1) % processorArraySize;
                int avgLoadUnits = (rightNeighborLoadUnits + currentProcessorLoadUnits) / 2;
                processorLoadUnits[currentProcessor] = avgLoadUnits;
                processorLoadUnits[rightNeighborIndex] = avgLoadUnits;
            }
            else if (leftNeighborLoadUnits <= currentProcessorLoadUnits && rightNeighborLoadUnits > currentProcessorLoadUnits)
            {
                int leftNeighborIndex = (currentProcessor - 1) % processorArraySize;
                int avgLoadUnits = (leftNeighborLoadUnits + currentProcessorLoadUnits) / 2;
                processorLoadUnits[currentProcessor] = avgLoadUnits;
                processorLoadUnits[leftNeighborIndex] = avgLoadUnits;
            }
        }
        // printf("Updated load units at time: %d:\n", timeTaken);
        // debug(processorLoadUnits, processorArraySize);
        if (checkbalanced(processorArraySize) == 1)
        {
            break;
        }
        // printf("\n");
        timeTaken++;
    }
    return timeTaken;
}

int main(int argc, char const *argv[])
{
    int k_values[3];
    k_values[0] = 5;
    k_values[1] = 10;
    k_values[2] = 100;
    for (int i = 0; i < sizeof(k_values) / sizeof(k_values[0]); i++)
    {
        memset(processorLoadUnits, 0, sizeof(processorLoadUnits));
        memset(processorTimeUnits, 0, sizeof(processorTimeUnits));
        int processorArraySize = k_values[i];
        printf("Assigning load units to processors\n");
        assignProcessorLoadUnits(processorArraySize);
        debug(processorLoadUnits, processorArraySize);
        printf("Assigning time units to processors\n");
        assignProcessorTimeUnits(processorArraySize);
        debug(processorTimeUnits, processorArraySize);
        printf("Updating hash table\n");
        updateHashTable(processorArraySize);
        debugHashTable();
        int timeTaken = simulate(processorArraySize);
        if (timeTaken == MAX_CYCLES)
            printf("Not possible to balance\n");
        else
        {
            printf("Time taken to balance: %d\n", timeTaken);
            printf("-----------------------------------\n");
        }
    }
    return 0;
}
