/***************************************************************************
* Copyright (c) 2016, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_HASH_HPP
#define XTL_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <type_traits>

namespace xtl
{

    std::size_t hash_bytes(const void* buffer, std::size_t lenght, std::size_t seed);

    /******************************
     *  hash_bytes implementation *
     ******************************/

    namespace detail
    {
        // Dummy hash implementation for unusual sizeof(std::size_t)
        template <std::size_t N>
        std::size_t murmur_hash(const void* buffer, std::size_t length, std::size_t seed)
        {
            std::size_t hash = seed;
            const char* data = static_cast<const char*>(buffer);
            for(; length !=0; --length)
            {
                hash = (hash * 131) + *data++;
            }
            return hash;
        }

        // Murmur hash is an algorithm written by Austin Appleby. See https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
        template <>
        inline std::size_t murmur_hash<4>(const void* buffer, std::size_t length, std::size_t seed)
        {
            constexpr std::size_t m = 0x5bd1e995;
            std::size_t hash = seed ^ length;
            const unsigned char* data = static_cast<const unsigned char*>(buffer);

            // Mix 4 bytes at a time into the hash.
            while(length >= 4)
            {
                std::size_t k;
                std::memcpy(&k, data, sizeof(k));
                k *= m;
                k ^= k >> 24;
                k *= m;
                hash *= m;
                hash^= k;
                data += 4;
                length -=4;
            }

            // Handle the last frwe bytes of the input array.
            switch(length)
            {
            case 3:
                hash ^= data[2] << 16;
            case 2:
                hash ^= data[1] << 8;
            case 1:
                hash ^= data[0];
                hash *= m;
            }

            // Do a few final mix of the hash to ensure the last few
            // bytes are well-incorporated.
            
            hash ^= hash >> 13;
            hash *= m;
            hash ^= hash >> 15;
            return hash;
        }

        inline std::size_t load_bytes(const char* p, int n)
        {
            std::size_t result = 0;
            --n;
            do
            {
                result = (result << 8) + static_cast<unsigned char>(p[n]);
            }
            while(--n >= 0);
            return result;
        }

#if INTPTR_MAX == INT64_MAX
        // 64-bits hash for 64-bits platform
        template <>
        inline std::size_t murmur_hash<8>(const void* buffer, std::size_t length, std::size_t seed)
        {
            constexpr std::size_t m = (static_cast<std::size_t>(0xc6a4a793UL) << 32UL) +
                                       static_cast<std::size_t>(0x5bd1e995UL);
            constexpr int r = 47;
            const char* data = static_cast<const char*>(buffer);
            const char* end = data + (length & ~0x7);
            std::size_t hash = seed ^ (length  * m);
            while(data != end)
            {
                std::size_t k;
                std::memcpy(&k, data, sizeof(k));
                k *= m;
                k ^= k >> r;
                k *= m;
                hash ^= k;
                hash *= m;
                data += 8;
            }
            if((length & 0x7) != 0)
            {
                std::size_t k = load_bytes(end, length & 0x7);
                hash ^= k;
                hash *= m;
            }
            hash ^= hash >> r;
            hash *= m;
            hash ^= hash >> r;

            return hash;
        }
#elif INTPTR_MAX == INT32_MAX
        //64-bits hash for 32-bits platform
        template <>
        inline std::size_t murmur_hash<8>(const void* buffer, std::size_t length, std::size_t seed)
        {
            constexpr uint32_t m = 0x5bd1e995;
            constexpr int r = 24;

            uint32_t h1 = uint32_t(seed) ^ int(length);
            uint32_t h2 = uint32_t(seed >> 32);

            const uint32_t* data = static_cast<const uint32_t*>(buffer);

            while(length >= 8)
            {
                uint32_t k1 = *data++;
                k1 *= m; k1 ^= k1 >> r; k1 *= m;
                h1 *= m; h1 ^= k1;
                length -= 4;

                uint32_t k2 = *data++;
                k2 *= m; k2 ^= k2 >> r; k2 *= m;
                h2 *= m; h2 ^= k2;
                length -= 4;
            }

            if(length >= 4)
            {
                uint32_t k1 = *data++;
                k1 *= m; k1 ^= k1 >> r; k1 *= m;
                h1 *= m; h1 ^= k1;
                length -= 4;
            }

            switch(length)
            {
                case 3:
                    h2 ^= ((unsigned char*)data)[2] << 16;
                case 2:
                    h2 ^= ((unsigned char*)data)[1] << 8;
                case 1:
                    h2 ^= ((unsigned char*)data)[0];
                    h2 *= m;
            }

            h1 ^= h2 >> 18; h1 *= m;
            h2 ^= h1 >> 22; h2 *= m;
            h1 ^= h2 >> 17; h1 *= m;
            h2 ^= h1 >> 19; h2 *= m;

            uint64_t h = h1;
            h = (h << 32) | h2;
            return h;
        }
#else
#error Unknown pointer size or missing size macros!
#endif
    }

    inline std::size_t hash_bytes(const void* buffer, std::size_t length, std::size_t seed)
    {
        return detail::murmur_hash<sizeof(std::size_t)>(buffer, length, seed);
    }

}

#endif

