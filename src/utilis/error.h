#ifndef _UTILIS_ERROR_INCLUDED
#define _UTILIS_ERROR_INCLUDED
// utilis for runtime errors

#include <stdio.h>

// prints error and exits the function with -1 result
#define ERROR_FUNCTION(type, str) printf("ERROR: " #str "\n") return (type)(-1);
// prints error and exits the program with -1 result
#define ERROR_FATAL(str) printf("FATAL ERROR: " #str "\n"); exit(-1);
    
#endif