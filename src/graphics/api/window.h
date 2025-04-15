#ifndef _GRAPHICS_API_WINDOW_INCLUDED
#define _GRAPHICS_API_WINDOW_INCLUDED
// implements window interface
// also does makes a limited interface for interaction with glfw

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include "../../utilis/alloc.h"

// describes window
static struct {
    GLFWwindow* p_window;
    void (*p_on_resize)(uint32_t width, uint32_t height);
} graphics_api_window;


// returns true if extensions is supported by glfw, else false
bool graphicsApiCheckWindowExtension(
    const char* p_extension
) {
    return glfwExtensionSupported(p_extension);
}

// returns true if all of the extensions are supported by glfw, else false
bool graphicsApiCheckWindowExtensions(
    const uint32_t extension_count,
    const char** extensions
) {
    for(uint32_t i = 0; i < extension_count; i++) {
        if(!glfwExtensionSupported(extensions[i])) {
            return false;
        }
    }
    return true;
}

static void graphicsApiWindowResizeCallback(
    GLFWwindow* p_window, 
    int width, 
    int height
) {
    (*graphics_api_window.p_on_resize)((uint32_t)width, (uint32_t)height);
}

// starts window lifetime
void graphicsApiWindowInit(
    const uint32_t width,
    const uint32_t height,
    const char* name,
    const bool resizable,
    const bool fullscreen,
    void (*p_on_resize)(uint32_t width, uint32_t height)
) {
    glfwInit();
    // window parameters
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, (int)resizable);
    if(fullscreen) {
        graphics_api_window.p_window = glfwCreateWindow(width, height, name, glfwGetPrimaryMonitor(), NULL);
    }
    else {
        graphics_api_window.p_window = glfwCreateWindow(width, height, name, NULL, NULL);
    }
    glfwSetFramebufferSizeCallback(graphics_api_window.p_window, graphicsApiWindowResizeCallback);
    graphics_api_window.p_on_resize = p_on_resize;
}

// tuns main loop of glfw (graphics game loop function)
void graphicsApiMainLoop(
    void (*p_loop) (double time, double delta)
) {
    while (!glfwWindowShouldClose(graphics_api_window.p_window)) {
        glfwPollEvents();
        (*p_loop)(0, 0); //should be replaced with actual time and delta time
    }
}

// ends window lifetime
void graphicsApiWindowTerminate(void) {
    glfwDestroyWindow(graphics_api_window.p_window);
    glfwTerminate();
}

#endif