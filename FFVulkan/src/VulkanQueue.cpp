#include "FFVulkan/VulkanQueue.h"

#include "FFVulkan/VulkanCore.h"

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
    return VulkanCore::AcquireNextImage(_device, _swapChain, _presentCompleteSemaphore);
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

    VulkanCore::SubmitQueueAsync(_queue, 1, &submitInfo, nullptr);
}

void VulkanQueue::Present(u32 ImageIndex)
{
}

void VulkanQueue::WaitIdle()
{
    vkQueueWaitIdle(_queue);
}

void VulkanQueue::CreateSemaphores()
{
    _renderCompleteSemaphore = VulkanCore::CreateSemaphoreCustom(_device);
    _presentCompleteSemaphore = VulkanCore::CreateSemaphoreCustom(_device);
}
