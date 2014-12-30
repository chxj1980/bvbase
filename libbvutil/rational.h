/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
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
 * rational numbers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef BVUTIL_RATIONAL_H
#define BVUTIL_RATIONAL_H

#include <stdint.h>
#include <limits.h>
#include "attributes.h"

/**
 * @addtogroup lavu_math
 * @{
 */

/**
 * rational number numerator/denominator
 */
typedef struct BVRational{
    int num; ///< numerator
    int den; ///< denominator
} BVRational;

/**
 * Create a rational.
 * Useful for compilers that do not support compound literals.
 * @note  The return value is not reduced.
 */
static inline BVRational bv_make_q(int num, int den)
{
    BVRational r = { num, den };
    return r;
}

/**
 * Compare two rationals.
 * @param a first rational
 * @param b second rational
 * @return 0 if a==b, 1 if a>b, -1 if a<b, and INT_MIN if one of the
 * values is of the form 0/0
 */
static inline int bv_cmp_q(BVRational a, BVRational b){
    const int64_t tmp= a.num * (int64_t)b.den - b.num * (int64_t)a.den;

    if(tmp) return (int)((tmp ^ a.den ^ b.den)>>63)|1;
    else if(b.den && a.den) return 0;
    else if(a.num && b.num) return (a.num>>31) - (b.num>>31);
    else                    return INT_MIN;
}

/**
 * Convert rational to double.
 * @param a rational to convert
 * @return (double) a
 */
static inline double bv_q2d(BVRational a){
    return a.num / (double) a.den;
}

/**
 * Reduce a fraction.
 * This is useful for framerate calculations.
 * @param dst_num destination numerator
 * @param dst_den destination denominator
 * @param num source numerator
 * @param den source denominator
 * @param max the maximum allowed for dst_num & dst_den
 * @return 1 if exact, 0 otherwise
 */
int bv_reduce(int *dst_num, int *dst_den, int64_t num, int64_t den, int64_t max);

/**
 * Multiply two rationals.
 * @param b first rational
 * @param c second rational
 * @return b*c
 */
BVRational bv_mul_q(BVRational b, BVRational c) bv_const;

/**
 * Divide one rational by another.
 * @param b first rational
 * @param c second rational
 * @return b/c
 */
BVRational bv_div_q(BVRational b, BVRational c) bv_const;

/**
 * Add two rationals.
 * @param b first rational
 * @param c second rational
 * @return b+c
 */
BVRational bv_add_q(BVRational b, BVRational c) bv_const;

/**
 * Subtract one rational from another.
 * @param b first rational
 * @param c second rational
 * @return b-c
 */
BVRational bv_sub_q(BVRational b, BVRational c) bv_const;

/**
 * Invert a rational.
 * @param q value
 * @return 1 / q
 */
static bv_always_inline BVRational bv_inv_q(BVRational q)
{
    BVRational r = { q.den, q.num };
    return r;
}

/**
 * Convert a double precision floating point number to a rational.
 * inf is expressed as {1,0} or {-1,0} depending on the sign.
 *
 * @param d double to convert
 * @param max the maximum allowed numerator and denominator
 * @return (BVRational) d
 */
BVRational bv_d2q(double d, int max) bv_const;

/**
 * @return 1 if q1 is nearer to q than q2, -1 if q2 is nearer
 * than q1, 0 if they have the same distance.
 */
int bv_nearer_q(BVRational q, BVRational q1, BVRational q2);

/**
 * Find the nearest value in q_list to q.
 * @param q_list an array of rationals terminated by {0, 0}
 * @return the index of the nearest value found in the array
 */
int bv_find_nearest_q_idx(BVRational q, const BVRational* q_list);

/**
 * @}
 */

#endif /* BVUTIL_RATIONAL_H */
