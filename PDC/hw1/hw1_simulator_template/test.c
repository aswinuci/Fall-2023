#include<stdio.h>
#include "queue.h"

int main(){
    Queue q;
    initialize(&q);
    enqueue(&q,1,1);
    enqueue(&q,2,2);
    enqueue(&q,3,3);
    enqueue(&q,4,4);
    enqueue(&q,5,5);
     enqueue(&q,5,5);
    int i=0;
    while(!isEmpty(&q)){
        dequeue(&q);
        i++;
        printf("%d",i);
    }

    return 0;
}