#include "FFVulkan/VulkanShader.h"

#include "VulkanUtils.h"
#include "FFCore/ResourceManagement/FileSystem.h"
#include "FFVulkan/VulkanCore.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"

using namespace FFVk;

struct Shader
{
    std::vector<u32> SPIRV;
    VkShaderModule shaderModule = nullptr;
    
    void Initialize(glslang_program_t* program)
    {
        size_t programSize = glslang_program_SPIRV_get_size(program);
        SPIRV.resize(programSize);
        glslang_program_SPIRV_get(program, SPIRV.data());
    }
};

static glslang_stage_t ShaderStageFromFilename(const char* fileName)
{
    std::string_view s(fileName ? fileName : "");

    if (s.ends_with(".vert")) return GLSLANG_STAGE_VERTEX;
    if (s.ends_with(".frag")) return GLSLANG_STAGE_FRAGMENT;
    if (s.ends_with(".geom")) return GLSLANG_STAGE_GEOMETRY;
    if (s.ends_with(".comp")) return GLSLANG_STAGE_COMPUTE;
    if (s.ends_with(".tesc")) return GLSLANG_STAGE_TESSCONTROL;
    if (s.ends_with(".tese")) return GLSLANG_STAGE_TESSEVALUATION;
    
    ASSERT_NO_ENTRY()
    return GLSLANG_STAGE_VERTEX;
}

static bool CompileShader(VkDevice& device, glslang_stage_t stage, const char* src, Shader& shaderModule)
{
    glslang_input_t input = 
    {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = stage,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = src,
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE, 
        .force_default_version_and_profile = false,
        .forward_compatible =  false,
        .messages = GLSLANG_MSG_DEFAULT_BIT, 
        .resource = glslang_default_resource()
    };
    
    glslang_shader_t* shader = glslang_shader_create(&input);
    
    if (!glslang_shader_preprocess(shader, &input))
    {
        LOG_ERROR("GLSL preprocessing failed\n%s\n%s", glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader))
        return 0;
    }
    
    if (!glslang_shader_parse(shader, &input))
    {
        LOG_ERROR("GLSL parsing failed\n%s\n%s", glslang_shader_get_info_log(shader), glslang_shader_get_info_debug_log(shader))
        return 0;
    }
    
    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);
    
    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        LOG_ERROR("GLSL linking failed\n%s\n%s", glslang_program_get_info_log(program), glslang_program_get_info_debug_log(program))
        return 0;
    }
    
    glslang_program_SPIRV_generate(program, stage);
    
    shaderModule.Initialize(program);
    
    const char* spirvMessages = glslang_program_SPIRV_get_messages(program);
    
    if (spirvMessages)
    {
        LOG_ERROR("spirv messages: %s", spirvMessages);
    }
    
    VkShaderModuleCreateInfo shaderCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderModule.SPIRV.size() * sizeof(u32),
        .pCode = (const u32*)shaderModule.SPIRV.data(),
    };
    
    VK_CALL_AND_CHECK
    (
        vkCreateShaderModule, 
        "failed to create shader module",
        device, &shaderCreateInfo, nullptr, &shaderModule.shaderModule
    )
    
    glslang_program_delete(program);
    glslang_shader_delete(shader);
    
    return shaderModule.SPIRV.size() > 0;
}

VkShaderModule FFVk::CreateShaderModuleFromBinary(VkDevice& device, const char* fileName)
{
    i32 codeSize = 0;
    char* shaderCode = FF::FileSystem::ReadBinaryFile(fileName, codeSize);
    ASSERT(shaderCode, "Failed reading shader binary");
    
    VkShaderModuleCreateInfo shaderCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = (size_t)codeSize,
        .pCode = (const u32*)shaderCode,
    };
    
    VkShaderModule shaderModule; 
    VK_CALL_AND_CHECK
    (
        vkCreateShaderModule,
        "failed to create shader module",
        device, &shaderCreateInfo, nullptr, &shaderModule
    )
    
    free(shaderCode);
    return shaderModule;
}

VkShaderModule FFVk::CreateShaderModuleFromText(VkDevice& device, const char* fileName)
{
    std::string src;
    if (!FF::FileSystem::ReadFile(fileName, src))
    {
        ASSERT_NO_ENTRY()
    }
    
    Shader shader;
    
    glslang_stage_t shaderStage = ShaderStageFromFilename(fileName);
    VkShaderModule ret = nullptr;
    
    glslang_initialize_process();
    
    if (CompileShader(device, shaderStage, src.c_str(), shader))
    {
        ret = shader.shaderModule;
        std::string binaryFilename;
        binaryFilename.reserve(std::strlen(fileName) + 4);
        binaryFilename.append(fileName);
        binaryFilename.append(".spv");
        FF::FileSystem::WriteBinaryFile(binaryFilename.c_str(), shader.SPIRV.data(), shader.SPIRV.size() * sizeof(u32));
    }
    
    glslang_finalize_process();
    return ret;
}