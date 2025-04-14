#ifndef _RESOURCESS_FILE_INCLUDED
#define _RESOURCESS_FILE_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include "path.h"
#include "../utilis/alloc.h"

bool resourcesFileReadByte(const char* p_path, size_t* p_file_length, unsigned char** pp_file) {
    FILE *p_file;
    p_file = fopen(p_path, "rb");
    if(p_file == NULL) {
        return false;
    }
    fseek(p_file, 0L, SEEK_END);
    *p_file_length = ftell(p_file);
    fclose(p_file);

    p_file = fopen(p_path, "rb");
    *pp_file = (unsigned char*)allocMalloc(*p_file_length * sizeof(unsigned char));
    fread(*pp_file, sizeof(unsigned char), *p_file_length, p_file);
    fclose(p_file);
    return true;
}

bool resourcesFileReadText(const char* p_path, size_t* p_file_length, char** pp_file) {
    FILE *p_file;
    p_file = fopen(p_path, "rb");
    if(p_file == NULL) {
        return false;
    }
    fseek(p_file, 0L, SEEK_END);
    *p_file_length = ftell(p_file) + 1;//+'\0'
    fclose(p_file);

    p_file = fopen(p_path, "r");
    *pp_file = (char*)allocMalloc(*p_file_length * sizeof(char));
    fread(*pp_file, sizeof(char), *p_file_length-1, p_file);
    (*pp_file)[*p_file_length - 1] = '\0';
    fclose(p_file);
    return true;
}

#endif