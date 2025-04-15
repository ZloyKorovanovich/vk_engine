#ifndef _GRAPHICS_API_DEVICE_INCLUDED
#define _GRAPHICS_API_DEVICE_INCLUDED
// implements vulkan logical device aka VkDevice
// also cares about logical queues

#include "gpu.h"

// describes graphics logical device and queues
static struct {
    VkDevice device;
    uint32_t queue_count;
    VkQueue* p_queues; // element i points to queue it self
    uint32_t* p_queue_families; // element i points to family index of queue
    uint32_t* p_present_indices; // element i points to best queue for presentation (best case same queue as i)
} graphics_api_device;


// finds information about unique families and how many queues they should have
// if arrays are NULL, sets only queue_count
void graphicsApiDeviceFindUniqueFamilies(
    const uint32_t queue_count,
    const uint32_t* p_families,
    uint32_t* p_unique_family_count,
    uint32_t* p_uinque_families,
    uint32_t* p_unique_family_index_counts
) {
    *p_unique_family_count = 0;
    for(uint32_t i = 0; i < queue_count; i++) {
        bool same_family = false;
        for(uint32_t j = 0; j < i; j++) {
            if(p_families[i] == p_families[j]) {
                same_family = true;
                break;
            }
        }
        if(same_family) {
            if(p_uinque_families != NULL && p_unique_family_index_counts != NULL) {
                uint32_t family_id;
                for(uint32_t k = 0; k < *p_unique_family_count; k++) {
                    if(p_uinque_families[k] == p_families[i]) {
                        family_id = k;
                        break;
                    }
                }
                p_unique_family_index_counts[family_id]++;
            }
        }
        else {
            if(p_uinque_families != NULL && p_unique_family_index_counts != NULL) {
                p_uinque_families[*p_unique_family_count] = p_families[i];
                p_unique_family_index_counts[*p_unique_family_count] = 1;
            }
            (*p_unique_family_count)++;
        }
    }
}

// starts graphics device lifetime
void graphicsApiDeviceInit(
    const uint32_t queue_count,
    const VkQueueFlagBits* p_queue_flags,
    const VkPhysicalDeviceFeatures* p_device_features,
    const uint32_t device_extension_count,
    const char** pp_device_extensions
) {
    // allocate queue indices
    uint32_t total_queue_count = queue_count + 1; // present queue treated always separately
    uint32_t p_queue_family_indices[total_queue_count];
    uint32_t p_queue_local_indices[total_queue_count];
    graphicsApiGpuFindQueueIndices(
        queue_count,
        p_queue_flags,
        p_queue_family_indices,
        p_queue_local_indices
    );
    // find present queue
    uint32_t p_queue_present_indices[total_queue_count];
    graphicsApiGpuFindPresentQueue(
        p_queue_family_indices + queue_count,
        p_queue_local_indices + queue_count
    );
    // link each queue to present queue which is itself or specially found one (id: queue_count + 1)
    for(uint32_t i = 0; i < queue_count; i++) {
        VkBool32 is_present;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            graphics_api_gpu.gpu, 
            p_queue_family_indices[i], 
            graphics_api_surface.surface,
            &is_present
        );
        if(is_present) {
            p_queue_present_indices[i] = i;
        }
        else {
            p_queue_present_indices[i] = queue_count;
        }
    }

    // find unique queues for create infos
    uint32_t unique_queue_count;
    uint32_t p_unique_families[total_queue_count];
    uint32_t p_unique_family_index_counts[total_queue_count];
    graphicsApiDeviceFindUniqueFamilies(
        total_queue_count,
        p_queue_family_indices,
        &unique_queue_count,
        p_unique_families,
        p_unique_family_index_counts
    );
    // create infos for queues (for each unique queue)
    VkDeviceQueueCreateInfo p_queue_infos[unique_queue_count];
    uint32_t max_local_index_count = 0;
    for(uint32_t i = 0; i < unique_queue_count; i++) {
        if(p_unique_family_index_counts[i] > max_local_index_count) {
            max_local_index_count = p_unique_family_index_counts[i];
        }
    }
    float p_queue_priorities[max_local_index_count]; // allocate max size of possible index count
    memset(p_queue_priorities, 1.0f, max_local_index_count *  sizeof(float)); // set all priorities to 1
    for(uint32_t i = 0; i < unique_queue_count; i++) {
        p_queue_infos[i] = (VkDeviceQueueCreateInfo){0};
        p_queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        p_queue_infos[i].queueFamilyIndex = p_unique_families[i];
        p_queue_infos[i].queueCount = p_unique_family_index_counts[i];
        p_queue_infos[i].pQueuePriorities = p_queue_priorities;
    }

    uint32_t device_extension_full_count = device_extension_count + 1; // including swapchain
    const char* pp_device_extensions_full[device_extension_full_count];
    // copy extensions set from external
    if(device_extension_count > 0) {
        memcpy(pp_device_extensions_full, pp_device_extensions, device_extension_count * sizeof(const char*));
    }
    pp_device_extensions_full[device_extension_count] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    // device create info
    VkDeviceCreateInfo create_info = (VkDeviceCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = p_queue_infos;
    create_info.queueCreateInfoCount = unique_queue_count;
    create_info.pEnabledFeatures = p_device_features;
    create_info.enabledExtensionCount = device_extension_full_count;
    create_info.ppEnabledExtensionNames = pp_device_extensions_full;

    if (vkCreateDevice(graphics_api_gpu.gpu, &create_info, NULL, &graphics_api_device.device) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE LOGICAL DEVICE")
    }
    // allocate queue arrays on heap
    graphics_api_device.queue_count = total_queue_count;
    graphics_api_device.p_queues = allocMalloc(graphics_api_device.queue_count * sizeof(VkQueue));
    graphics_api_device.p_queue_families = allocMalloc(graphics_api_device.queue_count * sizeof(uint32_t));
    graphics_api_device.p_present_indices = allocMalloc(graphics_api_device.queue_count * sizeof(uint32_t));
    // copy from stack queue arrays to heap queue arrays
    for(uint32_t i = 0; i < graphics_api_device.queue_count; i++) {
        vkGetDeviceQueue(graphics_api_device.device, p_queue_family_indices[i], p_queue_local_indices[i], graphics_api_device.p_queues + i);
        graphics_api_device.p_queue_families[i] = p_queue_family_indices[i];
        graphics_api_device.p_present_indices[i] = p_queue_present_indices[i];
    }
}

// ends graphics device lifetime
void graphicsApiDeviceTerminate() {
    // free queue arrays
    allocFree(graphics_api_device.p_queues);
    allocFree(graphics_api_device.p_queue_families);
    allocFree(graphics_api_device.p_present_indices);
    vkDestroyDevice(graphics_api_device.device, NULL);
}

#endif