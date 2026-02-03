#pragma once

#include "VulkanPhysicalDevice.h"
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
        void CreateDevice();
        u32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& capabilities);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
        VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
        VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags, VkImageViewType imageViewType, u16 layerCount, u16 mipLevel);
        void CreateSwapChain();
        
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;
        VulkanPhysicalDevices _physicalDevices;
        u32 _queueFamily{};
        VkDevice _device = VK_NULL_HANDLE;
        VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> _images;
        std::vector<VkImageView> _imageViews;
    };
}