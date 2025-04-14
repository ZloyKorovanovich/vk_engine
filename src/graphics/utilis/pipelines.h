#ifndef _GRAPHICS_UTILIS_PIPELINES_INCLUDED
#define _GRAPHICS_UTILIS_PIPELINES_INCLUDED

#include <vulkan/vulkan.h>

// returns default(empty) configuration of pipeline layout info
VkPipelineLayoutCreateInfo graphicsUtilisPipelineLayout() {
    VkPipelineLayoutCreateInfo create_info = (VkPipelineLayoutCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = 0;
    create_info.pSetLayouts = NULL;
    create_info.pushConstantRangeCount = 0;
    create_info.pPushConstantRanges = NULL;
    return create_info;
};

#endif