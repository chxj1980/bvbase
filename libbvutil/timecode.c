/*
 * Copyright (c) 2006 Smartjog S.A.S, Baptiste Coudurier <baptiste.coudurier@gmail.com>
 * Copyright (c) 2011-2012 Smartjog S.A.S, Clément Bœsch <clement.boesch@smartjog.com>
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
 * Timecode helpers
 * @see https://en.wikipedia.org/wiki/SMPTE_time_code
 * @see http://www.dropframetimecode.org
 */

#include <stdio.h>
#include "timecode.h"
#include "log.h"
#include "error.h"

int bv_timecode_adjust_ntsc_framenum2(int framenum, int fps)
{
    /* only works for NTSC 29.97 and 59.94 */
    int drop_frames = 0;
    int d, m, frames_per_10mins;

    if (fps == 30) {
        drop_frames = 2;
        frames_per_10mins = 17982;
    } else if (fps == 60) {
        drop_frames = 4;
        frames_per_10mins = 35964;
    } else
        return framenum;

    d = framenum / frames_per_10mins;
    m = framenum % frames_per_10mins;

    return framenum + 9 * drop_frames * d + drop_frames * ((m - drop_frames) / (frames_per_10mins / 10));
}

uint32_t bv_timecode_get_smpte_from_framenum(const BVTimecode *tc, int framenum)
{
    unsigned fps = tc->fps;
    int drop = !!(tc->flags & BV_TIMECODE_FLAG_DROPFRAME);
    int hh, mm, ss, ff;

    framenum += tc->start;
    if (drop)
        framenum = bv_timecode_adjust_ntsc_framenum2(framenum, tc->fps);
    ff = framenum % fps;
    ss = framenum / fps      % 60;
    mm = framenum / (fps*60) % 60;
    hh = framenum / (fps*3600) % 24;
    return 0         << 31 | // color frame flag (0: unsync mode, 1: sync mode)
           drop      << 30 | // drop  frame flag (0: non drop,    1: drop)
           (ff / 10) << 28 | // tens  of frames
           (ff % 10) << 24 | // units of frames
           0         << 23 | // PC (NTSC) or BGF0 (PAL)
           (ss / 10) << 20 | // tens  of seconds
           (ss % 10) << 16 | // units of seconds
           0         << 15 | // BGF0 (NTSC) or BGF2 (PAL)
           (mm / 10) << 12 | // tens  of minutes
           (mm % 10) <<  8 | // units of minutes
           0         <<  7 | // BGF2 (NTSC) or PC (PAL)
           0         <<  6 | // BGF1
           (hh / 10) <<  4 | // tens  of hours
           (hh % 10);        // units of hours
}

char *bv_timecode_make_string(const BVTimecode *tc, char *buf, int framenum)
{
    int fps = tc->fps;
    int drop = tc->flags & BV_TIMECODE_FLAG_DROPFRAME;
    int hh, mm, ss, ff, neg = 0;

    framenum += tc->start;
    if (drop)
        framenum = bv_timecode_adjust_ntsc_framenum2(framenum, fps);
    if (framenum < 0) {
        framenum = -framenum;
        neg = tc->flags & BV_TIMECODE_FLAG_ALLOWNEGATIVE;
    }
    ff = framenum % fps;
    ss = framenum / fps        % 60;
    mm = framenum / (fps*60)   % 60;
    hh = framenum / (fps*3600);
    if (tc->flags & BV_TIMECODE_FLAG_24HOURSMAX)
        hh = hh % 24;
    snprintf(buf, BV_TIMECODE_STR_SIZE, "%s%02d:%02d:%02d%c%02d",
             neg ? "-" : "",
             hh, mm, ss, drop ? ';' : ':', ff);
    return buf;
}

static unsigned bcd2uint(uint8_t bcd)
{
   unsigned low  = bcd & 0xf;
   unsigned high = bcd >> 4;
   if (low > 9 || high > 9)
       return 0;
   return low + 10*high;
}

