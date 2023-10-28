/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include "system.h"
#include "scheduler.h"

static void
_thread_(void *arg)
{
	const char *name;
	int i;

	name = (const char *)arg;
	for (i=0; i<200; ++i) {
		printf("%s %d\n", name, i);
		us_sleep(20000);
		// scheduler_yield();
	}
}

int
main(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	if (scheduler_create(_thread_,"hello") ||
	    scheduler_create(_thread_, "world") ||
	    scheduler_create(_thread_,"this") ||
	    scheduler_create(_thread_,"is") ||
	    scheduler_create(_thread_,"awesome")) {
		TRACE(0);
		return -1;
	}
	scheduler_execute();
	return 0;
}
