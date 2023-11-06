#include<stdio.h>
#include<stdlib.h>  
int main(){
    long long* pointerToMemory = malloc(1);
    *pointerToMemory = INT64_MAX;
    long long* oldadrr = pointerToMemory;
    printf("pointerToMemory: %p and data inside is %lld\n", pointerToMemory, *pointerToMemory);
    pointerToMemory+= 1;
    printf("pointerToMemory: %p and data inside is %lld\n", pointerToMemory, *pointerToMemory);
    printf("Memory diff is %ld\n",pointerToMemory-oldadrr);
    return 0;
}