#ifndef _GRAPHICS_RENDER_OBJECTS_SYNC_INCLUDED
#define _GRAPHICS_RENDER_OBJECTS_SYNC_INCLUDED
// implements synchronization render objects aka semaphores and fences

#include "../api/api.h"
#include "frame.h"

// describes sync objects arrays
static struct {
    uint32_t frames_count; // taken from graphics_frame.max_frames_in_flight
    VkSemaphore* p_image_available_semaphores; // marks if image is available to write
    VkSemaphore* p_image_finished_semaphores; // marks if writting to image is finished
    VkFence* p_in_flight_fences; // cpu command fence
} graphics_sync;

// starts lifetime of sync objects
void graphicsRenderObjectsSyncInit() {
    // allocate sync objects
    graphics_sync.frames_count = graphics_frame.max_frames_in_flight;
    graphics_sync.p_image_available_semaphores = (VkSemaphore*)allocMalloc(graphics_sync.frames_count * sizeof(VkSemaphore));
    graphics_sync.p_image_finished_semaphores = (VkSemaphore*)allocMalloc(graphics_sync.frames_count * sizeof(VkSemaphore));
    graphics_sync.p_in_flight_fences = (VkFence*)allocMalloc(graphics_sync.frames_count * sizeof(VkFence));

    // create infos are same for same objects on different frames
    VkSemaphoreCreateInfo semaphore_info = (VkSemaphoreCreateInfo){0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fence_info = (VkFenceCreateInfo){0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // starting from signaled state
    // create sync objects
    for(uint32_t i = 0; i < graphics_sync.frames_count; i++) {
        if (vkCreateSemaphore(graphics_api_device.device, &semaphore_info, NULL, graphics_sync.p_image_available_semaphores + i) != VK_SUCCESS ||
            vkCreateSemaphore(graphics_api_device.device, &semaphore_info, NULL, graphics_sync.p_image_finished_semaphores + i) != VK_SUCCESS ||
            vkCreateFence(graphics_api_device.device, &fence_info, NULL, graphics_sync.p_in_flight_fences + i) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE GLOBAL SYNC OBJECTS")
        }
    }
}

// ends lifetime of sync objects
void graphicsRenderObjectsSyncTerminate() {
    for(uint32_t i = 0; i < graphics_sync.frames_count; i++) {
        vkDestroySemaphore(graphics_api_device.device, graphics_sync.p_image_available_semaphores[i], NULL);
        vkDestroySemaphore(graphics_api_device.device, graphics_sync.p_image_finished_semaphores[i], NULL);
        vkDestroyFence(graphics_api_device.device, graphics_sync.p_in_flight_fences[i], NULL);
    }
    allocFree(graphics_sync.p_image_available_semaphores);
    allocFree(graphics_sync.p_image_finished_semaphores);
    allocFree(graphics_sync.p_in_flight_fences);
}


#endif