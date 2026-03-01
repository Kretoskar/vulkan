#include "FFEngine/Rendering/VulkanRenderer.h"

#include "FFVulkan/VulkanUtils.h"

using namespace FFE;

void VulkanRenderer::RecordCommandBuffers()
{
    VkClearColorValue clearColor = { 1.0f, 0.0f, 0.0f, 0.0f };

    VkImageSubresourceRange imageRange =
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,    // just clear the color buffers
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    for (u32 i = 0; i < _cmdBuffers.size(); ++i)
    {
        _vulkanCore.Cmd_ClearColorImage(
            _cmdBuffers[i],
            _vulkanCore.GetImage(i),
            VK_IMAGE_LAYOUT_GENERAL,
            &clearColor,
            /* number of elements in imageRange */ 1,
            &imageRange);
    }
}
