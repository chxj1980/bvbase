/**
 *          File: bvpu_config.c
 *
 *        Create: 2014年12月22日 星期一 16时46分34秒
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

#include <stdio.h>

#include "bvpu_config.h"

//
// Config of the system, save them all one copy,
// other program get config from this interface.
// 1. The system hardware info.
// 2. Each channel config of video / audio (in, out).
// 3. Each ptz config info.
// 4. Each gps config info.
// 5. Each serial port config info.
//
// This config implemented by the low config interface, also
// can be done by read config from file self.
//

typedef struct {
    BVPU_SDK_ChannelDeviceInfo         stChannelDeviceInfo;
} BVPU_Config_Global;


/*=============== End of file: bvpu_config.c ==========================*/
