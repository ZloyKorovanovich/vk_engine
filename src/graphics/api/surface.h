#ifndef _GRAPHICS_API_SURFACE_INCLUDED
#define _GRAPHICS_API_SURFACE_INCLUDED
// implements surface, which is necessary for presentation

#include "vulkan.h"

// describes surface
static struct {
    VkSurfaceKHR surface;
} graphics_api_surface;

// starts surface lifetime
void graphicsApiSurfaceInit() {
    if (glfwCreateWindowSurface(graphics_api_vulkan.instance, graphics_api_window.p_window, NULL, &graphics_api_surface.surface) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE VULKAN SURFACE")
    }
}

// ends surface lifetime
void graphicsApiSurfaceTerminate() {
    vkDestroySurfaceKHR(graphics_api_vulkan.instance, graphics_api_surface.surface, NULL);
}

#endif