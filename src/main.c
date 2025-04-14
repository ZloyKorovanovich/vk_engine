#include "utilis/alloc.h"
#include "resources/resources.h"
#include "graphics/graphics.h"

int main(int argv, char* argc[]) {
    resourcesRun();
    graphicsInit();
    graphicsRun();
    graphicsTerminate();
    printf("total allocations: %llu end allocations: %llu \n", alloc_info.total_allocations, alloc_info.current_allocations);
    return 0;
}