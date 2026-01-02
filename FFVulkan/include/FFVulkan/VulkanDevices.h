#pragma once
#include <vulkan/vulkan_core.h>

namespace FFVk
{
    struct PhysicalDevice
    {
        VkPhysicalDevice _physicalDevice;
        VkPhysicalDeviceProperties _physicalDeviceProperties;
    };
}
