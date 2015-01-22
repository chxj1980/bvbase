/*************************************************************************
	> File Name: device_scan.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月31日 星期三 17时38分54秒
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */
#include <libbvdevice/bvdevice.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>
#include <libbvconfig/common.h>

int main(int argc, const char *argv[])
{
    BVDeviceContext *devctx = NULL; 
    BVMobileDevice device[20] = { 0 };
    int max_ret = 20;
    BVDictionary *opn = NULL;
    bv_device_register_all();
    bv_dict_set(&opn, "timeout", "5", 0);
    if (bv_device_open(&devctx, NULL, "onvif_dev://", &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open device error \n");
        bv_dict_free(&opn);
        return BVERROR(EIO);
    }
    bv_device_scan(devctx, device, &max_ret);
    int i = 0;
    for (i = 0; i < max_ret; i++) {
       bv_log(devctx, BV_LOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
       bv_log(devctx, BV_LOG_INFO, "server service url %s\n", device[i].url);
       bv_log(devctx, BV_LOG_INFO, "server type %d\n", device[i].type);
       bv_log(devctx, BV_LOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    }

    bv_dict_free(&opn);
    bv_device_close(&devctx);
    return 0;
}
