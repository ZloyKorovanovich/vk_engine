#ifndef _GRAPHICS_UTILIS_SHADER_INCLUDED
#define _GRAPHICS_UTILIS_SHADER_INCLUDED
// implements functions for shader compilation and shader module creation
// helps in render pipeline configuration

#include "../api/api.h"
#include "../../resources/resources.h"


const struct {
    const char* p_shader_path; // local path to shaders (in resources)
} graphics_utilis_shader = {
    .p_shader_path = "/shaders"
};


// reads code from shader file
// keep in mind, shader code will be allocated and is managed by user
bool graphicsUtilisShaderRead(
    const char* const p_file, 
    size_t* const p_shader_code_length, 
    unsigned char** const pp_shader_code
) {

    char p_file_path[MAX_PATH_LENGTH]; // global path variable
    strcpy(p_file_path, graphics_utilis_shader.p_shader_path); // local shader path
    strcat(p_file_path, p_file); // local path to file
    resourcesPathGetLocal(p_file_path); // global path
    return resourcesFileReadByte(p_file_path, p_shader_code_length, pp_shader_code);
}

// creates a shader module from spir-v code
bool graphicsUtilisShaderModuleCreate(
    const uint32_t shader_code_size, 
    const uint32_t* const p_shader_code,
    VkShaderModule* const p_shader_module
) {
    VkShaderModuleCreateInfo create_info = (VkShaderModuleCreateInfo){0};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code_size;
    create_info.pCode = p_shader_code;
    return (vkCreateShaderModule(graphics_api_device.device, &create_info, NULL, p_shader_module) == VK_SUCCESS);
}

#endif