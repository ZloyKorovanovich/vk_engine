#ifndef _GRAPHICS_RENDER_INCLUDED
#define _GRAPHICS_RENDER_INCLUDED
// implements render

#include "render_passes/render_passes.h"
#include "setup.h"

// describes render info for single frame
typedef struct {
    VkCommandBuffer cmbuffer; // graphics queue buffer
    VkSemaphore image_available_semaphore; // marks if image is available to write
    VkSemaphore image_finished_semaphore; // marks if writting to image is finished
    VkFence in_flight_fence; // cpu command fence
} RenderFrame_t;

// describes render info
static struct {
    RenderFrame_t* p_frames;
    uint32_t frame_id;
} graphics_render;

// draws the frame
void graphicsRenderDraw(double time, double delta) {
    RenderFrame_t frame = graphics_render.p_frames[graphics_render.frame_id];

    vkWaitForFences(graphics_api_device.device, 1, &frame.in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(graphics_api_device.device, 1, &frame.in_flight_fence);
    uint32_t image_id;
    vkAcquireNextImageKHR(
        graphics_api_device.device, 
        graphics_api_swapchain.swapchain, 
        UINT64_MAX, 
        frame.image_available_semaphore, 
        VK_NULL_HANDLE, 
        &image_id
    );
    vkResetCommandBuffer(frame.cmbuffer, 0);
    graphicsApiCmbufferBegin(frame.cmbuffer);
    graphicsRenderPassesExecute(frame.cmbuffer, image_id);
    graphicsApiCmbufferEnd(frame.cmbuffer);

    VkSemaphore p_wait_semaphores[1] = {frame.image_available_semaphore};
    VkPipelineStageFlags p_wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore p_signal_semaphores[1] = {frame.image_finished_semaphore};

    // queue submition
    VkSubmitInfo submit_info = (VkSubmitInfo){0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = p_wait_semaphores;
    submit_info.pWaitDstStageMask = p_wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &frame.cmbuffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = p_signal_semaphores;
    if (vkQueueSubmit(graphics_api_device.p_queues[GRAPHICS_QUEUE_GRAPHICS_ID], 1, &submit_info, frame.in_flight_fence) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO SUBMIT COMMAND BUFFER");
    }

    // returning to swapchain and presentation
    VkPresentInfoKHR present_info = (VkPresentInfoKHR){0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = p_signal_semaphores;
    VkSwapchainKHR p_swapchains[1] = {graphics_api_swapchain.swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = p_swapchains;
    present_info.pImageIndices = &image_id;
    present_info.pResults = NULL;
    vkQueuePresentKHR(graphics_api_device.p_queues[graphics_api_device.p_present_indices[GRAPHICS_QUEUE_GRAPHICS_ID]], &present_info);

    graphics_render.frame_id = (graphics_render.frame_id + 1) % graphics_setup.max_frames_in_flight; // next frame
}

// starts render lifetime
void graphicsRenderInit(void) {
    graphicsApiSwapchainInit(
        VK_SHARING_MODE_EXCLUSIVE, 
        &graphicsApiSwapchainDefaultSelectFormat, 
        &graphicsApiSwapchainDefaultSelectMode
    );
    graphicsRenderPassesInit();

    graphics_render.frame_id = 0;
    // allocate render objects
    graphics_render.p_frames = (RenderFrame_t*)allocMalloc(graphics_setup.max_frames_in_flight * sizeof(RenderFrame_t));
    // create infos are same for same objects on different frames
    VkSemaphoreCreateInfo semaphore_info = (VkSemaphoreCreateInfo){0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fence_info = (VkFenceCreateInfo){0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // starting from signaled state
    // create render objects
    for(uint32_t i = 0; i < graphics_setup.max_frames_in_flight; i++) {
        graphicsApiCmbufferCreate(GRAPHICS_QUEUE_GRAPHICS_ID, 1, &graphics_render.p_frames[i].cmbuffer); // doesnt need destruction, handled by cmpool
        if (vkCreateSemaphore(graphics_api_device.device, &semaphore_info, NULL, &graphics_render.p_frames[i].image_available_semaphore) != VK_SUCCESS ||
            vkCreateSemaphore(graphics_api_device.device, &semaphore_info, NULL, &graphics_render.p_frames[i].image_finished_semaphore) != VK_SUCCESS ||
            vkCreateFence(graphics_api_device.device, &fence_info, NULL, &graphics_render.p_frames[i].in_flight_fence) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE GLOBAL SYNC OBJECTS")
        }
    }
}

// ends render lifetime
void graphicsRenderTerminate(void) {
    for(uint32_t i = 0; i < graphics_setup.max_frames_in_flight; i++) {
        vkDestroySemaphore(graphics_api_device.device, graphics_render.p_frames[i].image_available_semaphore, NULL);
        vkDestroySemaphore(graphics_api_device.device, graphics_render.p_frames[i].image_finished_semaphore, NULL);
        vkDestroyFence(graphics_api_device.device, graphics_render.p_frames[i].in_flight_fence, NULL);
    }
    allocFree(graphics_render.p_frames);

    grpahicsRnederPassesTerminate();
    graphicsApiSwapchainTerminate();
}

#endif