#include "FFCore/Core/HString.h"
#include "FFCore/Memory/AllocTracker.h"
#include "FFCore/ResourceManagement/Resource_TextFile.h"
#include "FFEngine/Engine.h"

int main(int argc, char* argv[])
{
    FFE::Engine engine;
    engine.Init();
    
    auto skibidi = engine.ResourceManager.Load<FF::Resource_TextFile>("skibidi");
    if (skibidi.IsValid())
    {
        LOG_MESSAGE("%s", skibidi.Get()->GetContent().data())
    }
    
    FFE::Window::InitData initData =
    {
        .Name = "FF Sandbox",
        .Width = 1280,
        .Height = 720,
    };
    
    engine.MakeWindow(initData);
    
    engine.Update();
    engine.Cleanup();
    
    FF::Memory::DumpAllTags();
    
    return 0;
}