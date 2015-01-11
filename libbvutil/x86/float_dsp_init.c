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

#include "libbvutil/attributes.h"
#include "libbvutil/cpu.h"
#include "libbvutil/float_dsp.h"
#include "cpu.h"
#include "asm.h"

void bb_vector_fmul_sse(float *dst, const float *src0, const float *src1,
                        int len);
void bb_vector_fmul_avx(float *dst, const float *src0, const float *src1,
                        int len);

void bb_vector_fmac_scalar_sse(float *dst, const float *src, float mul,
                               int len);
void bb_vector_fmac_scalar_avx(float *dst, const float *src, float mul,
                               int len);
void bb_vector_fmac_scalar_fma3(float *dst, const float *src, float mul,
                                int len);

void bb_vector_fmul_scalar_sse(float *dst, const float *src, float mul,
                               int len);

void bb_vector_dmul_scalar_sse2(double *dst, const double *src,
                                double mul, int len);
void bb_vector_dmul_scalar_avx(double *dst, const double *src,
                               double mul, int len);

void bb_vector_fmul_window_3dnowext(float *dst, const float *src0,
                                    const float *src1, const float *win, int len);
void bb_vector_fmul_window_sse(float *dst, const float *src0,
                               const float *src1, const float *win, int len);

void bb_vector_fmul_add_sse(float *dst, const float *src0, const float *src1,
                            const float *src2, int len);
void bb_vector_fmul_add_avx(float *dst, const float *src0, const float *src1,
                            const float *src2, int len);
void bb_vector_fmul_add_fma3(float *dst, const float *src0, const float *src1,
                             const float *src2, int len);

void bb_vector_fmul_reverse_sse(float *dst, const float *src0,
                                const float *src1, int len);
void bb_vector_fmul_reverse_avx(float *dst, const float *src0,
                                const float *src1, int len);

float bb_scalarproduct_float_sse(const float *v1, const float *v2, int order);

void bb_butterflies_float_sse(float *src0, float *src1, int len);

bv_cold void bb_float_dsp_init_x86(BVFloatDSPContext *fdsp)
{
    int cpu_flags = bv_get_cpu_flags();

    if (EXTERNAL_AMD3DNOWEXT(cpu_flags)) {
        fdsp->vector_fmul_window = bb_vector_fmul_window_3dnowext;
    }
    if (EXTERNAL_SSE(cpu_flags)) {
        fdsp->vector_fmul = bb_vector_fmul_sse;
        fdsp->vector_fmac_scalar = bb_vector_fmac_scalar_sse;
        fdsp->vector_fmul_scalar = bb_vector_fmul_scalar_sse;
        fdsp->vector_fmul_window = bb_vector_fmul_window_sse;
        fdsp->vector_fmul_add    = bb_vector_fmul_add_sse;
        fdsp->vector_fmul_reverse = bb_vector_fmul_reverse_sse;
        fdsp->scalarproduct_float = bb_scalarproduct_float_sse;
        fdsp->butterflies_float   = bb_butterflies_float_sse;
    }
    if (EXTERNAL_SSE2(cpu_flags)) {
        fdsp->vector_dmul_scalar = bb_vector_dmul_scalar_sse2;
    }
    if (EXTERNAL_AVX(cpu_flags)) {
        fdsp->vector_fmul = bb_vector_fmul_avx;
        fdsp->vector_fmac_scalar = bb_vector_fmac_scalar_avx;
        fdsp->vector_dmul_scalar = bb_vector_dmul_scalar_avx;
        fdsp->vector_fmul_add    = bb_vector_fmul_add_avx;
        fdsp->vector_fmul_reverse = bb_vector_fmul_reverse_avx;
    }
    if (EXTERNAL_FMA3(cpu_flags)) {
        fdsp->vector_fmac_scalar = bb_vector_fmac_scalar_fma3;
        fdsp->vector_fmul_add    = bb_vector_fmul_add_fma3;
    }
}
