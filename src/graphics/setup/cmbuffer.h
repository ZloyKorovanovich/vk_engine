#ifndef _GRAPHICS_CMBUFFER_INCLUDED
#define _GRAPHICS_CMBUFFER_INCLUDED
// implements simple interface for recording command buffer aka cmbuffer

#include <vulkan/vulkan.h>
#include "cmpools.h"

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