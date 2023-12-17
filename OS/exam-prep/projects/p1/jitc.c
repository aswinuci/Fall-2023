/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"
#include <stdio.h>

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

/* research the above Needed API and design accordingly */

struct jitc {
    void *handle;
};

struct jitc *jitc_open(const char *pathname){
    struct jitc *jitc = (struct jitc *)malloc(sizeof(struct jitc));
    jitc->handle = dlopen(pathname, RTLD_LAZY);
    if(!jitc->handle){
        printf("dlopen error\n");
        return NULL;
    }
    return jitc;
}

void jitc_close(struct jitc *jitc){
    dlclose(jitc->handle);
    free(jitc);
}

long jitc_lookup(struct jitc *jitc, const char *symbol){
    long addr = (long)dlsym(jitc->handle, symbol);
    if(!addr){
        printf("dlsym error\n");
        return 0;
    }
    return addr;
}

int jitc_compile(const char *input, const char *output){
    pid_t pid;
    int status;
    pid = fork();
    if(pid < 0){
        printf("fork error\n");
        return -1;
    }
    else if(pid == 0){
        char *argv[] = {"gcc", "-shared", "-fPIC", NULL, "-o", NULL, NULL};
        argv[3] = (char *)input;
        argv[5] = (char *)output;
        execv("/usr/bin/gcc", argv);
        printf("execv error\n");
        return -1;
    }
    else{
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) == 0){
                return 0;
            }
            else{
                printf("gcc error\n");
                return -1;
            }
        }
        else{
            printf("gcc error\n");
            return -1;
        }
    }
}
