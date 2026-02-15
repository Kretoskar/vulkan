#pragma once

#include "Logger.h"
#if defined (_WIN32)
    #include <windows.h>
#endif
#include <cstdlib>

namespace FF
{
    inline void AssertImpl(bool condition, const char* message, const char* file, i32 line)
    {
        if (!condition)
        {
            char buffer[LOG_TEMPLATE_LENGTH];
            
            sprintf_s(
                buffer,
                "%s\n\n[Assert] [File: %s] [Line: %d]\n",
                message,
                file,
                line
            );

            LOG_ERROR(message);

            MessageBoxA(
                nullptr,
                buffer,
                "Assert Entered",
                MB_ICONERROR | MB_OK
            );

            std::abort();
        }
    }
}

#ifdef FF_DEBUG

#define ASSERT(condition, message) \
    {FF::AssertImpl((condition), (message), __FILE__, __LINE__);}

#define ASSERT_NO_ENTRY() \
    ASSERT(false, "No entry assert entered")

#else
    #   define ASSERT(condition, message) {}
    #   define ASSERT_NO_ENTRY(message) {}
#endif