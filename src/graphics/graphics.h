#ifndef _GRAPHICS_INCLUDED
#define _GRAPHICS_INCLUDED
// header includes all of the graphics-related headers
// implements modifialbe general graphics control

#include "setup/setup.h"
#include "sync.h"
#include "queues.h"
#include "render_passes/render_passes.h"


static struct {
    VkCommandBuffer graphics_buffer;
} graphics_cmd;

// starts graphics lifetime
void graphicsInit() {
    // setup
    graphicsWindowInit(800, 600, "vk_engine");
    graphicsVulkanInit();
    graphicsSurfaceInit();
    graphicsGPUPick(
        graphics_required_queues.queue_count, 
        graphics_required_queues.p_queue_flags, 
        &graphicsGpuEvaluateDefault
    );
    graphicsDeviceInit(
        graphics_required_queues.queue_count, 
        graphics_required_queues.p_queue_flags, 
        NULL, 
        0, 
        NULL
    );
    graphicsSwapchainInit(
        VK_SHARING_MODE_EXCLUSIVE, 
        &graphicsSwapchainDefaultSelectFormat, 
        &graphicsSwapchainDefaultSelectMode
    );
    // render passes
    graphicsPassesInit(passes_count);
    graphicsPipelinesInit(1);
    graphicsFramebuffersInit(1);
    renderPassesCreate();
    // command buffers and command pools
    graphicsCmpoolsInit();
    graphicsCmbufferCreate(
        GRAPHICS_QUEUE_ID,
        1,
        &graphics_cmd.graphics_buffer
    );
    // sync objects
    graphicsSyncInit();
}

// draw frame functions from interface meant to be inside of graphicsMainLoop
void graphicsDraw(double time, double delta) {
    vkWaitForFences(graphics_device.device, 1, &graphics_sync.in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(graphics_device.device, 1, &graphics_sync.in_flight_fence);
    uint32_t image_id;
    vkAcquireNextImageKHR(
        graphics_device.device, 
        graphics_swapchain.swapchain, 
        UINT64_MAX, 
        graphics_sync.image_available_semaphore, 
        VK_NULL_HANDLE, 
        &image_id
    );
    vkResetCommandBuffer(graphics_cmd.graphics_buffer, 0);
    graphicsCmbufferBegin(graphics_cmd.graphics_buffer);
    renderPassesExecute(graphics_cmd.graphics_buffer, image_id);
    graphicsCmbufferEnd(graphics_cmd.graphics_buffer);

    VkSemaphore p_wait_semaphores[1] = {graphics_sync.image_available_semaphore};
    VkPipelineStageFlags p_wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore p_signal_semaphores[1] = {graphics_sync.image_finished_semaphore};

    // need separation to header 
    // queue submition
    VkSubmitInfo submit_info = (VkSubmitInfo){0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = p_wait_semaphores;
    submit_info.pWaitDstStageMask = p_wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &graphics_cmd.graphics_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = p_signal_semaphores;
    if (vkQueueSubmit(graphics_device.p_queues[GRAPHICS_QUEUE_ID], 1, &submit_info, graphics_sync.in_flight_fence) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO SUBMIT COMMAND BUFFER");
    }

    // need separation to header
    // returning to swapchain and presentation
    VkPresentInfoKHR present_info = (VkPresentInfoKHR){0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = p_signal_semaphores;
    VkSwapchainKHR p_swapchains[1] = {graphics_swapchain.swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = p_swapchains;
    present_info.pImageIndices = &image_id;
    present_info.pResults = NULL;
    vkQueuePresentKHR(graphics_device.p_queues[graphics_device.p_present_indices[GRAPHICS_QUEUE_ID]], &present_info);
}

// runs whole graphics of application
void graphicsRun() {
    graphicsMainLoop(&graphicsDraw);
    vkDeviceWaitIdle(graphics_device.device);
}

// ends graphics lifetime
void graphicsTerminate() {
    graphicsSyncTerminate();
    graphicsCmpoolsTerminate();
    graphicsFramebuffersTerminate();
    graphicsPipelinesTerminate();
    graphicsPassesTerminate();
    graphicsSwapchainTerminate();
    graphicsDeviceTerminate();
    graphicsSurfaceTerminate();
    graphicsVulkanTerminate();
    graphicsWindowTerminate();
}

#endif