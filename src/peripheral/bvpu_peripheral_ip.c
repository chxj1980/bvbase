/**
 *          File: bvpu_peripheral_ip.c
 *
 *        Create: 2014年12月23日 星期二 15时43分21秒
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

#include "bvpu_peripheral.h"

static BVPU_SDK_Result bvpu_peripheral_ip_open(const char *device, int flag, int indx, void *priv)
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

static BVPU_SDK_Result bvpu_peripheral_ip_read(void *priv, void *data, int len)
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

static BVPU_SDK_Result bvpu_peripheral_ip_write(void *priv, void *data, int len)
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

static BVPU_SDK_Result bvpu_peripheral_ip_control(int iCmd, void *data)
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

static BVPU_SDK_Result bvpu_peripheral_ip_seek(int whence, long offset)
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

static BVPU_SDK_Result bvpu_peripheral_ip_close()
{
     BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    
     return result;
}

BVPU_SDK_HardwareDeviceInterface stDeviceInterfaceIP = {
    .iHardwareType = BVPU_SDK_HARDWARE_DEVICETYPE_IP_VIDEOIN,
    .deviceName    = "ip",
    .iIndex        = 0,
    .OnOpen        = bvpu_peripheral_ip_open,
    .OnRead        = bvpu_peripheral_ip_read,
    .OnWrite       = bvpu_peripheral_ip_write,
    .OnControl     = bvpu_peripheral_ip_control,
    .OnSeek        = bvpu_peripheral_ip_seek,
    .OnClose       = bvpu_peripheral_ip_close,
};


/*=============== End of file: bvpu_peripheral_ip.c ==========================*/
