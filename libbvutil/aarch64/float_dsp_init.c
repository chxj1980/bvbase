/*
 * ARM NEON optimised Float DSP functions
 * Copyright (c) 2008 Mans Rullgard <mans@mansr.com>
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

#include <stdint.h>

#include "libbvutil/attributes.h"
#include "libbvutil/cpu.h"
#include "libbvutil/float_dsp.h"
#include "cpu.h"

void bb_vector_fmul_neon(float *dst, const float *src0, const float *src1,
                         int len);

void bb_vector_fmac_scalar_neon(float *dst, const float *src, float mul,
                                int len);

void bb_vector_fmul_scalar_neon(float *dst, const float *src, float mul,
                                int len);

void bb_vector_dmul_scalar_neon(double *dst, const double *src, double mul,
                                int len);

void bb_vector_fmul_window_neon(float *dst, const float *src0,
                                const float *src1, const float *win, int len);

void bb_vector_fmul_add_neon(float *dst, const float *src0, const float *src1,
                             const float *src2, int len);

void bb_vector_fmul_reverse_neon(float *dst, const float *src0,
                                 const float *src1, int len);

void bb_butterflies_float_neon(float *v1, float *v2, int len);

float bb_scalarproduct_float_neon(const float *v1, const float *v2, int len);

bv_cold void bb_float_dsp_init_aarch64(BVFloatDSPContext *fdsp)
{
    int cpu_flags = bv_get_cpu_flags();

    if (have_neon(cpu_flags)) {
        fdsp->butterflies_float   = bb_butterflies_float_neon;
        fdsp->scalarproduct_float = bb_scalarproduct_float_neon;
        fdsp->vector_dmul_scalar  = bb_vector_dmul_scalar_neon;
        fdsp->vector_fmul         = bb_vector_fmul_neon;
        fdsp->vector_fmac_scalar  = bb_vector_fmac_scalar_neon;
        fdsp->vector_fmul_add     = bb_vector_fmul_add_neon;
        fdsp->vector_fmul_reverse = bb_vector_fmul_reverse_neon;
        fdsp->vector_fmul_scalar  = bb_vector_fmul_scalar_neon;
        fdsp->vector_fmul_window  = bb_vector_fmul_window_neon;
    }
}
