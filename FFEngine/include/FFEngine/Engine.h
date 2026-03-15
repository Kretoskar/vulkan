#pragma once

#include "Window.h"
#include "FFCore/Core/Logger.h"
#include "FFCore/ResourceManagement/ResourceManagement.h"

struct GLFWwindow;

namespace FFE
{
    class Engine
    {
    public:
        void Init();
        Window& MakeWindow(const Window::InitData& initData);
        void Update();
        void Cleanup();

        FF::ResourceManager ResourceManager;
        
    private:
        Window& GetWindow(u16 idx);
        std::vector<Window> _windows;
    };
}
