/*
 * Copyright (c) 2010 Mans Rullgard <mans@mansr.com>
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

#ifndef BVUTIL_INTMATH_H
#define BVUTIL_INTMATH_H

#include <stdint.h>

#include "config.h"
#include "attributes.h"

#if BV_ARCH_ARM
#   include "arm/intmath.h"
#endif

/**
 * @addtogroup lavu_internal
 * @{
 */

#if BV_HAVE_FAST_CLZ
#if BV_GCC_VERSION_AT_LEAST(3,4)
#ifndef bb_log2
#   define bb_log2(x) (31 - __builtin_clz((x)|1))
#   ifndef bb_log2_16bit
#      define bb_log2_16bit bv_log2
#   endif
#endif /* bb_log2 */
#elif defined( __INTEL_COMPILER )
#ifndef bb_log2
#   define bb_log2(x) (_bit_scan_reverse(x|1))
#   ifndef bb_log2_16bit
#      define bb_log2_16bit bv_log2
#   endif
#endif /* bb_log2 */
#endif
#endif /* BV_GCC_VERSION_AT_LEAST(3,4) */

extern const uint8_t bb_log2_tab[256];

#ifndef bb_log2
#define bb_log2 bb_log2_c
#if !defined( _MSC_VER )
static bv_always_inline bv_const int bb_log2_c(unsigned int v)
{
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += bb_log2_tab[v];

    return n;
}
#else
static bv_always_inline bv_const int bb_log2_c(unsigned int v)
{
    unsigned long n;
    _BitScanReverse(&n, v|1);
    return n;
}
#define bb_log2_16bit bv_log2
#endif
#endif

#ifndef bb_log2_16bit
#define bb_log2_16bit bb_log2_16bit_c
static bv_always_inline bv_const int bb_log2_16bit_c(unsigned int v)
{
    int n = 0;
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += bb_log2_tab[v];

    return n;
}
#endif

#define bv_log2       bb_log2
#define bv_log2_16bit bb_log2_16bit

/**
 * @}
 */

/**
 * @addtogroup lavu_math
 * @{
 */

#if BV_HAVE_FAST_CLZ
#if BV_GCC_VERSION_AT_LEAST(3,4)
#ifndef bb_ctz
#define bb_ctz(v) __builtin_ctz(v)
#endif
#elif defined( __INTEL_COMPILER )
#ifndef bb_ctz
#define bb_ctz(v) _bit_scan_forward(v)
#endif
#endif
#endif

#ifndef bb_ctz
#define bb_ctz bb_ctz_c
#if !defined( _MSC_VER )
static bv_always_inline bv_const int bb_ctz_c(int v)
{
    int c;

    if (v & 0x1)
        return 0;

    c = 1;
    if (!(v & 0xffff)) {
        v >>= 16;
        c += 16;
    }
    if (!(v & 0xff)) {
        v >>= 8;
        c += 8;
    }
    if (!(v & 0xf)) {
        v >>= 4;
        c += 4;
    }
    if (!(v & 0x3)) {
        v >>= 2;
        c += 2;
    }
    c -= v & 0x1;

    return c;
}
#else
static bv_always_inline bv_const int bb_ctz_c( int v )
{
    unsigned long c;
    _BitScanForward(&c, v);
    return c;
}
#endif
#endif

/**
 * Trailing zero bit count.
 *
 * @param v  input value. If v is 0, the result is undefined.
 * @return   the number of trailing 0-bits
 */
int bv_ctz(int v);

/**
 * @}
 */
#endif /* BVUTIL_INTMATH_H */
