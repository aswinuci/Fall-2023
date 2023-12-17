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

/**
 * Needs:
 *   fork() - Creates a new Child Process.
 *   execv() - Function that overwrites the current process with a different process.
 *   waitpid() - Checks if the Child Process has executed and exited.
 *   WIFEXITED() - Checks if the Child Process exited normally.
 *   WEXITSTATUS() Use in Compiler
 *   dlopen() In Open Func - Opens a new file.
 *   dlclose() - Closes an opened file.
 *   dlsym() - Lookup a file for a specific symbol or character.
 */


int jitc_compile(const char *input, const char *output) {
    char *inputFile = (char *)input;
    char *outputFile = (char *)output;
    int status;
    pid_t pid = fork();
    if(pid == -1) {
        TRACE("Fork Error");
        return -1;
    }
    if(!pid) {
        char *argv[] = {"gcc", "-O3", "-fpic", "-shared", "-o", NULL, NULL, "-lm", NULL};
        argv[5] = outputFile;
        argv[6] = inputFile;
        execv("/usr/bin/gcc", argv);
        TRACE("EXECV Error");
        exit(0);
    }
    else {
        if(waitpid(pid, &status, 0) == -1) {
            TRACE("Waitpid Error");
            return -1;
        }
        if(!WIFEXITED(status)) {
            TRACE("WIFEXITED FAILED");
            return -1;
        }
    }
    return 0;
}

struct jitc *jitc_open(const char *pathname) {
    void *cFile = dlopen(pathname, RTLD_LAZY | RTLD_LOCAL);
    if(cFile == NULL) {
        TRACE("Open Error");
        return cFile;
    }
    return cFile;
}

void jitc_close(struct jitc *jitc) {
    if(dlclose(jitc) != 0) {
        TRACE("Close Error");
    }
}

long jitc_lookup(struct jitc *jitc, const char *symbol) {
    char *address = dlsym(jitc, symbol);
    if(address == NULL) {
        TRACE("Lookup");
        return (long)address;
    }
    return (long)address;
}

/* research the above Needed API and design accordingly */
