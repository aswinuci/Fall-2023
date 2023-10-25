#ifndef Queue_H
#define Queue_H

#define MAX_SIZE 100

typedef struct {
    int first;
    int second;
} Pair;

typedef struct {
    Pair array[MAX_SIZE];
    int front;
    int rear;
} Queue;

void initialize(Queue* queue);
int isEmpty(Queue* queue);
int isFull(Queue* queue);
void enqueue(Queue* queue, int first, int second);
Pair dequeue(Queue* queue);
int countElements(Queue* queue);

#endif
