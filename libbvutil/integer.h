/*
 * arbitrary precision integers
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
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
 * arbitrary precision integers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef BVUTIL_INTEGER_H
#define BVUTIL_INTEGER_H

#include <stdint.h>
#include "common.h"

#define BV_INTEGER_SIZE 8

typedef struct BVInteger{
    uint16_t v[BV_INTEGER_SIZE];
} BVInteger;

BVInteger bv_add_i(BVInteger a, BVInteger b) bv_const;
BVInteger bv_sub_i(BVInteger a, BVInteger b) bv_const;

/**
 * Return the rounded-down value of the base 2 logarithm of the given
 * BVInteger. This is simply the index of the most significant bit
 * which is 1, or 0 if all bits are 0.
 */
int bv_log2_i(BVInteger a) bv_const;
BVInteger bv_mul_i(BVInteger a, BVInteger b) bv_const;

/**
 * Return 0 if a==b, 1 if a>b and -1 if a<b.
 */
int bv_cmp_i(BVInteger a, BVInteger b) bv_const;

/**
 * bitwise shift
 * @param s the number of bits by which the value should be shifted right,
            may be negative for shifting left
 */
BVInteger bv_shr_i(BVInteger a, int s) bv_const;

/**
 * Return a % b.
 * @param quot a/b will be stored here.
 */
BVInteger bv_mod_i(BVInteger *quot, BVInteger a, BVInteger b);

/**
 * Return a/b.
 */
BVInteger bv_div_i(BVInteger a, BVInteger b) bv_const;

/**
 * Convert the given int64_t to an BVInteger.
 */
BVInteger bv_int2i(int64_t a) bv_const;

/**
 * Convert the given BVInteger to an int64_t.
 * If the BVInteger is too large to fit into an int64_t,
 * then only the least significant 64 bits will be used.
 */
int64_t bv_i2int(BVInteger a) bv_const;

#endif /* BVUTIL_INTEGER_H */