char *bv_timecode_make_smpte_tc_string(char *buf, uint32_t tcsmpte, int prevent_df)
{
    unsigned hh   = bcd2uint(tcsmpte     & 0x3f);    // 6-bit hours
    unsigned mm   = bcd2uint(tcsmpte>>8  & 0x7f);    // 7-bit minutes
    unsigned ss   = bcd2uint(tcsmpte>>16 & 0x7f);    // 7-bit seconds
    unsigned ff   = bcd2uint(tcsmpte>>24 & 0x3f);    // 6-bit frames
    unsigned drop = tcsmpte & 1<<30 && !prevent_df;  // 1-bit drop if not arbitrary bit
    snprintf(buf, BV_TIMECODE_STR_SIZE, "%02u:%02u:%02u%c%02u",
             hh, mm, ss, drop ? ';' : ':', ff);
    return buf;
}

char *bv_timecode_make_mpeg_tc_string(char *buf, uint32_t tc25bit)
{
    snprintf(buf, BV_TIMECODE_STR_SIZE, "%02u:%02u:%02u%c%02u",
             tc25bit>>19 & 0x1f,              // 5-bit hours
             tc25bit>>13 & 0x3f,              // 6-bit minutes
             tc25bit>>6  & 0x3f,              // 6-bit seconds
             tc25bit     & 1<<24 ? ';' : ':', // 1-bit drop flag
             tc25bit     & 0x3f);             // 6-bit frames
    return buf;
}

static int check_fps(int fps)
{
    int i;
    static const int supported_fps[] = {24, 25, 30, 48, 50, 60};

    for (i = 0; i < FF_ARRAY_ELEMS(supported_fps); i++)
        if (fps == supported_fps[i])
            return 0;
    return -1;
}

static int check_timecode(void *log_ctx, BVTimecode *tc)
{
    if (tc->fps <= 0) {
        bv_log(log_ctx, BV_LOG_ERROR, "Timecode frame rate must be specified\n");
        return BVERROR(EINVAL);
    }
    if ((tc->flags & BV_TIMECODE_FLAG_DROPFRAME) && tc->fps != 30 && tc->fps != 60) {
        bv_log(log_ctx, BV_LOG_ERROR, "Drop frame is only allowed with 30000/1001 or 60000/1001 FPS\n");
        return BVERROR(EINVAL);
    }
    if (check_fps(tc->fps) < 0) {
        bv_log(log_ctx, BV_LOG_ERROR, "Timecode frame rate %d/%d not supported\n",
               tc->rate.num, tc->rate.den);
        return BVERROR_PATCHWELCOME;
    }
    return 0;
}

static int fps_from_frame_rate(BVRational rate)
{
    if (!rate.den || !rate.num)
        return -1;
    return (rate.num + rate.den/2) / rate.den;
}

int bv_timecode_check_frame_rate(BVRational rate)
{
    return check_fps(fps_from_frame_rate(rate));
}

int bv_timecode_init(BVTimecode *tc, BVRational rate, int flags, int frame_start, void *log_ctx)
{
    memset(tc, 0, sizeof(*tc));
    tc->start = frame_start;
    tc->flags = flags;
    tc->rate  = rate;
    tc->fps   = fps_from_frame_rate(rate);
    return check_timecode(log_ctx, tc);
}

int bv_timecode_init_from_string(BVTimecode *tc, BVRational rate, const char *str, void *log_ctx)
{
    char c;
    int hh, mm, ss, ff, ret;

    if (sscanf(str, "%d:%d:%d%c%d", &hh, &mm, &ss, &c, &ff) != 5) {
        bv_log(log_ctx, BV_LOG_ERROR, "Unable to parse timecode, "
                                      "syntax: hh:mm:ss[:;.]ff\n");
        return BVERROR_INVALIDDATA;
    }

    memset(tc, 0, sizeof(*tc));
    tc->flags = c != ':' ? BV_TIMECODE_FLAG_DROPFRAME : 0; // drop if ';', '.', ...
    tc->rate  = rate;
    tc->fps   = fps_from_frame_rate(rate);

    ret = check_timecode(log_ctx, tc);
    if (ret < 0)
        return ret;

    tc->start = (hh*3600 + mm*60 + ss) * tc->fps + ff;
    if (tc->flags & BV_TIMECODE_FLAG_DROPFRAME) { /* adjust frame number */
        int tmins = 60*hh + mm;
        tc->start -= 2 * (tmins - tmins/10);
    }
    return 0;
}
