#ifndef _GRAPHICS_API_COMMAND_INCLUDED
#define _GRAPHICS_API_COMMAND_INCLUDED
// implements storage of command pools aka cmpools
// implements simple interface for recording command buffer aka cmbuffer

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


// allocates command buffer in specified command pool
void graphicsCmbufferCreate(
    const uint32_t cmpool_id,
    const uint32_t cmbuffer_count,
    VkCommandBuffer* const p_cmbuffers
) {
    VkCommandBufferAllocateInfo alloc_info = (VkCommandBufferAllocateInfo){0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = graphics_cmpools.p_cmpools[cmpool_id];
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = cmbuffer_count;
    if (vkAllocateCommandBuffers(graphics_device.device, &alloc_info, p_cmbuffers) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO ALLOCATE CMBUFFERS!")
    }
}

// starts command buffer record
void graphicsCmbufferBegin(
    const VkCommandBuffer cmbuffer
) {
    VkCommandBufferBeginInfo begin_info = (VkCommandBufferBeginInfo){0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(cmbuffer, &begin_info) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO START COMMAND BUFFER")
    }
}

// ends command buffer record
void graphicsCmbufferEnd(
    const VkCommandBuffer cmbuffer
) {
    if (vkEndCommandBuffer(cmbuffer) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO END COMMAND BUFFER")
    }
}

#endif