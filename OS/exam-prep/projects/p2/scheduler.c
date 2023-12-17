/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

#undef _FORTIFY_SOURCE

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "system.h"
#include "scheduler.h"

/**
 * Needs:
 *   alarm()
 *   signal()
 *   setjmp()
 *   longjmp()
 */

/* research the above Needed API and design accordingly */

#define SZ_STACK 1024 * 1024
#define ALARM_PERIOD 1 /* seconds */

static struct {
    struct thread {
        enum {
            THREAD_STATUS_INITIAL,
            THREAD_STATUS_RUNNING,
            THREAD_STATUS_SLEEPING,
            THREAD_STATUS_TERMINARTED
        }status;
        struct {
            char* memory;
            char* memory_;
        }stack;
        struct {
            scheduler_fnc_t fnc;
            void* arg;
        }code;
        jmp_buf ctx;
        struct thread* link;
    } *head,*thread;
    jmp_buf ctx;
}state;

static void __signal__(int signum){
    assert(!signum || SIGALRM == signum);
    signal(SIGALRM, __signal__);
    alarm(ALARM_PERIOD);
    if(signum){
        scheduler_yield();
    }
}

static void destroy(){
    struct thread* thread, *thread_;
    thread = state.head;
    while(thread){
        thread_ = thread->link;
        free(thread->stack.memory_);
        free(thread);
        thread = thread_;
    }
    memset(&state, 0, sizeof(state));
}

static struct thread* thread_candidate(){
    struct thread* thread;
    thread = state.thread->link ? state.thread->link : state.head;
    do {
        if(thread->status == THREAD_STATUS_INITIAL || thread->status == THREAD_STATUS_SLEEPING){
            return thread;
        }
        thread = thread->link ? thread->link : state.head;
    }while(thread != state.thread);
    return NULL;
}

static void schedule(){
    struct thread* thread;
    uint64_t rsp;
    if((thread = thread_candidate())){
        state.thread = thread;
        /*If thread is in initial state */
        if(thread->status == THREAD_STATUS_INITIAL){
           rsp = (uint64_t)thread->stack.memory_ + SZ_STACK;
           __asm__ volatile ("mov %[rs], %%rsp \n" : [rs] "+r" (rsp) ::);
           thread->status = THREAD_STATUS_RUNNING;
           thread->code.fnc(thread->code.arg);
           thread->status = THREAD_STATUS_TERMINARTED;
           longjmp(state.ctx, 1);
           EXIT("software");
        }
        thread->status = THREAD_STATUS_RUNNING;
        longjmp(thread->ctx, 1);
        EXIT("software");
    }
}

int scheduler_create(scheduler_fnc_t fnc, void* arg){
    struct thread* thread;
    size_t page;
    page = page_size();
    if(!(thread = malloc(sizeof(struct thread)))){
        TRACE(0);
        return -1;
    }
    memset(thread, 0, sizeof(struct thread));
    if(!(thread->stack.memory_ = malloc(SZ_STACK + page))){
        TRACE(0);
        free(thread);
        return -1;
    }
    thread->stack.memory = memory_align(thread->stack.memory_, page);
    thread->code.fnc = fnc;
    thread->code.arg = arg;
    thread->link = state.head;
    state.head = thread;
    state.thread = thread;
    return 0;
}

void scheduler_execute(){
    assert(state.head);
    __signal__(0);
    setjmp(state.ctx);
    schedule();
    alarm(0);
    destroy();
}

void scheduler_yield(){
    assert(state.head);
    if(!setjmp(state.thread->ctx)){
        state.thread->status = THREAD_STATUS_SLEEPING;
        longjmp(state.ctx, 1);
        EXIT("software");
    }
}