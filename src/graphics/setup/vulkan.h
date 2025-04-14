#ifndef _GRAPHICS_VULKAN_INCLUDED
#define _GRAPHICS_VULKAN_INCLUDED
// implements vulkan instance an validation layers
// do "#define VALIDATION_LAYERS" before this header included to turn layers on

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <string.h>

#include "window.h"
#include "../../utilis/alloc.h"
#include "../../utilis/error.h"
#include "../../utilis/str.h"


// describes vulkan instance
static struct {
    VkInstance instance;
} graphics_vulkan;

// validation layers-only
#ifdef VALIDATION_LAYERS
// describes validation layers and related data
// has pre-initialized fields
struct {
    const uint32_t count;
    const char* pp_layers[1]; // layer names
    const VkDebugUtilsMessageSeverityFlagsEXT severity; // if message severity less, then won't be printed
    VkDebugUtilsMessengerEXT debug_messenger;
} validation_layers = {
    .count = 1,
    .pp_layers = {"VK_LAYER_KHRONOS_validation"},
    .severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
};

// returns true if vulkan configuration supports validation layers, else false
bool graphicsApiCheckLayersSupport() {
    // get supproted layers
    uint32_t layers_count;
    vkEnumerateInstanceLayerProperties(&layers_count, NULL);
    VkLayerProperties p_layers[layers_count];
    vkEnumerateInstanceLayerProperties(&layers_count, p_layers);
    // check if all of required layers match supproted layers
    for(size_t i = 0; i < validation_layers.count; i++) {
        bool matched = false;
        for(size_t j = 0; j < layers_count; j++) {
            if(strcmp(validation_layers.pp_layers[i], p_layers[j].layerName) == 0) {
                matched = true;
                break;
            }
        }
        if(!matched) {
            return false;
        }
    }
    return true;
}

// callback function for validation layers, prints messages
VKAPI_ATTR VkBool32 VKAPI_CALL graphicsDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
    void* p_user_data
) {
    // check if message is important enough to show
    if (message_severity >= validation_layers.severity) {
        printf("%s\n", p_callback_data->pMessage);
    }
    return VK_FALSE;
}

// wraper for vkCreateDebugUtilsMessengerEXT
VkResult graphicsApiCreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* p_createinfo, 
    const VkAllocationCallbacks* p_allocator, 
    VkDebugUtilsMessengerEXT* p_debugMessenger
) {

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, p_createinfo, p_allocator, p_debugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// wraper for vkDestroyDebugUtilsMessengerEXT
void graphicsApiDestroyDebugUtilsMessengerEXT(
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debug_messenger, 
    const VkAllocationCallbacks* p_allocator
) {

    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, p_allocator);
    }
}

// creates info for debug messenger creation (used to debug vulkan instance creation)
VkDebugUtilsMessengerCreateInfoEXT graphicsApiDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT create_info = (VkDebugUtilsMessengerCreateInfoEXT){0};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = graphicsDebugCallback;
    return create_info;
}

// creates actual debug messnger
void graphicsApiSetupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT create_info = graphicsApiDebugMessengerCreateInfo();
    if (graphicsApiCreateDebugUtilsMessengerEXT(graphics_vulkan.instance, &create_info, NULL, &validation_layers.debug_messenger) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE DEBUG MESSENGER!")
    }
}
#endif

// returns true if extension is supported by vulkan configuration, else false
bool graphicsCheckVulkanExtension(
    const char* p_extension
) {
    // get supproted vulkan extensions
    uint32_t vulkan_extension_count;
    vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, NULL);
    VkExtensionProperties p_vulkan_extensions[vulkan_extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, p_vulkan_extensions);
    // check if extension match vulkan extensions
    for(size_t i = 0; i < vulkan_extension_count; i++) {
        if(strcmp(p_extension, p_vulkan_extensions[i].extensionName) == 0) {
            return true;
        }
    }
    return false;
}

// returns true if all of the extensions are supported by vulkan configuration, else false
bool graphicsCheckVulkanExtensions(
    const uint32_t extension_count,
    const char** pp_extensions
) {
    // get supported vulkan extensions
    uint32_t vulkan_extension_count;
    vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, NULL);
    VkExtensionProperties p_vulkan_extensions[vulkan_extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &vulkan_extension_count, p_vulkan_extensions);
    // check if required extensions match supproted extensions
    for(size_t i = 0; i < extension_count; i++) {
        bool matched = false;
        for(size_t j = 0; j < vulkan_extension_count; j++) {
            if(strcmp(pp_extensions[i], p_vulkan_extensions[j].extensionName) == 0) {
                matched = true;
                break;
            }
        }
        if(!matched) {
            return false;
        }
    }
    return true;
}

// starts vulkan instance lifetime (and validation layers if defined macro)
void graphicsVulkanInit() {
#ifdef VALIDATION_LAYERS // check if validation layers are supported
    if(!graphicsApiCheckLayersSupport()) {
        ERROR_FATAL("VALIDATION LAYERS NOT SUPPORTED")
    }
#endif

    // application information
    VkApplicationInfo app_info = (VkApplicationInfo){0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "vk_engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = (VkInstanceCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    //find required extensions
    uint32_t glfw_extension_count;
    const char** pp_glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
#ifdef VALIDATION_LAYERS // glfw extensions + 2
    uint32_t required_extension_count = glfw_extension_count + 2;
    const char* pp_required_extensions[required_extension_count];
    memcpy(pp_required_extensions, pp_glfw_extensions, glfw_extension_count * sizeof(const char*)); // puts glfw extensions into required
    pp_required_extensions[glfw_extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    pp_required_extensions[glfw_extension_count + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // for validation layers
#else // glfw extensions + 1
    uint32_t required_extension_count = glfw_extension_count + 1;
    const char* pp_required_extensions[required_extension_count];
    memcpy(pp_required_extensions, pp_glfw_extensions, glfw_extension_count * sizeof(const char*)); // puts glfw extensions into required
    pp_required_extensions[glfw_extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
#endif
    // check if required extensions are supproted, thats crusial for program
    if(!graphicsCheckVulkanExtensions(
        required_extension_count, 
        pp_required_extensions
    )) {
        ERROR_FATAL("FAILED TO MATCH EXTENSIONS")
    }

    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.enabledExtensionCount = required_extension_count;
    create_info.ppEnabledExtensionNames = pp_required_extensions;

#ifdef VALIDATION_LAYERS // add validation layers and debug messenger to create info
    create_info.enabledLayerCount = validation_layers.count;
    create_info.ppEnabledLayerNames = validation_layers.pp_layers;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = graphicsApiDebugMessengerCreateInfo(); // used to debug instance creation
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info; // used to debug instance creation
#else // no validation layers
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;
#endif
    // creates vulkan instance
    if (vkCreateInstance(&create_info, NULL, &graphics_vulkan.instance) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE VULKAN INSTANCE")
    }

#ifdef VALIDATION_LAYERS // actual creation of debug messenger
    graphicsApiSetupDebugMessenger();
#endif
}

// ends vulkan instance lifetime (and validation layers if defined macro)
void graphicsVulkanTerminate() {
#ifdef VALIDATION_LAYERS // destroy debug messenger
    graphicsApiDestroyDebugUtilsMessengerEXT(graphics_vulkan.instance, validation_layers.debug_messenger, NULL);
#endif
    vkDestroyInstance(graphics_vulkan.instance, NULL);
}

#endif