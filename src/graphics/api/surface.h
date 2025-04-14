#ifndef _GRAPHICS_API_SURFACE_INCLUDED
#define _GRAPHICS_API_SURFACE_INCLUDED
// implements surface, which is necessary for presentation

#include "vulkan.h"

// describes surface
static struct {
    VkSurfaceKHR surface;
} graphics_surface;

// starts surface lifetime
void graphicsSurfaceInit() {
    if (glfwCreateWindowSurface(graphics_vulkan.instance, graphics_window.p_window, NULL, &graphics_surface.surface) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE VULKAN SURFACE")
    }
}

// ends surface lifetime
void graphicsSurfaceTerminate() {
    vkDestroySurfaceKHR(graphics_vulkan.instance, graphics_surface.surface, NULL);
}

#endif