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

#include "libbvutil/attributes.h"
#include "libbvutil/float_dsp.h"
#include "cpu.h"
#include "float_dsp_arm.h"

bv_cold void ff_float_dsp_init_arm(BVFloatDSPContext *fdsp)
{
    int cpu_flags = bv_get_cpu_flags();

    if (have_vfp(cpu_flags))
        ff_float_dsp_init_vfp(fdsp, cpu_flags);
    if (have_neon(cpu_flags))
        ff_float_dsp_init_neon(fdsp);
}
