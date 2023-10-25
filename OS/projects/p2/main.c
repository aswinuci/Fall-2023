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

/* static void
_thread_(void *arg)
{
	const char *name;
	int i;

	name = (const char *)arg;
	for (i=0; i<5; ++i) {
		printf("%s %d\n", name, i);
		us_sleep(20000);
		scheduler_yield();
	}
} */

int
main(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	if (scheduler_create() ||
	    scheduler_create() ||
	    scheduler_create() ||
	    scheduler_create() ||
	    scheduler_create()) {
		TRACE(0);
		return -1;
	}
	scheduler_execute();
	return 0;
}
