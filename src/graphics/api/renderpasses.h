#ifndef _GRAPHICS_API_PASSES_INCLUDED
#define _GRAPHICS_API_PASSES_INCLUDED
// implements render passes storage
// implements render pipelines storage

#include "device.h"

// describes render passes array
static struct {
    uint32_t pass_count;
    VkRenderPass* p_passes;
} graphics_passes;

// binding local data
static struct {
    uint32_t iterator;
} _graphics_passes_binding;


// returns index of last added render pass
uint32_t graphicsPassesCurrentId() {
    // flattening if statement
    uint32_t options[2] = {_graphics_passes_binding.iterator, _graphics_passes_binding.iterator - 1};
    return options[_graphics_passes_binding.iterator != 0];
}

// sterts render passes lifetime
void graphicsPassesInit(
    uint32_t pass_count
) {
    graphics_passes.pass_count = pass_count;
    _graphics_passes_binding.iterator = 0;
    if(pass_count != 0) {
        graphics_passes.p_passes = (VkRenderPass*)allocMalloc(pass_count * sizeof(VkRenderPass));
    }
    else {
        graphics_passes.p_passes = NULL;
    }
}

// adds render pass to iterator location and ajusts iterator
// use argument function for creating render pass
uint32_t graphicsPassesAdd(
    void (*p_create_pass) (const VkDevice, VkRenderPass* const)
) {
    if(_graphics_passes_binding.iterator >= graphics_passes.pass_count) {
        ERROR_FATAL("TRYING TO ADD NOT ALLOCATED RENDER PASSES")
    }

    (*p_create_pass)(graphics_device.device, graphics_passes.p_passes + _graphics_passes_binding.iterator);
    return _graphics_passes_binding.iterator++;
}

// ends render passes lifetime
void graphicsPassesTerminate() {
    for(uint32_t i = 0; i < graphics_passes.pass_count; i++) {
        vkDestroyRenderPass(graphics_device.device, graphics_passes.p_passes[i], NULL);
    }
    if(graphics_passes.p_passes != NULL) {
        allocFree(graphics_passes.p_passes);
    }
}

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