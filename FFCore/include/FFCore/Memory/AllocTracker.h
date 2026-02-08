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

    struct AllocationHeader
    {
        u64 Size;
        MemoryTag Tag;
        u32 Magic;
    };
    
    static constexpr u32 memMagic = 0xB00B6967;
    
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

    
    inline void DumpAllTags()
    {
        for (u32 i = 0; i < static_cast<u32>(MemoryTag::Count); ++i)
        {
            const MemoryTag tag = static_cast<MemoryTag>(i);
            const TagMetrics& m = GlobalTagMetrics[i];

            const u64 allocated = m.Allocated.load(std::memory_order_relaxed);
            const u64 freed = m.Freed.load(std::memory_order_relaxed);
            const u64 usage = allocated - freed;

            if (allocated == 0 && freed == 0)
            {
                continue;
            }

            LOG_MESSAGE("[%s] Allocated: %llu Freed: %llu Usage: %llu",
                ToString(tag).Get(), allocated, freed, usage)
        }
    }

    inline void* AllocateWithHeader(size_t size)
    {
        const MemoryTag tag = TL_CurrentTag;

        const size_t total = sizeof(AllocationHeader) + size;
        void* raw = std::malloc(total);
        if (!raw)
        {
            throw std::bad_alloc{};
        }

        auto* h = static_cast<AllocationHeader*>(raw);
        h->Size  = static_cast<u64>(size);
        h->Tag   = tag;
        h->Magic = memMagic;
        
        GlobalAllocationMetrics.TotalAllocated.fetch_add(size, std::memory_order_relaxed);
        GlobalTagMetrics[static_cast<size_t>(tag)].Allocated.fetch_add(size, std::memory_order_relaxed);

        // Return pointer after header
        return static_cast<void*>(h + 1);
    }

    inline AllocationHeader* HeaderFromUserPtr(void* p)
    {
        return static_cast<AllocationHeader*>(p) - 1;
    }
}