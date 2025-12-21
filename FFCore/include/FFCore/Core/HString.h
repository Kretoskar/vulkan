#pragma once

#include <string>

#include "Assert.h"
#include "FFCore/Core/Types.h"

namespace FF
{
    class HString
    {
    public:
        HString()
            : _hash(hashTableSize + 1) {}

        HString(const char* s)
            : _hash(Hash(s)) {}

        HString(std::string s)
            : _hash(Hash(s.c_str())) {}

        [[nodiscard]]
        u32 GetHash() const { return _hash; }

        static HString None;

    private:
        u32 _hash {};

    public:
        [[nodiscard]]
        const char* Get() const
        {
            return hashTable[_hash];
        }

    private:
        // hash using prime numbers
        static constexpr u32 Hash(const char* s)
        {
            u32 h = 37;
            u32 size = 0;

            while (*s)
            {
                h = (h * 54059) ^ (s[0] * 76963);
                size++;
                s++;
            }

            h %= hashTableSize;

            s = s - size;

#ifdef FF_DEBUG
            // Check for hash conflicts
            if (hashTable[h][0] != '\0')
            {
                for (u32 i = 0; i < hashTableMaxStringLength; i++)
                {
                    const char existingChar = hashTable[h][i];
                    const char newChar = *(s + i);

                    if ((existingChar == newChar) && newChar == '\0')
                    {
                        break;
                    }

                    ASSERT(existingChar == newChar, "HASH_CONFLICT")
                }
            }
#endif

            u32 i = 0;

            while (*s)
            {
                hashTable[h][i] = *s;
                i++;
                s++;
            }

            return h;
        }

        static constexpr u32 hashTableSize = 65536;
        static constexpr u32 hashTableMaxStringLength = 512;
        static char hashTable[hashTableSize][hashTableMaxStringLength];

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