#ifndef _GRAPHICS_INCLUDED
#define _GRAPHICS_INCLUDED

#include "api/api.h"
#include "setup.h"
#include "render.h"

void graphicsOnResize(
    uint32_t width,
    uint32_t height
) {
    vkDeviceWaitIdle(graphics_api_device.device);
    graphicsRenderTerminate();
    graphicsRenderInit();
}

void graphicsInit(void) {
    graphicsApiWindowInit(
        800, 
        600, 
        "vk_engine", 
        true, 
        false,
        &graphicsOnResize
    );
    graphicsApiVulkanInit();
    graphicsApiSurfaceInit();
    graphicsApiGPUPick(
        graphics_setup.queue_count, 
        graphics_setup.p_queue_flags, 
        &graphicsApiGpuEvaluateDefault
    );
    graphicsApiDeviceInit(
        graphics_setup.queue_count, 
        graphics_setup.p_queue_flags, 
        NULL, 
        0, 
        NULL
    );
    graphicsApiCmpoolsInit();
    graphicsRenderInit();
}

void graphicsMainLoop(void) {
    graphicsApiMainLoop(&graphicsRenderDraw);
    vkDeviceWaitIdle(graphics_api_device.device);
}

void graphicsTerminate(void) {
    graphicsRenderTerminate();

    graphicsApiCmpoolsTerminate();
    graphicsApiDeviceTerminate();
    graphicsApiSurfaceTerminate();
    graphicsApiVulkanTerminate();
    graphicsApiWindowTerminate();
}

#endif