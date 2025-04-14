#ifndef _GRAPHICS_API_FRAMEBUFFERS_INCLUDED
#define _GRAPHICS_API_FRAMEBUFFERS_INCLUDED
// implements framebuffers storage

#include "renderpasses.h"

// describes framebuffers and layouts
static struct {
    uint32_t framebuffer_count;
    VkFramebuffer* p_framebuffers;
} graphics_framebuffers;

// binding local data
static struct {
    uint32_t iterator;
    uint32_t pass_id;
} _graphics_frambuffers_binding;


// starts framebuffers lifetime
void graphicsFramebuffersInit(
    uint32_t buffer_count
) {
    graphics_framebuffers.framebuffer_count = buffer_count * graphics_swapchain.image_count;
    _graphics_frambuffers_binding.iterator = 0;
    _graphics_frambuffers_binding.pass_id = 0;
    if(graphics_framebuffers.framebuffer_count != 0) {
        graphics_framebuffers.p_framebuffers = (VkFramebuffer*)allocMalloc(graphics_framebuffers.framebuffer_count * sizeof(VkFramebuffer));
    }
    else {
        graphics_framebuffers.p_framebuffers = NULL;
    }
}

// binds render pass
void graphicsFramebuffersBindRenderPass(
    uint32_t pass_id
) {
    _graphics_frambuffers_binding.pass_id = pass_id;
}

// adds framebuffers to iterator location and adjusts iterator
void graphicsFramebuffersAdd() {
    if(_graphics_frambuffers_binding.iterator >= graphics_framebuffers.framebuffer_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PIPELINES")
    }

    uint32_t start = _graphics_frambuffers_binding.iterator;
    for(uint32_t i = start; i < start + graphics_swapchain.image_count; i++) {
        // will be changed in the future to add depth support
        VkFramebufferCreateInfo frame_buffer_info = (VkFramebufferCreateInfo){0};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.renderPass = graphics_passes.p_passes[_graphics_frambuffers_binding.pass_id];
        frame_buffer_info.attachmentCount = 1;
        frame_buffer_info.pAttachments = graphics_swapchain.p_views + i;
        frame_buffer_info.width = graphics_swapchain.extent.width;
        frame_buffer_info.height = graphics_swapchain.extent.height;
        frame_buffer_info.layers = 1;

        if (vkCreateFramebuffer(graphics_device.device, &frame_buffer_info, NULL, graphics_framebuffers.p_framebuffers + i) != VK_SUCCESS) {
            ERROR_FATAL("FAILED TO CREATE FRAME BUFFER!")
        }
    }
}

// ends framebuffers lifetime
void graphicsFramebuffersTerminate() {
    for(uint32_t i = 0; i < graphics_framebuffers.framebuffer_count; i++) {
        vkDestroyFramebuffer(graphics_device.device, graphics_framebuffers.p_framebuffers[i], NULL);
    }
    if(graphics_framebuffers.p_framebuffers != NULL) {
        allocFree(graphics_framebuffers.p_framebuffers);
    }
}

#endif