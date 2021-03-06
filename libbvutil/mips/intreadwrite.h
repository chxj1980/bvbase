/*
 * Copyright (c) 2009 Mans Rullgard <mans@mansr.com>
 *
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

#ifndef BVUTIL_MIPS_INTREADWRITE_H
#define BVUTIL_MIPS_INTREADWRITE_H

#include <stdint.h>
#include "config.h"

#if BV_ARCH_MIPS64 && BV_HAVE_INLINE_ASM

#define BV_RN32 BV_RN32
static bv_always_inline uint32_t BV_RN32(const void *p)
{
    struct __attribute__((packed)) u32 { uint32_t v; };
    const uint8_t *q = p;
    const struct u32 *pl = (const struct u32 *)(q + 3 * !BV_HAVE_BIGENDIAN);
    const struct u32 *pr = (const struct u32 *)(q + 3 *  BV_HAVE_BIGENDIAN);
    uint32_t v;
    __asm__ ("lwl %0, %1  \n\t"
             "lwr %0, %2  \n\t"
             : "=&r"(v)
             : "m"(*pl), "m"(*pr));
    return v;
}

#endif /* BV_ARCH_MIPS64 && BV_HAVE_INLINE_ASM */

#endif /* BVUTIL_MIPS_INTREADWRITE_H */
