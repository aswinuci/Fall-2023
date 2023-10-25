#include <stdio.h>

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

void initialize(Queue* queue) {
    queue->front = -1;
    queue->rear = -1;
}

int isEmpty(Queue* queue) {
    return (queue->front == -1);
}

int isFull(Queue* queue) {
    return (queue->rear == MAX_SIZE - 1);
}

void enqueue(Queue* queue, int first, int second) {
    if (isFull(queue)) {
        printf("Queue is full. Cannot enqueue.\n");
    } else {
        if (isEmpty(queue)) {
            queue->front = 0;
        }
        queue->rear++;
        queue->array[queue->rear].first = first;
        queue->array[queue->rear].second = second;
    }
}

Pair dequeue(Queue* queue) {
    Pair pair;
    pair.first = -1;  // Initialize to sentinel values
    pair.second = -1;

    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
    } else {
        pair = queue->array[queue->front];
        if (queue->front == queue->rear) {
            queue->front = queue->rear = -1;
        } else {
            queue->front++;
        }
    }

    return pair;
}

int countElements(Queue* queue) {
    if (isEmpty(queue)) {
        return 0;
    } else {
        return queue->rear - queue->front + 1;
    }
}

// int main() {
//     Queue queue;
//     initialize(&queue);

//     enqueue(&queue, 1, 2);
//     enqueue(&queue, 3, 4);
//     enqueue(&queue, 5, 6);

//     Pair pair1 = dequeue(&queue);
//     Pair pair2 = dequeue(&queue);
//     Pair pair3 = dequeue(&queue);
//     Pair pair4 = dequeue(&queue); // This will print an error message

//     printf("Dequeued: (%d, %d)\n", pair1.first, pair1.second);
//     printf("Dequeued: (%d, %d)\n", pair2.first, pair2.second);
//     printf("Dequeued: (%d, %d)\n", pair3.first, pair3.second);
//     printf("Dequeued: (%d, %d)\n", pair4.first, pair4.second);

//     return 0;
// }
