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

typedef struct OnvifPTZContext {
    const AVClass *av_class;
    int time_out;
    char *url;
} OnvifPTZContext;

static int onvif_ptz_open(BVDeviceContext *h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    av_log(onvif_ptz, AV_LOG_ERROR, "onvif ptz open\n");
//    if (h->url)
    return 0;
}

static int onvif_ptz_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    return 0;
}

static int onvif_ptz_close(BVDeviceContext*h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
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
    .dev_control = onvif_ptz_control,
    .dev_close = onvif_ptz_close,
    .priv_class = &onvif_class,
};
