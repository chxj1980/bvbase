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

/**
 * @file
 * Replacements for frequently missing libm functions
 */

#ifndef BVUTIL_LIBM_H
#define BVUTIL_LIBM_H

#include <math.h>
#include "config.h"
#include "attributes.h"
#include "intfloat.h"

#if BV_HAVE_MIPSFPU && BV_HAVE_INLINE_ASM
#include "libbvutil/mips/libm_mips.h"
#endif /* BV_HAVE_MIPSFPU && BV_HAVE_INLINE_ASM*/

#if !BV_HAVE_ATANF
#undef atanf
#define atanf(x) ((float)atan(x))
#endif

#if !BV_HAVE_ATAN2F
#undef atan2f
#define atan2f(y, x) ((float)atan2(y, x))
#endif

#if !BV_HAVE_POWF
#undef powf
#define powf(x, y) ((float)pow(x, y))
#endif

#if !BV_HAVE_CBRT
static bv_always_inline double cbrt(double x)
{
    return x < 0 ? -pow(-x, 1.0 / 3.0) : pow(x, 1.0 / 3.0);
}
#endif

#if !BV_HAVE_CBRTF
static bv_always_inline float cbrtf(float x)
{
    return x < 0 ? -powf(-x, 1.0 / 3.0) : powf(x, 1.0 / 3.0);
}
#endif

#if !BV_HAVE_COSF
#undef cosf
#define cosf(x) ((float)cos(x))
#endif

#if !BV_HAVE_EXPF
#undef expf
#define expf(x) ((float)exp(x))
#endif

#if !BV_HAVE_EXP2
#undef exp2
#define exp2(x) exp((x) * 0.693147180559945)
#endif /* BV_HAVE_EXP2 */

#if !BV_HAVE_EXP2F
#undef exp2f
#define exp2f(x) ((float)exp2(x))
#endif /* BV_HAVE_EXP2F */

#if !BV_HAVE_ISINF
static bv_always_inline bv_const int isinf(float x)
{
    uint32_t v = bv_float2int(x);
    if ((v & 0x7f800000) != 0x7f800000)
        return 0;
    return !(v & 0x007fffff);
}
#endif /* BV_HAVE_ISINF */

#if !BV_HAVE_ISNAN
static bv_always_inline bv_const int isnan(float x)
{
    uint32_t v = bv_float2int(x);
    if ((v & 0x7f800000) != 0x7f800000)
        return 0;
    return v & 0x007fffff;
}
#endif /* BV_HAVE_ISNAN */

#if !BV_HAVE_LDEXPF
#undef ldexpf
#define ldexpf(x, exp) ((float)ldexp(x, exp))
#endif

#if !BV_HAVE_LLRINT
#undef llrint
#define llrint(x) ((long long)rint(x))
#endif /* BV_HAVE_LLRINT */

#if !BV_HAVE_LLRINTF
#undef llrintf
#define llrintf(x) ((long long)rint(x))
#endif /* BV_HAVE_LLRINT */

#if !BV_HAVE_LOG2
#undef log2
#define log2(x) (log(x) * 1.44269504088896340736)
#endif /* BV_HAVE_LOG2 */

#if !BV_HAVE_LOG2F
#undef log2f
#define log2f(x) ((float)log2(x))
#endif /* BV_HAVE_LOG2F */

#if !BV_HAVE_LOG10F
#undef log10f
#define log10f(x) ((float)log10(x))
#endif

#if !BV_HAVE_SINF
#undef sinf
#define sinf(x) ((float)sin(x))
#endif

#if !BV_HAVE_RINT
static inline double rint(double x)
{
    return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* BV_HAVE_RINT */

#if !BV_HAVE_LRINT
static bv_always_inline bv_const long int lrint(double x)
{
    return rint(x);
}
#endif /* BV_HAVE_LRINT */

#if !BV_HAVE_LRINTF
static bv_always_inline bv_const long int lrintf(float x)
{
    return (int)(rint(x));
}
#endif /* BV_HAVE_LRINTF */

#if !BV_HAVE_ROUND
static bv_always_inline bv_const double round(double x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* BV_HAVE_ROUND */

#if !BV_HAVE_ROUNDF
static bv_always_inline bv_const float roundf(float x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* BV_HAVE_ROUNDF */

#if !BV_HAVE_TRUNC
static bv_always_inline bv_const double trunc(double x)
{
    return (x > 0) ? floor(x) : ceil(x);
}
#endif /* BV_HAVE_TRUNC */

#if !BV_HAVE_TRUNCF
static bv_always_inline bv_const float truncf(float x)
{
    return (x > 0) ? floor(x) : ceil(x);
}
#endif /* BV_HAVE_TRUNCF */

#endif /* BVUTIL_LIBM_H */
