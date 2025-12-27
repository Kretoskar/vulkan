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
        void CreateInstance(const char* appName);
        void CreateDebugCallback();
        
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    };
}