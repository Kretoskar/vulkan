#pragma once
#include <atomic>
#include <cstdint>

#include "FFCore/Core/Logger.h"
#include "FFCore/Core/Types.h"

namespace FF::Memory
{
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

        void Dump()
        {
           LOG_MESSAGE("Allocated: %d Freed: %d Usage: %d", GetTotalAllocated(), GetTotalFreed(), CurrentUsage());
        }
    };

    inline AllocationMetrics GlobalAllocationMetrics;
}