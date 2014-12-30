/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * byte swapping routines
 */

#ifndef BVUTIL_BSWAP_H
#define BVUTIL_BSWAP_H

#include <stdint.h>
#include "libbvutil/bvconfig.h"
#include "attributes.h"

#ifdef HAVE_BV_CONFIG_H

#include "config.h"

#if   ARCH_AARCH64
#   include "aarch64/bswap.h"
#elif ARCH_ARM
#   include "arm/bswap.h"
#elif ARCH_AVR32
#   include "avr32/bswap.h"
#elif ARCH_SH4
#   include "sh4/bswap.h"
#elif ARCH_X86
#   include "x86/bswap.h"
#endif

#endif /* HAVE_BV_CONFIG_H */

#define BV_BSWAP16C(x) (((x) << 8 & 0xff00)  | ((x) >> 8 & 0x00ff))
#define BV_BSWAP32C(x) (BV_BSWAP16C(x) << 16 | BV_BSWAP16C((x) >> 16))
#define BV_BSWAP64C(x) (BV_BSWAP32C(x) << 32 | BV_BSWAP32C((x) >> 32))

#define BV_BSWAPC(s, x) BV_BSWAP##s##C(x)

#ifndef bv_bswap16
static bv_always_inline bv_const uint16_t bv_bswap16(uint16_t x)
{
    x= (x>>8) | (x<<8);
    return x;
}
#endif

#ifndef bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
    return BV_BSWAP32C(x);
}
#endif

#ifndef bv_bswap64
static inline uint64_t bv_const bv_bswap64(uint64_t x)
{
    return (uint64_t)bv_bswap32(x) << 32 | bv_bswap32(x >> 32);
}
#endif

// be2ne ... big-endian to native-endian
// le2ne ... little-endian to native-endian

#if BV_HAVE_BIGENDIAN
#define bv_be2ne16(x) (x)
#define bv_be2ne32(x) (x)
#define bv_be2ne64(x) (x)
#define bv_le2ne16(x) bv_bswap16(x)
#define bv_le2ne32(x) bv_bswap32(x)
#define bv_le2ne64(x) bv_bswap64(x)
#define BV_BE2NEC(s, x) (x)
#define BV_LE2NEC(s, x) BV_BSWAPC(s, x)
#else
#define bv_be2ne16(x) bv_bswap16(x)
#define bv_be2ne32(x) bv_bswap32(x)
#define bv_be2ne64(x) bv_bswap64(x)
#define bv_le2ne16(x) (x)
#define bv_le2ne32(x) (x)
#define bv_le2ne64(x) (x)
#define BV_BE2NEC(s, x) BV_BSWAPC(s, x)
#define BV_LE2NEC(s, x) (x)
#endif

#define BV_BE2NE16C(x) BV_BE2NEC(16, x)
#define BV_BE2NE32C(x) BV_BE2NEC(32, x)
#define BV_BE2NE64C(x) BV_BE2NEC(64, x)
#define BV_LE2NE16C(x) BV_LE2NEC(16, x)
#define BV_LE2NE32C(x) BV_LE2NEC(32, x)
#define BV_LE2NE64C(x) BV_LE2NEC(64, x)

#endif /* BVUTIL_BSWAP_H */
