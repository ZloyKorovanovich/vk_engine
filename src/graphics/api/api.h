#ifndef _GRAPHICS_API_INCLUDED
#define _GRAPHICS_API_INCLUDED
// header includes all of setup-related headers
// this part of code provides highly abstract interface for rendering
// it was designed as highly configurable from outside

// most of headers in this folder will have one struct with module data and 2 main functions:
// ...Init(), that starts lifetime of module
// ...Terminate(), that ends lifetime of module

#define VALIDATION_LAYERS // enables debug utilit for vulkan api

#include "window.h"
#include "vulkan.h"
#include "surface.h"
#include "gpu.h"
#include "device.h"
#include "swapchain.h"
#include "shader.h"
#include "renderpasses.h"
#include "framebuffers.h"
#include "command.h"

#endif