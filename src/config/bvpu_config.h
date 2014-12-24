/**
 *          File: bvpu_config.h
 *
 *        Create: 2014年12月22日 星期一 16时48分22秒
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

#ifndef __BVPU_CONFIG_H
#define __BVPU_CONFIG_H

#include "BVPU_SDK_DataTypes.h"

// 
// Config contain not only the config will set to param file but also the config
// in mememory for other program use.

// Init every config from file.
// And keep them in the defined structs.
int bvpu_config_init();

// Just from something
int bvpu_config_deinit();

BVPU_SDK_ChannelDeviceInfo *bvpu_config_get_channel_info();
int bvpu_config_set_channel_info(BVPU_SDK_ChannelDeviceInfo *stChannelDeviceInfo);

#endif  // __BVPU_CONFIG_H

/*=============== End of file: bvpu_config.h =====================*/
