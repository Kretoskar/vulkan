#pragma once

#include "FFCore/Core/Assert.h"
#include "FFCore/Core/Logger.h"
#include "vulkan/vulkan.h"

namespace FFVk
{
    template<typename Func, typename... Args>
    VkResult CallVkFunc(Func&& func, const char* errorMesssage, Args&&... args)
    {
        VkResult result = std::forward<Func>(func)(
            std::forward<Args>(args)...
        );

        char buffer[LOG_TEMPLATE_LENGTH];
        sprintf_s(buffer, "Vulkan Error %d\n%s", result, errorMesssage);
        
        ASSERT(result == VK_SUCCESS, buffer)

        return result;
    }
}