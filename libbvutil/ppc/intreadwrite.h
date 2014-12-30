/*
 * Copyright (c) 2008 Mans Rullgard <mans@mansr.com>
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

#ifndef BVUTIL_PPC_INTREADWRITE_H
#define BVUTIL_PPC_INTREADWRITE_H

#include <stdint.h>
#include "config.h"

/*
 * -O0 would compile the packed struct version, which is used by
 * default, in an overly verbose fashion, so we override it here.
 */
#if HAVE_BIGENDIAN
#define BV_RB64(p) (*(const uint64_t *)(p))
#define BV_WB64(p, v) (*(uint64_t *)(p) = (v))

#else
#define BV_RL64(p) (*(const uint64_t *)(p))
#define BV_WL64(p, v) (*(uint64_t *)(p) = (v))

#endif

#if HAVE_XFORM_ASM

#if HAVE_BIGENDIAN
#define BV_RL16 bv_read_bswap16
#define BV_WL16 bv_write_bswap16
#define BV_RL32 bv_read_bswap32
#define BV_WL32 bv_write_bswap32
#define BV_RL64 bv_read_bswap64
#define BV_WL64 bv_write_bswap64

#else
#define BV_RB16 bv_read_bswap16
#define BV_WB16 bv_write_bswap16
#define BV_RB32 bv_read_bswap32
#define BV_WB32 bv_write_bswap32
#define BV_RB64 bv_read_bswap64
#define BV_WB64 bv_write_bswap64

#endif

static bv_always_inline uint16_t bv_read_bswap16(const void *p)
{
    uint16_t v;
    __asm__ ("lhbrx   %0, %y1" : "=r"(v) : "Z"(*(const uint16_t*)p));
    return v;
}

static bv_always_inline void bv_write_bswap16(void *p, uint16_t v)
{
    __asm__ ("sthbrx  %1, %y0" : "=Z"(*(uint16_t*)p) : "r"(v));
}

static bv_always_inline uint32_t bv_read_bswap32(const void *p)
{
    uint32_t v;
    __asm__ ("lwbrx   %0, %y1" : "=r"(v) : "Z"(*(const uint32_t*)p));
    return v;
}

static bv_always_inline void bv_write_bswap32(void *p, uint32_t v)
{
    __asm__ ("stwbrx  %1, %y0" : "=Z"(*(uint32_t*)p) : "r"(v));
}

#if HAVE_LDBRX

static bv_always_inline uint64_t bv_read_bswap64(const void *p)
{
    uint64_t v;
    __asm__ ("ldbrx   %0, %y1" : "=r"(v) : "Z"(*(const uint64_t*)p));
    return v;
}

static bv_always_inline void bv_write_bswap64(void *p, uint64_t v)
{
    __asm__ ("stdbrx  %1, %y0" : "=Z"(*(uint64_t*)p) : "r"(v));
}

#else

static bv_always_inline uint64_t bv_read_bswap64(const void *p)
{
    union { uint64_t v; uint32_t hl[2]; } v;
    __asm__ ("lwbrx   %0, %y2  \n\t"
             "lwbrx   %1, %y3  \n\t"
             : "=&r"(v.hl[1]), "=r"(v.hl[0])
             : "Z"(*(const uint32_t*)p), "Z"(*((const uint32_t*)p+1)));
    return v.v;
}

static bv_always_inline void bv_write_bswap64(void *p, uint64_t v)
{
    union { uint64_t v; uint32_t hl[2]; } vv = { v };
    __asm__ ("stwbrx  %2, %y0  \n\t"
             "stwbrx  %3, %y1  \n\t"
             : "=Z"(*(uint32_t*)p), "=Z"(*((uint32_t*)p+1))
             : "r"(vv.hl[1]), "r"(vv.hl[0]));
}

#endif /* HAVE_LDBRX */

#endif /* HAVE_XFORM_ASM */

#endif /* BVUTIL_PPC_INTREADWRITE_H */
