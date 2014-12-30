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

#include "avassert.h"
#include <limits.h>

#include "common.h"
#include "mathematics.h"
#include "rational.h"

int bv_reduce(int *dst_num, int *dst_den,
              int64_t num, int64_t den, int64_t max)
{
    BVRational a0 = { 0, 1 }, a1 = { 1, 0 };
    int sign = (num < 0) ^ (den < 0);
    int64_t gcd = bv_gcd(FFABS(num), FFABS(den));

    if (gcd) {
        num = FFABS(num) / gcd;
        den = FFABS(den) / gcd;
    }
    if (num <= max && den <= max) {
        a1 = (BVRational) { num, den };
        den = 0;
    }

    while (den) {
        uint64_t x        = num / den;
        int64_t next_den  = num - den * x;
        int64_t a2n       = x * a1.num + a0.num;
        int64_t a2d       = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) x =          (max - a0.num) / a1.num;
            if (a1.den) x = FFMIN(x, (max - a0.den) / a1.den);

            if (den * (2 * x * a1.den + a0.den) > num * a1.den)
                a1 = (BVRational) { x * a1.num + a0.num, x * a1.den + a0.den };
            break;
        }

        a0  = a1;
        a1  = (BVRational) { a2n, a2d };
        num = den;
        den = next_den;
    }
    bv_assert2(bv_gcd(a1.num, a1.den) <= 1U);
    bv_assert2(a1.num <= max && a1.den <= max);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}

BVRational bv_mul_q(BVRational b, BVRational c)
{
    bv_reduce(&b.num, &b.den,
               b.num * (int64_t) c.num,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

BVRational bv_div_q(BVRational b, BVRational c)
{
    return bv_mul_q(b, (BVRational) { c.den, c.num });
}

BVRational bv_add_q(BVRational b, BVRational c) {
    bv_reduce(&b.num, &b.den,
               b.num * (int64_t) c.den +
               c.num * (int64_t) b.den,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

BVRational bv_sub_q(BVRational b, BVRational c)
{
    return bv_add_q(b, (BVRational) { -c.num, c.den });
}

BVRational bv_d2q(double d, int max)
{
    BVRational a;
#define LOG2  0.69314718055994530941723212145817656807550013436025
    int exponent;
    int64_t den;
    if (isnan(d))
        return (BVRational) { 0,0 };
    if (fabs(d) > INT_MAX + 3LL)
        return (BVRational) { d < 0 ? -1 : 1, 0 };
    exponent = FFMAX( (int)(log(fabs(d) + 1e-20)/LOG2), 0);
    den = 1LL << (61 - exponent);
    // (int64_t)rint() and llrint() do not work with gcc on ia64 and sparc64
    bv_reduce(&a.num, &a.den, floor(d * den + 0.5), den, max);
    if ((!a.num || !a.den) && d && max>0 && max<INT_MAX)
        bv_reduce(&a.num, &a.den, floor(d * den + 0.5), den, INT_MAX);

    return a;
}

int bv_nearer_q(BVRational q, BVRational q1, BVRational q2)
{
    /* n/d is q, a/b is the median between q1 and q2 */
    int64_t a = q1.num * (int64_t)q2.den + q2.num * (int64_t)q1.den;
    int64_t b = 2 * (int64_t)q1.den * q2.den;

    /* rnd_up(a*d/b) > n => a*d/b > n */
    int64_t x_up = bv_rescale_rnd(a, q.den, b, BV_ROUND_UP);

    /* rnd_down(a*d/b) < n => a*d/b < n */
    int64_t x_down = bv_rescale_rnd(a, q.den, b, BV_ROUND_DOWN);

    return ((x_up > q.num) - (x_down < q.num)) * bv_cmp_q(q2, q1);
}

int bv_find_nearest_q_idx(BVRational q, const BVRational* q_list)
{
    int i, nearest_q_idx = 0;
    for (i = 0; q_list[i].den; i++)
        if (bv_nearer_q(q, q_list[i], q_list[nearest_q_idx]) > 0)
            nearest_q_idx = i;

    return nearest_q_idx;
}

#ifdef TEST
int main(void)
{
    BVRational a,b,r;
    for (a.num = -2; a.num <= 2; a.num++) {
        for (a.den = -2; a.den <= 2; a.den++) {
            for (b.num = -2; b.num <= 2; b.num++) {
                for (b.den = -2; b.den <= 2; b.den++) {
                    int c = bv_cmp_q(a,b);
                    double d = bv_q2d(a) == bv_q2d(b) ?
                               0 : (bv_q2d(a) - bv_q2d(b));
                    if (d > 0)       d = 1;
                    else if (d < 0)  d = -1;
                    else if (d != d) d = INT_MIN;
                    if (c != d)
                        bv_log(NULL, BV_LOG_ERROR, "%d/%d %d/%d, %d %f\n", a.num,
                               a.den, b.num, b.den, c,d);
                    r = bv_sub_q(bv_add_q(b,a), b);
                    if(b.den && (r.num*a.den != a.num*r.den || !r.num != !a.num || !r.den != !a.den))
                        bv_log(NULL, BV_LOG_ERROR, "%d/%d ", r.num, r.den);
                }
            }
        }
    }

    for (a.num = 1; a.num <= 10; a.num++) {
        for (a.den = 1; a.den <= 10; a.den++) {
            if (bv_gcd(a.num, a.den) > 1)
                continue;
            for (b.num = 1; b.num <= 10; b.num++) {
                for (b.den = 1; b.den <= 10; b.den++) {
                    int start;
                    if (bv_gcd(b.num, b.den) > 1)
                        continue;
                    if (bv_cmp_q(b, a) < 0)
                        continue;
                    for (start = 0; start < 10 ; start++) {
                        int acc= start;
                        int i;

                        for (i = 0; i<100; i++) {
                            int exact = start + bv_rescale_q(i+1, b, a);
                            acc = bv_add_stable(a, acc, b, 1);
                            if (FFABS(acc - exact) > 2) {
                                bv_log(NULL, BV_LOG_ERROR, "%d/%d %d/%d, %d %d\n", a.num,
                                       a.den, b.num, b.den, acc, exact);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#endif
