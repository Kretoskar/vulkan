#include "FFVulkan/VulkanPhysicalDevice.h"

#include "FFVulkan/VulkanUtils.h"

namespace FFVk
{
    static void PrintImageUsageFlags(const VkImageUsageFlags& flags)
    {
        if (flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        {
            LOG_MESSAGE("Image usage transfer src is supported");
        }

        if (flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        {
            LOG_MESSAGE("Image usage transfer dest is supported");
        }

        if (flags & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            LOG_MESSAGE("Image usage sampled is supported");
        }

        if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            LOG_MESSAGE("Image usage color attachment is supported");
        }

        if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            LOG_MESSAGE("Image usage depth stencil attachment is supported");
        }

        if (flags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
        {
            LOG_MESSAGE("Image usage transient attachment is supported");
        }

        if (flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
        {
            LOG_MESSAGE("Image usage input attachment is supported");
        }
    }

    
    void VulkanPhysicalDevices::Init(const VkInstance& vkInstance, const VkSurfaceKHR& surface)
    {
        u32 numDevices{};
        VK_CALL_AND_CHECK
        (
            vkEnumeratePhysicalDevices,
            "Couldn't enumerate physical devices",
            vkInstance, &numDevices, nullptr
        )

        LOG_MESSAGE("Num physical devices: %d", numDevices)

        _physicalDevices.resize(numDevices);
        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(numDevices);

        VK_CALL_AND_CHECK
        (
            vkEnumeratePhysicalDevices,
            "Couldn't enumerate physical devices",
            vkInstance, &numDevices, physicalDevices.data()
        )

        for (u32 i = 0; i < numDevices; ++i)
        {
            const VkPhysicalDevice& device = physicalDevices[i];
            _physicalDevices[i].PhysDevice = device;

            vkGetPhysicalDeviceProperties(device, &_physicalDevices[i].PhysicalDeviceProperties);

            const char* name = _physicalDevices[i].PhysicalDeviceProperties.deviceName;
            const u32 apiVersion = _physicalDevices[i].PhysicalDeviceProperties.apiVersion;
            
            LOG_MESSAGE("Physical device: %hs\n "
                        "API Version: %d. %d. %d. %d",
                        name,
                        VK_API_VERSION_VARIANT(apiVersion),
                        VK_API_VERSION_MAJOR(apiVersion),
                        VK_API_VERSION_MINOR(apiVersion),
                        VK_API_VERSION_PATCH(apiVersion))

            u32 numFamilies{};
            vkGetPhysicalDeviceQueueFamilyProperties(device, &numFamilies, nullptr);

            _physicalDevices[i].QueueFamilyProperties.resize(numFamilies);
            _physicalDevices[i].SupportsPresenting.resize(numFamilies);
            
            vkGetPhysicalDeviceQueueFamilyProperties(device, &numFamilies, _physicalDevices[i].QueueFamilyProperties.data());
            
            for (u32 q = 0; q < numFamilies; ++q)
            {
                const VkQueueFamilyProperties& famProp = _physicalDevices[i].QueueFamilyProperties[q];

                LOG_MESSAGE("Family: %d Num Queues: %d", q, famProp.queueCount)
                VkQueueFlags flags = famProp.queueFlags;
                LOG_MESSAGE("GFX %s, Compute %s, Transfer %s, Sparse binding %s",
                    flags & VK_QUEUE_GRAPHICS_BIT ? "yes" : "no",
                    flags & VK_QUEUE_COMPUTE_BIT ? "yes" : "no",
                    flags & VK_QUEUE_TRANSFER_BIT ? "yes" : "no",
                    flags & VK_QUEUE_SPARSE_BINDING_BIT ? "yes" : "no")

                VK_CALL_AND_CHECK
                (
                    vkGetPhysicalDeviceSurfaceSupportKHR,
                    "Couldn't check physical device surface support KHR",
                    device, q, surface, &_physicalDevices[i].SupportsPresenting[q]
                )
            }

            u32 numFormats{};
            VK_CALL_AND_CHECK
            (
                vkGetPhysicalDeviceSurfaceFormatsKHR,
                "Couldn't get physical device surface formats",
                device, surface, &numFormats, nullptr
            )
            ASSERT(numFormats > 0, "No supported surface formats")

            _physicalDevices[i].SurfaceFormats.resize(numFormats);
            VK_CALL_AND_CHECK
            (
                vkGetPhysicalDeviceSurfaceFormatsKHR,
                "Couldn't get physical device surface formats",
                device, surface, &numFormats, _physicalDevices[i].SurfaceFormats.data()
            )

            if (0)
            {
                for (u32 j = 0; j < numFormats; ++j)
                {
                    const VkSurfaceFormatKHR& format = _physicalDevices[i].SurfaceFormats[j];
                    LOG_MESSAGE("Format %x Color space %x", format.format, format.colorSpace)
                }
            }

            VK_CALL_AND_CHECK
            (
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
                "Couldn't get physical device surface capabilities",
                device, surface, &_physicalDevices[i].SurfaceCapabilities
            )

            if (0)
             {
                PrintImageUsageFlags(_physicalDevices[i].SurfaceCapabilities.supportedUsageFlags);
            }

            u32 numPresentationModes{};
            VK_CALL_AND_CHECK
            (
                vkGetPhysicalDeviceSurfacePresentModesKHR,
                "Couldn't get physical device surface present modes",
                device, surface, &numPresentationModes, nullptr
            )

            _physicalDevices[i].PresentingModes.resize(numPresentationModes);

            VK_CALL_AND_CHECK
            (
                vkGetPhysicalDeviceSurfacePresentModesKHR,
                "Couldn't get physical device surface present modes",
                device, surface, &numPresentationModes, _physicalDevices[i].PresentingModes.data()
            )

            vkGetPhysicalDeviceMemoryProperties(device, &_physicalDevices[i].MemoryProperties);
        }
    }

    uint32_t VulkanPhysicalDevices::SelectDevice(VkQueueFlags reqQueueType, bool supportsPresent)
    {
        for (u32 i = 0; i < _physicalDevices.size(); ++i)
        {
            for (u32 j = 0; j < _physicalDevices[i].QueueFamilyProperties.size(); j++)
            {
                const VkQueueFamilyProperties& QFamilyProp = _physicalDevices[i].QueueFamilyProperties[j];

                if ((QFamilyProp.queueFlags & reqQueueType) && ((bool)_physicalDevices[i].SupportsPresenting[j] == supportsPresent))
                {
                    _selectedDevice = i;
                    i32 QueueFamily = j;
                    LOG_MESSAGE("Using GFX device %d (%s) and queue family %d\n", _selectedDevice, _physicalDevices[i].PhysicalDeviceProperties.deviceName, QueueFamily)
                    return QueueFamily;
                }
            }
        }
    
        LOG_ERROR("Required queue type %x and supports present %d not found\n", reqQueueType, supportsPresent)
    
        return 0;
    }

    const PhysicalDevice& FFVk::VulkanPhysicalDevices::SelectedDevice() const
    {
        return _physicalDevices[_selectedDevice];
    }
}
