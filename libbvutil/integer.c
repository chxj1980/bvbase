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

#include "common.h"
#include "integer.h"
#include "avassert.h"

BVInteger bv_add_i(BVInteger a, BVInteger b){
    int i, carry=0;

    for(i=0; i<BV_INTEGER_SIZE; i++){
        carry= (carry>>16) + a.v[i] + b.v[i];
        a.v[i]= carry;
    }
    return a;
}

BVInteger bv_sub_i(BVInteger a, BVInteger b){
    int i, carry=0;

    for(i=0; i<BV_INTEGER_SIZE; i++){
        carry= (carry>>16) + a.v[i] - b.v[i];
        a.v[i]= carry;
    }
    return a;
}

int bv_log2_i(BVInteger a){
    int i;

    for(i=BV_INTEGER_SIZE-1; i>=0; i--){
        if(a.v[i])
            return bv_log2_16bit(a.v[i]) + 16*i;
    }
    return -1;
}

BVInteger bv_mul_i(BVInteger a, BVInteger b){
    BVInteger out;
    int i, j;
    int na= (bv_log2_i(a)+16) >> 4;
    int nb= (bv_log2_i(b)+16) >> 4;

    memset(&out, 0, sizeof(out));

    for(i=0; i<na; i++){
        unsigned int carry=0;

        if(a.v[i])
            for(j=i; j<BV_INTEGER_SIZE && j-i<=nb; j++){
                carry= (carry>>16) + out.v[j] + a.v[i]*b.v[j-i];
                out.v[j]= carry;
            }
    }

    return out;
}

int bv_cmp_i(BVInteger a, BVInteger b){
    int i;
    int v= (int16_t)a.v[BV_INTEGER_SIZE-1] - (int16_t)b.v[BV_INTEGER_SIZE-1];
    if(v) return (v>>16)|1;

    for(i=BV_INTEGER_SIZE-2; i>=0; i--){
        int v= a.v[i] - b.v[i];
        if(v) return (v>>16)|1;
    }
    return 0;
}

BVInteger bv_shr_i(BVInteger a, int s){
    BVInteger out;
    int i;

    for(i=0; i<BV_INTEGER_SIZE; i++){
        unsigned int index= i + (s>>4);
        unsigned int v=0;
        if(index+1<BV_INTEGER_SIZE) v = a.v[index+1]<<16;
        if(index  <BV_INTEGER_SIZE) v+= a.v[index  ];
        out.v[i]= v >> (s&15);
    }
    return out;
}

BVInteger bv_mod_i(BVInteger *quot, BVInteger a, BVInteger b){
    int i= bv_log2_i(a) - bv_log2_i(b);
    BVInteger quot_temp;
    if(!quot) quot = &quot_temp;

    bv_assert2((int16_t)a.v[BV_INTEGER_SIZE-1] >= 0 && (int16_t)b.v[BV_INTEGER_SIZE-1] >= 0);
    bv_assert2(bv_log2_i(b)>=0);

    if(i > 0)
        b= bv_shr_i(b, -i);

    memset(quot, 0, sizeof(BVInteger));

    while(i-- >= 0){
        *quot= bv_shr_i(*quot, -1);
        if(bv_cmp_i(a, b) >= 0){
            a= bv_sub_i(a, b);
            quot->v[0] += 1;
        }
        b= bv_shr_i(b, 1);
    }
    return a;
}

BVInteger bv_div_i(BVInteger a, BVInteger b){
    BVInteger quot;
    bv_mod_i(&quot, a, b);
    return quot;
}

BVInteger bv_int2i(int64_t a){
    BVInteger out;
    int i;

    for(i=0; i<BV_INTEGER_SIZE; i++){
        out.v[i]= a;
        a>>=16;
    }
    return out;
}

int64_t bv_i2int(BVInteger a){
    int i;
    int64_t out=(int8_t)a.v[BV_INTEGER_SIZE-1];

    for(i= BV_INTEGER_SIZE-2; i>=0; i--){
        out = (out<<16) + a.v[i];
    }
    return out;
}

#ifdef TEST

const uint8_t ff_log2_tab[256]={
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

int main(void){
    int64_t a,b;

    for(a=7; a<256*256*256; a+=13215){
        for(b=3; b<256*256*256; b+=27118){
            BVInteger ai= bv_int2i(a);
            BVInteger bi= bv_int2i(b);

            bv_assert0(bv_i2int(ai) == a);
            bv_assert0(bv_i2int(bi) == b);
            bv_assert0(bv_i2int(bv_add_i(ai,bi)) == a+b);
            bv_assert0(bv_i2int(bv_sub_i(ai,bi)) == a-b);
            bv_assert0(bv_i2int(bv_mul_i(ai,bi)) == a*b);
            bv_assert0(bv_i2int(bv_shr_i(ai, 9)) == a>>9);
            bv_assert0(bv_i2int(bv_shr_i(ai,-9)) == a<<9);
            bv_assert0(bv_i2int(bv_shr_i(ai, 17)) == a>>17);
            bv_assert0(bv_i2int(bv_shr_i(ai,-17)) == a<<17);
            bv_assert0(bv_log2_i(ai) == bv_log2(a));
            bv_assert0(bv_i2int(bv_div_i(ai,bi)) == a/b);
        }
    }
    return 0;
}
#endif
