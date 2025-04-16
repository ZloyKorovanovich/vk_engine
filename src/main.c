#define ALLOCATION_DEBUG // enables debug utilis for allocation
#define VALIDATION_LAYERS // enables debug utilit for vulkan api

#include "utilis/alloc.h"
#include "resources/resources.h"
#include "graphics/graphics.h"

int main(int argv, char* argc[]) {
    allocTableInit(1024);
    resourcesRun();
    graphicsInit();
    graphicsMainLoop();
    graphicsTerminate();
    allocTableTerminate();
    
    return 0;
}