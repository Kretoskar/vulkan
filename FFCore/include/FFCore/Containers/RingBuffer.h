#pragma once

#include <array>
#include <mutex>
#include <vector>
#include <cstddef>

namespace FF
{
    template <typename T, u64 Cap>
    class RingBuffer
    {
        static_assert(Cap > 0, "Capacity must be > 0");

    public:
        RingBuffer() = default;
        
        void Push(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);

            data[write] = value;
            write = (write + 1) % Cap;

            if (size < Cap)
            {
                ++size;
            }
        }

        void Push(T&& value)
        {
            std::lock_guard<std::mutex> lock(mutex);

            data[write] = std::move(value);
            write = (write + 1) % Cap;

            if (size < Cap)
            {
                ++size;
            }
        }

        u64 CopyTo(T* out, u64 maxCount) const
        {
            std::lock_guard<std::mutex> lock(mutex);

            const std::size_t count = (size < maxCount) ? size : maxCount;
            const std::size_t start = (size < Cap) ? 0 : write;

            for (std::size_t i = 0; i < count; ++i)
            {
                const std::size_t idx = (start + i) % Cap;
                out[i] = data[idx];
            }
            return count;
        }
        
        std::size_t Count() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return size;
        }

        constexpr std::size_t Capacity() const
        {
            return Cap;
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(mutex);
            write = 0;
            size = 0;
        }

    private:
        mutable std::mutex mutex;
        std::array<T, Cap> data{};

        std::size_t write = 0; // next write position
        std::size_t size = 0;  // how many valid elements
    };
}