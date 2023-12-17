/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scm.c
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "scm.h"

/**
 * Needs:
 *   fstat()
 *   S_ISREG()
 *   open()
 *   close()
 *   sbrk()
 *   mmap()
 *   munmap()
 *   msync()
 */

/* research the above Needed API and design accordingly */

#define SZ_META (5 * 8)
#define SIGNATURE 0x6f06738f1a7d0048
#define VIRT_ADDR 0x600000000000

struct scm
{
    int fd;
    char *mem;
    size_t utilized;
    size_t capacity;
};

static size_t file_size(struct scm *scm)
{
    struct stat st;
    size_t page;
    page = page_size();
    if (fstat(scm->fd, &st))
    {
        TRACE("fstat failed\n");
        return -1;
    }
    if (S_ISREG(st.st_mode))
    {
        scm->capacity = st.st_size;
        scm->capacity = scm->capacity / page * page;
        if (SZ_META >= scm->capacity)
        {
            TRACE("file utilized too small\n");
            return -1;
        }
        return 0;
    }
    TRACE("not a regular file\n");
    return -1;
}

static void store(struct scm *scm)
{
    uint64_t *meta;
    meta = (uint64_t *)scm->mem;
    meta[0] = SIGNATURE;
    meta[1] = VIRT_ADDR;
    meta[2] = (uint64_t)scm->utilized;
    meta[3] = (uint64_t)scm->capacity;
    meta[4] = meta[0] ^ meta[1] ^ meta[2] ^ meta[3];
}

static void load(struct scm *scm, int truncate)
{
    uint64_t *meta;
    meta = (uint64_t *)scm->mem;
    if (truncate ||
        meta[0] != SIGNATURE ||
        meta[1] != VIRT_ADDR ||
        meta[2] != (uint64_t)scm->utilized ||
        meta[3] != (uint64_t)scm->capacity ||
        meta[4] != (meta[0] ^ meta[1] ^ meta[2] ^ meta[3]))
    {
        memset(meta,0,scm->capacity);
        scm->utilized = SZ_META;
        store(scm);
    }
    scm->utilized = meta[2];
}

struct scm* scm_open(const char* pathname , int truncate){
    struct scm *scm;
    long dif,cur;
    size_t page;
    char* addr;

    /* Virtual Address Space
 _______________________________
|            Text segment       |
|-------------------------------|
|           Data segment        |
|       [Heap + Free space]     |
|-------------------------------|      ___________________
|           Stack segment       |     |                   |
|_______________________________|     |                   |
                                       |   Shared Memory   |
                                       |   Segment (scm)   |
                                       |                   |
                                       |___________________|
                                                 ^
                                                 |
                                     VIRT_ADDR - cur (difference)
 */

    assert(safe_strlen(pathname) > 0);
    cur = (long)sbrk(0); // returns the end of the data segment
    dif = VIRT_ADDR - cur;
    printf("Difference between VIRT_ADDR and cur: %ld\n",dif);
    if(dif <= 0){
        TRACE("sbrk failed\n");
        return NULL;
    }
    if(!(scm = malloc(sizeof(struct scm)))){
        TRACE("malloc failed\n");
        return NULL;
    }
    memset(scm,0,sizeof(struct scm));
    page = page_size();
    addr = (char *)(VIRT_ADDR/page*page);

    if((scm->fd = open(pathname,O_RDWR)) <= 0){
        scm_close(scm);
        TRACE("open failed\n");
        return NULL;
    }
    if(file_size(scm)){
        scm_close(scm);
        TRACE("file_size failed\n");
        return NULL;
    }
    printf("page size: %ld\n",page);
    printf("Capacity: %ld\n",scm->capacity);
    printf("Addr: %p\n",addr);
    if((scm->mem = mmap(addr,scm->capacity,PROT_READ|PROT_WRITE, MAP_FIXED | MAP_SHARED,scm->fd,0)) == MAP_FAILED){
        scm->mem = NULL;
        scm_close(scm);
        TRACE("mmap failed\n");
        return NULL;
    }
    printf("Mapped end address: %p\n",scm->mem+scm->capacity-1);

    load(scm,truncate);

    printf("\n"
    "********************************************************************************\n"
    " MAPPED REGION    : %012lx - %012lx\n"
    " UNMAPPED REGION  : %012lx - %012lx\n"
    " SCM REGION       : %012lx - %012lx\n" 
    "********************************************************************************\n",
    (long)(0),
    (long)(cur-1),
    (long)(cur),
    (long)(scm->mem-1),
    (long)(scm->mem),
    (long)(scm->mem+scm->capacity-1)
    );
    return scm;
}

void scm_close(struct scm* scm){
    if(scm){
        if(scm->fd > 0){
            if(close(scm->fd)){
                TRACE("close failed\n");
            }
        }
        if(scm->mem){
            if(msync(scm->mem,scm->capacity,MS_SYNC)){
                TRACE("msync failed\n");
            }
            if(munmap(scm->mem,scm->capacity)){
                TRACE("munmap failed\n");
            }
        }
        memset(scm,0,sizeof(struct scm));
    }
    FREE(scm);
}

void* scm_malloc(struct scm *scm, size_t n){
    void *p;
    if((scm->utilized + n) > scm->capacity){
        TRACE("out of memory\n");
        return NULL;
    }
    p = scm->mem + scm->utilized;
    scm->utilized += n;
    store(scm);
    return p;
}

void scm_free(struct scm* scm,const void* p){
    UNUSED(scm);
    UNUSED(p);
    free((void*)p);
}

char* scm_strdup(struct scm* scm,const char* s_){
    size_t n;
    char *s;
    n = safe_strlen(s_);
    if(!(s = scm_malloc(scm,n+1))){
        TRACE("scm_malloc failed\n");
        return NULL;
    }
    safe_sprintf(s,n+1,"%s",s_);
    return s;
}

size_t scm_utilized(const struct scm* scm){
    assert(scm);
    return scm->utilized - SZ_META;
}

size_t scm_capacity(const struct scm* scm){
    assert(scm);
    return scm->capacity - SZ_META;
}

void* scm_mbase(struct scm* scm){
    assert(scm);
    return scm->mem + SZ_META;
}