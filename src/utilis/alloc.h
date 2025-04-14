#ifndef _UTILIS_MEMORY_INCLUDED
#define _UTILIS_MEMORY_INCLUDED
// utilis for tarcking memory allocation and debug memory leaks

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// use counters to debug if memory leaks 
struct {
    size_t total_allocations;
    size_t current_allocations;
} alloc_info = {
    .total_allocations = 0,
    .current_allocations = 0
};

// malloc with allocations tracking
void* allocMalloc(size_t bytes) {
    void* p = malloc(bytes);
    alloc_info.total_allocations++;
    alloc_info.current_allocations++;
    return p;
}

// realloc with allocations tracking
void* allocRealloc(void* p, size_t size) {
    void* p_new = realloc(p, size);
    alloc_info.total_allocations++;
    return p_new;
}

//free with allocations tracking
void allocFree(void* p) {
    free(p);
    alloc_info.current_allocations--;
}

#endif