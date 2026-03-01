#pragma once
#include "FFVulkan/VulkanCore.h"

struct GLFWwindow;

namespace FFE
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer(const char* appName, GLFWwindow* window)
            : _vulkanCore(appName, window)
        {
            _numImages = _vulkanCore.GetNumImages();
            _cmdBuffers.resize(_numImages);
            _vulkanCore.CreateCommandBuffers(_numImages, _cmdBuffers.data());

            RecordCommandBuffers();
        }

        ~VulkanRenderer()
        {
            _vulkanCore.FreeCommandBuffers(_cmdBuffers);
        }

    private:
        void RecordCommandBuffers();
        
        FFVk::VulkanCore _vulkanCore;
        i32 _numImages = 0;
        std::vector<VkCommandBuffer> _cmdBuffers;
    };
}
