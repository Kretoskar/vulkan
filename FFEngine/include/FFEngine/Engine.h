#pragma once

#include "Rendering/VulkanRenderer.h"

class GLFWwindow;

namespace FFE
{
    class Engine
    {
    public:
        Engine(GLFWwindow* window)
            : _vulkanRenderer("FFEngine App", window) {}
        ~Engine() = default;

    private:
        VulkanRenderer _vulkanRenderer;
    };
}
