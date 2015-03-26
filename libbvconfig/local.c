/*************************************************************************
    > File Name: local.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月13日 星期二 10时56分35秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

#line 25 "local.c"

#include <libbvutil/bvstring.h>

#include "bvconfig.h"

typedef struct _LocalContext {
    const BVClass       *bv_class;
    char                value[512]; //warning:must enough to save string
    int64_t             lvalue;
    BVConfigObject      *obj;
    BVConfigObject      *elem;
    BVConfigObject      *elem2;
    BVConfigObject      *memb;
    BVConfigObject      *memb2;
}LocalContext;

static int local_probe(BVConfigContext *h, BVProbeData *p)
{
    if (bv_strstart(p->filename, "local:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

//url like: test.json.
static int local_open(BVConfigContext *h)
{
    int ret = 0;
    BVDictionary *opn = NULL;
    LocalContext *localctx = h->priv_data;
#if 0
    if (!url) {
        bv_log(h, BV_LOG_ERROR, "file name must set\n");
        return BVERROR(EINVAL);
    }
    if ((ret = bv_config_file_open(&h->pdb, url, NULL, &opn)) < 0) {
        bv_log(h, BV_LOG_ERROR, "open config error\n");
        return BVERROR(EINVAL);
    }
#endif
    bv_log(h, BV_LOG_DEBUG, "open config, return [%d]\n", ret);
    return ret;
}

static int local_close(BVConfigContext *h)
{
    int ret = 0;
    LocalContext *localctx = h->priv_data;
    //ret = bv_config_file_close(&h);
    return ret;
}

static int local_get_device_info(BVConfigContext *h, BVDeviceInfo *devinfo)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "device_info");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[device_info] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[device_info] type %d\n", localctx->obj->type);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "device_model");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[device_model] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->device_model, localctx->value, sizeof(devinfo->device_model));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "device_type");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[device_type] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->device_type, localctx->value, sizeof(devinfo->device_type));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "software_version");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[software_version] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->software_version, localctx->value, sizeof(devinfo->software_version));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "hardware_version");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[hardware_version] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->hardware_version, localctx->value, sizeof(devinfo->hardware_version));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "hardware_model");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[hardware_model] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->hardware_model, localctx->value, sizeof(devinfo->hardware_model));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "manufacturer_id");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[manufacturer_id] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->manufacturer_id, localctx->value, sizeof(devinfo->manufacturer_id));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "manufacturer");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[manufacturer] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->manufacturer_name, localctx->value, sizeof(devinfo->manufacturer_name));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "device_id");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[device_id] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->device_id, localctx->value, sizeof(devinfo->device_id));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "device_name");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[device_name] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(devinfo->device_name, localctx->value, sizeof(devinfo->device_name));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "wifi_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[wifi_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->wifi_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "wireless_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[wireless_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->wireless_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "channel_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[channel_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->channel_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "video_in_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[video_in_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->video_sources = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "video_out_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[video_out_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->video_outputs = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "audio_in_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_in_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->audio_sources = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "audio_out_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_out_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->audio_outputs = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "serial_port_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[serial_port_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->serial_ports = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "alert_in_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[alert_in_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->alert_in_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "alert_out_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[alert_out_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->alert_out_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "storage_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[storage_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->storage_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "ptz_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[ptz_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->ptz_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "gps_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[gps_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->gps_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "support_sms");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[support_sms] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->support_sms = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "support_call");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[support_call] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->support_call = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "preset_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[preset_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->preset_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->obj, "cruise_count");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[cruise_count] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        devinfo->cruise_count = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_encoders] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);                     
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "encoding");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[encoding] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "H264") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_H264;
        } else if (bv_strcasecmp(localctx->value, "MPEG") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_MPEG;
        } else if (bv_strcasecmp(localctx->value, "JPEG") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_JPEG;
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "width");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[width] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.width = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "height");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[height] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.height = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "quality");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[quality] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.quality = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "rate_control");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[rate_control] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "VBR") == 0) {
            config->codec_context.mode_id = BV_RC_MODE_ID_VBR;
        } else if (bv_strcasecmp(localctx->value, "CBR") == 0) {
            config->codec_context.mode_id = BV_RC_MODE_ID_CBR;
        } else if (bv_strcasecmp(localctx->value, "ABR") == 0) {
            config->codec_context.mode_id = BV_RC_MODE_ID_ABR;
        } else if (bv_strcasecmp(localctx->value, "FIXQP") == 0) {
            config->codec_context.mode_id = BV_RC_MODE_ID_FIXQP;
        } else if (bv_strcasecmp(localctx->value, "BUTT") == 0) {
            config->codec_context.mode_id = BV_RC_MODE_ID_BUTT;
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "gop");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[gop] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.gop_size = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "framerate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[framerate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.time_base = (BVRational) {1, localctx->lvalue};
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "bitrate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[bitrate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.bit_rate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_set_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "set member[video_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "set member[video_encoders] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "set element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "set element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[token] error\n");
    } else {
        bv_strlcpy(localctx->value, config->token, sizeof(config->token));
        bv_config_object_set_value(h->pdb, localctx->memb, localctx->value);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "encoding");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[encoding] error\n");
    } else {
        switch (config->codec_context.codec_id) {
            case BV_CODEC_ID_H264:
            {
                bv_strlcpy(localctx->value, "H264", sizeof(localctx->value));
                break;
            }
            case BV_CODEC_ID_MPEG:
            {
                bv_strlcpy(localctx->value, "MPEG", sizeof(localctx->value));
                break;
            }
            case BV_CODEC_ID_JPEG:
            {
                bv_strlcpy(localctx->value, "JPEG", sizeof(localctx->value));
                break;
            }
        }
        bv_config_object_set_value(h->pdb, localctx->memb, localctx->value);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "width");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[width] error\n");
    } else {
        localctx->lvalue = config->codec_context.width;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "height");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[height] error\n");
    } else {
        localctx->lvalue = config->codec_context.height;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "quality");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[quality] error\n");
    } else {
        localctx->lvalue = config->codec_context.quality;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "rate_control");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[rate_control] error\n");
    } else {
        switch (config->codec_context.mode_id) {
            case BV_RC_MODE_ID_VBR:
            {
                bv_strlcpy(localctx->value, "VBR", sizeof(localctx->value));
                break;
            }
            case BV_RC_MODE_ID_CBR:
            {
                bv_strlcpy(localctx->value, "CBR", sizeof(localctx->value));
                break;
            }
            case BV_RC_MODE_ID_ABR:
            {
                bv_strlcpy(localctx->value, "ABR", sizeof(localctx->value));
                break;
            }
            case BV_RC_MODE_ID_FIXQP:
            {
                bv_strlcpy(localctx->value, "FIXQP", sizeof(localctx->value));
                break;
            }
            case BV_RC_MODE_ID_BUTT:
            {
                bv_strlcpy(localctx->value, "BUTT", sizeof(localctx->value));
                break;
            }   
        }
        bv_config_object_set_value(h->pdb, localctx->memb, localctx->value);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "gop");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[gop] error\n");
    } else {
        localctx->lvalue = config->codec_context.gop_size;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "framerate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[framerate] error\n");
    } else {
        if (config->codec_context.time_base.num == 0) {
            config->codec_context.time_base.den = 1;
            config->codec_context.time_base.num = 25;
        }
        localctx->lvalue = config->codec_context.time_base.den / config->codec_context.time_base.num;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "bitrate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "set member[bitrate] error\n");
    } else {
        localctx->lvalue = config->codec_context.bit_rate;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_video_encoder_options(BVConfigContext *h, int channel, int index, BVVideoEncoderOption *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoder_options");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_encoder_options] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_encoder_options] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }

    bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_audio_encoder_options(BVConfigContext *h, int channel, int index, BVAudioEncoderOption *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoder_options");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoder_options] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoder_options] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    } 

    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_encoders] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "encoding");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[encoding] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "G711A") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_G711A;
        } else if (bv_strcasecmp(localctx->value,"G711U") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_G711U;
        } else if (bv_strcasecmp(localctx->value, "G726") == 0) {
            config->codec_context.codec_id = BV_CODEC_ID_G726;
        } else if (bv_strcasecmp(localctx->value, "AAC") == 0)  {
            config->codec_context.codec_id = BV_CODEC_ID_AAC;
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "bitrate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[bitrate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.bit_rate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "sample");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->codec_context.sample_rate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_set_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "set member[audio_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "set member[audio_encoders] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_strlcpy(localctx->value, config->token, sizeof(config->token));
        bv_config_object_set_value(h->pdb, localctx->memb, localctx->value);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "encoding");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[encoding] error\n");
    } else {
        switch (config->codec_context.codec_id) {
            case BV_CODEC_ID_G711A:
            {
                bv_strlcpy(localctx->value, "G711A", sizeof(localctx->value));
                break;
            }
            case BV_CODEC_ID_G711U:
            {
                bv_strlcpy(localctx->value, "G711U", sizeof(localctx->value));
                break;
            }
            case BV_CODEC_ID_G726:
            {
                bv_strlcpy(localctx->value, "G726", sizeof(localctx->value));
                break;
            }
            case BV_CODEC_ID_AAC:
            {
                bv_strlcpy(localctx->value, "AAC", sizeof(localctx->value));
                break;
            }
        }
        bv_config_object_set_value(h->pdb, localctx->memb, localctx->value);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "bitrate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[bitrate] error\n");
    } else {
        localctx->lvalue = config->codec_context.bit_rate;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "sample");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample] error\n");
    } else {
        localctx->lvalue = config->codec_context.sample_rate;
        bv_config_object_set_value(h->pdb, localctx->memb, &localctx->lvalue);
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_ptz_device(BVConfigContext *h, int channel, int index, BVPTZDevice *config)
{
    int ret = 0;
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "PTZDevice");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[PTZDevice] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[PTZDevice] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, channel);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->elem, index);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "url");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[url] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "pan");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[pan] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        ret = sscanf(localctx->value, "[%f..%f]", &config->pan_range.min, &config->pan_range.max);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[pan] value is invalid\n");
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "tilt");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[tilt] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        ret = sscanf(localctx->value, "[%f..%f]", &config->tilt_range.min, &config->tilt_range.max);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[tilt] value is invalid\n");
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "zoom");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[zoom] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        ret = sscanf(localctx->value, "[%f..%f]", &config->zoom_range.min, &config->zoom_range.max);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[zoom] value is invalid\n");
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "protocol");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[protocol] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "PELCO_P") == 0) {
            config->protocol = BV_PTZ_PROTO_PELCO_P;
        } else if (bv_strcasecmp(localctx->value, "PELCO_D") == 0) {
            config->protocol = BV_PTZ_PROTO_PELCO_D;
        } else if (bv_strcasecmp(localctx->value, "SAMSUNG") == 0) {
            config->protocol = BV_PTZ_PROTO_SAMSUNG;
        } else if (bv_strcasecmp(localctx->value, "VISCA") == 0) {
            config->protocol = BV_PTZ_PROTO_VISCA;
        } else if (bv_strcasecmp(localctx->value, "YAAN") == 0) {
            config->protocol = BV_PTZ_PROTO_YAAN;
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem2, "rs232");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[rs232] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "data_bits");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[data_bits] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->rs485.data_bits = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "baud_rate");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[baud_rate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->rs485.baud_rate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }
    
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "stop_bit");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[stop_bit] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->rs485.stop_bit = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "parity");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[parity] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->rs485.parity = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "flow_control");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[flow_control] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->rs485.flow_control = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb2);
    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_video_source_device(BVConfigContext *h, int index, BVVideoSourceDevice *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_source_devices");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_source_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_source_devices] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "interface");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[interface] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->interface, localctx->value, sizeof(config->interface));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "work_mode");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[work_mode] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->work_mode, localctx->value, sizeof(config->work_mode));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_audio_source_device(BVConfigContext *h, int index, BVAudioSourceDevice *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_source_devices");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_source_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_source_devices] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }
    
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "channels");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[channels] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->channel_counts = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "sample_format");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample_format] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->sample_format = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "sample_rate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample_rate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->sample_rate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "work_mode");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[work_mode] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->work_mode, localctx->value, sizeof(config->work_mode));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_video_output_device(BVConfigContext *h, int index, BVVideoOutputDevice *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_output_devices");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_output_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_output_devices] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "interface");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[interface] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->interface, localctx->value, sizeof(config->interface));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "work_mode");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[work_mode] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->work_mode, localctx->value, sizeof(config->work_mode));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "display");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[display] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "left");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[left] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->display.x = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "top");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[top] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->display.y = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "width");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[width] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->display.width = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "height");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[height] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->display.height = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb2);
    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_audio_output_device(BVConfigContext *h, int index, BVAudioOutputDevice *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_output_devices");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_output_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_output_devices] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "channels");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[channels] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->channel_counts = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "sample_format");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample_format] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->sample_format = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "sample_rate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[sample_rate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->sample_format = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "work_mode");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[work_mode] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->sample_format = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_video_source(BVConfigContext *h, int index, BVVideoSource *config)
{
    int ret = 0;
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "video_sources");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_sources] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_sources] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "framerate");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[framerate] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->framerate = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "day_to_night");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[day_to_night] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        config->day_capture.date_time.second = 0;
        ret = sscanf(localctx->value, "%d:%d", &config->day_capture.date_time.hour, &config->day_capture.date_time.minute);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[day_to_night] value is invalid\n");
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "night_to_day");
    if (!localctx->memb){
        bv_log(h, BV_LOG_ERROR, "get member[night_to_day] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        config->night_capture.date_time.second = 0;
        ret = sscanf(localctx->value, "%d:%d", &config->night_capture.date_time.hour, &config->night_capture.date_time.minute);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[night_to_day] value is invalid\n");
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "capture_rect");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[capture_rect] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "left");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[left] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->bounds.x = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "top");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[top] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->bounds.y = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "width");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[width] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->bounds.width = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->memb, "height");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[height] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->bounds.height = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "imaging_setting");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[imaging_setting] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[imaging_setting] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get member[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "vision_control");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[vision_control] error\n");
        return BVERROR(EINVAL);
    }
    localctx->elem2 = bv_config_get_element(h->pdb, localctx->memb, 0);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[0] error\n");
        return BVERROR(EINVAL);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "luminance");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[luminance] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->day_capture.imaging.luminance = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "contrast");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[contrast] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->day_capture.imaging.contrast = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "hue");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[hue] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->day_capture.imaging.hue = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }
    
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "satuature");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[satuature] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->day_capture.imaging.saturation = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "sharpness");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[sharpness] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->day_capture.imaging.sharpness = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->elem2 = bv_config_get_element(h->pdb, localctx->memb, 1);
    if (!localctx->elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[1] error\n");
        return BVERROR(EINVAL);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "luminance");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[luminance] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->night_capture.imaging.luminance = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "contrast");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[contrast] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->night_capture.imaging.contrast = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "hue");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[hue] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->night_capture.imaging.hue = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }
    
    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "satuature");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[satuature] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->night_capture.imaging.saturation = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    localctx->memb2 = bv_config_get_member(h->pdb, localctx->elem2, "sharpness");
    if (!localctx->memb2) {
        bv_log(h, BV_LOG_ERROR, "get member[sharpness] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb2, &localctx->lvalue);
        config->night_capture.imaging.sharpness = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb2->type, localctx->lvalue);
    }

    bv_config_object_decref(h->pdb, localctx->memb2);
    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem2);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_audio_source(BVConfigContext *h, int index, BVAudioSource *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_sources");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_sources] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_sources] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "token");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[token] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->token, localctx->value, sizeof(config->token));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "channels");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[channels] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, &localctx->lvalue);
        config->channels = localctx->lvalue;
        bv_log(h, BV_LOG_DEBUG, "member type %d value %lld\n", localctx->memb->type, localctx->lvalue);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "input_type");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[channels] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "mic") == 0) {
            config->input_type = BV_AUDIO_INPUT_TYPE_MIC;
        } else if (bv_strcasecmp(localctx->value, "line_in") == 0) {
            config->input_type = BV_AUDIO_INPUT_TYPE_LINE_IN;
        }
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

static int local_get_media_device(BVConfigContext *h, int index, BVMediaDevice *config)
{
    LocalContext *localctx = h->priv_data;
    localctx->obj = bv_config_get_member(h->pdb, h->pdb->root, "media_devices");
    if (!localctx->obj) {
        bv_log(h, BV_LOG_ERROR, "get member[media_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[media_devices] type %d\n", localctx->obj->type);
    }
    localctx->elem = bv_config_get_element(h->pdb, localctx->obj, index);
    if (!localctx->elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        return BVERROR(EINVAL);
    }
    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "name");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[name] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        bv_strlcpy(config->name, localctx->value, sizeof(config->name));
        bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
    }

    localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "type");
    if (!localctx->memb) {
        bv_log(h, BV_LOG_ERROR, "get member[type] error\n");
    } else {
        bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
        if (bv_strcasecmp(localctx->value, "onvif") == 0) {
            config->type = BV_MEDIA_DEVICE_TYPE_IPC;
            config->devinfo = bv_mallocz(sizeof(BVMobileDevice));
            localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "url");
            if (!localctx->memb) {
                bv_log(h, BV_LOG_ERROR, "get member[url] error\n");
            } else {
                bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
                bv_strlcpy(((BVMobileDevice *)(config->devinfo))->url, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->url));
                bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
            }
            
            localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "user");
            if (!localctx->memb) {
                bv_log(h, BV_LOG_ERROR, "get member[user] error\n");
            } else {
                bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
                bv_strlcpy(((BVMobileDevice *)(config->devinfo))->user, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->user));
                bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
            }

            localctx->memb = bv_config_get_member(h->pdb, localctx->elem, "passwd");
            if (!localctx->memb) {
                bv_log(h, BV_LOG_ERROR, "get_member[passwd] error\n");
            } else {
                bv_config_object_get_value(h->pdb, localctx->memb, localctx->value);
                bv_strlcpy(((BVMobileDevice *)(config->devinfo))->pswd, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->pswd));
                bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
            }

        } else if (bv_strcasecmp(localctx->value, "cvbs") == 0) {
            config->type = BV_MEDIA_DEVICE_TYPE_CVBS;
            config->devinfo = NULL;
            bv_log(h, BV_LOG_DEBUG, "member type %d value %s\n", localctx->memb->type, localctx->value);
        }
    }

    bv_config_object_decref(h->pdb, localctx->memb);
    bv_config_object_decref(h->pdb, localctx->elem);
    bv_config_object_decref(h->pdb, localctx->obj);
    return 0;
}

#define OFFSET(x) offsetof(LocalContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass local_class = {
    .class_name     = "local config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfig bv_local_config = {
    .name                       = "local",
    .type                       = BV_CONFIG_TYPE_LOCAL,
    .priv_data_size             = sizeof(LocalContext),
    .priv_class                 = &local_class,
    .config_probe               = local_probe,
	.config_open                = local_open,
	.config_close               = local_close,
    .get_device_info            = local_get_device_info,
    .get_video_encoder          = local_get_video_encoder,
    .set_video_encoder          = local_set_video_encoder,
    .get_video_encoder_options  = local_get_video_encoder_options,
    .get_audio_encoder          = local_get_audio_encoder,
    .set_audio_encoder          = local_set_audio_encoder,
    .get_audio_encoder_options  = local_get_audio_encoder_options,
    .get_ptz_device             = local_get_ptz_device,
    .get_media_device           = local_get_media_device,
    .get_video_source_device    = local_get_video_source_device,
    .get_audio_source_device    = local_get_audio_source_device,
    .get_video_output_device    = local_get_video_output_device,
    .get_audio_output_device    = local_get_audio_output_device,
    .get_video_source           = local_get_video_source,
    .get_audio_source           = local_get_audio_source,
};
