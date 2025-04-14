#ifndef _UTILIS_IMAT_INCLUDED
#define _UTILIS_IMAT_INCLUDED
// utilis for advanced int math operations

#include <stdio.h>
#include <stdint.h>

// returns minimum of 2 uint32_t aka unsigned int
uint32_t imathU32Min(const uint32_t a, const uint32_t b) {
    uint32_t ab[2] = {a, b};
    return ab[a > b];
}

// returns maximum of 2 uint32_t aka unsigned int
uint32_t imathU32Max(const uint32_t a, const uint32_t b) {
    uint32_t ab[2] = {a, b};
    return ab[a < b];
}

// clamps uint32_t a between min and max values, including min and max values
uint32_t imathU32Clamp(const uint32_t a, const uint32_t min, const uint32_t max){
    return imathU32Max(imathU32Min(a, max), min);
}

#endif