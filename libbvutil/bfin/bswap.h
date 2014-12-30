/*
 * Copyright (C) 2007 Marc Hoffman
 *
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

/**
 * @file
 * byte swapping routines
 */

#ifndef BVUTIL_BFIN_BSWAP_H
#define BVUTIL_BFIN_BSWAP_H

#include <stdint.h>
#include "config.h"
#include "libbvutil/attributes.h"

#define bv_bswap32 bv_bswap32
static bv_always_inline bv_const uint32_t bv_bswap32(uint32_t x)
{
    unsigned tmp;
    __asm__("%1 = %0 >> 8 (V);      \n\t"
            "%0 = %0 << 8 (V);      \n\t"
            "%0 = %0 | %1;          \n\t"
            "%0 = PACK(%0.L, %0.H); \n\t"
            : "+d"(x), "=&d"(tmp));
    return x;
}

#endif /* BVUTIL_BFIN_BSWAP_H */
