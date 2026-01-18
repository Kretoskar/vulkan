#include "FFVulkan/VulkanPhysicalDevice.h"

#include "FFVulkan/VulkanUtils.h"

namespace FFVk
{
    void VulkanPhysicalDevices::Init(const VkInstance& vkInstance, const VkSurfaceKHR& surfaceKHR)
    {
        u32 numDevices{};
        VK_CALL_AND_CHECK(vkEnumeratePhysicalDevices(vkInstance, &numDevices, nullptr),
            "Couldn't enumerate physical devices")

        LOG_MESSAGE("Num physical devices: %d", numDevices)

        _physicalDevices.resize(numDevices);
        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(numDevices);

        VK_CALL_AND_CHECK(vkEnumeratePhysicalDevices(vkInstance, &numDevices, physicalDevices.data()),
            "Couldn't enumerate physical devices")

        for (u32 i = 0; i < numDevices; ++i)
        {
            const VkPhysicalDevice& device = physicalDevices[i];
            _physicalDevices[i].PhysicalDevice = device;

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
            _physicalDevices[i].SupportsPresent.resize(numFamilies);
            
            vkGetPhysicalDeviceQueueFamilyProperties(device, &numFamilies, _physicalDevices[i].QueueFamilyProperties.data());
            

        }
    }

    uint32_t VulkanPhysicalDevices::SelectDevice(VkQueueFlags reqQueueType, bool supportsPresent)
    {
    }

    const PhysicalDevice& FFVk::VulkanPhysicalDevices::SelectedDevice() const
    {
    }
}
