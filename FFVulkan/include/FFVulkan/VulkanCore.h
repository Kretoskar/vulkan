#pragma once

#include "vulkan/vulkan_core.h"

namespace FFVk
{
    class VulkanCore
    {
    public:

        VulkanCore(const char* appName);
        ~VulkanCore();
        
    private:
        VkInstance _instance = nullptr;
    };
}