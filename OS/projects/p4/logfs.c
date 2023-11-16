#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"
#include "logfs.h"

#define WCACHE_BLOCKS 32
#define RCACHE_BLOCKS 256

typedef struct cache_entry
{
    uint64_t offset;
    void *data;
    size_t size;
    pthread_mutex_t mutex;
} cache_entry_t;

typedef struct wcache_t
{
    cache_entry_t entries[WCACHE_BLOCKS];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} wcache_t;

typedef struct rcache_t
{
    cache_entry_t entries[RCACHE_BLOCKS];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} rcache_t;

wcache_t wcache;
rcache_t rcache;

void cache_init(void)
{
    // Initialize mutexes and condition variables
    pthread_mutex_init(&wcache.mutex, NULL);
    pthread_cond_init(&wcache.not_empty, NULL);
    pthread_mutex_init(&rcache.mutex, NULL);
    pthread_cond_init(&rcache.not_empty, NULL);

    // Initialize cache entries
    for (int i = 0; i < WCACHE_BLOCKS; i++)
    {
        wcache.entries[i].data = NULL;
        pthread_mutex_init(&wcache.entries[i].mutex, NULL);
    }

    for (int i = 0; i < RCACHE_BLOCKS; i++)
    {
        rcache.entries[i].data = NULL;
        pthread_mutex_init(&rcache.entries[i].mutex, NULL);
    }
}

cache_entry_t *find_cache_entry(uint64_t offset)
{
    // Check the write cache first
    pthread_mutex_lock(&wcache.mutex);
    for (int i = 0; i < WCACHE_BLOCKS; i++)
    {
        if (wcache.entries[i].offset == offset)
        {
            pthread_mutex_lock(&wcache.entries[i].mutex);
            pthread_mutex_unlock(&wcache.mutex);
            return &wcache.entries[i];
        }
    }
    pthread_mutex_unlock(&wcache.mutex);

    // Check the read cache if the block is not in the write cache
    pthread_mutex_lock(&rcache.mutex);
    for (int i = 0; i < RCACHE_BLOCKS; i++)
    {
        if (rcache.entries[i].offset == offset)
        {
            pthread_mutex_lock(&rcache.entries[i].mutex);
            pthread_mutex_unlock(&rcache.mutex);
            return &rcache.entries[i];
        }
    }
    pthread_mutex_unlock(&rcache.mutex);

    return NULL;
}

