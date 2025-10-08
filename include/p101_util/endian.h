#ifndef LIBP101_UTIL_P101_ENDIAN_H
#define LIBP101_UTIL_P101_ENDIAN_H

/*
 * Copyright 2025-2025 D'Arcy Smith.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __has_builtin
    #if __has_builtin(__builtin_bswap16)
        #define HAVE_BSWAP 1
    #endif
#endif
#ifndef HAVE_BSWAP
    #if defined(__GNUC__) || defined(__clang__)
        #define HAVE_BSWAP 1
    #endif
#endif

#if HAVE_BSWAP
    #define bswap16 __builtin_bswap16
    #define bswap32 __builtin_bswap32
    #define bswap64 __builtin_bswap64
#else
static inline uint16_t bswap16(uint16_t x)
{
    return (x >> 8) | (x << 8);
}

static inline uint32_t bswap32(uint32_t x)
{
    return ((x & 0x000000FFu) << 24) | ((x & 0x0000FF00u) << 8) | ((x & 0x00FF0000u) >> 8) | ((x & 0xFF000000u) >> 24);
}

static inline uint64_t bswap64(uint64_t x)
{
    return ((x & 0x00000000000000FFull) << 56) | ((x & 0x000000000000FF00ull) << 40) | ((x & 0x0000000000FF0000ull) << 24) | ((x & 0x00000000FF000000ull) << 8) | ((x & 0x000000FF00000000ull) >> 8) | ((x & 0x0000FF0000000000ull) >> 24) |
           ((x & 0x00FF000000000000ull) >> 40) | ((x & 0xFF00000000000000ull) >> 56);
}
#endif

#if !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__) || !defined(__ORDER_BIG_ENDIAN__)
    #error "Compiler does not define __BYTE_ORDER__ macros"
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define le16toh(x) (uint16_t)(x)
    #define le32toh(x) (uint32_t)(x)
    #define le64toh(x) (uint64_t)(x)
    #define be16toh(x) bswap16((uint16_t)(x))
    #define be32toh(x) bswap32((uint32_t)(x))
    #define be64toh(x) bswap64((uint64_t)(x))
    #define htole16(x) (uint16_t)(x)
    #define htole32(x) (uint32_t)(x)
    #define htole64(x) (uint64_t)(x)
    #define htobe16(x) bswap16((uint16_t)(x))
    #define htobe32(x) bswap32((uint32_t)(x))
    #define htobe64(x) bswap64((uint64_t)(x))
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define be16toh(x) (uint16_t)(x)
    #define be32toh(x) (uint32_t)(x)
    #define be64toh(x) (uint64_t)(x)
    #define le16toh(x) bswap16((uint16_t)(x))
    #define le32toh(x) bswap32((uint32_t)(x))
    #define le64toh(x) bswap64((uint64_t)(x))
    #define htobe16(x) (uint16_t)(x)
    #define htobe32(x) (uint32_t)(x)
    #define htobe64(x) (uint64_t)(x)
    #define htole16(x) bswap16((uint16_t)(x))
    #define htole32(x) bswap32((uint32_t)(x))
    #define htole64(x) bswap64((uint64_t)(x))
#else
    #error "Unknown byte order"
#endif

    int p101_is_little_endian(void);

#ifdef __cplusplus
}
#endif

#endif    // LIBP101_UTIL_P101_ENDIAN_H
