#pragma once
#include <array>
#include <atomic>
#include <cinttypes>
#include <cstdint>

#include "FFCore/Core/HString.h"
#include "FFCore/Core/Logger.h"
#include "FFCore/Core/Types.h"

namespace FF::Memory
{
    enum class MemoryTag : u32
    {
        Default = 0,
        Core,
        Rendering,
        
        Count
    };

    inline HString ToString(MemoryTag t)
    {
        switch (t)
        {
            case MemoryTag::Default:        return "Default";
            case MemoryTag::Core:           return "Core";
            case MemoryTag::Rendering:      return "Rendering";
            default:                        return "Untagged";
        }
    }

    struct TagMetrics
    {
        std::atomic<u64> Allocated{0};
        std::atomic<u64> Freed{0};

        u64 Usage() const
        {
            return Allocated.load(std::memory_order_relaxed)
                 - Freed.load(std::memory_order_relaxed);
        }
    };
    
    struct AllocationMetrics
    {
        std::atomic<u64> TotalAllocated{0};
        std::atomic<u64> TotalFreed{0};

        u64 CurrentUsage() const
        {
            return TotalAllocated.load(std::memory_order_relaxed)
                - TotalFreed.load(std::memory_order_relaxed);
        }
        
        u64 GetTotalAllocated() const
        {
            return TotalAllocated.load(std::memory_order_relaxed);
        }
        
        u64 GetTotalFreed() const
        {
            return TotalFreed.load(std::memory_order_relaxed);
        }
    };

    inline AllocationMetrics GlobalAllocationMetrics;

    // Per-tag counters
    inline std::array<TagMetrics, static_cast<size_t>(MemoryTag::Count)> GlobalTagMetrics;

    // Thread-local tag
    inline thread_local MemoryTag TL_CurrentTag = MemoryTag::Default;

    struct MemoryTagScope
    {
        MemoryTag Prev;

        explicit MemoryTagScope(MemoryTag tag) noexcept
            : Prev(TL_CurrentTag)
        {
            TL_CurrentTag = tag;
        }

        ~MemoryTagScope() noexcept
        {
            TL_CurrentTag = Prev;
        }

        MemoryTagScope(const MemoryTagScope&) = delete;
        MemoryTagScope& operator=(const MemoryTagScope&) = delete;
    };

    
    inline void DumpAllTags();
}