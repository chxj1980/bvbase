/*
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BVBase; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_DYNARRAY_H
#define BVUTIL_DYNARRAY_H

#include "log.h"
#include "mem.h"

/**
 * Add an element of to a dynamic array.
 *
 * The array is reallocated when its number of elements reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the size is incremented.
 *
 * @param bv_size_max  maximum size of the array, usually the MAX macro of
 *                     the type of the size
 * @param bv_elt_size  size of the elements in the array, in bytes
 * @param bv_array     pointer to the array, must be a lvalue
 * @param bv_size      size of the array, must be an integer lvalue
 * @param bv_success   statement to execute on success; at this point, the
 *                     size variable is not yet incremented
 * @param bv_failure   statement to execute on failure; if this happens, the
 *                     array and size are not changed; the statement can end
 *                     with a return or a goto
 */
#define BV_DYNARRAY_ADD(bv_size_max, bv_elt_size, bv_array, bv_size, \
                        bv_success, bv_failure) \
    do { \
        size_t bv_size_new = (bv_size); \
        if (!((bv_size) & ((bv_size) - 1))) { \
            bv_size_new = (bv_size) ? (bv_size) << 1 : 1; \
            if (bv_size_new > (bv_size_max) / (bv_elt_size)) { \
                bv_size_new = 0; \
            } else { \
                void *bv_array_new = \
                    bv_realloc((bv_array), bv_size_new * (bv_elt_size)); \
                if (!bv_array_new) \
                    bv_size_new = 0; \
                else \
                    (bv_array) = bv_array_new; \
            } \
        } \
        if (bv_size_new) { \
            { bv_success } \
            (bv_size)++; \
        } else { \
            bv_failure \
        } \
    } while (0)

#endif /* BVUTIL_DYNARRAY_H */
