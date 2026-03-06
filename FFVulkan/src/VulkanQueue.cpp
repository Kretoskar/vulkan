#include "FFVulkan/VulkanQueue.h"
#include "VulkanUtils.h"

using namespace FFVk;

void VulkanQueue::Init(VkDevice Device, VkSwapchainKHR SwapChain, u32 QueueFamily, u32 QueueIndex)
{
    _device = Device;
    _swapChain = SwapChain;

    vkGetDeviceQueue(Device, QueueFamily, QueueIndex, &_queue);
    CreateSemaphores();
}

void VulkanQueue::Destroy()
{
    vkDestroySemaphore(_device, _renderCompleteSemaphore, nullptr);
    vkDestroySemaphore(_device, _presentCompleteSemaphore, nullptr);
}

u32 VulkanQueue::AcquireNextImage()
{
    u32 imageIndex = 0;
    VK_CALL_AND_CHECK
    (
        vkAcquireNextImageKHR,
        "Failed to acquire next swapchain image",
        _device,
        _swapChain,
        UINT64_MAX, // no timeout
        _presentCompleteSemaphore,
        nullptr,
        &imageIndex
    )
    return imageIndex;
}

void VulkanQueue::SubmitSync(VkCommandBuffer CmbBuf)
{
}

void VulkanQueue::SubmitAsync(VkCommandBuffer CmbBuf)
{
    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &_presentCompleteSemaphore,
        .pWaitDstStageMask = &waitFlags,
        .commandBufferCount = 1,
        .pCommandBuffers = &CmbBuf,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &_renderCompleteSemaphore,
    };

    VK_CALL_AND_CHECK
    (
        vkQueueSubmit,
        "Failed to submit queue",
        _queue,
        1,
        &submitInfo,
        nullptr
    )
}

void VulkanQueue::Present(u32 ImageIndex)
{
    VkPresentInfoKHR presentInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &_renderCompleteSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &_swapChain,
        .pImageIndices = &ImageIndex,
    };
    
    VK_CALL_AND_CHECK
    (
        vkQueuePresentKHR,
        "Failed to present queue",
        _queue,
        &presentInfo
    )

    // hotfix for semaphores synchronization
    vkQueueWaitIdle(_queue);
}

void VulkanQueue::WaitIdle()
{
    vkQueueWaitIdle(_queue);
}

VkSemaphore VulkanQueue::CreateSemaphoreFF(VkDevice Device)
{
    // FF because there is a macro CreateSemaphore already in vulkan
    VkSemaphoreCreateInfo createInfo =
           {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkSemaphore semaphore;
    VK_CALL_AND_CHECK
    (
        vkCreateSemaphore,
        "Failed to create semaphore",
        Device,
        &createInfo,
        nullptr,
        &semaphore
    )
    
    return semaphore;
}

void VulkanQueue::CreateSemaphores()
{
    _renderCompleteSemaphore = CreateSemaphoreFF(_device);
    _presentCompleteSemaphore = CreateSemaphoreFF(_device);
}
