/*
 * Copyright (c) 2008 Siarhei Siamashka <ssvb@users.sourceforge.net>
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

#include "libbvutil/attributes.h"
#include "libbvutil/float_dsp.h"
#include "cpu.h"
#include "float_dsp_arm.h"

void ff_vector_fmul_vfp(float *dst, const float *src0, const float *src1,
                        int len);

void ff_vector_fmul_window_vfp(float *dst, const float *src0,
                               const float *src1, const float *win, int len);

void ff_vector_fmul_reverse_vfp(float *dst, const float *src0,
                                const float *src1, int len);

void ff_butterflies_float_vfp(float *bv_restrict v1, float *bv_restrict v2, int len);

bv_cold void ff_float_dsp_init_vfp(BVFloatDSPContext *fdsp, int cpu_flags)
{
    if (!have_vfpv3(cpu_flags)) {
        fdsp->vector_fmul = ff_vector_fmul_vfp;
        fdsp->vector_fmul_window = ff_vector_fmul_window_vfp;
    }
    fdsp->vector_fmul_reverse = ff_vector_fmul_reverse_vfp;
    if (!have_vfpv3(cpu_flags))
        fdsp->butterflies_float = ff_butterflies_float_vfp;
}
