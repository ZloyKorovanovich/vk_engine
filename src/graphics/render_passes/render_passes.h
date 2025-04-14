#ifndef _GRAPHICS_RENDER_PASSESES_INCLUDED
#define _GRAPHICS_RENDER_PASSESES_INCLUDED

#include <vulkan/vulkan.h>

#include "triangle.h"

// required for passes allocation
const uint32_t passes_count = 1; 

// creates and adds all render passes
// change this function if you want to add custom pass or remove one
void renderPassesCreate() {
    renderPassesTriangleCreate();
}

void renderPassesExecute(
    VkCommandBuffer cmbuffer, 
    uint32_t frame_index
) {
    renderPassesTriangleExecute(cmbuffer, frame_index);
}

#endif