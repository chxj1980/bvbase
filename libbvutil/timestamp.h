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
 * timestamp utils, mostly useful for debugging/logging purposes
 */

#ifndef BVUTIL_TIMESTAMP_H
#define BVUTIL_TIMESTAMP_H

#include "common.h"

#if defined(__cplusplus) && !defined(__STDC_FORMAT_MACROS) && !defined(PRId64)
#error missing -D__STDC_FORMAT_MACROS / #define __STDC_FORMAT_MACROS
#endif

#define BV_TS_MAX_STRING_SIZE 32

/**
 * Fill the provided buffer with a string containing a timestamp
 * representation.
 *
 * @param buf a buffer with size in bytes of at least BV_TS_MAX_STRING_SIZE
 * @param ts the timestamp to represent
 * @return the buffer in input
 */
static inline char *bv_ts_make_string(char *buf, int64_t ts)
{
    if (ts == BV_NOPTS_VALUE) snprintf(buf, BV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(buf, BV_TS_MAX_STRING_SIZE, "%"PRId64, ts);
    return buf;
}

/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
#define bv_ts2str(ts) bv_ts_make_string((char[BV_TS_MAX_STRING_SIZE]){0}, ts)

/**
 * Fill the provided buffer with a string containing a timestamp time
 * representation.
 *
 * @param buf a buffer with size in bytes of at least BV_TS_MAX_STRING_SIZE
 * @param ts the timestamp to represent
 * @param tb the timebase of the timestamp
 * @return the buffer in input
 */
static inline char *bv_ts_make_time_string(char *buf, int64_t ts, BVRational *tb)
{
    if (ts == BV_NOPTS_VALUE) snprintf(buf, BV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(buf, BV_TS_MAX_STRING_SIZE, "%.6g", bv_q2d(*tb) * ts);
    return buf;
}

/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
#define bv_ts2timestr(ts, tb) bv_ts_make_time_string((char[BV_TS_MAX_STRING_SIZE]){0}, ts, tb)

#endif /* BVUTIL_TIMESTAMP_H */
