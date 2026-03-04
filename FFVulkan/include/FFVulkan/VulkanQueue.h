#pragma once

#include <stdio.h>

#include <vulkan/vulkan.h>
#include "FFCore/Core/Types.h"

namespace FFVk
{

    class VulkanQueue
    {
    public:
        VulkanQueue() = default;
        ~VulkanQueue() = default;

        void Init(VkDevice Device, VkSwapchainKHR SwapChain, u32 QueueFamily, u32 QueueIndex);
        void Destroy();
        
        u32 AcquireNextImage();
        void SubmitSync(VkCommandBuffer CmbBuf);
        void SubmitAsync(VkCommandBuffer CmbBuf);
        void Present(u32 ImageIndex);
        // Hang until queue finishes all the cmd buffs inside it
        void WaitIdle();

    private:
        void CreateSemaphores();

        VkDevice _device = VK_NULL_HANDLE;
        VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
        VkQueue _queue = VK_NULL_HANDLE;
        // rendering to image is completed
        VkSemaphore _renderCompleteSemaphore;
        // presentation engine finished reading the image
        VkSemaphore _presentCompleteSemaphore;
    };
}
