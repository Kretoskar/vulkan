#include "FFVulkan/VulkanCore.h"

#include <vector>

#include "FFCore/Core/Types.h"
#include "FFVulkan/VulkanUtils.h"

namespace FFVk
{
    VulkanCore::VulkanCore(const char* appName)
    {
        std::vector<const char*> Layers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> Extensions =
        {		
            VK_KHR_SURFACE_EXTENSION_NAME,
    #if defined (_WIN32)
            "VK_KHR_win32_surface",
    #endif
    #if defined (__APPLE__)
            "VK_MVK_macos_surface",
    #endif
    #if defined (__linux__)
            "VK_KHR_xcb_surface",
    #endif
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        };

        VkApplicationInfo AppInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = appName,
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "FF Engine",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0
        };

        VkInstanceCreateInfo CreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &AppInfo,
            .enabledLayerCount = (u32)(Layers.size()),
            .ppEnabledLayerNames = Layers.data(),
            .enabledExtensionCount = (u32)(Extensions.size()),
            .ppEnabledExtensionNames = Extensions.data()
        };

        CallVkFunc(vkCreateInstance, "Failed when trying to create instance", &CreateInfo, nullptr, &_instance);
    }

    VulkanCore::~VulkanCore()
    {
        vkDestroyInstance(_instance, nullptr);
    }
}
