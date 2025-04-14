#ifndef _GRAPHICS_CMPOOLS_INCLUDED
#define _GRAPHICS_CMPOOLS_INCLUDED
// implements storage of command pools aka cmpools

#include <vulkan/vulkan.h>
#include "device.h"

// describes command pool array
static struct {
    uint32_t cmpool_count;
    VkCommandPool* p_cmpools;
} graphics_cmpools;

// starts cmpools lifetime
void graphicsCmpoolsInit() {
    // allocate cmpools, 1 cmpool per queue
    graphics_cmpools.cmpool_count = graphics_device.queue_count;
    graphics_cmpools.p_cmpools = (VkCommandPool*)allocMalloc(graphics_cmpools.cmpool_count * sizeof(VkCommandPool));
    // create cmpools
    for(uint32_t i = 0; i < graphics_cmpools.cmpool_count; i++) {
        VkCommandPoolCreateInfo pool_info = (VkCommandPoolCreateInfo){0};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = graphics_device.p_queue_families[i];
    
        if (vkCreateCommandPool(graphics_device.device, &pool_info, NULL, graphics_cmpools.p_cmpools + i) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE COMMAND POOL");
        }
    }
}

// ends cmpools lifetime
void graphicsCmpoolsTerminate() {
    for(uint32_t i = 0; i < graphics_cmpools.cmpool_count; i++) {
        vkDestroyCommandPool(graphics_device.device, graphics_cmpools.p_cmpools[i], NULL);
    }
    allocFree(graphics_cmpools.p_cmpools);
}

#endif