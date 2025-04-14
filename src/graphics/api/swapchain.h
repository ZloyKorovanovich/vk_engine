#ifndef _GRAPHICS_API_SWAPCHAIN_INCLUDED
#define _GRAPHICS_API_SWAPCHAIN_INCLUDED
// implements swapchain

#include "device.h"
#include "../../utilis/imath.h"

// describes swapchain and arrays of images
static struct {
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR format; // format which is used by swapchain
    VkPresentModeKHR mode; // present mode which is used by swapchain
    VkExtent2D extent;

    // images configuration might change in runtime with window resize
    uint32_t image_count;
    VkImage* p_images;
    VkImageView* p_views; // views to images used by framebuffers

    // viewport configuration, might change in runtime with window resize
    VkViewport viewport;
    VkRect2D scissors;
} graphics_swapchain;


// default function for selecting best format from given (meant to used as argument graphicsSwapchainInit())
VkSurfaceFormatKHR graphicsSwapchainDefaultSelectFormat(
    const uint32_t format_count, 
    const VkSurfaceFormatKHR* p_formats
) {
    for(uint32_t i = 0; i < format_count; i++) {
        if (p_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && p_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return p_formats[i];
        }
    }
    return p_formats[0];
}

// default function for selecting present mode from given (meant to used as argument graphicsSwapchainInit())
VkPresentModeKHR graphicsSwapchainDefaultSelectMode(
    const uint32_t mode_count, 
    const VkPresentModeKHR* p_modes
) {
    for(uint32_t i = 0; i < mode_count; i++) {
        if(p_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return p_modes[i];
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

// updates viewport and scissors settings
void graphicsSurfaceUpdateViewport() {
    graphics_swapchain.viewport.x = 0.0f;
    graphics_swapchain.viewport.y = 0.0f;
    graphics_swapchain.viewport.width = (float)graphics_swapchain.extent.width;
    graphics_swapchain.viewport.height = (float)graphics_swapchain.extent.height;
    graphics_swapchain.viewport.minDepth = 0.0f;
    graphics_swapchain.viewport.maxDepth = 1.0f;

    graphics_swapchain.scissors.extent = graphics_swapchain.extent;
    graphics_swapchain.scissors.offset = (VkOffset2D){0, 0};
}

// creates swapcahin images
void graphicsSwapchainCreateImages() {
    vkGetSwapchainImagesKHR(
        graphics_device.device,
        graphics_swapchain.swapchain,
        &graphics_swapchain.image_count,
        NULL
    );
    graphics_swapchain.p_images = allocMalloc(graphics_swapchain.image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(
        graphics_device.device, 
        graphics_swapchain.swapchain, 
        &graphics_swapchain.image_count, 
        graphics_swapchain.p_images
    );

    graphics_swapchain.p_views = allocMalloc(graphics_swapchain.image_count * sizeof(VkImageView));
    for (size_t i = 0; i < graphics_swapchain.image_count; i++) {
        VkImageViewCreateInfo create_info = (VkImageViewCreateInfo){0};

        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = graphics_swapchain.p_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = graphics_swapchain.format.format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(graphics_device.device, &create_info, NULL, &graphics_swapchain.p_views[i]) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE SWAP CHAIN VIEWS");
        }
    }
}

// destroy swapchain images
void graphicsSwapchainDestroyImages() {
    for (uint32_t i = 0; i < graphics_swapchain.image_count; i++) {
        vkDestroyImageView(graphics_device.device, graphics_swapchain.p_views[i], NULL);
    }
    allocFree(graphics_swapchain.p_images);
    allocFree(graphics_swapchain.p_views);
}

// starts swapchain lifetime
void graphicsSwapchainInit(
    const VkSharingMode sharing_mode,
    VkSurfaceFormatKHR (*p_select_format)(const uint32_t, const VkSurfaceFormatKHR*),
    VkPresentModeKHR (*p_select_mode)(const uint32_t, const VkPresentModeKHR*)
) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphics_gpu.gpu, graphics_surface.surface, &capabilities);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics_gpu.gpu, graphics_surface.surface, &format_count, NULL);
    VkSurfaceFormatKHR p_formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics_gpu.gpu, graphics_surface.surface, &format_count, p_formats);
    uint32_t mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(graphics_gpu.gpu, graphics_surface.surface, &mode_count, NULL);
    VkPresentModeKHR p_modes[mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(graphics_gpu.gpu, graphics_surface.surface, &mode_count, p_modes);

    graphics_swapchain.format = (*p_select_format)(format_count, p_formats);
    graphics_swapchain.mode = (*p_select_mode)(mode_count, p_modes);

    // calculate image resolution
    graphics_swapchain.extent = capabilities.currentExtent;
    if (graphics_swapchain.extent.width == UINT32_MAX || graphics_swapchain.extent.height == UINT32_MAX) {
        int width, height;
        glfwGetFramebufferSize(graphics_window.p_window, &width, &height);
        graphics_swapchain.extent.width = imathU32Clamp((uint32_t)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        graphics_swapchain.extent.height = imathU32Clamp((uint32_t)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    // calculate image count
    uint32_t min_image_count = capabilities.minImageCount + 1;
    if ((capabilities.maxImageCount > 0) && (min_image_count > capabilities.maxImageCount)) {
        min_image_count = capabilities.maxImageCount;
    }

    uint32_t unique_family_count = 0;
    uint32_t p_unique_families[graphics_device.queue_count];
    for(uint32_t i = 0; i < graphics_device.queue_count; i++) {
        bool found = false;
        for(uint32_t j = 0; j < i; j++) {
            if(graphics_device.p_queue_families[i] == graphics_device.p_queue_families[j]) {
                found = true;
                break;
            }
        }
        if(!found) {
            p_unique_families[unique_family_count] = graphics_device.p_queue_families[i];
            unique_family_count++;
        }
    }

    VkSwapchainCreateInfoKHR create_info = (VkSwapchainCreateInfoKHR){0};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = graphics_surface.surface;
    create_info.minImageCount = min_image_count;
    create_info.imageFormat = graphics_swapchain.format.format;
    create_info.imageColorSpace = graphics_swapchain.format.colorSpace;
    create_info.imageExtent = graphics_swapchain.extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    create_info.imageSharingMode = sharing_mode;
    create_info.queueFamilyIndexCount = unique_family_count;
    create_info.pQueueFamilyIndices = p_unique_families;

    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = graphics_swapchain.mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(graphics_device.device, &create_info, NULL, &graphics_swapchain.swapchain) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE SWAPCHAIN")
    }
    graphicsSwapchainCreateImages();
    graphicsSurfaceUpdateViewport();
}

// ends swapchain lifetime
void graphicsSwapchainTerminate() {
    graphicsSwapchainDestroyImages();
    vkDestroySwapchainKHR(graphics_device.device, graphics_swapchain.swapchain, NULL);
}

#endif