#pragma once

#include <vector>

#include "FFCore/Core/Types.h"
#include "vulkan/vulkan_core.h"

namespace FFVk
{
    struct PhysicalDevice
    {
        VkPhysicalDevice PhysDevice;
        VkPhysicalDeviceProperties PhysicalDeviceProperties;
        std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
        std::vector<VkBool32> SupportsPresenting;
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        VkPhysicalDeviceMemoryProperties MemoryProperties;
        std::vector<VkPresentModeKHR> PresentingModes;
    };

    class VulkanPhysicalDevices
    {
    public:
        VulkanPhysicalDevices() = default;
        ~VulkanPhysicalDevices() = default;

        // TODO: Why not in ctor
        void Init(const VkInstance& vkInstance, const VkSurfaceKHR& surface);
        u32 SelectDevice(VkQueueFlags reqQueueType, bool supportsPresent);
        const PhysicalDevice& SelectedDevice() const;

    private:
        std::vector<PhysicalDevice> _physicalDevices;
        i32 _selectedDevice = -1;
    };
}
