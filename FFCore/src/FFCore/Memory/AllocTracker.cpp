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
