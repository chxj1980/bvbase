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
#include <libbvutil/bvstring.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>
#include <libbvconfig/common.h>

int main(int argc, const char *argv[])
{
    BVDeviceContext *devctx = NULL; 
    BVMobileDevice *device = NULL ;
    BVMobileDevice ipc;
    BVDictionary *opn = NULL;
    BVControlPacket pkt_in;
    BVControlPacket pkt_out;
    bv_device_register_all();
    bv_dict_set(&opn, "timeout", "1", 0);
    bv_dict_set(&opn, "user", "admin", 0);
    bv_dict_set(&opn, "passwd", "12345", 0);
    if (bv_device_open(&devctx, "onvif_dev://192.168.6.149:80/onvif/device_service", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open device error \n");
        bv_dict_free(&opn);
        return BVERROR(EIO);
    }
#if 0
    if (bv_device_control(devctx, BV_DEV_MESSAGE_TYPE_SEARCH_IPC, NULL, &pkt_out) < 0 ) {
        bv_log(devctx, BV_LOG_ERROR, "device control error\n");
    } else {
        device = pkt_out.data;
        int i = 0;
        for (i = 0; i < pkt_out.size; i++) {
           bv_log(devctx, BV_LOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
           bv_log(devctx, BV_LOG_INFO, "server service url %s\n", device[i].url);
           bv_log(devctx, BV_LOG_INFO, "server type %d\n", device[i].type);
           bv_log(devctx, BV_LOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        }
        //NOTICE free data
        bv_free(pkt_out.data);
    }
    /**
     *  detect ipc whether on line
     *  return 0 online otherwise off line
     */
    bv_sprintf(ipc.url, sizeof(ipc.url), "http://192.168.6.149:80/onvif/device_service");
    ipc.timeout = 1;
    pkt_in.data = (void *)&ipc;
    bv_device_control(devctx, BV_DEV_MESSAGE_TYPE_DETECT_IPC, &pkt_in, NULL);
#else
    BVEthernetInfo info[2];
    bv_strlcpy(info[0].address, "61.191.27.18", sizeof(info[0].address));
    bv_strlcpy(info[1].address, "192.168.6.146", sizeof(info[0].address));
    pkt_in.data = &info;
    pkt_in.size = 2;
    if (bv_device_control(devctx, BV_DEV_MESSAGE_TYPE_SET_RMTDP, &pkt_in, &pkt_out) < 0 ) {
        bv_log(devctx, BV_LOG_ERROR, "device control error\n");
    }

    if (bv_device_control(devctx, BV_DEV_MESSAGE_TYPE_GET_RMTDP, &pkt_in, &pkt_out) < 0 ) {
        bv_log(devctx, BV_LOG_ERROR, "device control error\n");
    }

#endif
    bv_dict_free(&opn);
    bv_device_close(&devctx);
    return 0;
}
