#pragma once

#include <atomic>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "FFCore/Core/Types.h"

namespace FF
{
    // 16 bytes, cheap enough to pass by value
    struct Cookie
    {
        // TODO: HString?
        const char* Name = "none";
        u32 Hash = 0;

        static Cookie MakeCookie(const char* name);
    };

    struct Counters
    {
        std::atomic<std::size_t> Bytes{0};
        std::atomic<std::size_t> Allocs{0};
        std::atomic<std::size_t> Frees{0};
    };

    class AllocatorStats
    {
    public:
        static void Add(Cookie id, std::size_t bytes);
        static void Subtract(Cookie id, std::size_t bytes);
        static std::size_t Bytes(const char* cookieName);
        static void Dump(std::ostream& os = std::cout);
        
    private:
        struct KeyHash
        {
           std::size_t operator()(Cookie const& k) const noexcept;
        };

        struct KeyEq
        {
            bool operator()(Cookie const& a, Cookie const& b) const noexcept;
        };

        static std::unordered_map<Cookie, Counters, KeyHash, KeyEq>& Map();
        static std::mutex& Mutex();
        static Counters& GetOrCreate(Cookie ID);
        static Counters* Find(Cookie ID);
    };
}
