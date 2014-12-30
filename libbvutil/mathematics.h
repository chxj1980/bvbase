/*
 * copyright (c) 2005-2012 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef BVUTIL_MATHEMATICS_H
#define BVUTIL_MATHEMATICS_H

#include <stdint.h>
#include <math.h>
#include "attributes.h"
#include "rational.h"
#include "intfloat.h"

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_LOG2_10
#define M_LOG2_10      3.32192809488736234787  /* log_2 10 */
#endif
#ifndef M_PHI
#define M_PHI          1.61803398874989484820   /* phi / golden ratio */
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif
#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif
#ifndef NAN
#define NAN            bv_int2float(0x7fc00000)
#endif
#ifndef INFINITY
#define INFINITY       bv_int2float(0x7f800000)
#endif

/**
 * @addtogroup lavu_math
 * @{
 */


enum BVRounding {
    BV_ROUND_ZERO     = 0, ///< Round toward zero.
    BV_ROUND_INF      = 1, ///< Round away from zero.
    BV_ROUND_DOWN     = 2, ///< Round toward -infinity.
    BV_ROUND_UP       = 3, ///< Round toward +infinity.
    BV_ROUND_NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
    BV_ROUND_PASS_MINMAX = 8192, ///< Flag to pass INT64_MIN/MAX through instead of rescaling, this avoids special cases for BV_NOPTS_VALUE
};

/**
 * Return the greatest common divisor of a and b.
 * If both a and b are 0 or either or both are <0 then behavior is
 * undefined.
 */
int64_t bv_const bv_gcd(int64_t a, int64_t b);

/**
 * Rescale a 64-bit integer with rounding to nearest.
 * A simple a*b/c isn't possible as it can overflow.
 */
int64_t bv_rescale(int64_t a, int64_t b, int64_t c) bv_const;

/**
 * Rescale a 64-bit integer with specified rounding.
 * A simple a*b/c isn't possible as it can overflow.
 *
 * @return rescaled value a, or if BV_ROUND_PASS_MINMAX is set and a is
 *         INT64_MIN or INT64_MAX then a is passed through unchanged.
 */
int64_t bv_rescale_rnd(int64_t a, int64_t b, int64_t c, enum BVRounding) bv_const;

/**
 * Rescale a 64-bit integer by 2 rational numbers.
 */
int64_t bv_rescale_q(int64_t a, BVRational bq, BVRational cq) bv_const;

/**
 * Rescale a 64-bit integer by 2 rational numbers with specified rounding.
 *
 * @return rescaled value a, or if BV_ROUND_PASS_MINMAX is set and a is
 *         INT64_MIN or INT64_MAX then a is passed through unchanged.
 */
int64_t bv_rescale_q_rnd(int64_t a, BVRational bq, BVRational cq,
                         enum BVRounding) bv_const;

/**
 * Compare 2 timestamps each in its own timebases.
 * The result of the function is undefined if one of the timestamps
 * is outside the int64_t range when represented in the others timebase.
 * @return -1 if ts_a is before ts_b, 1 if ts_a is after ts_b or 0 if they represent the same position
 */
int bv_compare_ts(int64_t ts_a, BVRational tb_a, int64_t ts_b, BVRational tb_b);

/**
 * Compare 2 integers modulo mod.
 * That is we compare integers a and b for which only the least
 * significant log2(mod) bits are known.
 *
 * @param mod must be a power of 2
 * @return a negative value if a is smaller than b
 *         a positive value if a is greater than b
 *         0                if a equals          b
 */
int64_t bv_compare_mod(uint64_t a, uint64_t b, uint64_t mod);

/**
 * Rescale a timestamp while preserving known durations.
 *
 * @param in_ts Input timestamp
 * @param in_tb Input timebase
 * @param fs_tb Duration and *last timebase
 * @param duration duration till the next call
 * @param out_tb Output timebase
 */
int64_t bv_rescale_delta(BVRational in_tb, int64_t in_ts,  BVRational fs_tb, int duration, int64_t *last, BVRational out_tb);

/**
 * Add a value to a timestamp.
 *
 * This function guarantees that when the same value is repeatly added that
 * no accumulation of rounding errors occurs.
 *
 * @param ts Input timestamp
 * @param ts_tb Input timestamp timebase
 * @param inc value to add to ts
 * @param inc_tb inc timebase
 */
int64_t bv_add_stable(BVRational ts_tb, int64_t ts, BVRational inc_tb, int64_t inc);


    /**
 * @}
 */

#endif /* BVUTIL_MATHEMATICS_H */
