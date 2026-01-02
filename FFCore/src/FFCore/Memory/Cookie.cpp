#include "FFCore/Memory/Cookie.h"

#include <map>
#include <mutex>
#include <unordered_map>

namespace FF
{
    constexpr uint32_t hash_fnv1a_32(const char* s)
    {
        uint32_t h = 2166136261u;
        while (*s)
        {
            h ^= u8(*s++);
            h *= 16777619u;
        }
        return h;
    }
    
    Cookie Cookie::MakeCookie(const char* name)
    {
        return Cookie(name, hash_fnv1a_32(name));
    }

    void AllocatorStats::Add(Cookie id, std::size_t bytes)
    {
        Counters& c = GetOrCreate(id);
        c.Bytes.fetch_add(bytes, std::memory_order_relaxed);
        c.Allocs.fetch_add(1, std::memory_order_relaxed);
    }

    void AllocatorStats::Subtract(Cookie id, std::size_t bytes)
    {
        Counters& c = GetOrCreate(id);
        c.Bytes.fetch_sub(bytes, std::memory_order_relaxed);
        c.Frees.fetch_add(1, std::memory_order_relaxed);
    }

    std::size_t AllocatorStats::Bytes(const char* cookieName)
    {
        Cookie id = Cookie::MakeCookie(cookieName);
        Counters* c = Find(id);
        return c ? c->Bytes.load(std::memory_order_relaxed) : 0;
    }

    void AllocatorStats::Dump(std::ostream& os)
    {
        std::lock_guard<std::mutex> lock(Mutex());
        os << "---- Cookie Allocation Stats ----\n";
        for (auto& kv : Map())
        {
            const Cookie& id = kv.first;
            const Counters& c = kv.second;
            os << id.Name
               << " | bytes=" << c.Bytes.load()
               << " allocs=" << c.Allocs.load()
               << " frees=" << c.Frees.load()
               << "\n";
        }
    }

    std::unordered_map<Cookie, Counters, AllocatorStats::KeyHash, AllocatorStats::KeyEq>& AllocatorStats::Map()
    {
        static std::unordered_map<Cookie, Counters, KeyHash, KeyEq> m;
        return m;
    }

    std::mutex& AllocatorStats::Mutex()
    {
        static std::mutex mx;
        return mx;
    }

    Counters& AllocatorStats::GetOrCreate(Cookie ID)
    {
        // TODO: FIXME: Now map uses the custom new with cookies and it breaks bcs ofd multiple access
        
        // unordered_map isn't thread-safe. Always lock and rely on atomics for performance.
        std::lock_guard<std::mutex> lock(Mutex());
        return Map().try_emplace(ID).first->second;
    }

    Counters* AllocatorStats::Find(Cookie ID)
    {
        std::lock_guard<std::mutex> lock(Mutex());
        auto it = Map().find(ID);
        return (it == Map().end()) ? nullptr : &it->second;
    }
}

void* operator new(std::size_t sz)
{
    return FF::CookieMalloc(sz);
}

void operator delete(void* p) noexcept
{
    FF::CookieFree(p);
}

void* operator new[](std::size_t sz)
{
    return FF::CookieMalloc(sz);
}

void operator delete[](void* p) noexcept {
    FF::CookieFree(p);
}

// handle sized delete in case I feel fancy
void operator delete(void* p, std::size_t) noexcept
{
    FF::CookieFree(p);
}

void operator delete[](void* p, std::size_t) noexcept
{
    FF::CookieFree(p);
}
