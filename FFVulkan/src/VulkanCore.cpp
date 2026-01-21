#include "FFVulkan/VulkanCore.h"

#include <sstream>
#include <vector>
#include <GLFW/glfw3.h>

#include "FFCore/Core/Types.h"
#include "FFVulkan/VulkanUtils.h"

namespace FFVk
{
    namespace
    {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
            void* userData)
        {
            std::ostringstream objects;
            for (u32 i = 0; i < callbackData->objectCount; ++i)
            {
                if (i > 0)
                {
                    objects << ' ';
                }

                objects << callbackData->pObjects[i].objectHandle;
            }
        
            LOG_ERROR(
                "\nVULKAN ERROR\n"
                "Severity: %s\n"
                "Type: %s\n"
                "Message:\n%s\n"
                "Objects: %s\n",
            
                GetDebugSeverityStr(severity),
                GetDebugType(type),
                callbackData->pMessage,
                objects.str().c_str())

            return VK_FALSE;
        }
    }
    
    void VulkanCore::CreateInstance(const char* appName)
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
            .enabledLayerCount = static_cast<u32>(Layers.size()),
            .ppEnabledLayerNames = Layers.data(),
            .enabledExtensionCount = static_cast<u32>(Extensions.size()),
            .ppEnabledExtensionNames = Extensions.data()
        };

        CallAndCheckVkFunc(vkCreateInstance, "Failed when trying to create instance", &CreateInfo, nullptr, &_instance);
    }

    void VulkanCore::CreateDebugCallback()
    {
        VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = &DebugCallback,
            .pUserData = nullptr
        };

        VK_FIND_CALL_AND_CHECK
        (
            PFN_vkCreateDebugUtilsMessengerEXT,
            _instance,
            "Failed to create debug utils messenger",
            _instance,  &MessengerCreateInfo, nullptr, &_debugMessenger
        )
    }

    void VulkanCore::CreateSurface(GLFWwindow* window)
    {
        VK_CALL_AND_CHECK
        (
            glfwCreateWindowSurface,
            "Failed to create surface",
            _instance, window, nullptr, &_surface
        )
    }

    void VulkanCore::CreateDevice()
    {
        float qPriorities[] = { 1.0f };

		VkDeviceQueueCreateInfo qInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = _queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &qPriorities[0]
		};

		std::vector<const char*> DevExts =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
		};
	
		ASSERT (_physicalDevices.SelectedDevice().Features.geometryShader,
			"Geometry Shader is not supported!")
	
		ASSERT (_physicalDevices.SelectedDevice().Features.tessellationShader,
			"The Tessellation Shader is not supported!")

		VkPhysicalDeviceFeatures DeviceFeatures = { 0 };
		DeviceFeatures.geometryShader = VK_TRUE;
		DeviceFeatures.tessellationShader = VK_TRUE;

		VkDeviceCreateInfo DeviceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &qInfo,
			.enabledLayerCount = 0,			// DEPRECATED
			.ppEnabledLayerNames = nullptr,    // DEPRECATED
			.enabledExtensionCount = (u32)DevExts.size(),
			.ppEnabledExtensionNames = DevExts.data(),
			.pEnabledFeatures = &DeviceFeatures
		};

    	VK_CALL_AND_CHECK
    	(
    		vkCreateDevice,
    		"Couldn't create device",
    		_physicalDevices.SelectedDevice().PhysDevice, &DeviceCreateInfo, nullptr, &_device
    	)
    }

    VulkanCore::VulkanCore(const char* appName, GLFWwindow* window)
    {
        CreateInstance(appName);
        CreateDebugCallback();
        CreateSurface(window);
        _physicalDevices.Init(_instance, _surface);
        _queueFamily = _physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
    	CreateDevice();
    }

    VulkanCore::~VulkanCore()
    {
		vkDestroyDevice(_device, nullptr);
    	
        VK_FIND_AND_CALL
        (
            PFN_vkDestroySurfaceKHR,
            _instance,
            _instance, _surface, nullptr
        )

        VK_FIND_AND_CALL
        (
            PFN_vkDestroyDebugUtilsMessengerEXT,
            _instance,
            _instance, _debugMessenger, nullptr
        )
        
        vkDestroyInstance(_instance, nullptr);
    }
}
