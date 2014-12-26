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
#include <libconfig/common.h>

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
    if ((ret = bv_device_open(&device_context, NULL, "onvif_ptz://192.168.6.149:8899/onvif/ptz_service", &opn)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "open device error %d\n", ret);
        av_dict_free(&opn);
        return AVERROR(EIO);
    }
    BVDevicePacket pkt_in;
    BVPTZContinuousMove continuous_move;
    BVPTZVector vector;
    BVPTZStop stop;
    stop.pan_tilt = true;
    stop.zoom = true;

    //Left
    continuous_move.velocity.pan_tilt.x = -0.5f;
    continuous_move.velocity.pan_tilt.y = 0.0f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    //Left Down
    continuous_move.velocity.pan_tilt.x = -0.5f;
    continuous_move.velocity.pan_tilt.y = -0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    //Down
    continuous_move.velocity.pan_tilt.x = 0.0f;
    continuous_move.velocity.pan_tilt.y = -0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);
    
    //RightDown
    continuous_move.velocity.pan_tilt.x = 0.5f;
    continuous_move.velocity.pan_tilt.y = -0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    //Right
    continuous_move.velocity.pan_tilt.x = 0.5f;
    continuous_move.velocity.pan_tilt.y = 0.0f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    //Right Up
    continuous_move.velocity.pan_tilt.x = 0.5f;
    continuous_move.velocity.pan_tilt.y = 0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);
    
    //up
    continuous_move.velocity.pan_tilt.x = 0.0f;
    continuous_move.velocity.pan_tilt.y = 0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    //Left Up
    continuous_move.velocity.pan_tilt.x = -0.5f;
    continuous_move.velocity.pan_tilt.y = 0.5f;
    continuous_move.velocity.zoom.x = 0.0f;
    continuous_move.duration = 3000LL;
    pkt_in.data = &continuous_move;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, &pkt_in, NULL);
    usleep(300000);
    pkt_in.data = &stop;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_PTZ_STOP, &pkt_in, NULL);
    sleep(1);

    av_dict_free(&opn);
    bv_device_close(&device_context);
    return 0;
}
