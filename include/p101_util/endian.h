#ifndef LIBP101_UTIL_ENDIAN_H
#define LIBP101_UTIL_ENDIAN_H

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

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct p101_env;

    uint16_t p101_bswap16(const struct p101_env *env, uint16_t value);
    uint32_t p101_bswap32(const struct p101_env *env, uint32_t value);
    uint64_t p101_bswap64(const struct p101_env *env, uint64_t value);

    uint16_t p101_le16toh(const struct p101_env *env, uint16_t value);
    uint32_t p101_le32toh(const struct p101_env *env, uint32_t value);
    uint64_t p101_le64toh(const struct p101_env *env, uint64_t value);
    uint16_t p101_be16toh(const struct p101_env *env, uint16_t value);
    uint32_t p101_be32toh(const struct p101_env *env, uint32_t value);
    uint64_t p101_be64toh(const struct p101_env *env, uint64_t value);

    uint16_t p101_htole16(const struct p101_env *env, uint16_t value);
    uint32_t p101_htole32(const struct p101_env *env, uint32_t value);
    uint64_t p101_htole64(const struct p101_env *env, uint64_t value);
    uint16_t p101_htobe16(const struct p101_env *env, uint16_t value);
    uint32_t p101_htobe32(const struct p101_env *env, uint32_t value);
    uint64_t p101_htobe64(const struct p101_env *env, uint64_t value);

    int p101_is_little_endian(const struct p101_env *env);

#ifdef __cplusplus
}
#endif

#endif
