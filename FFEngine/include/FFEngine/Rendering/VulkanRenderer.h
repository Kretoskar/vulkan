#pragma once
#include "FFVulkan/VulkanCore.h"

struct GLFWwindow;

namespace FFE
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer(const char* appName, GLFWwindow* window)
            : vulkanCore(appName, window)
        {}

    private:
        FFVk::VulkanCore vulkanCore;
    };
}
