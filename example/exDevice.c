/*************************************************************************
	> File Name: exDevice.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月14日 星期日 17时11分53秒
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

#include <libdevice/device.h>

int main(int argc, const char *argv[])
{
    int ret;
    BVDevice *device = NULL;
    BVDeviceContext *device_context = NULL;
    AVDictionary *opn = NULL;


    bv_device_register_all();
    
    device = bv_device_find_device(BV_DEVICE_TYPE_ONVIF_PTZ);
    if (!device) {
        av_log(NULL, AV_LOG_ERROR, "Not Find This device\n");
        return AVERROR(EINVAL);
    }
    av_dict_set(&opn, "fd", "1", 0);
    av_dict_set(&opn, "size", "800x600", 0);
    if ((ret = bv_dev_open(&device_context, device, "onvifptz://xxxx", &opn)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "open device error %d\n", ret);
        av_dict_free(&opn);
        return AVERROR(EIO);
    }
    av_dict_free(&opn);
    bv_dev_close(&device_context);
    return 0;
}
