#include "FFCore/Memory/AllocTracker.h"
#include <memory>

#include "FFCore/Core/Assert.h"

void* operator new(size_t size)
{
    FF::Memory::GlobalAllocationMetrics.TotalAllocated.fetch_add(
        size, std::memory_order_relaxed);

    if (void* p = std::malloc(size))
    {
        return p;
    }
    throw std::bad_alloc{};
}

void operator delete(void* memory, size_t size)
{
    FF::Memory::GlobalAllocationMetrics.TotalFreed.fetch_add(
        size, std::memory_order_relaxed);

    std::free(memory);
}

void operator delete(void* memory)
{
    ASSERT_NO_ENTRY("Don't use unsized delete")
    free(memory);
}