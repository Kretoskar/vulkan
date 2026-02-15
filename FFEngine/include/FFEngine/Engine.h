#pragma once

#include "Rendering/VulkanRenderer.h"
#include "FFCore/Core/Logger.h"

class GLFWwindow;

namespace FFE
{
    class Engine
    {
    public:
        Engine(GLFWwindow* window)
            : _vulkanRenderer("FFEngine App", window)
        {
            FF::Logger::GetInstance().Start();
        }
        ~Engine()
        {
            FF::Logger::GetInstance().Stop();   
        }

    private:
        VulkanRenderer _vulkanRenderer;
    };
}
