#ifndef _GRAPHICS_RENDER_OBJECTS_INCLUDED
#define _GRAPHICS_RENDER_OBJECTS_INCLUDED

#include "frame.h"
#include "cmbuffers.h"
#include "sync.h"

void graphicsRenderObjectsInit(
    uint32_t queue_id
) {
    graphicsRenderObjectsCmbuffersInit(queue_id);
    graphicsRenderObjectsSyncInit();
}

void graphicsRenderObjectsTerminate() {
    graphicsRenderObjectsSyncTerminate();
    graphicsRenderObjectsCmbuffersTerminate();
}

#endif