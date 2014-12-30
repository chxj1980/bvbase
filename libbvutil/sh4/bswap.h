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

#ifndef BVUTIL_SH4_BSWAP_H
#define BVUTIL_SH4_BSWAP_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/attributes.h"

#define bv_bswap16 bv_bswap16
static bv_always_inline bv_const uint16_t bv_bswap16(uint16_t x)
{
    __asm__("swap.b %0,%0" : "+r"(x));
    return x;
}

#define bv_bswap32 bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
    __asm__("swap.b %0,%0\n"
            "swap.w %0,%0\n"
            "swap.b %0,%0\n"
            : "+r"(x));
    return x;
}

#endif /* BVUTIL_SH4_BSWAP_H */
