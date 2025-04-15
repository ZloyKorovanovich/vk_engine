#ifndef _GRAPHICS_API_GPU_INCLUDED
#define _GRAPHICS_API_GPU_INCLUDED
// implements usage of gpu aka physical device
// this module does not have init and terminate functions, but is necessary to use for graphics
// should be initialized with graphicsApiPickGPU fucntion call

#include "surface.h"

// describes gpu
static struct {
    VkPhysicalDevice gpu;
} graphics_api_gpu;


// returns true if extension is supported by gpu, else false
bool graphicsApiGpuCheckExtension(
    const VkPhysicalDevice gpu,
    const char* p_extension
) {
    // get gpu extensions
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, NULL);
    VkExtensionProperties p_device_extensions[device_extension_count];
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, p_device_extensions);
    // check if required extension matches gpu extensions
    for(size_t j = 0; j < device_extension_count; j++) {
        if(strcmp(p_extension, p_device_extensions[j].extensionName) == 0) {
            return true;
            break;
        }
    }
    return false;
}

// returns true if all of extensions are supported by gpu, else false
bool graphicsGpuCheckExtensions(
    const VkPhysicalDevice gpu,
    const uint32_t extension_count,
    const char** p_extensions
) {
    // get gpu extensions
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, NULL);
    VkExtensionProperties p_device_extensions[device_extension_count];
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, p_device_extensions);
    // check if required extensions match gpu extensions
    for(size_t i = 0; i < extension_count; i++) {
        bool matched = false;
        for(size_t j = 0; j < device_extension_count; j++) {
            if(strcmp(p_extensions[i], p_device_extensions[j].extensionName) == 0) {
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

// returns true if all of graphics families exist on gpu, else false
bool graphicsApiGpuCheckQueueFamilies(
    const VkPhysicalDevice gpu,
    const uint32_t queue_family_count,
    const VkQueueFlagBits* p_queue_family_flags
) {
    // get gpu queue families
    uint32_t families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &families_count, NULL);
    VkQueueFamilyProperties p_families[families_count];
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &families_count, p_families);
    // check if suitable queue families exist
    uint32_t queue_count_local[families_count] = {}; // calculate queue count per family
    for(uint32_t i = 0; i < queue_family_count; i++) {
        bool match = true;
        for(uint32_t j = 0; j < families_count; j++) {
            // if flags are suitable and empty slots for queues in family exist
            if((p_queue_family_flags[i] & p_families[j].queueFlags) 
                && (queue_count_local[j] < p_families[j].queueCount)) {
                queue_count_local[j]++;
                match = true;
                break;
            }
        }
        if(!match) {
            return false;
        }
    }

    // check present queue support (required for presentation)
    VkBool32 present_supproted;
    for(uint32_t i = 0; i < families_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, graphics_api_surface.surface, &present_supproted);
        if(present_supproted) {
            return true;
        }
    }
    return false;
}

// returns true if swapchain is supported by gpu, else false
bool graphicsApiGpuCheckSwapchain(
    const VkPhysicalDevice gpu
) {
    // get device extensions
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, NULL);
    VkExtensionProperties p_device_extensions[device_extension_count];
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, p_device_extensions);
    // check if swapchain exists in supproted extensions
    bool found = false;
    for(size_t i = 0; i < device_extension_count; i++) {
        if(strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, p_device_extensions[i].extensionName) == 0) {
            found = true;
            break;
        }
    }
    if(!found) {
        return false;
    }
    // get supported format and mode count
    uint32_t format_count, mode_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, graphics_api_surface.surface, &format_count, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, graphics_api_surface.surface, &mode_count, NULL);
    // check if swapchain supports any mode and format
    return !((format_count == 0) && (mode_count == 0));
}

// default function that helps to select best gpu
uint32_t graphicsApiGpuEvaluateDefault(
    const VkPhysicalDevice gpu
) {
    // get device information
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(gpu, &properties);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(gpu, &features);

    uint32_t score = 1;
    score += (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) * 1000; // separate gpu have better performance
    score += (features.tessellationShader) * 500; // tessellation support
    score += (features.geometryShader) * 500; // geometry shader support
    return score;
}

// returns true if those queues exist on gpu, and finds there information, else returns false
bool graphicsApiGpuFindQueueIndices(
    const uint32_t queue_count,
    const VkQueueFlagBits* p_queue_flags,
    uint32_t* p_queue_family_indices,
    uint32_t* p_queue_local_indices
) {
    // get gpu queue families
    uint32_t families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_api_gpu.gpu, &families_count, NULL);
    VkQueueFamilyProperties p_families[families_count];
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_api_gpu.gpu, &families_count, p_families);

    // find families and local indices
    for(uint32_t i = 0; i < queue_count; i++) {
        bool found = false;
        for(uint32_t j = 0; j < families_count; j++) {
            if(p_queue_flags[i] & p_families[j].queueFlags) {
                uint32_t local_index = 0; // index of queue in family
                for(uint32_t k = 0; k < i; k++) {
                    if(p_queue_family_indices[k] == j){
                        local_index++;
                    }
                }
                // if local index is grater than queue count in family, need to find another queue
                if(local_index < p_families[i].queueCount) {
                    p_queue_family_indices[i] = j;
                    p_queue_local_indices[i] = local_index;
                    found = true;
                    break;
                }
            }
        }
        if(!found) {
            return false; // if no matching, then failed to find
        }
    }
    return true; // success
}

// finds queue that supports presentation
bool graphicsApiGpuFindPresentQueue(
    uint32_t* const p_family_id,
    uint32_t* const p_local_id
) {
    // get gpu queue families
    uint32_t families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_api_gpu.gpu, &families_count, NULL);
    VkQueueFamilyProperties p_families[families_count];
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_api_gpu.gpu, &families_count, p_families);
    // find family, that supports presentation
    VkBool32 present_supproted;
    for(uint32_t i = 0; i < families_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(graphics_api_gpu.gpu, i, graphics_api_surface.surface, &present_supproted);
        if(present_supproted) {
            *p_family_id = i;
            *p_local_id = 0; // use 1-st queue from family (can be different)
            return true;
        }
    }
    return false;
}

// selects suitable gpu from a list of available based on evaluation function (for default use graphicsApiGpuEvaluateDefault())
void graphicsApiGPUPick(
    const uint32_t required_queue_family_count,
    const VkQueueFlagBits* p_queue_family_flags,
    uint32_t (*p_evaluate)(const VkPhysicalDevice)
) {
    uint32_t devices_count = 0;
    vkEnumeratePhysicalDevices(graphics_api_vulkan.instance, &devices_count, NULL);
    if (devices_count == 0) {
        ERROR_FATAL("FAILED TO FIND ANY GPU")
    }
    VkPhysicalDevice p_devices[devices_count];
    vkEnumeratePhysicalDevices(graphics_api_vulkan.instance, &devices_count, p_devices);
    
    uint32_t best_score = 0;
    for(uint32_t i = 0; i < devices_count; i++) {
        if(!graphicsApiGpuCheckQueueFamilies(
            p_devices[i], 
            required_queue_family_count, 
            p_queue_family_flags
        )) continue;

        if(!graphicsApiGpuCheckSwapchain(
            p_devices[i]
        )) continue;

        uint32_t score = (*p_evaluate)(p_devices[i]);
        if(score >= best_score) {
            best_score = score;
            graphics_api_gpu.gpu = p_devices[i];
        }
    }
    if (graphics_api_gpu.gpu == VK_NULL_HANDLE) {
        ERROR_FATAL("NO GPU IS SUITABLE")
    }
}

#endif