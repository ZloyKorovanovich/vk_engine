#ifndef _GRAPHICS_RENDER_PASSESES_INCLUDED
#define _GRAPHICS_RENDER_PASSESES_INCLUDED
// implements render passes usage

#include <vulkan/vulkan.h>
#include "triangle.h"

// required for passes allocation
const struct {
    uint32_t passes_count; 
    uint32_t pipelines_count;
} graphics_render_passes = {
    .passes_count = 1,
    .pipelines_count = 1
};

// executes render passes
void graphicsRenderPassesExecute(
    VkCommandBuffer cmbuffer, 
    uint32_t frame_index
) {
    graphicsRenderPassesTriangleExecute(cmbuffer, frame_index);
}

void graphicsRenderPassesInit(void) {
    graphicsApiPassesInit(graphics_render_passes.passes_count);
    graphicsApiPipelinesInit(graphics_render_passes.pipelines_count);
    graphicsApiFramebuffersInit(graphics_render_passes.passes_count);
    
    graphicsRenderPassesTriangleCreate();
}

void grpahicsRnederPassesTerminate(void) {
    graphicsApiPassesTerminate();
    graphicsApiFramebuffersTerminate();
    graphicsApiPipelinesTerminate();
}

#endif