#pragma once

#include "Vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace FFVk
{
    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(VkDevice device, GLFWwindow* window, VkRenderPass renderPass, VkShaderModule vs, VkShaderModule fs);
        ~GraphicsPipeline();
        
        void Bind(VkCommandBuffer commandBuffer);
        
    private:
        VkDevice _device = VK_NULL_HANDLE;
        VkPipeline _pipeline = VK_NULL_HANDLE;
        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    };
}