#pragma once

#include "FFCore/Core/Assert.h"
#include "FFCore/Core/Logger.h"
#include "vulkan/vulkan.h"

namespace FFVk
{
    #define VK_FIND_CALL_AND_CHECK(FuncType, instance, errorMessage, ...)       \
        FindCallAndCheckVKFunc<FuncType>(                                       \
        instance,                                                               \
        (#FuncType) + 4, /* strip 'PFN_' from FuncType */                       \
        errorMessage,                                                           \
        __VA_ARGS__                                                             \
        );

    #define VK_CALL(Func, ErrorMessage, ...)                                    \
        CallVkFunc(                                                             \
        Func,                                                                   \
        ErrorMessage,                                                           \
        __VA_ARGS__                                                             \
        );

    #define VK_FIND_AND_CALL(FuncType, instance, ...)                           \
        FindAndCallVKFunc<FuncType>(                                            \
        instance,                                                               \
        (#FuncType) + 4, /* strip 'PFN_' from FuncType */                       \
        __VA_ARGS__                                                             \
        );

    
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

    template<typename T>
    T FindVKFunc(VkInstance instance, const char* name)
    {
        T function = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
        ASSERT(function, "Failed to load vulkan function '%s'", name)
        return function;
    }

    template<typename FuncT, typename... Args>
    VkResult FindCallAndCheckVKFunc(
        VkInstance instance,
        const char* funcName,
        const char* errorMessage,
        Args&&... args
    )
    {
        FuncT func = FindVKFunc<FuncT>(instance, funcName);

        return CallVkFunc(
            func,
            errorMessage,
            std::forward<Args>(args)...
        );
    }

    template<typename FuncT, typename... Args>
    void FindAndCallVKFunc(
        VkInstance instance,
        const char* funcName,
        Args&&... args
    )
    {
        FuncT func = FindVKFunc<FuncT>(instance, funcName);
        func(std::forward<Args>(args)...);
    }
    
    const char* GetDebugSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
    const char* GetDebugType(VkDebugUtilsMessageTypeFlagsEXT Type);
}