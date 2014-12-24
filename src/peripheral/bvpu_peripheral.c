/**
 *          File: bvpu_peripheral.c
 *
 *        Create: 2014年12月19日 星期五 17时13分39秒
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
#include <stdlib.h>

#include "bvpu_peripheral.h"

#include "BVPU_SDK_DataTypes.h"

#include "bvpu_pj.h"

// 
// Control of device, (ex: network, storage, module, video, audio, display,
//                         gps, serialport ...)
// TODO:
// 1. Add every port control interface though call user's callback or
//    below sdk implement.
// 2. Query device capability.
// 3. Query each channel capability. (ex: onvif, ...)
// 4. Control ptz
// 5. Query channel devices. (ex: onvif, ...)
//

struct _bvpu_peripherals {
    struct _bvpu_peripherals *next;
    BVPU_SDK_HardwareDeviceInterface *stDeviceInterface;
};

static struct _bvpu_peripherals *all_peripherals;

// Impletement in other files
extern BVPU_SDK_HardwareDeviceInterface stDeviceInterfaceIP;
extern BVPU_SDK_HardwareDeviceInterface stDeviceInterfaceAnalog;

static int bvpu_peripheral_inited = 0;

BVPU_SDK_Result bvpu_peripheral_register_all()
{
    if (bvpu_peripheral_inited) return BVPU_SDK_RESULT_S_OK;
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // Create pool for all periperals use.
    // Get caching pool
    pj_caching_pool *bvpu_pj_in_pool = bvpu_pj_get_pj_pool();
    if (!bvpu_pj_in_pool) return BVPU_SDK_RESULT_E_PJ_NOT_INIT;
    bvpu_peripheral_pool = pj_pool_create(&bvpu_pj_in_pool->factory, "peripheral",
                           2048, 1024, NULL);
    if (!bvpu_peripheral_pool) return BVPU_SDK_RESULT_E_MEM_FAIL;

    // Register the first, it is video of ip (onvif)
    struct _bvpu_peripherals *cur_peris = all_peripherals;
    cur_peris = pj_pool_calloc(bvpu_peripheral_pool, 1, sizeof(struct _bvpu_peripherals));
    cur_peris->stDeviceInterface = &stDeviceInterfaceIP;

    // register next one.
    cur_peris = cur_peris->next;

    // All register success, set it.
    bvpu_peripheral_inited = 1;
    return result;
};

BVPU_SDK_Result bvpu_peripheral_unregister_all()
{
    if (!bvpu_peripheral_inited) return BVPU_SDK_RESULT_S_OK;

    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // All malloced data just release this pool 
    pj_pool_release(bvpu_peripheral_pool);

    return result;
}

BVPU_SDK_Result bvpu_peripheral_register(BVPU_SDK_HardwareDeviceInterface *stDeviceInterface)
{
    if (!stDeviceInterface) return BVPU_SDK_RESULT_E_INVALIDPARAM;

    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    struct _bvpu_peripherals *cur_peris = all_peripherals;
    while (cur_peris) {
        if (cur_peris->stDeviceInterface->iHardwareType == stDeviceInterface->iHardwareType) {
            cur_peris->stDeviceInterface = stDeviceInterface;
            break;
        }
        cur_peris = cur_peris->next;
    }
    // cur_peris == NULL means has no
    if (!cur_peris) {
        cur_peris = pj_pool_calloc(bvpu_peripheral_pool, 1, sizeof(struct _bvpu_peripherals));
        cur_peris->stDeviceInterface = stDeviceInterface;
    }
    return result;
}

BVPU_SDK_HardwareDeviceInterface *bvpu_peripheral_find_device(BVPU_SDK_HardwareDeviceType iHardwareType)
{
    BVPU_SDK_HardwareDeviceInterface *find = NULL;

    struct _bvpu_peripherals *cur_peris = all_peripherals;
    while (cur_peris) {
        if (cur_peris->stDeviceInterface->iHardwareType == iHardwareType) {
            find = cur_peris->stDeviceInterface;
            break;
        }
        cur_peris = cur_peris->next;
    }
 
    return find;
}

/*=============== End of file: bvpu_peripheral.c ==========================*/
