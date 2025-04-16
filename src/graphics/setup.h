#ifndef _GRAPHICS_SETUP_INCLUDED
#define _GRAPHICS_SETUP_INCLUDED
// implements setup configuration and helper macros

#include "api/api.h"

// describes setup configuration
const struct {
    uint32_t queue_count;
    const VkQueueFlagBits* p_queue_flags;
    uint32_t max_frames_in_flight;
} graphics_setup = {
    .queue_count = 3,
    .p_queue_flags = (VkQueueFlagBits[]){VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT},
    .max_frames_in_flight = 2
};

#define GRAPHICS_QUEUE_GRAPHICS_ID 0
#define GRAPHICS_QUEUE_COMPUTE_ID 1
#define GRAPHICS_QUEUE_TRANSFER_ID 2
#define GRAPHICS_GLOBAL_PRESENT_QUEUE 3

#endif