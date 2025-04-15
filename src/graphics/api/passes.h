#ifndef _GRAPHICS_API_PASSES_INCLUDED
#define _GRAPHICS_API_PASSES_INCLUDED
// implements render passes storage

#include "device.h"

// describes render passes array
static struct {
    uint32_t pass_count;
    VkRenderPass* p_passes;
} graphics_api_passes;


// binding local data
static struct {
    uint32_t iterator;
} _graphics_api_passes_binding;


// returns index of last added render pass
uint32_t graphicsApiPassesCurrentId() {
    // flattening if statement
    uint32_t options[2] = {_graphics_api_passes_binding.iterator, _graphics_api_passes_binding.iterator - 1};
    return options[_graphics_api_passes_binding.iterator != 0];
}

// sterts render passes lifetime
void graphicsApiPassesInit(
    uint32_t pass_count
) {
    graphics_api_passes.pass_count = pass_count;
    _graphics_api_passes_binding.iterator = 0;
    if(pass_count != 0) {
        graphics_api_passes.p_passes = (VkRenderPass*)allocMalloc(pass_count * sizeof(VkRenderPass));
    }
    else {
        graphics_api_passes.p_passes = NULL;
    }
}

// adds render pass to iterator location and ajusts iterator
// returns added pass id
// use argument function for creating render pass
uint32_t graphicsApiPassesAdd(
    void (*p_create_pass) (const VkDevice, VkRenderPass* const)
) {
    if(_graphics_api_passes_binding.iterator >= graphics_api_passes.pass_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PASSES")
    }

    (*p_create_pass)(graphics_api_device.device, graphics_api_passes.p_passes + _graphics_api_passes_binding.iterator);
    return _graphics_api_passes_binding.iterator++;
}

// ends render passes lifetime
void graphicsApiPassesTerminate() {
    for(uint32_t i = 0; i < graphics_api_passes.pass_count; i++) {
        vkDestroyRenderPass(graphics_api_device.device, graphics_api_passes.p_passes[i], NULL);
    }
    if(graphics_api_passes.p_passes != NULL) {
        allocFree(graphics_api_passes.p_passes);
    }
}


#endif