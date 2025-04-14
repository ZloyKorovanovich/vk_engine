#ifndef _RENDER_PASS_PIPELINE_STAGES_INCLUDED
#define _RENDER_PASS_PIPELINE_STAGES_INCLUDED
// provides with functions for easy pipeline creation

#include "../setup/setup.h"

// crteates shader stage and shader module
// shader module should be cared untill pipeline is created
// returns true if shader was sucessfuly read and module was created
bool renderPassesShaderStage(
    const char* p_entry_name, // name for entry function (usually "main")
    const char* p_shader_file, // local shader path to file
    const VkShaderStageFlagBits flag, // stage flag (vertex, fragment, etc.)
    VkShaderModule* const p_module, // output shader module
    VkPipelineShaderStageCreateInfo* const p_stage_info // output stage create info
) {
    size_t code_size;
    unsigned char* p_code;
    if(!graphicsShaderRead(p_shader_file, &code_size, &p_code)) {
        return false; // failed to read shader
    }
    if(!graphicsShaderModuleCreate(code_size, (uint32_t*)p_code, p_module)) {
        return false; // failed to create module
    }
    allocFree(p_code); // now carried in shader module

    *p_stage_info = (VkPipelineShaderStageCreateInfo){0};
    p_stage_info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    p_stage_info->stage = flag;
    p_stage_info->module = *p_module;
    p_stage_info->pName = p_entry_name;
    return true; // success
}

// returns default configuration of pipeline viewport state create info
VkPipelineViewportStateCreateInfo renderPassesDefaultViewportState() {
    VkPipelineViewportStateCreateInfo create_info = (VkPipelineViewportStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    create_info.viewportCount = 1;
    create_info.viewportCount = 1;
    create_info.pViewports = &graphics_swapchain.viewport;
    create_info.scissorCount = 1;
    create_info.pScissors = &graphics_swapchain.scissors;
    return create_info;
};

// returns default configuration of pipeline dynamic state create info
VkPipelineDynamicStateCreateInfo renderPassesDefaultDynamicState() {
    // constant-like declaration, but inside of function
    // constant array of dynamic states
    static uint32_t dynamic_state_count = 2; 
    static VkDynamicState p_dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo create_info = (VkPipelineDynamicStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    create_info.dynamicStateCount = dynamic_state_count;
    create_info.pDynamicStates = p_dynamic_states;
    return create_info;
};

// returns default configuration of pipeline rasterization state create info
VkPipelineRasterizationStateCreateInfo renderPassesDefaultRasterizationState() {
    VkPipelineRasterizationStateCreateInfo create_info = (VkPipelineRasterizationStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    create_info.depthClampEnable = VK_FALSE;
    create_info.rasterizerDiscardEnable = VK_FALSE;
    create_info.polygonMode = VK_POLYGON_MODE_FILL;
    create_info.lineWidth = 1.0f;
    create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    create_info.depthBiasEnable = VK_FALSE;
    create_info.depthBiasConstantFactor = 0.0f;
    create_info.depthBiasClamp = 0.0f;
    create_info.depthBiasSlopeFactor = 0.0f;
    return create_info;
};

// returns default configuration of pipeline vertex input state create info
VkPipelineVertexInputStateCreateInfo renderPassesDefaultVertexInputState() {
    VkPipelineVertexInputStateCreateInfo create_info = (VkPipelineVertexInputStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    create_info.vertexBindingDescriptionCount = 0;
    create_info.pVertexBindingDescriptions = NULL;
    create_info.vertexAttributeDescriptionCount = 0;
    create_info.pVertexAttributeDescriptions = NULL;
    return create_info;
}

// returns default configuration of pipeline assembly state create info
VkPipelineInputAssemblyStateCreateInfo renderPassesDefaultAssemblyState() {
    VkPipelineInputAssemblyStateCreateInfo create_info = (VkPipelineInputAssemblyStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    create_info.primitiveRestartEnable = VK_FALSE;
    return create_info;
};

// returns default configuration of pipeline multisample state create info
VkPipelineMultisampleStateCreateInfo renderPassesDefaultMultisampleState() {
    VkPipelineMultisampleStateCreateInfo create_info = (VkPipelineMultisampleStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    create_info.sampleShadingEnable = VK_FALSE;
    create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    create_info.minSampleShading = 1.0f;
    create_info.pSampleMask = NULL;
    create_info.alphaToCoverageEnable = VK_FALSE;
    create_info.alphaToOneEnable = VK_FALSE;
    return create_info;
};

// returns default configuration of pipeline viewport blend create info
VkPipelineColorBlendStateCreateInfo renderPassesDefaultBlendState() {
    static VkPipelineColorBlendAttachmentState color_blend_state = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };
    VkPipelineColorBlendStateCreateInfo create_info = (VkPipelineColorBlendStateCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    create_info.logicOpEnable = VK_FALSE;
    create_info.logicOp = VK_LOGIC_OP_COPY;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_blend_state;
    create_info.blendConstants[0] = 0.0f;
    create_info.blendConstants[1] = 0.0f;
    create_info.blendConstants[2] = 0.0f;
    create_info.blendConstants[3] = 0.0f;
    return create_info;
};

// returns default(empty) configuration of pipeline layout info
VkPipelineLayoutCreateInfo renderPassesDefaultPipelineLayout() {
    VkPipelineLayoutCreateInfo create_info = (VkPipelineLayoutCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = 0;
    create_info.pSetLayouts = NULL;
    create_info.pushConstantRangeCount = 0;
    create_info.pPushConstantRanges = NULL;
    return create_info;
};


#endif