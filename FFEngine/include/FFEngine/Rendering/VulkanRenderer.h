#pragma once
#include "FFVulkan/VulkanCore.h"

namespace FFE
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer(const char* appName)
            : vulkanCore(appName)
        {}

    private:
        FFVk::VulkanCore vulkanCore;
    };
}
