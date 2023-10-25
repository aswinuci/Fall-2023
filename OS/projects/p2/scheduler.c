/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

#undef _FORTIFY_SOURCE
#define STACK_SIZE 1000

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "system.h"
#include "scheduler.h"

/**
 * Needs:
 *   setjmp()
 *   longjmp()
 */

/* research the above Needed API and design accordingly */
static int id = 0;
struct thread
{
    int tid;
    jmp_buf context; /* This will be used only in execute, where we store the context of thread */
    enum status
    {
        STATUS_INIT,
        STATUS_RUNNING,
        STATUS_SLEEPING,
        STATUS_TERMINATED
    } status; /* While creating the thread , maark status as staus_init */
    struct stack
    {
        void *memory_; /* THis will also be used to assign memory */
        void *memory;
    } stack;
    struct thread *next;
};

static struct
{
    struct thread *head;
    struct thread *current;
    jmp_buf context;
} state;

int scheduler_create()
{
    /* Allocate memory for thread */
    /* Set status as init */
    /* Allocate memory for stack in memory_ */
    /* Realign the memory to memory using memory_realign(memory_,page_size()) */
    /* Add the thread to the end pf the state structure */

    struct thread *new_thread = (struct thread *)malloc(sizeof(struct thread));
    if (new_thread == NULL)
    {
        fprintf(stderr, "Memory allocation for thread failed.\n");
        return 1;
    }

    new_thread->tid = ++id;
    new_thread->status = STATUS_INIT;

    new_thread->stack.memory_ = malloc(STACK_SIZE);
    if (new_thread->stack.memory_ == NULL)
    {
        fprintf(stderr, "Memory allocation for stack failed.\n");
        free(new_thread);
        return 1;
    }

    new_thread->stack.memory = memory_align(new_thread->stack.memory_, STACK_SIZE);

    new_thread->next = NULL;
    if (state.head == NULL)
    {
        state.head = new_thread;
    }
    else
    {
        struct thread *current = state.head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_thread;
    }

    return 0;
}

struct thread* thread_candidate(){
    /* Need to check for thread that is in Initial state or sleeping state and return it */
    struct thread *current = state.head;
    while(current != NULL){
        if(current->status == STATUS_INIT || current->status == STATUS_SLEEPING){
            return current;
        }
        current = current->next;
    }
    return NULL;
}


void scheduler_execute(void)
{
    if (state.head == NULL)
    {
        printf("No threads to execute\n");
        return;
    }

    /* Loop through till all threads execute */
    struct thread* candidate = NULL;
    do{
        candidate = thread_candidate();
        if(candidate == NULL){
            printf("No threads to execute\n");
            break;
        }
        candidate->status = STATUS_RUNNING;
        state.current = candidate;
        printf("Thread candidate is %d\n",candidate->tid);
        /* if(setjmp(state.context) == 0){
            longjmp(candidate->context,1);
        }
        else{
            candidate->status = STATUS_TERMINATED;
            free(candidate->stack.memory_);
            free(candidate);
        } */
    }while(candidate!=NULL);

    struct thread *current = state.head;
    while (current != NULL)
    {
        printf("Thread %d is present, its stack address was %p (hex) and is now aligned to %lu \n", current->tid, current->stack.memory_, (size_t)current->stack.memory);
        current = current->next;
    }
}

