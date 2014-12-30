/*
 * Lagged Fibonacci PRNG
 * Copyright (c) 2008 Michael Niedermayer
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

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include "lfg.h"
#include "md5.h"
#include "intreadwrite.h"
#include "attributes.h"

bv_cold void bv_lfg_init(BVLFG *c, unsigned int seed)
{
    uint8_t tmp[16] = { 0 };
    int i;

    for (i = 8; i < 64; i += 4) {
        BV_WL32(tmp, seed);
        tmp[4] = i;
        bv_md5_sum(tmp, tmp, 16);
        c->state[i    ] = BV_RL32(tmp);
        c->state[i + 1] = BV_RL32(tmp + 4);
        c->state[i + 2] = BV_RL32(tmp + 8);
        c->state[i + 3] = BV_RL32(tmp + 12);
    }
    c->index = 0;
}

void bv_bmg_get(BVLFG *lfg, double out[2])
{
    double x1, x2, w;

    do {
        x1 = 2.0 / UINT_MAX * bv_lfg_get(lfg) - 1.0;
        x2 = 2.0 / UINT_MAX * bv_lfg_get(lfg) - 1.0;
        w  = x1 * x1 + x2 * x2;
    } while (w >= 1.0);

    w = sqrt((-2.0 * log(w)) / w);
    out[0] = x1 * w;
    out[1] = x2 * w;
}

#ifdef TEST
#include "log.h"
#include "timer.h"

int main(void)
{
    int x = 0;
    int i, j;
    BVLFG state;

    bv_lfg_init(&state, 0xdeadbeef);
    for (j = 0; j < 10000; j++) {
        START_TIMER
        for (i = 0; i < 624; i++) {
            //bv_log(NULL, BV_LOG_ERROR, "%X\n", bv_lfg_get(&state));
            x += bv_lfg_get(&state);
        }
        STOP_TIMER("624 calls of bv_lfg_get");
    }
    bv_log(NULL, BV_LOG_ERROR, "final value:%X\n", x);

    /* BMG usage example */
    {
        double mean   = 1000;
        double stddev = 53;

        bv_lfg_init(&state, 42);

        for (i = 0; i < 1000; i += 2) {
            double bmg_out[2];
            bv_bmg_get(&state, bmg_out);
            bv_log(NULL, BV_LOG_INFO,
                   "%f\n%f\n",
                   bmg_out[0] * stddev + mean,
                   bmg_out[1] * stddev + mean);
        }
    }

    return 0;
}
#endif
