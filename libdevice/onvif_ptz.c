/*************************************************************************
	> File Name: onvif_ptz.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月14日 星期日 17时17分33秒
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

#include "device.h"
#include <bvonvif.h>

typedef struct OnvifPTZContext {
    const AVClass *av_class;
    int time_out;
    char url[1024];
} OnvifPTZContext;

static int onvif_ptz_open(BVDeviceContext *h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    char *p;
    av_log(onvif_ptz, AV_LOG_ERROR, "onvif ptz open\n");
    if (h->url[0] == '\0') {
        av_log(onvif_ptz, AV_LOG_ERROR, "url is NULL\n");
        return -1;
    }
    p = strstr(h->url, ":");
    sprintf(onvif_ptz->url, "http%s",p);
    
    return 0;
}

static int onvif_probe(BVDeviceContext *h, const char *args)
{
    if (strcmp("onvif_ptz", args) == 0) {
        return 100;
    }
    return 0;
}

static int onvif_ptz_continuous_move(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZContinuousMove *continuous_move = (BVPTZContinuousMove *) pkt_in->data;
    BVOnvifDevice device_info;
    memset(&device_info, 0, sizeof(device_info));
    strncpy(device_info.service_url, onvif_ptz->url, sizeof(device_info.service_url));
    strcpy(device_info.profile_token, "P2");
    ret = bv_onvif_continuous_move(&device_info, continuous_move);
    if (ret < 0) {
        av_log(h, AV_LOG_ERROR, "onvif ptz continuous move error\n");
    }
    return ret;
}

static int onvif_ptz_stop(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZStop *stop = (BVPTZStop *)pkt_in->data;
    BVOnvifDevice device_info;
    memset(&device_info, 0, sizeof(device_info));
    strcpy(device_info.profile_token, "P2");
    strncpy(device_info.service_url, onvif_ptz->url, sizeof(device_info.service_url));
    ret = bv_onvif_continuous_stop(&device_info, stop);
    if (ret < 0) {
        av_log(h, AV_LOG_ERROR, "onvif ptz continuous stop error\n");
    }

    return 0;
}

static int onvif_ptz_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    switch(type) {
        case BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE:
            ret = onvif_ptz_continuous_move(h, pkt_in, pkt_out);
            break;
        case BV_DEV_MESSAGE_TYPE_PTZ_STOP:
            ret = onvif_ptz_stop(h, pkt_in, pkt_out);
            break;
        default:
            av_log(h, AV_LOG_ERROR, "Not Support This command \n");
            break;
    }
    return ret;
}

static int onvif_ptz_close(BVDeviceContext*h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    if (!onvif_ptz)
        return -1;
    return 0;
}

#define OFFSET(x) offsetof(OnvifPTZContext, x)
#define DEC AV_OPT_FLAG_DECODING_PARAM
static const AVOption options[] = {
	{"time_out", "read write time out", OFFSET(time_out), AV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
	{NULL}
};

static const AVClass onvif_class = {
	.class_name     = "onvif ptz device",
	.item_name      = av_default_item_name,
	.option         = options,
	.version        = LIBAVUTIL_VERSION_INT,
	.category       = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

BVDevice bv_onvif_ptz_device = {
    .name = "onvif_ptz",
    .type = BV_DEVICE_TYPE_ONVIF_PTZ,
    .priv_data_size = sizeof(OnvifPTZContext),
    .dev_open = onvif_ptz_open,
    .dev_probe = onvif_probe,
    .dev_control = onvif_ptz_control,
    .dev_close = onvif_ptz_close,
    .priv_class = &onvif_class,
};
