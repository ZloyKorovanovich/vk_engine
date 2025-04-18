#ifndef _GRAPHICS_RENDER_PASSES_TRIANGLE_INCLUDED
#define _GRAPHICS_RENDER_PASSES_TRAINGLE_INCLUDED
// implements trinagle render pass
// draws rgb trinagle on screen

#include <vulkan/vulkan.h>
#include "../api/api.h"
#include "../utilis/utilis.h"
#include "../../utilis/error.h"

// describes traingle pass information
static struct {
    uint32_t triangle_render_pass_id;
    uint32_t traingle_pipeline_id;
} graphics_triangle_pass;

// creates triangle render pass, used as argument for graphicsApiPassesAdd
void graphicsRenderPassesTrianglePass(
    const VkDevice device, 
    VkRenderPass* const p_pass
) {
    VkAttachmentDescription color_attachment = (VkAttachmentDescription){0};
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.format = graphics_api_swapchain.format.format;

    VkAttachmentReference color_attachment_reference = (VkAttachmentReference){0};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = (VkSubpassDescription){0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    VkSubpassDependency dependency = (VkSubpassDependency){0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = (VkRenderPassCreateInfo){0};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &render_pass_info, NULL, p_pass) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE TRIANGLE RENDER PASS")
    }
}

// creates triangle render pipeline, used as argument for graphicsApiPipelinesAdd
void grapicsRenderPassesPipelineTriangle(
    const VkDevice device, 
    const VkRenderPass pass, 
    const uint32_t subpass, 
    VkPipeline* const p_pipeline,
    VkPipelineLayout* const p_layout
) {
    const uint32_t shader_count = 2;
    const char* pp_shader_files[2] = {"/triangle_vert.spv", "/triangle_frag.spv"};
    VkShaderStageFlagBits p_shader_flags[2] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
    
    VkShaderModule p_shader_modules[2];
    VkPipelineShaderStageCreateInfo p_shader_stages[2];
    for(uint32_t i = 0; i < shader_count; i++) {
        if(!graphicsUtilisStagesShader(
            "main",
            pp_shader_files[i],
            p_shader_flags[i],
            p_shader_modules + i,
            p_shader_stages + i
        )) {
            ERROR_FATAL("TRAINGLE PASS FAILED TO COMPILE SHADERS")
        }
    }

    VkPipelineLayoutCreateInfo layout_info = graphicsUtilisPipelineLayout();
    if (vkCreatePipelineLayout(graphics_api_device.device, &layout_info, NULL, p_layout) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CTRATE TRAINGLE PIPELINE LAYOUT")
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_state = graphicsUtilisStagesVertexInput();
    VkPipelineInputAssemblyStateCreateInfo assembly_state = graphicsUtilisStagesAssembly();
    VkPipelineRasterizationStateCreateInfo rasterization_state = graphicsUtilisStagesRasterization();
    VkPipelineMultisampleStateCreateInfo multisample_state = graphicsUtilisStagesMultisample();
    VkPipelineViewportStateCreateInfo viewport_state = graphicsUtilisStagesViewport();
    VkPipelineColorBlendStateCreateInfo color_blend_state = graphicsUtilisStagesBlend();
    VkPipelineDynamicStateCreateInfo dynamic_state = graphicsUtilisStagesDynamic();

    VkGraphicsPipelineCreateInfo pipeline_info = (VkGraphicsPipelineCreateInfo){0};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = p_shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pMultisampleState = &multisample_state;
    pipeline_info.pDepthStencilState = NULL;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = *p_layout;
    pipeline_info.renderPass = pass;
    pipeline_info.subpass = subpass;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(graphics_api_device.device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, p_pipeline) != VK_SUCCESS) {
        ERROR_FATAL("FAILED TO CREATE TRIANGLE PIPELINE")
    }

    for(uint32_t i = 0; i < shader_count; i++) {
        vkDestroyShaderModule(graphics_api_device.device, p_shader_modules[i], NULL);
    }
}

// creates triangle render passes and related
void graphicsRenderPassesTriangleCreate(void) {
    graphics_triangle_pass.triangle_render_pass_id = graphicsApiPassesAdd(&graphicsRenderPassesTrianglePass);
    graphicsApiPipelinesBindRenderPass(graphics_triangle_pass.triangle_render_pass_id, 0);
    graphics_triangle_pass.traingle_pipeline_id = graphicsApiPipelinesAdd(&grapicsRenderPassesPipelineTriangle);
    graphicsApiFramebuffersBindRenderPass(graphicsApiPassesCurrentId());
    graphicsApiFramebuffersAdd();
}

// executes triangle render pass (draws rgb triangle)
void graphicsRenderPassesTriangleExecute(
    const VkCommandBuffer cmbuffer,
    const uint32_t frame_index
) {
    VkRenderPassBeginInfo pass_info = (VkRenderPassBeginInfo){0};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = graphics_api_passes.p_passes[graphics_triangle_pass.triangle_render_pass_id];
    pass_info.framebuffer = graphics_api_framebuffers.p_framebuffers[frame_index];

    pass_info.renderArea.offset = (VkOffset2D){0.0f, 0.0f};
    pass_info.renderArea.extent = graphics_api_swapchain.extent;

    VkClearValue clear_color = (VkClearValue){0};
    clear_color.color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    pass_info.clearValueCount = 1;
    pass_info.pClearValues = &clear_color;
    
    vkCmdBeginRenderPass(cmbuffer, &pass_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_api_pipelines.p_pipelines[graphics_triangle_pass.traingle_pipeline_id]);
    vkCmdSetViewport(cmbuffer, 0, 1, &graphics_api_swapchain.viewport);
    vkCmdSetScissor(cmbuffer, 0, 1, &graphics_api_swapchain.scissors);

    vkCmdDraw(cmbuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(cmbuffer);
}

#endif