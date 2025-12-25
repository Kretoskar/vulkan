#pragma once

#include "Rendering/VulkanRenderer.h"

namespace FFE
{
    class Engine
    {
    public:
        Engine()
            : _vulkanRenderer("FFEngine App") {}
        ~Engine() = default;

    private:
        VulkanRenderer _vulkanRenderer;
    };
}
