#ifndef _RESOURCES_PATH_INCLUDED
#define _RESOURCES_PATH_INCLUDED

#include <stdint.h>
#include <windows.h>

#define MAX_PATH_LENGTH 256

struct {
    size_t application_path_length;
    char p_application_path[MAX_PATH_LENGTH];
    size_t resource_path_length;
    char p_resource_path[MAX_PATH_LENGTH];
} resources_path;


void resourcesPathUpFolder(size_t* p_path_length, char* p_path) {
    size_t i = 0;
    size_t slash_id = 0;
    while(p_path[i] != '\0') {
        if(i == MAX_PATH_LENGTH) {
            return;
        }
        if(p_path[i] == '\\') {
            slash_id = i;
        }
        i++;
    }
    p_path[slash_id] = '\0';
    *p_path_length = slash_id;
}

void resourcesPathInit() {
    resources_path.application_path_length = GetModuleFileNameA(NULL, resources_path.p_application_path, MAX_PATH_LENGTH);
    size_t i = resources_path.application_path_length - 1;
    while (resources_path.p_application_path[i] != '\\' && i > 0) {
        i--;
    }
    resources_path.p_application_path[i] = '\0';
    resources_path.application_path_length = i;

    strcpy(resources_path.p_resource_path, resources_path.p_application_path);
    resourcesPathUpFolder(&resources_path.resource_path_length, resources_path.p_resource_path);
    strcat(resources_path.p_resource_path, "/cres");
    resources_path.resource_path_length = strlen(resources_path.p_resource_path);
}

char* resourcesPathGetLocal(char* p_local_path) {
    char p_add_path[MAX_PATH_LENGTH];
    strcpy(p_add_path, p_local_path);
    strcpy(p_local_path, resources_path.p_resource_path);
    strcat(p_local_path, p_add_path);
    return p_local_path;
}

#endif