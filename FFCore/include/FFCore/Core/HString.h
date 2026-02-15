#pragma once

#include <mutex>
#include <string>

#include "Assert.h"
#include "FFCore/Core/Types.h"

namespace FF
{
    class HString
    {
    public:
        HString()
            : _hash(0) {}

        HString(const char* s)
            : _hash(Hash(s)) {}

        HString(std::string s)
            : _hash(Hash(s.c_str())) {}

        [[nodiscard]]
        u32 GetHash() const { return _hash; }

        static HString None;

    private:
        u32 _hash {0};

    public:
        [[nodiscard]]
        const char* Get() const
        {
            std::lock_guard<std::mutex> lock(hashTableMutex);
            return hashTable[_hash];
        }

    private:
        // hash using prime numbers
        static u32 Hash(const char* s)
        {
            if (!s)
            {
                return 0;
            }

            const char* start = s;
            u32 h = 37;

            while (*s)
            {
                h = (h * 54059) ^ (s[0] * 76963);
                ++s;
            }

            h %= hashTableSize;

            std::lock_guard<std::mutex> lock(hashTableMutex);
            
#ifdef FF_DEBUG
            // Check for hash conflicts
            if (hashTable[h][0] != '\0')
            {
                for (u32 i = 0; i < hashTableMaxStringLength; i++)
                {
                    if (std::strncmp(hashTable[h], start, hashTableMaxStringLength) != 0)
                    {
                        ASSERT_NO_ENTRY(false, "HASH_CONFLICT")
                    }
                }
            }
#endif
            if (hashTable[h][0] == '\0')
            {
                const size_t len = std::min<std::size_t>(std::strlen(start), hashTableMaxStringLength - 1);
                std::memcpy(hashTable[h], start, len);
                hashTable[h][len] = '\0';
            }
            
            return h;
        }

        static constexpr u32 hashTableSize = 65536;
        static constexpr u32 hashTableMaxStringLength = 512;
        static char hashTable[hashTableSize][hashTableMaxStringLength];
        static std::mutex hashTableMutex;
        
    public:
        bool operator==(const HString& other) const
        {
            return other._hash == _hash;
        }

        bool operator!=(const HString& other) const
        {
            return !(*this == other);
        }

        bool operator<(const HString other) const
        {
            return _hash < other._hash;
        }

        struct HStringHasher
        {
            std::size_t operator()(const HString& k) const
            {
                return k.GetHash();
            }
        };
    };
}