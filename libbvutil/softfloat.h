/*
 * Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef BVUTIL_SOFTFLOAT_H
#define BVUTIL_SOFTFLOAT_H

#include <stdint.h>
#include "common.h"

#include "bvassert.h"

#define MIN_EXP -126
#define MAX_EXP  126
#define ONE_BITS 29

typedef struct SoftFloat{
    int32_t  exp;
    int32_t mant;
}SoftFloat;

static bv_const SoftFloat bv_normalize_sf(SoftFloat a){
    if(a.mant){
#if 1
        while((a.mant + 0x20000000U)<0x40000000U){
            a.mant += a.mant;
            a.exp  -= 1;
        }
#else
        int s=ONE_BITS + 1 - bv_log2(a.mant ^ (a.mant<<1));
        a.exp   -= s;
        a.mant <<= s;
#endif
        if(a.exp < MIN_EXP){
            a.exp = MIN_EXP;
            a.mant= 0;
        }
    }else{
        a.exp= MIN_EXP;
    }
    return a;
}

static inline bv_const SoftFloat bv_normalize1_sf(SoftFloat a){
#if 1
    if((int32_t)(a.mant + 0x40000000U) < 0){
        a.exp++;
        a.mant>>=1;
    }
    bv_assert2(a.mant < 0x40000000 && a.mant > -0x40000000);
    return a;
#elif 1
    int t= a.mant + 0x40000000 < 0;
    return (SoftFloat){a.exp+t, a.mant>>t};
#else
    int t= (a.mant + 0x40000000U)>>31;
    return (SoftFloat){a.exp+t, a.mant>>t};
#endif
}

/**
 * @return Will not be more denormalized than a+b. So if either input is
 *         normalized, then the output will not be worse then the other input.
 *         If both are normalized, then the output will be normalized.
 */
static inline bv_const SoftFloat bv_mul_sf(SoftFloat a, SoftFloat b){
    a.exp += b.exp;
    bv_assert2((int32_t)((a.mant * (int64_t)b.mant) >> ONE_BITS) == (a.mant * (int64_t)b.mant) >> ONE_BITS);
    a.mant = (a.mant * (int64_t)b.mant) >> ONE_BITS;
    return bv_normalize1_sf(a);
}

/**
 * b has to be normalized and not zero.
 * @return Will not be more denormalized than a.
 */
static bv_const SoftFloat bv_div_sf(SoftFloat a, SoftFloat b){
    a.exp -= b.exp+1;
    a.mant = ((int64_t)a.mant<<(ONE_BITS+1)) / b.mant;
    return bv_normalize1_sf(a);
}

static inline bv_const int bv_cmp_sf(SoftFloat a, SoftFloat b){
    int t= a.exp - b.exp;
    if(t<0) return (a.mant >> (-t)) -  b.mant      ;
    else    return  a.mant          - (b.mant >> t);
}

static inline bv_const SoftFloat bv_add_sf(SoftFloat a, SoftFloat b){
    int t= a.exp - b.exp;
    if      (t <-31) return b;
    else if (t <  0) return bv_normalize1_sf((SoftFloat){b.exp, b.mant + (a.mant >> (-t))});
    else if (t < 32) return bv_normalize1_sf((SoftFloat){a.exp, a.mant + (b.mant >>   t )});
    else             return a;
}

static inline bv_const SoftFloat bv_sub_sf(SoftFloat a, SoftFloat b){
    return bv_add_sf(a, (SoftFloat){b.exp, -b.mant});
}

//FIXME sqrt, log, exp, pow, sin, cos

static inline bv_const SoftFloat bv_int2sf(int v, int frac_bits){
    return bv_normalize_sf((SoftFloat){ONE_BITS-frac_bits, v});
}

/**
 * Rounding is to -inf.
 */
static inline bv_const int bv_sf2int(SoftFloat v, int frac_bits){
    v.exp += frac_bits - ONE_BITS;
    if(v.exp >= 0) return v.mant <<  v.exp ;
    else           return v.mant >>(-v.exp);
}

#endif /* BVUTIL_SOFTFLOAT_H */
