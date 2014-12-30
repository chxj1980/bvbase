/*
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
 * @ingroup lavu
 * Utility Preprocessor macros
 */

#ifndef BVUTIL_MACROS_H
#define BVUTIL_MACROS_H

/**
 * @addtogroup preproc_misc Preprocessor String Macros
 *
 * String manipulation macros
 *
 * @{
 */

#define BV_STRINGIFY(s)         BV_TOSTRING(s)
#define BV_TOSTRING(s) #s

#define BV_GLUE(a, b) a ## b
#define BV_JOIN(a, b) BV_GLUE(a, b)

/**
 * @}
 */

#define BV_PRAGMA(s) _Pragma(#s)

#endif /* BVUTIL_MACROS_H */
