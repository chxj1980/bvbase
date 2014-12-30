/*
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVbase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_ARM_INTREADWRITE_H
#define BVUTIL_ARM_INTREADWRITE_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/attributes.h"

#if HAVE_FAST_UNALIGNED && HAVE_INLINE_ASM && !BV_GCC_VERSION_AT_LEAST(4,7)

#define BV_RN16 BV_RN16
static bv_always_inline unsigned BV_RN16(const void *p)
{
    const uint8_t *q = p;
    unsigned v;
#if !BV_GCC_VERSION_AT_LEAST(4,6)
    __asm__ ("ldrh %0, %1" : "=r"(v) : "m"(*(const uint16_t *)q));
#elif defined __thumb__
    __asm__ ("ldrh %0, %1" : "=r"(v) : "m"(q[0]), "m"(q[1]));
#else
    __asm__ ("ldrh %0, %1" : "=r"(v) : "Uq"(q[0]), "m"(q[1]));
#endif
    return v;
}

#define BV_WN16 BV_WN16
static bv_always_inline void BV_WN16(void *p, uint16_t v)
{
    __asm__ ("strh %1, %0" : "=m"(*(uint16_t *)p) : "r"(v));
}

#define BV_RN32 BV_RN32
static bv_always_inline uint32_t BV_RN32(const void *p)
{
    const struct __attribute__((packed)) { uint32_t v; } *q = p;
    uint32_t v;
    __asm__ ("ldr  %0, %1" : "=r"(v) : "m"(*q));
    return v;
}

#define BV_WN32 BV_WN32
static bv_always_inline void BV_WN32(void *p, uint32_t v)
{
    __asm__ ("str  %1, %0" : "=m"(*(uint32_t *)p) : "r"(v));
}

#if HAVE_ASM_MOD_Q

#define BV_RN64 BV_RN64
static bv_always_inline uint64_t BV_RN64(const void *p)
{
    const struct __attribute__((packed)) { uint32_t v; } *q = p;
    uint64_t v;
    __asm__ ("ldr   %Q0, %1  \n\t"
             "ldr   %R0, %2  \n\t"
             : "=&r"(v)
             : "m"(q[0]), "m"(q[1]));
    return v;
}

#define BV_WN64 BV_WN64
static bv_always_inline void BV_WN64(void *p, uint64_t v)
{
    __asm__ ("str  %Q2, %0  \n\t"
             "str  %R2, %1  \n\t"
             : "=m"(*(uint32_t*)p), "=m"(*((uint32_t*)p+1))
             : "r"(v));
}

#endif /* HAVE_ASM_MOD_Q */

#endif /* HAVE_INLINE_ASM */

#endif /* BVUTIL_ARM_INTREADWRITE_H */
