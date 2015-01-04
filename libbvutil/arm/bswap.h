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

#ifndef BVUTIL_ARM_BSWAP_H
#define BVUTIL_ARM_BSWAP_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/attributes.h"

#ifdef __ARMCC_VERSION

#if BV_HAVE_ARMV6
#define bv_bswap32 bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
    return __rev(x);
}
#endif /* BV_HAVE_ARMV6 */

#elif BV_HAVE_INLINE_ASM

#if BV_HAVE_ARMV6_INLINE
#define bv_bswap16 bv_bswap16
static bv_always_inline bv_const unsigned bv_bswap16(unsigned x)
{
    __asm__("rev16 %0, %0" : "+r"(x));
    return x;
}
#endif

#if !BV_GCC_VERSION_AT_LEAST(4,5)
#define bv_bswap32 bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
#if BV_HAVE_ARMV6_INLINE
    __asm__("rev %0, %0" : "+r"(x));
#else
    uint32_t t;
    __asm__ ("eor %1, %0, %0, ror #16 \n\t"
             "bic %1, %1, #0xFF0000   \n\t"
             "mov %0, %0, ror #8      \n\t"
             "eor %0, %0, %1, lsr #8  \n\t"
             : "+r"(x), "=&r"(t));
#endif /* BV_HAVE_ARMV6_INLINE */
    return x;
}
#endif /* !BV_GCC_VERSION_AT_LEAST(4,5) */

#endif /* __ARMCC_VERSION */

#endif /* BVUTIL_ARM_BSWAP_H */
