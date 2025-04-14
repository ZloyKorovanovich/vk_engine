#ifndef _GRAPHICS_WINDOW_INCLUDED
#define _GRAPHICS_WINDOW_INCLUDED
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
} graphics_window;


// returns true if extensions is supported by glfw, else false
bool graphicsCheckWindowExtension(
    const char* p_extension
) {
    return glfwExtensionSupported(p_extension);
}

// returns true if all of the extensions are supported by glfw, else false
bool graphicsCheckWindowExtensions(
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


// starts window lifetime
void graphicsWindowInit(
    const uint32_t width, 
    const uint32_t height,
    const char* name
) {
    glfwInit();
    // window parameters
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    graphics_window.p_window = glfwCreateWindow(width, height, name, NULL, NULL);
}

// tuns main loop of glfw (graphics game loop function)
void graphicsMainLoop(
    void (*p_loop) (double time, double delta)
) {
    while (!glfwWindowShouldClose(graphics_window.p_window)) {
        glfwPollEvents();
        (*p_loop)(0, 0); //should be replaced with actual time and delta time
    }
}

// ends window lifetime
void graphicsWindowTerminate() {
    glfwDestroyWindow(graphics_window.p_window);
    glfwTerminate();
}

#endif