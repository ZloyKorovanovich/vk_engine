#ifndef _UTILIS_STR_INCLUDED
#define _UTILIS_STR_INCLUDED
// utilis for advanced string operations
// designed to extend string.h functionality

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// returns true if source string exists in target string array, else false
bool strMatch(
    const char* p_source,
    const size_t target_length,
    const char** pp_target
) {
    for(size_t i = 0; i < target_length; i++) {
        if(strcmp(p_source, pp_target[i]) == 0) {
            return true;
        }
    }
    return false;
}

// returns true if all strings from source array exist in target array, else false
bool strMatchArray(
    const size_t source_count,
    const char** pp_source,
    const size_t target_count,
    const char** pp_target
) {
    for(size_t i = 0; i < source_count; i++) {
        bool matched = false;
        for(size_t j = 0; j < target_count; j++) {
            if(strcmp(pp_source[i], pp_target[j]) == 0) {
                matched = true;
                break;
            }
        }
        if(!matched) {
            return false;
        }
    }
    return true;
}

#endif