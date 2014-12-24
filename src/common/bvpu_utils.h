/**
 *          File: bvpu_utils.h
 *
 *        Create: 2014年12月19日 星期五 14时13分45秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
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

#ifndef __BVPU_UTILS_H
#define __BVPU_UTILS_H

#include <time.h>

#include "BVPU_SDK_DataTypes.h"

// 0xff,0x88,0x9 --> 0xff 0x88 0x9 (in @data)
int bvpu_parse_data(unsigned char *data, char *string);

// ex: xx,xxx,xxxx
// parse from @string to find each @sign, with @maxlen
// save them to @save for max @arrsize
// return max found.
int bvpu_parse_data_string(char *string, char *save, int arrsize, int maxlen, const char sign);

// ex: d,s,2
// Parse from @string of @maxlen with @sign,
// save them to @data
// @data must enough long.
// return how many  be parsed.
int bvpu_parse_data_char(unsigned char *data, char *string, int maxlen, const char *sign);

// ex:d,s,2
// Parse from @string of @maxlen with @sign
// save them to @save.
// @save must enough long.
// return length be parsed.
int bvpu_parse_string_int(char *string, int *save, int maxlen, const char *sign);

// (xx,xx);(xx,xx)
// We don't control if great then imageSize max size
// Parse from @string with @sign,
// save to @imageSize
int bvpu_parse_string_imagerects(char *string, BVPU_SDK_ImageSize *imageSize, const char *sign);

// ex: (xxxx) -> xxxx
// remove parenthesis of @string, save it to @save.
int bvpu_clear_parenthesis(char *save, char *string);

// Like up, be change @string.
int bvpu_clear_parenthesis_self(char *string);

// Check if @now in @start and @end.
// Don't support compare for the second day with the first day.
int bvpu_in_walltime(BVPU_SDK_WallTime *start, BVPU_SDK_WallTime *end, struct tm *now);

// 
// Check if now time in daytimeslice
int bvpu_in_daytimeslice(BVPU_SDK_DayTimeSlice *slice, struct tm *now);

// Remote the last '\n' mark.
int bvpu_remove_last_break(char *args);

//////////////////////
// With Network
// Get the ip of @dn_or_ip, 
// @eth: device name (like eth,ppp1)
char *bvpu_get_ip(char *dn_or_ip, const char *eth);

// Get flow of pointed ip
// @interface (like eth,wlan ...)
int bvpu_get_flow(const char *interface, unsigned long long *recv,unsigned long long *send, unsigned long long *total);

#endif  // __BVPU_UTILS_H

/*=============== End of file: bvpu_utils.h =====================*/
