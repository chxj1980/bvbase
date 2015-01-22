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

#include "config.h"

#include "pixelutils.h"
#include "cpu.h"

int bb_pixelutils_sad_8x8_mmx(const uint8_t *src1, ptrdiff_t stride1,
                              const uint8_t *src2, ptrdiff_t stride2);
int bb_pixelutils_sad_8x8_mmxext(const uint8_t *src1, ptrdiff_t stride1,
                                 const uint8_t *src2, ptrdiff_t stride2);

int bb_pixelutils_sad_16x16_mmxext(const uint8_t *src1, ptrdiff_t stride1,
                                   const uint8_t *src2, ptrdiff_t stride2);
int bb_pixelutils_sad_16x16_sse2(const uint8_t *src1, ptrdiff_t stride1,
                                 const uint8_t *src2, ptrdiff_t stride2);
int bb_pixelutils_sad_a_16x16_sse2(const uint8_t *src1, ptrdiff_t stride1,
                                   const uint8_t *src2, ptrdiff_t stride2);
int bb_pixelutils_sad_u_16x16_sse2(const uint8_t *src1, ptrdiff_t stride1,
                                   const uint8_t *src2, ptrdiff_t stride2);

void bb_pixelutils_sad_init_x86(bv_pixelutils_sad_fn *sad, int aligned)
{
    int cpu_flags = bv_get_cpu_flags();

    if (EXTERNAL_MMX(cpu_flags)) {
        sad[2] = bb_pixelutils_sad_8x8_mmx;
    }

    if (EXTERNAL_MMXEXT(cpu_flags)) {
        sad[2] = bb_pixelutils_sad_8x8_mmxext;
        sad[3] = bb_pixelutils_sad_16x16_mmxext;
    }

    if (EXTERNAL_SSE2(cpu_flags)) {
        switch (aligned) {
        case 0: sad[3] = bb_pixelutils_sad_16x16_sse2;   break; // src1 unaligned, src2 unaligned
        case 1: sad[3] = bb_pixelutils_sad_u_16x16_sse2; break; // src1   aligned, src2 unaligned
        case 2: sad[3] = bb_pixelutils_sad_a_16x16_sse2; break; // src1   aligned, src2   aligned
        }
    }
}
