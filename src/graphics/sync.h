#ifndef _GRAPHICS_SYNC_INCLUDED
#define _GRAPHICS_SYNC_INCLUDED
// implements global sync objects

#include <vulkan/vulkan.h>
#include "setup/setup.h"

static struct {
    VkSemaphore image_available_semaphore;
    VkSemaphore image_finished_semaphore;
    VkFence in_flight_fence;
} graphics_sync;

void graphicsSyncInit() {
    VkSemaphoreCreateInfo semaphore_info = (VkSemaphoreCreateInfo){0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fence_info = (VkFenceCreateInfo){0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // starting from signaled state

    if (vkCreateSemaphore(graphics_device.device, &semaphore_info, NULL, &graphics_sync.image_available_semaphore) != VK_SUCCESS ||
    vkCreateSemaphore(graphics_device.device, &semaphore_info, NULL, &graphics_sync.image_finished_semaphore) != VK_SUCCESS ||
    vkCreateFence(graphics_device.device, &fence_info, NULL, &graphics_sync.in_flight_fence) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE GLOBAL SYNC OBJECTS");
    }
}

void graphicsSyncTerminate() {
    vkDestroySemaphore(graphics_device.device, graphics_sync.image_available_semaphore, NULL);
    vkDestroySemaphore(graphics_device.device, graphics_sync.image_finished_semaphore, NULL);
    vkDestroyFence(graphics_device.device, graphics_sync.in_flight_fence, NULL);
}

#endif