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

int jitc_compile(const char *input, const char *output) {
    pid_t pid;
    int status;

    /* Create a child process */
    pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) { /* Child process */
        /* Execute the compiler (e.g., gcc) to compile the C program */
        /* char *compiler = "gcc"; */
        char *compiler_args[] = {"gcc", NULL, "-shared", "-o",NULL, "-O3", "-fpic", NULL};
        compiler_args[1]=(char *)input,compiler_args[4]= (char *)output;
        execv("/usr/bin/gcc", compiler_args);

        /* If execv returns, there was an error */
        perror("execv");
        exit(EXIT_FAILURE);
    } else { /* Parent process */
        /* Wait for the child process to finish */
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);

            if (exit_status == 0) {
                return 0; /* Compilation succeeded */
            } else {
                fprintf(stderr, "Compilation failed with exit code %d\n", exit_status);
                return -1; /* Compilation error */
            }
        } else {
            perror("waitpid");
            return -1; /* Error in waiting for the child process */
        }
    }
}

struct jitc *jitc_open(const char *pathname) {
    struct jitc *jitc = (struct jitc *)malloc(sizeof(struct jitc));

    if (!jitc) {
        perror("malloc");
        return NULL;
    }

    /* Load the dynamically loadable module using dlopen */
    jitc->handle = dlopen(pathname, RTLD_LAZY);

    if (!jitc->handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        free(jitc);
        return NULL;
    }

    return jitc;
}

void jitc_close(struct jitc *jitc) {
    if (jitc) {
        /* Unload the dynamically loadable module using dlclose */
        dlclose(jitc->handle);
        free(jitc);
    }
}

long jitc_lookup(struct jitc *jitc, const char *symbol) {
    if (!jitc) {
        return 0;
    }

    /* Find a symbol in the dynamically loaded module using dlsym */
    void *sym = dlsym(jitc->handle, symbol);

    if (!sym) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 0;
    }

    return (long)sym;
}
