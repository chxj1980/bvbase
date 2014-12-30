/*
 * copyright (c) 2010 Michael Niedermayer <michaelni@gmx.at>
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
 * simple assert() macros that are a bit more flexible than ISO C assert().
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef BVUTIL_BVASSERT_H
#define BVUTIL_BVASSERT_H

#include <stdlib.h>
#include "bvutil.h"
#include "log.h"

/**
 * assert() equivalent, that is always enabled.
 */
#define bv_assert0(cond) do {                                           \
    if (!(cond)) {                                                      \
        bv_log(NULL, BV_LOG_PANIC, "Assertion %s failed at %s:%d\n",    \
               BV_STRINGIFY(cond), __FILE__, __LINE__);                 \
        abort();                                                        \
    }                                                                   \
} while (0)


/**
 * assert() equivalent, that does not lie in speed critical code.
 * These asserts() thus can be enabled without fearing speedloss.
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 0
#define bv_assert1(cond) bv_assert0(cond)
#else
#define bv_assert1(cond) ((void)0)
#endif


/**
 * assert() equivalent, that does lie in speed critical code.
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 1
#define bv_assert2(cond) bv_assert0(cond)
#else
#define bv_assert2(cond) ((void)0)
#endif

#endif /* BVUTIL_BVASSERT_H */
