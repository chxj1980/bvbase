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

#ifndef BVUTIL_AVR32_INTREADWRITE_H
#define BVUTIL_AVR32_INTREADWRITE_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/bswap.h"

/*
 * AVR32 does not support unaligned memory accesses, except for the AP
 * series which suppports unaligned 32-bit loads and stores.  16-bit
 * and 64-bit accesses must be aligned to 16 and 32 bits, respectively.
 * This means we cannot use the byte-swapping load/store instructions
 * here.
 *
 * For 16-bit, 24-bit, and (on UC series) 32-bit loads, we instead use
 * the LDINS.B instruction, which gcc fails to utilise with the
 * generic code.  GCC also fails to use plain LD.W and ST.W even for
 * AP processors, so we override the generic code.  The 64-bit
 * versions are improved by using our optimised 32-bit functions.
 */

#define BV_RL16 BV_RL16
static bv_always_inline uint16_t BV_RL16(const void *p)
{
    uint16_t v;
    __asm__ ("ld.ub    %0,   %1  \n\t"
             "ldins.b  %0:l, %2  \n\t"
             : "=&r"(v)
             : "m"(*(const uint8_t*)p), "RKs12"(*((const uint8_t*)p+1)));
    return v;
}

#define BV_RB16 BV_RB16
static bv_always_inline uint16_t BV_RB16(const void *p)
{
    uint16_t v;
    __asm__ ("ld.ub    %0,   %2  \n\t"
             "ldins.b  %0:l, %1  \n\t"
             : "=&r"(v)
             : "RKs12"(*(const uint8_t*)p), "m"(*((const uint8_t*)p+1)));
    return v;
}

#define BV_RB24 BV_RB24
static bv_always_inline uint32_t BV_RB24(const void *p)
{
    uint32_t v;
    __asm__ ("ld.ub    %0,   %3  \n\t"
             "ldins.b  %0:l, %2  \n\t"
             "ldins.b  %0:u, %1  \n\t"
             : "=&r"(v)
             : "RKs12"(* (const uint8_t*)p),
               "RKs12"(*((const uint8_t*)p+1)),
               "m"    (*((const uint8_t*)p+2)));
    return v;
}

#define BV_RL24 BV_RL24
static bv_always_inline uint32_t BV_RL24(const void *p)
{
    uint32_t v;
    __asm__ ("ld.ub    %0,   %1  \n\t"
             "ldins.b  %0:l, %2  \n\t"
             "ldins.b  %0:u, %3  \n\t"
             : "=&r"(v)
             : "m"    (* (const uint8_t*)p),
               "RKs12"(*((const uint8_t*)p+1)),
               "RKs12"(*((const uint8_t*)p+2)));
    return v;
}

#if BV_ARCH_AVR32_AP

#define BV_RB32 BV_RB32
static bv_always_inline uint32_t BV_RB32(const void *p)
{
    uint32_t v;
    __asm__ ("ld.w %0, %1" : "=r"(v) : "m"(*(const uint32_t*)p));
    return v;
}

#define BV_WB32 BV_WB32
static bv_always_inline void BV_WB32(void *p, uint32_t v)
{
    __asm__ ("st.w %0, %1" : "=m"(*(uint32_t*)p) : "r"(v));
}

/* These two would be defined by generic code, but we need them sooner. */
#define BV_RL32(p)    bv_bswap32(BV_RB32(p))
#define BV_WL32(p, v) BV_WB32(p, bv_bswap32(v))

#define BV_WB64 BV_WB64
static bv_always_inline void BV_WB64(void *p, uint64_t v)
{
    union { uint64_t v; uint32_t hl[2]; } vv = { v };
    BV_WB32(p, vv.hl[0]);
    BV_WB32((uint32_t*)p+1, vv.hl[1]);
}

#define BV_WL64 BV_WL64
static bv_always_inline void BV_WL64(void *p, uint64_t v)
{
    union { uint64_t v; uint32_t hl[2]; } vv = { v };
    BV_WL32(p, vv.hl[1]);
    BV_WL32((uint32_t*)p+1, vv.hl[0]);
}

#else /* BV_ARCH_AVR32_AP */

#define BV_RB32 BV_RB32
static bv_always_inline uint32_t BV_RB32(const void *p)
{
    uint32_t v;
    __asm__ ("ld.ub    %0,   %4  \n\t"
             "ldins.b  %0:l, %3  \n\t"
             "ldins.b  %0:u, %2  \n\t"
             "ldins.b  %0:t, %1  \n\t"
             : "=&r"(v)
             : "RKs12"(* (const uint8_t*)p),
               "RKs12"(*((const uint8_t*)p+1)),
               "RKs12"(*((const uint8_t*)p+2)),
               "m"    (*((const uint8_t*)p+3)));
    return v;
}

#define BV_RL32 BV_RL32
static bv_always_inline uint32_t BV_RL32(const void *p)
{
    uint32_t v;
    __asm__ ("ld.ub    %0,   %1  \n\t"
             "ldins.b  %0:l, %2  \n\t"
             "ldins.b  %0:u, %3  \n\t"
             "ldins.b  %0:t, %4  \n\t"
             : "=&r"(v)
             : "m"    (* (const uint8_t*)p),
               "RKs12"(*((const uint8_t*)p+1)),
               "RKs12"(*((const uint8_t*)p+2)),
               "RKs12"(*((const uint8_t*)p+3)));
    return v;
}

#endif /* BV_ARCH_AVR32_AP */

#define BV_RB64 BV_RB64
static bv_always_inline uint64_t BV_RB64(const void *p)
{
    union { uint64_t v; uint32_t hl[2]; } v;
    v.hl[0] = BV_RB32(p);
    v.hl[1] = BV_RB32((const uint32_t*)p+1);
    return v.v;
}

#define BV_RL64 BV_RL64
static bv_always_inline uint64_t BV_RL64(const void *p)
{
    union { uint64_t v; uint32_t hl[2]; } v;
    v.hl[1] = BV_RL32(p);
    v.hl[0] = BV_RL32((const uint32_t*)p+1);
    return v.v;
}

#endif /* BVUTIL_AVR32_INTREADWRITE_H */
