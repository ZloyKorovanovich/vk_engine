#ifndef _GRAPHICS_INCLUDED
#define _GRAPHICS_INCLUDED
// header includes all of the graphics-related headers
// implements modifiable general graphics control

#include "api/api.h"
#include "render_objects/render_objects.h"
#include "render_passes/render_passes.h"

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

// starts graphics lifetime
void graphicsInit() {
    // setup
    graphicsApiWindowInit(800, 600, "vk_engine");
    graphicsApiVulkanInit();
    graphicsApiSurfaceInit();
    graphicsApiGPUPick(
        graphics_required_queues.queue_count, 
        graphics_required_queues.p_queue_flags, 
        &graphicsApiGpuEvaluateDefault
    );
    graphicsApiDeviceInit(
        graphics_required_queues.queue_count, 
        graphics_required_queues.p_queue_flags, 
        NULL, 
        0, 
        NULL
    );
    graphicsApiSwapchainInit(
        VK_SHARING_MODE_EXCLUSIVE, 
        &graphicsApiSwapchainDefaultSelectFormat, 
        &graphicsApiSwapchainDefaultSelectMode
    );
    // render passes
    graphicsApiPassesInit(passes_count);
    graphicsApiPipelinesInit(1);
    graphicsApiFramebuffersInit(1);
    renderPassesCreate();
    // command pools
    graphicsApiCmpoolsInit();

    graphicsRenderObjectsInit(GRAPHICS_QUEUE_ID);
}

// draw frame functions from interface meant to be inside of graphicsMainLoop
void graphicsDraw(double time, double delta) {
    vkWaitForFences(graphics_api_device.device, 1, &graphics_sync.p_in_flight_fences[graphics_frame.frame_id], VK_TRUE, UINT64_MAX);
    vkResetFences(graphics_api_device.device, 1, &graphics_sync.p_in_flight_fences[graphics_frame.frame_id]);
    uint32_t image_id;
    vkAcquireNextImageKHR(
        graphics_api_device.device, 
        graphics_api_swapchain.swapchain, 
        UINT64_MAX, 
        graphics_sync.p_image_available_semaphores[graphics_frame.frame_id], 
        VK_NULL_HANDLE, 
        &image_id
    );
    vkResetCommandBuffer(graphics_cmbuffers.p_graphics_buffers[graphics_frame.frame_id], 0);
    graphicsApiCmbufferBegin(graphics_cmbuffers.p_graphics_buffers[graphics_frame.frame_id]);
    renderPassesExecute(graphics_cmbuffers.p_graphics_buffers[graphics_frame.frame_id], image_id);
    graphicsApiCmbufferEnd(graphics_cmbuffers.p_graphics_buffers[graphics_frame.frame_id]);

    VkSemaphore p_wait_semaphores[1] = {graphics_sync.p_image_available_semaphores[graphics_frame.frame_id]};
    VkPipelineStageFlags p_wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore p_signal_semaphores[1] = {graphics_sync.p_image_finished_semaphores[graphics_frame.frame_id]};

    // need separation to header 
    // queue submition
    VkSubmitInfo submit_info = (VkSubmitInfo){0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = p_wait_semaphores;
    submit_info.pWaitDstStageMask = p_wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &graphics_cmbuffers.p_graphics_buffers[graphics_frame.frame_id];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = p_signal_semaphores;
    if (vkQueueSubmit(graphics_api_device.p_queues[GRAPHICS_QUEUE_ID], 1, &submit_info, graphics_sync.p_in_flight_fences[graphics_frame.frame_id]) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO SUBMIT COMMAND BUFFER");
    }

    // need separation to header
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
    vkQueuePresentKHR(graphics_api_device.p_queues[graphics_api_device.p_present_indices[GRAPHICS_QUEUE_ID]], &present_info);
    graphicsRenderObjectsFrameNext();
}

// runs whole graphics of application
void graphicsRun() {
    graphicsApiMainLoop(&graphicsDraw);
    vkDeviceWaitIdle(graphics_api_device.device);
}

// ends graphics lifetime
void graphicsTerminate() {
    graphicsRenderObjectsTerminate();
    graphicsApiCmpoolsTerminate();
    graphicsApiFramebuffersTerminate();
    graphicsApiPipelinesTerminate();
    graphicsApiPassesTerminate();
    graphicsApiSwapchainTerminate();
    graphicsApiDeviceTerminate();
    graphicsApiSurfaceTerminate();
    graphicsApiVulkanTerminate();
    graphicsApiWindowTerminate();
}

#endif