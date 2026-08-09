/*
 * Copyright 2025-2026 D'Arcy Smith.
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

#include "p101_util/endian.h"
#include <p101_env/env.h>
#include <stdbool.h>

enum
{
    ONE_BYTE_SHIFT   = 8,
    THREE_BYTE_SHIFT = 24,
    FIVE_BYTE_SHIFT  = 40,
    SEVEN_BYTE_SHIFT = 56
};

static uint16_t bswap16_value(uint16_t value)
{
    return (uint16_t)((value >> ONE_BYTE_SHIFT) | (value << ONE_BYTE_SHIFT));
}

static uint32_t bswap32_value(uint32_t value)
{
    return ((value & UINT32_C(0x000000ff)) << THREE_BYTE_SHIFT) | ((value & UINT32_C(0x0000ff00)) << ONE_BYTE_SHIFT) | ((value & UINT32_C(0x00ff0000)) >> ONE_BYTE_SHIFT) | ((value & UINT32_C(0xff000000)) >> THREE_BYTE_SHIFT);
}

static uint64_t bswap64_value(uint64_t value)
{
    return ((value & UINT64_C(0x00000000000000ff)) << SEVEN_BYTE_SHIFT) | ((value & UINT64_C(0x000000000000ff00)) << FIVE_BYTE_SHIFT) | ((value & UINT64_C(0x0000000000ff0000)) << THREE_BYTE_SHIFT) | ((value & UINT64_C(0x00000000ff000000)) << ONE_BYTE_SHIFT) |
           ((value & UINT64_C(0x000000ff00000000)) >> ONE_BYTE_SHIFT) | ((value & UINT64_C(0x0000ff0000000000)) >> THREE_BYTE_SHIFT) | ((value & UINT64_C(0x00ff000000000000)) >> FIVE_BYTE_SHIFT) | ((value & UINT64_C(0xff00000000000000)) >> SEVEN_BYTE_SHIFT);
}

static bool is_little_endian_value(void)
{
    bool little_endian;

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    if(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)    // NOLINT(misc-redundant-expression)
    {
        little_endian = true;
    }
    else
    {
        little_endian = false;
    }
#else
    const uint16_t value = UINT16_C(0x0102);
    const uint8_t *bytes;

    bytes = (const uint8_t *)&value;
    if(bytes[0] == UINT8_C(0x02))
    {
        little_endian = true;
    }
    else
    {
        little_endian = false;
    }
#endif

    return little_endian;
}

uint16_t p101_bswap16(const struct p101_env *env, uint16_t value)
{
    uint16_t ret_val;

    P101_TRACE(env);
    ret_val = bswap16_value(value);
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint32_t p101_bswap32(const struct p101_env *env, uint32_t value)
{
    uint32_t ret_val;

    P101_TRACE(env);
    ret_val = bswap32_value(value);
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint64_t p101_bswap64(const struct p101_env *env, uint64_t value)
{
    uint64_t ret_val;

    P101_TRACE(env);
    ret_val = bswap64_value(value);
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint16_t p101_le16toh(const struct p101_env *env, uint16_t value)
{
    bool     little_endian;
    uint16_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap16_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint32_t p101_le32toh(const struct p101_env *env, uint32_t value)
{
    bool     little_endian;
    uint32_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap32_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint64_t p101_le64toh(const struct p101_env *env, uint64_t value)
{
    bool     little_endian;
    uint64_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap64_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint16_t p101_be16toh(const struct p101_env *env, uint16_t value)
{
    bool     little_endian;
    uint16_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap16_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint32_t p101_be32toh(const struct p101_env *env, uint32_t value)
{
    bool     little_endian;
    uint32_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap32_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint64_t p101_be64toh(const struct p101_env *env, uint64_t value)
{
    bool     little_endian;
    uint64_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap64_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint16_t p101_htole16(const struct p101_env *env, uint16_t value)
{
    bool     little_endian;
    uint16_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap16_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint32_t p101_htole32(const struct p101_env *env, uint32_t value)
{
    bool     little_endian;
    uint32_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap32_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint64_t p101_htole64(const struct p101_env *env, uint64_t value)
{
    bool     little_endian;
    uint64_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = value;
    }
    else
    {
        ret_val = bswap64_value(value);
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint16_t p101_htobe16(const struct p101_env *env, uint16_t value)
{
    bool     little_endian;
    uint16_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap16_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint32_t p101_htobe32(const struct p101_env *env, uint32_t value)
{
    bool     little_endian;
    uint32_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap32_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

uint64_t p101_htobe64(const struct p101_env *env, uint64_t value)
{
    bool     little_endian;
    uint64_t ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = bswap64_value(value);
    }
    else
    {
        ret_val = value;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}

bool p101_is_little_endian(const struct p101_env *env)
{
    bool little_endian;
    bool ret_val;

    P101_TRACE(env);
    little_endian = is_little_endian_value();
    if(little_endian)
    {
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }
    P101_TRACE_EXIT(env);

    return ret_val;
}
