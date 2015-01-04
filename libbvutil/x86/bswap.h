/*
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * byte swapping routines
 */

#ifndef BVUTIL_X86_BSWAP_H
#define BVUTIL_X86_BSWAP_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/attributes.h"

#if BV_HAVE_INLINE_ASM

#if !BV_GCC_VERSION_AT_LEAST(4,1)
#define bv_bswap16 bv_bswap16
static bv_always_inline bv_const unsigned bv_bswap16(unsigned x)
{
    __asm__("rorw $8, %w0" : "+r"(x));
    return x;
}
#endif /* !BV_GCC_VERSION_AT_LEAST(4,1) */

#if !BV_GCC_VERSION_AT_LEAST(4,5)
#define bv_bswap32 bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
    __asm__("bswap   %0" : "+r" (x));
    return x;
}

#if BV_ARCH_X86_64
#define bv_bswap64 bv_bswap64
static inline uint64_t bv_const bv_bswap64(uint64_t x)
{
    __asm__("bswap  %0": "=r" (x) : "0" (x));
    return x;
}
#endif
#endif /* !BV_GCC_VERSION_AT_LEAST(4,5) */

#endif /* BV_HAVE_INLINE_ASM */
#endif /* BVUTIL_X86_BSWAP_H */
