#ifndef _UTILIS_MEMORY_INCLUDED
#define _UTILIS_MEMORY_INCLUDED
// utilis for tarcking memory allocation and debug memory leaks

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef ALLOCATION_DEBUG
static struct {
    size_t total_allocations;
    size_t table_size;
    void** pp_table;
} alloc_info = {
    .total_allocations = 0,
    .table_size = 0,
    .pp_table = NULL
};

bool allocFindCell(const void* val, size_t* p_id) {
    for(size_t i = 0; i < alloc_info.table_size; i++) {
        if(alloc_info.pp_table[i] == val) {
            *p_id = i;
            return true;
        }
    }
    return false;
}

void allocTableInit(
    size_t size
) {
    alloc_info.table_size = size;
    alloc_info.pp_table = (void**)malloc(size * sizeof(void*));
    for(size_t i = 0; i < alloc_info.table_size; i++) {
        alloc_info.pp_table[i] = NULL;
    }
}

void allocTableTerminate(void) {
    size_t count = 0;
    for(size_t i = 0; i < alloc_info.table_size; i++) {
        count += (size_t)(alloc_info.pp_table[i] != NULL);        
    }
    printf("allocations left: %llu \n", count);
    free(alloc_info.pp_table);
}
#endif

// malloc with allocations tracking
void* allocMalloc(size_t bytes) {
    void* p = malloc(bytes);
#ifdef ALLOCATION_DEBUG
    size_t id;
    if(allocFindCell(NULL, &id)) {
        alloc_info.pp_table[id] = p;
    }
    alloc_info.total_allocations++;
#endif
    return p;
}

// realloc with allocations tracking
void* allocRealloc(void* p, size_t size) {
    void* p_new = realloc(p, size);
#ifdef ALLOCATION_DEBUG
    size_t id;
    if(allocFindCell(p, &id)) {
        alloc_info.pp_table[id] = NULL;
    }
    if(allocFindCell(NULL, &id)) {
        alloc_info.pp_table[id] = p_new;
    }
    alloc_info.total_allocations++;
#endif
    return p_new;
}

//free with allocations tracking
void allocFree(void* p) {
#ifdef ALLOCATION_DEBUG
    size_t id;
    if(allocFindCell(p, &id)) {
        alloc_info.pp_table[id] = NULL;
    }
#endif
    free(p);
}

#endif