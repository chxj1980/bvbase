/*************************************************************************
    > File Name: alldevices.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年09月25日 星期四 15时51分48秒
 ************************************************************************/
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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvdevice.h"
#include <config.h>

#define REGISTER_DEVICE(X, x)                                         \
    {                                                                   \
        extern BVDevice bv_##x##_device;                           \
        if (BV_CONFIG_##X##_DEVICE)                                      \
            bv_device_register(&bv_##x##_device);                \
    }

void bv_device_register_all(void)
{
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    //REGISTER_DEVICE(GPS, gps);
    //REGISTER_DEVICE(SERIAL, serial);
    //REGISTER_DEVICE(PTZ, ptz);
    REGISTER_DEVICE(ONVIF_DEV, onvif_dev);
    REGISTER_DEVICE(ONVIF_PTZ, onvif_ptz);
    REGISTER_DEVICE(DISK_DEV,  disk_dev);
}
