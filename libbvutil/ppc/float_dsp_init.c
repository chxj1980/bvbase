/*
 * Copyright (c) 2006 Luca Barbato <lu_zero@gentoo.org>
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

#include "config.h"
#include "libbvutil/attributes.h"
#include "libbvutil/cpu.h"
#include "libbvutil/float_dsp.h"
#include "libbvutil/ppc/cpu.h"
#include "float_dsp_altivec.h"

bv_cold void ff_float_dsp_init_ppc(BVFloatDSPContext *fdsp, int bit_exact)
{
    if (!PPC_ALTIVEC(bv_get_cpu_flags()))
        return;

    fdsp->vector_fmul = ff_vector_fmul_altivec;
    fdsp->vector_fmul_add = ff_vector_fmul_add_altivec;
    fdsp->vector_fmul_reverse = ff_vector_fmul_reverse_altivec;

    if (!bit_exact) {
        fdsp->vector_fmul_window = ff_vector_fmul_window_altivec;
    }
}
