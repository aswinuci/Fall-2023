#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "system.h"

#define MEMINFO_PATH "/proc/meminfo"
#define NET_DEV_PATH "/proc/net/dev"
#define PROC_STAT "/proc/stat"
#define NETWORK_INTERFACE "eno1"
#define MAX_LINE_LENGTH 256
#define DEVICE_NAME "loop0"
#define DEVICE_PATH "/proc/diskstats"

static volatile int done;

static void
_signal_(int signum)
{
    assert(SIGINT == signum);
    printf("\n");
    printf("\n");
    printf("\n");
    done = 1;
}

double
cpu_util()
{
    static unsigned sum_, vector_[7];
    unsigned sum, vector[7];
    const char *p;
    double util;
    uint64_t i;
    char line[MAX_LINE_LENGTH];
    FILE *file;
    if (!(file = fopen(PROC_STAT, "r")))
    {
        TRACE("fopen()");
        return -1;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (!(p = strstr(line, " ")) ||
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
    printf("Error reading /proc/stat\n");
    fclose(file);
    return -1;
}

double memory_util()
{
    FILE *meminfo_file = fopen(MEMINFO_PATH, "r");
    if (!meminfo_file)
    {
        fprintf(stderr, "Error opening %s\n", MEMINFO_PATH);
        return 0.0;
    }

    char line[256];
    unsigned long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;

    while (fgets(line, sizeof(line), meminfo_file))
    {
        unsigned long value;
        if (sscanf(line, "MemTotal: %lu kB", &value) == 1)
        {
            mem_total = value;
        }
        else if (sscanf(line, "MemFree: %lu kB", &value) == 1)
        {
            mem_free = value;
        }
        else if (sscanf(line, "Buffers: %lu kB", &value) == 1)
        {
            buffers = value;
        }
        else if (sscanf(line, "Cached: %lu kB", &value) == 1)
        {
            cached = value;
        }
    }

    fclose(meminfo_file);
    unsigned long memory_used = mem_total - (mem_free + buffers + cached);
    double memory_used_percentage = ((double)memory_used / mem_total) * 100.0;
    return memory_used_percentage;
}

void getNetworkStats(const char *interface_name)
{
    FILE *file = fopen(NET_DEV_PATH, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strstr(line, interface_name) != NULL)
        {
            unsigned long long packets_received, packets_transmitted;
            sscanf(line + strcspn(line, ":") + 1, "%llu %*u %*u %*u %*u %*u %*u %*u %*u %llu",
                   &packets_received, &packets_transmitted);
            printf("Transmitted packets: %llu | Received packets: %llu\n", packets_transmitted, packets_received);
            fflush(stdout);
            fclose(file);
            return;
        }
    }
    fclose(file);
}

void readDiskStats()
{
    char line[MAX_LINE_LENGTH];
    FILE *file;
    file = fopen(DEVICE_PATH, "r");
    while (fgets(line, sizeof(line), file) != NULL)
    {
        unsigned int major, minor;
        char dev_name[20];
        unsigned long long reads_completed, reads_merged, sectors_read, read_time,
            writes_completed, writes_merged, sectors_written, write_time;
        if (sscanf(line, "%u %u %s %llu %llu %llu %llu %llu %llu %llu %llu",
                   &major, &minor, dev_name,
                   &reads_completed, &reads_merged, &sectors_read, &read_time,
                   &writes_completed, &writes_merged, &sectors_written, &write_time) == 11)
        {

            if (strcmp(dev_name, DEVICE_NAME) == 0)
            {
                printf("Reads: %llu | Writes: %llu\n", reads_completed, writes_completed);
                fflush(stdout);
                return;
            }
        }
    }
    printf("Device not found\n");
    fclose(file);
}

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if (SIG_ERR == signal(SIGINT, _signal_))
    {
        TRACE("signal()");
        return -1;
    }
    fflush(stdout);

    while (!done)
    {
        printf("CPU Utilization: %5.1f%% | Memory Utilization: %5.1f%%\n", cpu_util(), memory_util());
        fflush(stdout);
        getNetworkStats(NETWORK_INTERFACE);
        fflush(stdout);
        readDiskStats();
        fflush(stdout);
        us_sleep(1000000);
        printf("\033[3A");
    }

    return 0;
}
