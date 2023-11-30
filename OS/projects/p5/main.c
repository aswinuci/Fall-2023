/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include <signal.h>
#include "system.h"

#define MEM_INFO_PATH "/proc/meminfo"
#define NET_STAT_PATH "/proc/net/dev"

/**
 * Needs:
 *   signal()
 */

static volatile int done;

static void
_signal_(int signum)
{
	assert(SIGINT == signum);

	done = 1;
}

typedef struct
{
	unsigned long total_memory;
	unsigned long free_memory;
	unsigned long available_memory;
	unsigned long buffers;
	unsigned long cached;
} MemoryInfo;

typedef struct {
    unsigned long rx_bytes;
    unsigned long tx_bytes;
    // Add more fields or stats related to network utilization
} NetworkInfo;

double
cpu_util(const char *s)
{
	static unsigned sum_, vector_[7];
	unsigned sum, vector[7];
	const char *p;
	double util;
	uint64_t i;

	/*
	  user
	  nice
	  system
	  idle
	  iowait
	  irq
	  softirq
	*/

	if (!(p = strstr(s, " ")) ||
		(7 != sscanf(p,
					 "%u %u %u %u %u %u %u",
					 &vector[0],
					 &vector[1],
					 &vector[2],
					 &vector[3],
					 &vector[4],
					 &vector[5],
					 &vector[6])))
	{
		return 0;
	}
	sum = 0.0;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		sum += vector[i];
	}
	util = (1.0 - (vector[3] - vector_[3]) / (double)(sum - sum_)) * 100.0;
	sum_ = sum;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		vector_[i] = vector[i];
	}
	return util;
}

int read_memory_info(MemoryInfo *mem_info)
{
	FILE *file = fopen(MEM_INFO_PATH, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Error opening /proc/meminfo\n");
		return -1;
	}

	char line[256];
	while (fgets(line, sizeof(line), file))
	{
		if (strstr(line, "MemTotal:") != NULL)
		{
			sscanf(line, "MemTotal: %lu kB", &mem_info->total_memory);
		}
		else if (strstr(line, "MemFree:") != NULL)
		{
			sscanf(line, "MemFree: %lu kB", &mem_info->free_memory);
		}
		else if (strstr(line, "MemAvailable:") != NULL)
		{
			sscanf(line, "MemAvailable: %lu kB", &mem_info->available_memory);
		}
		else if (strstr(line, "Buffers:") != NULL)
		{
			sscanf(line, "Buffers: %lu kB", &mem_info->buffers);
		}
		else if (strstr(line, "Cached:") != NULL)
		{
			sscanf(line, "Cached: %lu kB", &mem_info->cached);
		}
		// Add more conditions to parse other memory-related information if needed
	}

	fclose(file);
	return 0;
}

double memory_utilization()
{
	MemoryInfo mem_info;

	if (read_memory_info(&mem_info) == 0)
	{
		// Calculate memory utilization as per your requirements
		// Example calculation:
		double used_memory = mem_info.total_memory - mem_info.free_memory;
		return (used_memory / mem_info.total_memory) * 100.0;
	}
	else
	{
		return 0.0; // Return 0 if memory info retrieval fails
	}
}

int read_network_info(NetworkInfo *net_info) {
    FILE *file = fopen(NET_STAT_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening /proc/net/dev\n");
        return -1;
    }

    char line[256];
    if (fgets(line, sizeof(line), file)) {
        sscanf(line, "%*s %lu %*u %*u %*u %*u %*u %*u %*u %*u %lu", &net_info->rx_bytes, &net_info->tx_bytes);
    }

    fclose(file);
    return 0;
}

double network_utilization() {
    NetworkInfo net_info;

    if (read_network_info(&net_info) == 0) {
        double total_bytes = net_info.rx_bytes + net_info.tx_bytes;
        return total_bytes;
    } else {
        return 0.0;
    }
}

int main(int argc, char *argv[])
{
	char line[1024];
	FILE *file;
	const char *const PROC_STAT = "/proc/stat";

	UNUSED(argc);
	UNUSED(argv);

	if (SIG_ERR == signal(SIGINT, _signal_))
	{
		TRACE("signal()");
		return -1;
	}
	while (!done)
	{
		if (!(file = fopen(PROC_STAT, "r")))
		{
			TRACE("fopen()");
			return -1;
		}
		if (fgets(line, sizeof(line), file))
		{
			printf("\rCPU utilization: %5.1f%% Memory utilization: %5.1f%% Network utilization: %5.1f bytes/s",
				   cpu_util(line), memory_utilization(), network_utilization());
			fflush(stdout);
		}
		us_sleep(500000);
		fclose(file);
	}
	printf("\rDone!   \n");
	return 0;
}