void add_cache_entry(uint64_t offset, void *data, size_t size) {
    // Check if the cache is full
    int cache_full = (find_cache_entry(offset) != NULL) || (find_cache_entry(0) != NULL);

    // Evict an entry if the cache is full
    if (cache_full) {
        // TODO: Choose the eviction policy based on the cache type
        // (WCACHE or RCACHE)

        // Implement Least Recently Used (LRU) eviction policy for WCACHE
        if (cache_type == WCACHE) {
            // Find the least recently used entry
            cache_entry_t *lru_entry = NULL;
            uint64_t min_access_time = UINT64_MAX;

            for (int i = 0; i < WCACHE_BLOCKS; i++) {
                pthread_mutex_lock(&wcache.entries[i].mutex);
                if (wcache.entries[i].data != NULL && wcache.entries[i].last_access_time < min_access_time) {
                    lru_entry = &wcache.entries[i];
                    min_access_time = wcache.entries[i].last_access_time;
                }
                pthread_mutex_unlock(&wcache.entries[i].mutex);
            }

            // Evict the least recently used entry and add the new entry
            if (lru_entry != NULL) {
                pthread_mutex_lock(&lru_entry->mutex);
                free(lru_entry->data);
                lru_entry->data = NULL;
                pthread_mutex_unlock(&lru_entry->mutex);

                // Add the new entry
                lru_entry->offset = offset;
                lru_entry->data = malloc(size);
                memcpy(lru_entry->data, data, size);
                lru_entry->size = size;
                lru_entry->last_access_time = time(NULL);

                // Notify waiting threads if the cache was previously empty
                pthread_mutex_lock(&lru_entry->mutex);
                pthread_cond_signal(&rcache.not_empty);
                pthread_mutex_unlock(&lru_entry->mutex);
            }
        }

        // Implement Least Frequently Used (LFU) eviction policy for RCACHE
        if (cache_type == RCACHE) {
            // Find the least frequently used entry
            cache_entry_t *lfu_entry = NULL;
            int min_access_count = INT_MAX;

            for (int i = 0; i < RCACHE_BLOCKS; i++) {
                pthread_mutex_lock(&rcache.entries[i].mutex);
                if (rcache.entries[i].data != NULL && rcache.entries[i].access_count < min_access_count) {
                    lfu_entry = &rcache.entries[i];
                    min_access_count = rcache.entries[i].access_count;
                }
                pthread_mutex_unlock(&rcache.entries[i].mutex);
            }

            // Evict the least frequently used entry and add the new entry
            if (lfu_entry != NULL) {
                pthread_mutex_lock(&lfu_entry->mutex);
                free(lfu_entry->data);
                lfu_entry->data = NULL;
                pthread_mutex_unlock(&lfu_entry->mutex);

                // Add the new entry
                lfu_entry->offset = offset;
                lfu_entry->data = malloc(size);
                memcpy(lfu_entry->data, data, size);
                lfu_entry->size = size;
lfu_entry->access_count = 1; // Increment access count for the new entry

// Notify waiting threads if the cache was previously empty
pthread_mutex_lock(&lfu_entry->mutex);
pthread_cond_signal(&rcache.not_empty);
pthread_mutex_unlock(&lfu_entry->mutex);
            }
        }
    } else {
        // Cache is not full, simply add the new entry
        cache_entry_t *entry = find_cache_entry(offset);

        entry->offset = offset;
        entry->data = malloc(size);
        memcpy(entry->data, data, size);
        entry->size = size;

        // Update access time or access count depending on the cache type
        if (cache_type == WCACHE) {
            entry->last_access_time = time(NULL);
        } else if (cache_type == RCACHE) {
            entry->access_count++;
        }

        // Notify waiting threads if the cache was previously empty
        pthread_mutex_lock(&entry->mutex);
        pthread_cond_signal(&rcache.not_empty);
        pthread_mutex_unlock(&entry->mutex);
    }
}


int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len)
{
    // Check if the block is in the cache
    cache_entry_t *entry = find_cache_entry(off);
    if (entry)
    {
        // Copy the cached block data to the buffer
        pthread_mutex_lock(&entry->mutex);
        memcpy(buf, entry->data, entry->size);
        pthread_mutex_unlock(&entry->mutex);
        return 0;
    }

    // Read the block from the logfs if not cached
    int ret = logfs_read(logfs, buf, off, len);
    if (ret == 0)
    {
        // Add the block to the cache
        add_cache_entry(off, buf, len);
    }

    return ret;
}

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len)
{
    // Write the data to the logfs
    int ret = logfs_append(logfs, buf, len);
    if (ret == 0)
    {
        // Add the block to the write cache
        add_cache_entry(len - 1, buf, len);
    }

    return ret;
}

void cache_destroy(void)
{
    // Destroy mutexes and condition variables
    pthread_mutex_destroy(&wcache.mutex);
    pthread_cond_destroy(&wcache.not_empty);
    pthread_mutex_destroy(&rcache.mutex);
    pthread_cond_destroy(&rcache.not_empty);

    // Free cached block data
    for (int i = 0; i < WCACHE_BLOCKS; i++)
    {
        if (wcache.entries[i].data)
        {
            free(wcache.entries[i].data);
            wcache.entries[i].data = NULL;
            pthread_mutex_destroy(&wcache.entries[i].mutex);
        }

        for (int i = 0; i < RCACHE_BLOCKS; i++)
        {
            if (rcache.entries[i].data)
            {
                free(rcache.entries[i].data);
                rcache.entries[i].data = NULL;
                pthread_mutex_destroy(&rcache.entries[i].mutex);
            }
        }
    }
}
