#ifndef _GRAPHICS_API_PIPELINES_INCLUDED
#define _GRAPHICS_API_PIPELINES_INCLUDED
// implements render pipelines storage

#include "passes.h"

// describes pipelines and layouts
static struct {
    uint32_t pipeline_count;
    VkPipeline* p_pipelines;
    VkPipelineLayout* p_layouts;
} graphics_api_pipelines;

// binding local data
static struct {
    uint32_t iterator;
    uint32_t pass_id;
    uint32_t subpass;
} _graphics_api_pipelines_binding;


// starts pipelines lifetime
void graphicsApiPipelinesInit(
    uint32_t pipeline_count
) {
    graphics_api_pipelines.pipeline_count = pipeline_count;
    _graphics_api_pipelines_binding.iterator = 0;
    _graphics_api_pipelines_binding.pass_id = 0;
    _graphics_api_pipelines_binding.subpass = 0;
    if(pipeline_count != 0) {
        graphics_api_pipelines.p_pipelines = (VkPipeline*)allocMalloc(pipeline_count * sizeof(VkPipeline));
        graphics_api_pipelines.p_layouts = (VkPipelineLayout*)allocMalloc(pipeline_count * sizeof(VkPipelineLayout));
    }
    else {
        graphics_api_pipelines.p_pipelines = NULL;
        graphics_api_pipelines.p_layouts = NULL;
    }
}

// binds render pass & subpass for pipeline addition
void graphicsApiPipelinesBindRenderPass(
    uint32_t pass_id,
    uint32_t subpass
) {
    _graphics_api_pipelines_binding.pass_id = pass_id;
    _graphics_api_pipelines_binding.subpass = subpass;
}

// adds pipeline to iterator location and adjusts iterator
// returns added pipeline id
// use argument function for creation of render pipeline and its layout
uint32_t graphicsApiPipelinesAdd(
    void (*p_create_pipeline) (const VkDevice, const VkRenderPass, const uint32_t subpass, VkPipeline* const, VkPipelineLayout* const)
) {
    if(_graphics_api_pipelines_binding.iterator >= graphics_api_pipelines.pipeline_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PIPELINES")
    }

    (*p_create_pipeline)(
        graphics_api_device.device, 
        graphics_api_passes.p_passes[_graphics_api_pipelines_binding.pass_id], 
        _graphics_api_pipelines_binding.subpass,
        graphics_api_pipelines.p_pipelines + _graphics_api_pipelines_binding.iterator,
        graphics_api_pipelines.p_layouts + _graphics_api_pipelines_binding.iterator
    );

    return _graphics_api_pipelines_binding.iterator++;
}

// ends pipelines lifetime
void graphicsApiPipelinesTerminate() {
    for(uint32_t i = 0; i < graphics_api_pipelines.pipeline_count; i++) {
        vkDestroyPipeline(graphics_api_device.device, graphics_api_pipelines.p_pipelines[i], NULL);
        vkDestroyPipelineLayout(graphics_api_device.device, graphics_api_pipelines.p_layouts[i], NULL);
    }
    if(graphics_api_pipelines.p_pipelines != NULL && graphics_api_pipelines.p_layouts != NULL) {
        allocFree(graphics_api_pipelines.p_pipelines);
        allocFree(graphics_api_pipelines.p_layouts);
    }
}

#endif