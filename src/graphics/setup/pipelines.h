#ifndef _GRAPHICS_PIPELINES_INCLUDED
#define _GRAPHICS_PIPELINES_INCLUDED

#include <vulkan/vulkan.h>
#include "device.h"
#include "passes.h"
// implements render pipelines storage

// describes pipelines and layouts
static struct {
    uint32_t pipeline_count;
    VkPipeline* p_pipelines;
    VkPipelineLayout* p_layouts;
} graphics_pipelines;

// binding local data
static struct {
    uint32_t iterator;
    uint32_t pass_id;
    uint32_t subpass;
} _graphics_pipelines_binding;


// starts pipelines lifetime
void graphicsPipelinesInit(
    uint32_t pipeline_count
) {
    graphics_pipelines.pipeline_count = pipeline_count;
    _graphics_pipelines_binding.iterator = 0;
    _graphics_pipelines_binding.pass_id = 0;
    _graphics_pipelines_binding.subpass = 0;
    if(pipeline_count != 0) {
        graphics_pipelines.p_pipelines = (VkPipeline*)allocMalloc(pipeline_count * sizeof(VkPipeline));
        graphics_pipelines.p_layouts = (VkPipelineLayout*)allocMalloc(pipeline_count * sizeof(VkPipelineLayout));
    }
    else {
        graphics_pipelines.p_pipelines = NULL;
        graphics_pipelines.p_layouts = NULL;
    }
}

// binds render pass & subpass for pipeline addition
void graphicsPipelinesBindRenderPass(
    uint32_t pass_id,
    uint32_t subpass
) {
    _graphics_pipelines_binding.pass_id = pass_id;
    _graphics_pipelines_binding.subpass = subpass;
}

// adds pipeline to iterator location and adjusts iterator
// use argument function for creation of render pipeline and its layout
uint32_t graphicsPipelinesAdd(
    void (*p_create_pipeline) (const VkDevice, const VkRenderPass, const uint32_t subpass, VkPipeline* const, VkPipelineLayout* const)
) {
    if(_graphics_pipelines_binding.iterator >= graphics_pipelines.pipeline_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PIPELINES")
    }

    (*p_create_pipeline)(
        graphics_device.device, 
        graphics_passes.p_passes[_graphics_pipelines_binding.pass_id], 
        _graphics_pipelines_binding.subpass,
        graphics_pipelines.p_pipelines + _graphics_pipelines_binding.iterator,
        graphics_pipelines.p_layouts + _graphics_pipelines_binding.iterator
    );

    return _graphics_pipelines_binding.iterator++;
}

// ends pipelines lifetime
void graphicsPipelinesTerminate() {
    for(uint32_t i = 0; i < graphics_pipelines.pipeline_count; i++) {
        vkDestroyPipeline(graphics_device.device, graphics_pipelines.p_pipelines[i], NULL);
        vkDestroyPipelineLayout(graphics_device.device, graphics_pipelines.p_layouts[i], NULL);
    }
    if(graphics_pipelines.p_pipelines != NULL && graphics_pipelines.p_layouts != NULL) {
        allocFree(graphics_pipelines.p_pipelines);
        allocFree(graphics_pipelines.p_layouts);
    }
}

#endif