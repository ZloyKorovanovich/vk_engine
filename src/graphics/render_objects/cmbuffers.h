#ifndef _GRAPHICS_RENDER_OBJECTS_CMBUFFERS_INCLUDED
#define _GRAPHICS_RENDER_OBJECTS_CMBUFFERS_INCLUDED
// implements cmbuffers render objects

#include "frame.h"
#include "../api/command.h"

// describes command buffers arrays
struct {
    uint32_t buffer_count;
    VkCommandBuffer* p_graphics_buffers;
} graphics_cmbuffers;

// starts cmbuffers lifetime
void graphicsRenderObjectsCmbuffersInit(
    uint32_t pool_id
) {
    // allocate buffer arrays for all frames
    graphics_cmbuffers.buffer_count = graphics_frame.max_frames_in_flight;
    graphics_cmbuffers.p_graphics_buffers = (VkCommandBuffer*)allocMalloc(graphics_cmbuffers.buffer_count * sizeof(VkCommandBuffer));
    graphicsApiCmbufferCreate(
        pool_id,
        graphics_cmbuffers.buffer_count,
        graphics_cmbuffers.p_graphics_buffers
    );
}

// ends cmbuffers lifetime
void graphicsRenderObjectsCmbuffersTerminate() {
    allocFree(graphics_cmbuffers.p_graphics_buffers);
}

#endif