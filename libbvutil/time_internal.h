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

#ifndef BVUTIL_TIME_INTERNAL_H
#define BVUTIL_TIME_INTERNAL_H

#include <time.h>
#include "config.h"

#if !BV_HAVE_GMTIME_R && !defined(gmtime_r)
static inline struct tm *gmtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = gmtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#endif

#if !BV_HAVE_LOCALTIME_R && !defined(localtime_r)
static inline struct tm *localtime_r(const time_t* clock, struct tm *result)
{
    struct tm *ptr = localtime(clock);
    if (!ptr)
        return NULL;
    *result = *ptr;
    return result;
}
#endif

#endif /* BVUTIL_TIME_INTERNAL_H */
