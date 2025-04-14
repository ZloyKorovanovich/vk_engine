#ifndef _GRAPHICS_RENDER_INCLUDED
#define _GRAPHICS_RENDER_INCLUDED

// helpers
#define GRAPHICS_QUEUE_ID 0
#define COMPUTE_QUEUE_ID 1
#define TRANSFER_QUEUE_ID 2
#define PRESENT_QUEUE_ID (graphics_required_queues.queue_count + 1)

const struct {
    const uint32_t queue_count;
    const VkQueueFlagBits* p_queue_flags;
} graphics_required_queues = {
    .queue_count = 3,
    .p_queue_flags = (VkQueueFlagBits[]){VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT}
};

static struct {
    const uint32_t max_frames_in_flight;
    uint32_t frame_id;
} graphics_frame = {
    .max_frames_in_flight = 2,
    .frame_id = 0
};

static struct {
    uint32_t buffer_count;
    VkCommandBuffer* p_graphics_buffers;
} graphics_cmd;

// describes sync objects arrays
static struct {
    uint32_t frames_count;
    VkSemaphore* p_image_available_semaphores;
    VkSemaphore* p_image_finished_semaphores;
    VkFence* p_in_flight_fences;
} graphics_sync;


void graphicsFrameNext() {
    graphics_frame.frame_id = (graphics_frame.frame_id + 1) % graphics_frame.max_frames_in_flight;
}

void graphicsCmdInit() {
    // allocate buffer arrays for all frames
    graphics_cmd.buffer_count = graphics_frame.max_frames_in_flight;
    graphics_cmd.p_graphics_buffers = (VkCommandBuffer*)allocMalloc(graphics_cmd.buffer_count * sizeof(VkCommandBuffer));
    graphicsCmbufferCreate(
        GRAPHICS_QUEUE_ID,
        graphics_cmd.buffer_count,
        graphics_cmd.p_graphics_buffers
    );
}

void graphicsCmdTerminate() {
    allocFree(graphics_cmd.p_graphics_buffers);
}


// starts lifetime of sync objects
void graphicsSyncInit() {
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
        if (vkCreateSemaphore(graphics_device.device, &semaphore_info, NULL, graphics_sync.p_image_available_semaphores + i) != VK_SUCCESS ||
            vkCreateSemaphore(graphics_device.device, &semaphore_info, NULL, graphics_sync.p_image_finished_semaphores + i) != VK_SUCCESS ||
            vkCreateFence(graphics_device.device, &fence_info, NULL, graphics_sync.p_in_flight_fences + i) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE GLOBAL SYNC OBJECTS")
        }
    }
}

// ends lifetime of sync objects
void graphicsSyncTerminate() {
    for(uint32_t i = 0; i < graphics_sync.frames_count; i++) {
        vkDestroySemaphore(graphics_device.device, graphics_sync.p_image_available_semaphores[i], NULL);
        vkDestroySemaphore(graphics_device.device, graphics_sync.p_image_finished_semaphores[i], NULL);
        vkDestroyFence(graphics_device.device, graphics_sync.p_in_flight_fences[i], NULL);
    }
    allocFree(graphics_sync.p_image_available_semaphores);
    allocFree(graphics_sync.p_image_finished_semaphores);
    allocFree(graphics_sync.p_in_flight_fences);
}


#endif