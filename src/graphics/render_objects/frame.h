#ifndef _GRAPHICS_RENDER_OBJECTS_FRAME_INCLUDED
#define _GRAPHICS_RENDER_OBJECTS_FRAME_INCLUDED
// implements frame render object

#include <stdint.h>

// describes render frames
static struct {
    const uint32_t max_frames_in_flight;
    uint32_t frame_id;
} graphics_frame = {
    .max_frames_in_flight = 2,
    .frame_id = 0
};

// switches render to next frame
void graphicsRenderObjectsFrameNext() {
    graphics_frame.frame_id = (graphics_frame.frame_id + 1) % graphics_frame.max_frames_in_flight;
}

#endif