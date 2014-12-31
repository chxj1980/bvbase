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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvdevice.h"

#include <soapH.h>
#include <wsseapi.h>

typedef struct OnvifPTZContext {
    const BVClass *bv_class;
    int timeout;
    char *user;
    char *passwd;
    char url[1024];
    char token[64];
} OnvifPTZContext;

//Onvif default timeout	0.5s
#define ONVIF_TMO	(-500000)
#define MEMSET_STRUCT(X)    memset(&(X), 0, sizeof((X)));

static struct soap *bv_soap_new(OnvifPTZContext *onvif_ptz)
{
    int timeout = onvif_ptz->timeout;
	struct soap *soap = NULL;
	soap = soap_new();
	if (soap == NULL) {
		return NULL;
	}
	soap_set_namespaces(soap, namespaces);
	if (!timeout) {
		timeout = ONVIF_TMO;
	}

    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }
	soap->recv_timeout = timeout;
	soap->send_timeout = timeout;
	soap->connect_timeout = timeout;
	return soap;
}

static void bv_soap_free(struct soap *soap)
{
	if (soap == NULL) {
		return;
	}
	soap_destroy(soap);
	soap_end(soap);
	soap_free(soap);
}

static int onvif_ptz_open(BVDeviceContext *h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    char *p;
    bv_log(onvif_ptz, BV_LOG_ERROR, "onvif ptz open\n");
    if (h->url[0] == '\0') {
        bv_log(onvif_ptz, BV_LOG_ERROR, "url is NULL\n");
        return -1;
    }
    p = strstr(h->url, ":");
    sprintf(onvif_ptz->url, "http%s",p);
    sprintf(onvif_ptz->token, "00000"); 
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
	int retval = SOAP_OK;
    struct _tptz__ContinuousMove tptz__ContinuousMove; 
    struct tt__PTZSpeed Velocity;
    struct tt__Vector2D PanTilt;
    struct tt__Vector1D Zoom;
    struct _tptz__ContinuousMoveResponse tptz__ContinuousMoveResponse;
	struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZContinuousMove *continuous_move = (BVPTZContinuousMove *) pkt_in->data;

    if(!(soap = bv_soap_new(onvif_ptz))) {
        return -1;
    }

	soap_default_SOAP_ENV__Header(soap, &header);
    MEMSET_STRUCT(tptz__ContinuousMove);
    MEMSET_STRUCT(tptz__ContinuousMoveResponse);
    MEMSET_STRUCT(Velocity);
    MEMSET_STRUCT(PanTilt);
    MEMSET_STRUCT(Zoom);
    tptz__ContinuousMove.ProfileToken = onvif_ptz->token;
    tptz__ContinuousMove.Velocity = &Velocity;
    tptz__ContinuousMove.Timeout = &continuous_move->duration;
    Velocity.PanTilt = &PanTilt;
    Velocity.Zoom = & Zoom;

    PanTilt.x = continuous_move->velocity.pan_tilt.x;
    PanTilt.y = continuous_move->velocity.pan_tilt.y;
    Zoom.x = continuous_move->velocity.zoom.x;
    
    retval = soap_call___tptz__ContinuousMove(soap, onvif_ptz->url, NULL, &tptz__ContinuousMove, &tptz__ContinuousMoveResponse);

    if (retval != SOAP_OK) {
		printf("PTZ ContinuousMove error");
		printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    bv_soap_free(soap);

    return ret;
}

static int onvif_ptz_stop(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
	int retval = SOAP_OK;
	struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    struct _tptz__Stop tptz__Stop;
    struct _tptz__StopResponse tptz__StopResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZStop *stop = (BVPTZStop *)pkt_in->data;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__Stop);
    MEMSET_STRUCT(tptz__StopResponse);

    if(!(soap = bv_soap_new(onvif_ptz))) {
        return -1;
    }
	soap_default_SOAP_ENV__Header(soap, &header);

    tptz__Stop.ProfileToken = onvif_ptz->token;
    tptz__Stop.PanTilt = (enum xsd__boolean *)&stop->pan_tilt;
    tptz__Stop.Zoom = (enum xsd__boolean *)&stop->zoom;

    retval = soap_call___tptz__Stop(soap, onvif_ptz->url, NULL, &tptz__Stop, &tptz__StopResponse);
    if (retval != SOAP_OK) {
		printf("PTZ ContinuousMove error");
		printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    bv_soap_free(soap);

    return ret;
}

static int onvif_ptz_set_preset(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
	int retval = SOAP_OK;
	struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    struct _tptz__SetPreset tptz__SetPreset;
    struct _tptz__SetPresetResponse tptz__SetPresetResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZPreset *preset = (BVPTZPreset *)pkt_in->data;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__SetPreset);
    MEMSET_STRUCT(tptz__SetPresetResponse);

    if(!(soap = bv_soap_new(onvif_ptz))) {
        return -1;
    }
	soap_default_SOAP_ENV__Header(soap, &header);
    tptz__SetPreset.ProfileToken = onvif_ptz->token;
    tptz__SetPreset.PresetName = preset->name;
    tptz__SetPreset.PresetToken = preset->token;

    retval = soap_call___tptz__SetPreset(soap, onvif_ptz->url, NULL, &tptz__SetPreset, &tptz__SetPresetResponse);
    if (retval != SOAP_OK) {
		printf("PTZ SetPreset error");
		printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    bv_soap_free(soap);

    return ret;
}

static int onvif_ptz_goto_preset(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
	int retval = SOAP_OK;
	struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    struct _tptz__GotoPreset tptz__GotoPreset;
    struct _tptz__GotoPresetResponse tptz__GotoPresetResponse;
    struct tt__PTZSpeed Speed;
    struct tt__Vector2D PanTilt;
    struct tt__Vector1D Zoom;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZGotoPreset *goto_preset = (BVPTZGotoPreset*)pkt_in->data;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__GotoPreset);
    MEMSET_STRUCT(tptz__GotoPresetResponse);

    MEMSET_STRUCT(Speed);
    MEMSET_STRUCT(PanTilt);
    MEMSET_STRUCT(Zoom);

    if(!(soap = bv_soap_new(onvif_ptz))) {
        return -1;
    }
	soap_default_SOAP_ENV__Header(soap, &header);
    tptz__GotoPreset.ProfileToken = onvif_ptz->token;
    tptz__GotoPreset.PresetToken = goto_preset->token;
    tptz__GotoPreset.Speed = &Speed;

    Speed.PanTilt = &PanTilt;
    Speed.Zoom = & Zoom;

    PanTilt.x = goto_preset->speed.pan_tilt.x;
    PanTilt.y = goto_preset->speed.pan_tilt.y;
    Zoom.x = goto_preset->speed.zoom.x;

    retval = soap_call___tptz__GotoPreset(soap, onvif_ptz->url, NULL, &tptz__GotoPreset, &tptz__GotoPresetResponse);
    if (retval != SOAP_OK) {
		printf("PTZ SetPreset error");
		printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    bv_soap_free(soap);

    return ret;
}

static int onvif_ptz_remove_preset(BVDeviceContext *h, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
	int retval = SOAP_OK;
	struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    struct _tptz__RemovePreset tptz__RemovePreset;
    struct _tptz__RemovePresetResponse tptz__RemovePresetResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZPreset *preset = (BVPTZPreset *)pkt_in->data;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__RemovePreset);
    MEMSET_STRUCT(tptz__RemovePresetResponse);

    if(!(soap = bv_soap_new(onvif_ptz))) {
        return -1;
    }
	soap_default_SOAP_ENV__Header(soap, &header);
    tptz__RemovePreset.ProfileToken = onvif_ptz->token;
    tptz__RemovePreset.PresetToken = preset->token;

    retval = soap_call___tptz__RemovePreset(soap, onvif_ptz->url, NULL, &tptz__RemovePreset, &tptz__RemovePresetResponse);
    if (retval != SOAP_OK) {
		printf("PTZ RemovePreset error");
		printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    bv_soap_free(soap);

    return ret;
}

#if 0
/**
 *  不方便控制 type：参数不容意检测,容易出现段错误
 *  不知道效率能提高多少
 */
typedef struct OnvifPTZControl {
    int (*control)(BVDeviceContext *h, const BVDevicePacket *, BVDevicePacket *);
} OnvifPTZControl;


static OnvifPTZControl onvif_control[] = {
   [BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE] = onvif_ptz_continuous_move,
   [BV_DEV_MESSAGE_TYPE_PTZ_SET_PRESET] = onvif_ptz_set_preset,
};
#endif

static int onvif_ptz_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = -1;
  //  OnvifPTZContext *onvif_ptz = h->priv_data;
    switch(type) {
        case BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE:
            ret = onvif_ptz_continuous_move(h, pkt_in, pkt_out);
            break;
        case BV_DEV_MESSAGE_TYPE_PTZ_STOP:
            ret = onvif_ptz_stop(h, pkt_in, pkt_out);
            break;
        case BV_DEV_MESSAGE_TYPE_PTZ_SET_PRESET:
            ret = onvif_ptz_set_preset(h, pkt_in, pkt_out);
            break;
        case BV_DEV_MESSAGE_TYPE_PTZ_GOTO_PRESET:
            ret = onvif_ptz_goto_preset(h, pkt_in, pkt_out);
            break;
        case BV_DEV_MESSAGE_TYPE_PTZ_REMOVE_PRESET:
            ret = onvif_ptz_remove_preset(h, pkt_in, pkt_out);
            break;
        default:
            bv_log(h, BV_LOG_ERROR, "Not Support This command \n");
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
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
	{"timeout", "read write time out", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
    {"user", "user name", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "user password", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
	{NULL}
};

static const BVClass onvif_class = {
	.class_name     = "onvif ptz device",
	.item_name      = bv_default_item_name,
	.option         = options,
	.version        = LIBBVUTIL_VERSION_INT,
	.category       = BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
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
