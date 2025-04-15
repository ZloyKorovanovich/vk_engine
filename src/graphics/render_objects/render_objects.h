#ifndef _GRAPHICS_RENDER_OBJECTS_INCLUDED
#define _GRAPHICS_RENDER_OBJECTS_INCLUDED

#include "frame.h"
#include "cmbuffers.h"
#include "sync.h"

// starts render objects lifetime
void graphicsRenderObjectsInit(
    uint32_t queue_id
) {
    graphicsRenderObjectsCmbuffersInit(queue_id);
    graphicsRenderObjectsSyncInit();
}

// ends render objects lifetime
void graphicsRenderObjectsTerminate() {
    graphicsRenderObjectsSyncTerminate();
    graphicsRenderObjectsCmbuffersTerminate();
}

#endif