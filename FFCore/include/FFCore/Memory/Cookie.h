#pragma once

#include <atomic>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "FFCore/Core/Types.h"

#define COOKIE_SCOPE(name_literal) FF::CookieScope _cookie_scope_##__LINE__{name_literal};

#define COOKIE_NEW(name_literal) (FF::CookieScope{name_literal}, new)

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
           std::size_t operator()(Cookie const& k) const noexcept { return k.Hash; }
        };

        struct KeyEq
        {
            bool operator()(Cookie const& a, Cookie const& b) const noexcept
            {
                // hash collision is possible; compare strings too
                // TODO: maybe only check collisions in DEBUG
                return a.Hash == b.Hash && std::string_view(a.Name) == std::string_view(b.Name);
            }
        };

        static std::unordered_map<Cookie, Counters, KeyHash, KeyEq>& Map();
        static std::mutex& Mutex();
        static Counters& GetOrCreate(Cookie ID);
        static Counters* Find(Cookie ID);
    };

    inline thread_local Cookie tlCookie = Cookie::MakeCookie("UNSPECIFIED");

    class CookieScope
    {
    public:
        explicit CookieScope(const char* name) : prev(tlCookie)
        {
            tlCookie = Cookie::MakeCookie(name);
        }
        ~CookieScope() { tlCookie = prev; }
        CookieScope(CookieScope const&) = delete;
        CookieScope& operator=(CookieScope const&) = delete;
        
    private:
        Cookie prev;
    };

    struct Header
    {
        std::size_t size;
        Cookie cookie;
        uint32_t magic;
        
    };
    
    constexpr uint32_t kMagic = 0xB00B1E42u;

    // Align header to max_align_t.
    inline std::size_t AlignUp(std::size_t n, std::size_t a)
    {
        return (n + (a - 1)) & ~(a - 1);
    }

    inline void* CookieMalloc(std::size_t userSize)
    {
        const std::size_t headerSize = AlignUp(sizeof(Header), alignof(std::max_align_t));
        const std::size_t total = headerSize + userSize;

        void* raw = std::malloc(total);
        if (!raw) throw std::bad_alloc{};

        auto* h = reinterpret_cast<Header*>(raw);
        h->size = userSize;
        h->cookie = tlCookie;     // capture cookie at allocation time
        h->magic = kMagic;

        AllocatorStats::Add(h->cookie, userSize);

        std::byte* userPtr = reinterpret_cast<std::byte*>(raw) + headerSize;
        return userPtr;
    }

    inline void CookieFree(void* p) noexcept
    {
        if (!p)
        {
            return;
        }

        const std::size_t headerSize = AlignUp(sizeof(Header), alignof(std::max_align_t));
        std::byte* raw = reinterpret_cast<std::byte*>(p) - headerSize;
        auto* h = reinterpret_cast<Header*>(raw);

        if (h->magic == kMagic)
        {
            AllocatorStats::Subtract(h->cookie, h->size);
            std::free(raw);
        }
        else
        {
            // wasn't allocated by cookie
            std::free(p);
        }
    }
}
