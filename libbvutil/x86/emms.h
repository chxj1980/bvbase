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

#ifndef BVUTIL_X86_EMMS_H
#define BVUTIL_X86_EMMS_H

#include "config.h"
#include "libbvutil/attributes.h"
#include "libbvutil/cpu.h"

void bvpriv_emms_yasm(void);

#if BV_HAVE_MMX_INLINE
#   define emms_c emms_c
/**
 * Empty mmx state.
 * this must be called between any dsp function and float/double code.
 * for example sin(); dsp->idct_put(); emms_c(); cos()
 */
static bv_always_inline void emms_c(void)
{
    if(bv_get_cpu_flags() & BV_CPU_FLAG_MMX)
        __asm__ volatile ("emms" ::: "memory");
}
#elif BV_HAVE_MMX && BV_HAVE_MM_EMPTY
#   include <mmintrin.h>
#   define emms_c _mm_empty
#elif BV_HAVE_MMX_EXTERNAL
#   define emms_c bvpriv_emms_yasm
#endif /* BV_HAVE_MMX_INLINE */

#endif /* BVUTIL_X86_EMMS_H */
