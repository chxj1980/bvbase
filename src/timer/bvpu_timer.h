/**
 *          File: bvpu_timer.h
 *
 *        Create: 2014年12月19日 星期五 16时52分29秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) @ BesoVideo, 2014
 */

#ifndef __BVPU_TIMER_H
#define __BVPU_TIMER_H

#include <time.h>

#include "pjlib.h"

//
// timer manager
// All programs get time from here,
// only this manager get/set time,
// for reduce time of get time with system call.

typedef struct {
    time_t               t;
    struct tm           *tm;
    struct timeval      *tv;
    struct timespec      ts;
    long                 msec;  // millisecond
    long                 usec;  // microsecond
    pj_time_val          time_val;   // pj style time
} BVPUTime;

const BVPUTime *bvpu_time_get();

/////////////////////////////////////////////////////////
// Time manager 
// contain ntp update
/////////////////////////////////////////////////////////

typedef struct {
    char                 szAddr[128][3];
    int                  iPort[3];
    int                  iUpdateInterval;
    char                 szBVAddr[128];   // back ntp server if fail of other ntp server
    int                  iBVPort;
} BVPUTimeNtpInfo;

typedef struct {
    BVPUTimeNtpInfo      stNtpInfo;
    // Time changed of ntp update will invoke this function.
    int                  (*bvpu_time_change_report)(BVPUTime *stTime);
} BVPUTimeParam;

int bvpu_time_init(BVPUTimeParam *stTimeParam);

int bvpu_time_deinit();

// 
// Change ntp info when running.
int bvpu_time_change_ntp_info(BVPUTimeNtpInfo *stNtpInfo);

// NTP update start and stop.
// For control free.
int bvpu_time_ntp_update(int bStart);

#endif // __BVPU_TIME_H

/*=============== End of file: bvpu_timer.h ==========================*/
