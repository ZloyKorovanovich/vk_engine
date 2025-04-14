#ifndef _GRAPHICS_QUEUES_INCLUDED
#define _GRAPHICS_QUEUES_INCLUDED
// implements required graphics queues description
// change when want to use different queues

#include <stdbool.h>
#include "setup/setup.h"

// describes required queues
const struct {
    const uint32_t queue_count;
    const VkQueueFlagBits* p_queue_flags;
} graphics_required_queues = {
    .queue_count = 3,
    .p_queue_flags = (VkQueueFlagBits[]){VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT}
};

// helpers
#define GRAPHICS_QUEUE_ID 0
#define COMPUTE_QUEUE_ID 1
#define TRANSFER_QUEUE_ID 2
#define PRESENT_QUEUE_ID (graphics_required_queues.queue_count + 1)

#endif