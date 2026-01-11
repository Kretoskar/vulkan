#include "FFCore/Memory/AllocTracker.h"
#include <memory>

#include "FFCore/Core/Assert.h"

void* operator new(size_t size)
{
    using namespace FF::Memory;

    GlobalAllocationMetrics.TotalAllocated.fetch_add(size, std::memory_order_relaxed);

    const MemoryTag tag = TL_CurrentTag;
    GlobalTagMetrics[static_cast<size_t>(tag)].Allocated.fetch_add(size, std::memory_order_relaxed);

    if (void* p = std::malloc(size))
    {
        return p;
    }

    throw std::bad_alloc{};
}

void operator delete(void* memory, size_t size)
{
    using namespace FF::Memory;

    GlobalAllocationMetrics.TotalFreed.fetch_add(size, std::memory_order_relaxed);

    const MemoryTag tag = TL_CurrentTag;
    GlobalTagMetrics[static_cast<size_t>(tag)].Freed.fetch_add(size, std::memory_order_relaxed);

    std::free(memory);
}

void operator delete(void* memory)
{
    ASSERT_NO_ENTRY("Don't use unsized delete")
    free(memory);
}

void FF::Memory::DumpAllTags()
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
