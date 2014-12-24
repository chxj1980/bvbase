/**
 *          File: bvpu_peripheral.h
 *
 *        Create: 2014年12月19日 星期五 17时17分41秒
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

#ifndef __BVPU_PERIPHERAL_H
#define __BVPU_PERIPHERAL_H

#include "BVPU_SDK_DataTypes.h"

#include "pjlib.h"

// Used by caching pool.
// This variables will be inited in @bvpu_peripheral_register_all
pj_pool_t  *bvpu_peripheral_pool;

// Register all different devices with diff implement.
BVPU_SDK_Result bvpu_peripheral_register_all();

// Register one of the interface for implement by caller.
BVPU_SDK_Result bvpu_peripheral_register(BVPU_SDK_HardwareDeviceInterface *stDeviceInterface);

// Get the device to control by other threads.
BVPU_SDK_HardwareDeviceInterface *bvpu_peripheral_find_device(BVPU_SDK_HardwareDeviceType iHardwareType);

///////////////////////////////////////////////////////////////////////////
// Easy function to call for get something needed
///////////////////////////////////////////////////////////////////////////

BVPU_SDK_Result bvpu_peripheral_device_capability(BVPU_SDK_DeviceCapability *stDeviceCapability);

BVPU_SDK_Result bvpu_peripheral_ptz_capability(BVPU_SDK_PTZCapability *stPTZCapability, int indx);

BVPU_SDK_Result bvpu_peripheral_media_channel_capability(BVPU_SDK_DeviceMediaCapability *stDeviceMediaCapability, int indx);

// 
// Search channel device
// @stSearchDevice: input param for what protocol to search.
// @stDevices: return what found.
BVPU_SDK_Result bvpu_peripheral_device_search(BVPU_SDK_SearchLocalNetworkDevice *stSearchDevice, 
                                              BVPU_SDK_LocalNetworkDevice *stDevices);


#endif   // __BVPU_PERIPHERAL_H

/*=============== End of file: bvpu_peripheral.h =====================*/
