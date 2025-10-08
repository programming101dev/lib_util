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

#include "p101_util/endian.h"

/* Returns 1 if host is little-endian, 0 if big-endian */
int p101_is_little_endian(void)
{
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    /* compile-time constant path */
    return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;    // NOLINT(misc-redundant-expression)
#else
    /* portable runtime probe */
    const uint16_t v = 0x0102;
    const uint8_t *p = (const uint8_t *)&v;

    return p[0] == 0x02;
#endif
}
