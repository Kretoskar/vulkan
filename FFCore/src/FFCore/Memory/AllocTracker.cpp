#include "FFCore/Memory/AllocTracker.h"
#include <memory>

#include "FFCore/Core/Assert.h"

void* operator new(size_t size)
{
    return FF::Memory::AllocateWithHeader(size);
}

void operator delete(void* memory)
{
    if (!memory)
    {
        return;
    }

    using namespace FF::Memory;

    AllocationHeader* h = HeaderFromUserPtr(memory);
    
    if (h->Magic != memMagic)
    {
        // pointer not from our allocator
        std::free(h);
        return;
    }

    const u64 size = h->Size;
    const MemoryTag tag = h->Tag;
    
    GlobalAllocationMetrics.TotalFreed.fetch_add(size, std::memory_order_relaxed);
    GlobalTagMetrics[static_cast<size_t>(tag)].Freed.fetch_add(size, std::memory_order_relaxed);

    std::free(h);
}

void operator delete(void* memory, size_t size)
{
    ::operator delete(memory);
}