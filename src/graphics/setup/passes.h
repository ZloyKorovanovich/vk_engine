#ifndef _GRAPHICS_PASSES_INCLUDED
#define _GRAPHICS_PASSES_INCLUDED
// implements render passes storage

#include <vulkan/vulkan.h>
#include "device.h"
#include "../../utilis/alloc.h"

// describes render passes array
static struct {
    uint32_t pass_count;
    VkRenderPass* p_passes;
} graphics_passes;

// binding local data
static struct {
    uint32_t iterator;
} _graphics_passes_binding;


// returns index of last added render pass
uint32_t graphicsPassesCurrentId() {
    // flattening if statement
    uint32_t options[2] = {_graphics_passes_binding.iterator, _graphics_passes_binding.iterator - 1};
    return options[_graphics_passes_binding.iterator != 0];
}

// sterts render passes lifetime
void graphicsPassesInit(
    uint32_t pass_count
) {
    graphics_passes.pass_count = pass_count;
    _graphics_passes_binding.iterator = 0;
    if(pass_count != 0) {
        graphics_passes.p_passes = (VkRenderPass*)allocMalloc(pass_count * sizeof(VkRenderPass));
    }
    else {
        graphics_passes.p_passes = NULL;
    }
}

// adds render pass to iterator location and ajusts iterator
// use argument function for creating render pass
uint32_t graphicsPassesAdd(
    void (*p_create_pass) (const VkDevice, VkRenderPass* const)
) {
    if(_graphics_passes_binding.iterator >= graphics_passes.pass_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PASSES")
    }

    (*p_create_pass)(graphics_device.device, graphics_passes.p_passes + _graphics_passes_binding.iterator);
    return _graphics_passes_binding.iterator++;
}

// ends render passes lifetime
void graphicsPassesTerminate() {
    for(uint32_t i = 0; i < graphics_passes.pass_count; i++) {
        vkDestroyRenderPass(graphics_device.device, graphics_passes.p_passes[i], NULL);
    }
    if(graphics_passes.p_passes != NULL) {
        allocFree(graphics_passes.p_passes);
    }
}

#endif