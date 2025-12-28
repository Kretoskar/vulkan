#pragma once

#include "vulkan/vulkan_core.h"

struct GLFWwindow;

namespace FFVk
{
    class VulkanCore
    {
    public:
        VulkanCore(const char* appName, GLFWwindow* window);
        ~VulkanCore();
        
    private:
        void CreateInstance(const char* appName);
        void CreateDebugCallback();
        void CreateSurface(GLFWwindow* window);
        
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;
    };
}