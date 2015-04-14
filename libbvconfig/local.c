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
}LocalContext;


//warning: Z is string;
//         N is integer.
#define GET_VALUE(X, P, Y, Z, N)     \
        if (!X || !Y) return BVERROR(EINVAL);  \
        P = bv_config_get_member(h->pdb, X, Y);   \
        if (!P) {    \
            bv_log(h, BV_LOG_ERROR, "get member[%s] error\n", Y); \
        } else {     \
            if (Z) {  \
                bv_config_object_get_value(h->pdb, P, localctx->value);  \
                bv_strlcpy(Z, localctx->value, sizeof(Z));  \
                bv_log(h, BV_LOG_DEBUG, "get member type %d value %s\n", P->type, localctx->value);  \
            } else {  \
                bv_config_object_get_value(h->pdb, P, &localctx->lvalue);  \
                N = localctx->lvalue;  \
                bv_log(h, BV_LOG_DEBUG, "get member type %d value %lld\n", P->type, localctx->lvalue);  \
            }  \
            bv_config_object_decref(h->pdb, P);   \
        }

#define SET_VALUE(X, P, Y, Z, N)     \
        if (!X || !Y) return BVERROR(EINVAL);  \
        P = bv_config_get_member(h->pdb, X, Y);   \
        if (!P) {    \
            bv_log(h, BV_LOG_ERROR, "get member[%s] error\n", Y); \
        } else {     \
            if (Z) {  \
                bv_strlcpy(localctx->value, Z, sizeof(Z));   \
                bv_config_object_set_value(h->pdb, P, localctx->value);  \
                bv_log(h, BV_LOG_DEBUG, "set member type %d value %s\n", P->type, localctx->value);  \
            } else {  \
                localctx->lvalue = N;  \
                bv_config_object_set_value(h->pdb, P, &localctx->lvalue);  \
                bv_log(h, BV_LOG_DEBUG, "set member type %d value %lld\n", P->type, localctx->lvalue);  \
            }   \
            bv_config_object_decref(h->pdb, P);   \
        }

static int tmp = 0;
static BVConfigObject *memb = NULL;

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
    bv_log(h, BV_LOG_DEBUG, "open config, return [%d]\n", ret);
    return ret;
}

static int local_close(BVConfigContext *h)
{
    int ret = 0;
    bv_log(h, BV_LOG_DEBUG, "close config, return [%d]\n", ret);
    return ret;
}

static int local_get_device_info(BVConfigContext *h, BVDeviceInfo *devinfo)
{
    BVConfigObject *obj = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "device_info");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[device_info] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[device_info] type %d\n", obj->type);
    }

    GET_VALUE(obj, memb, "device_model", devinfo->device_model, tmp);
    GET_VALUE(obj, memb, "device_type", devinfo->device_type, tmp);
    GET_VALUE(obj, memb, "software_version", devinfo->software_version, tmp);
    GET_VALUE(obj, memb, "hardware_version", devinfo->hardware_version, tmp);
    GET_VALUE(obj, memb, "hardware_model", devinfo->hardware_model, tmp);
    GET_VALUE(obj, memb, "manufacturer_id", devinfo->manufacturer_id, tmp);
    GET_VALUE(obj, memb, "manufacturer", devinfo->manufacturer_name, tmp);
    GET_VALUE(obj, memb, "device_id", devinfo->device_id, tmp);
    GET_VALUE(obj, memb, "device_name", devinfo->device_name, tmp);
    GET_VALUE(obj, memb, "wifi_count", NULL, devinfo->wifi_count);
    GET_VALUE(obj, memb, "wireless_count", NULL, devinfo->wireless_count);
    GET_VALUE(obj, memb, "channel_count", NULL, devinfo->channel_count);
    GET_VALUE(obj, memb, "video_source_devices", NULL, devinfo->video_source_devices);
    GET_VALUE(obj, memb, "video_output_devices", NULL, devinfo->video_output_devices);
    GET_VALUE(obj, memb, "audio_source_devices", NULL, devinfo->audio_source_devices);
    GET_VALUE(obj, memb, "audio_output_devices", NULL, devinfo->audio_output_devices);
    GET_VALUE(obj, memb, "video_source", NULL, devinfo->video_sources);
    GET_VALUE(obj, memb, "video_outputs", NULL, devinfo->video_outputs);
    GET_VALUE(obj, memb, "audio_sources", NULL, devinfo->audio_sources);
    GET_VALUE(obj, memb, "audio_outputs", NULL, devinfo->audio_outputs);
    GET_VALUE(obj, memb, "serial_ports", NULL, devinfo->serial_ports);
    GET_VALUE(obj, memb, "alert_in_count", NULL, devinfo->alert_in_count);
    GET_VALUE(obj, memb, "alert_out_count", NULL, devinfo->alert_out_count);
    GET_VALUE(obj, memb, "storage_count", NULL, devinfo->storage_count);
    GET_VALUE(obj, memb, "relay_outputs", NULL, devinfo->relay_outputs);
    GET_VALUE(obj, memb, "ptz_count", NULL, devinfo->ptz_count);
    GET_VALUE(obj, memb, "gps_count", NULL, devinfo->gps_count);
    GET_VALUE(obj, memb, "support_sms", NULL, devinfo->support_sms);
    GET_VALUE(obj, memb, "support_call", NULL, devinfo->support_call);
    GET_VALUE(obj, memb, "preset_count", NULL, devinfo->preset_count);
    GET_VALUE(obj, memb, "cruise_count", NULL, devinfo->cruise_count);
    GET_VALUE(obj, memb, "temperature_sensor", NULL, devinfo->temperature_count);
    GET_VALUE(obj, memb, "voltage_count", NULL, devinfo->voltage_count);
    GET_VALUE(obj, memb, "speed_count", NULL, devinfo->speed_count);

    if (obj) 
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_encoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem2, memb,"token", config->token, tmp);
    GET_VALUE(elem2, memb, "width", NULL, config->codec_context.width);
    GET_VALUE(elem2, memb, "height", NULL, config->codec_context.height);
    GET_VALUE(elem2, memb, "quality", NULL, config->codec_context.quality);
    GET_VALUE(elem2, memb, "gop", NULL, config->codec_context.gop_size);
    GET_VALUE(elem2, memb, "bitrate", NULL, config->codec_context.bit_rate);
    GET_VALUE(elem2, memb, "framerate", NULL, config->codec_context.time_base.den);
    config->codec_context.time_base.num = 1;

    GET_VALUE(elem2, memb, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "H264") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_H264;
    } else if (bv_strcasecmp(localctx->value, "MPEG") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_MPEG;
    } else if (bv_strcasecmp(localctx->value, "JPEG") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_JPEG;
    }

    GET_VALUE(elem2, memb, "rate_control", localctx->value, tmp);
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

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_set_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "set member[video_encoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    switch (config->codec_context.codec_id) {
        case BV_CODEC_ID_H264:
        {
            SET_VALUE(elem2, memb, "encoding", "H264", tmp);
            break;
        }
        case BV_CODEC_ID_MPEG:
        {
            SET_VALUE(elem2, memb, "encoding", "MPEG", tmp);
            break;
        }
        case BV_CODEC_ID_JPEG:
        {
            SET_VALUE(elem2, memb, "encoding", "JPEG", tmp);
            break;
        }
    }

    SET_VALUE(elem2, memb, "token", config->token, tmp);
    SET_VALUE(elem2, memb, "width", NULL, config->codec_context.width);
    SET_VALUE(elem2, memb, "height", NULL, config->codec_context.height);
    SET_VALUE(elem2, memb, "quality", NULL, config->codec_context.quality);
    SET_VALUE(elem2, memb, "gop", NULL, config->codec_context.gop_size);
    SET_VALUE(elem2, memb, "bitrate", NULL, config->codec_context.bit_rate);

    switch (config->codec_context.mode_id) {
        case BV_RC_MODE_ID_VBR:
        {
            SET_VALUE(elem2, memb, "rate_control", "VBR", tmp);
            break;
        }
        case BV_RC_MODE_ID_CBR:
        {
            SET_VALUE(elem2, memb, "rate_control", "CBR", tmp);
            break;
        }
        case BV_RC_MODE_ID_ABR:
        {
            SET_VALUE(elem2, memb, "rate_control", "ABR", tmp);
            break;
        }
        case BV_RC_MODE_ID_FIXQP:
        {
            SET_VALUE(elem2, memb, "rate_control", "FIXQP", tmp);
            break;
        }
        case BV_RC_MODE_ID_BUTT:
        {
            SET_VALUE(elem2, memb, "rate_control", "BUTT", tmp);
            break;
        }   
    }
    
    if (config->codec_context.time_base.num == 0) {
        config->codec_context.time_base.den = 1;
        config->codec_context.time_base.num = 25;
    }
    SET_VALUE(elem2, memb, "framerate", NULL, config->codec_context.time_base.den / config->codec_context.time_base.num);

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int get_value_video_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVVideoOption *options)
{
    int i = 0;
    int ret = 0;
    char *ptr = NULL;
    char *saveptr = NULL;
    char string[512] = { 0 };
    LocalContext *localctx = h->priv_data;

    GET_VALUE(elem, memb, "framerate", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%lld..%lld]", &options->framerate_range.min, &options->framerate_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[framerate] value is invalid\n");
    }

    GET_VALUE(elem, memb, "gop", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%lld..%lld]", &options->gop_range.min, &options->gop_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[gop] value is invalid\n");
    }

    GET_VALUE(elem, memb, "bitrate", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%lld..%lld]", &options->bitrate_range.min, &options->bitrate_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[bitrate] value is invalid\n");
    }
    
    GET_VALUE(elem, memb, "nb_resolutions", NULL, options->nb_resolutions);
    if (options->nb_resolutions <= 0) {
        bv_log(h, BV_LOG_ERROR, "member[nb_resolutions] value is invalid\n");
        return BVERROR(EINVAL);
    }
    options->resolutions = bv_mallocz(sizeof(BVVideoResolution) * options->nb_resolutions);
    if (!options->resolutions) {
        bv_log(h, BV_LOG_ERROR, "malloc failed\n");
        return BVERROR(EINVAL);
    }
    GET_VALUE(elem, memb, "resolutions", localctx->value, tmp);
    ret = sscanf(localctx->value, "(%[^)])", string);
    if (ret != 1) {
        bv_log(h, BV_LOG_ERROR, "member[resolutions] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        i = 1;
        ret = sscanf(ptr, "%dx%d", &(options->resolutions)[0].width, &(options->resolutions)[0].height);
        if (ret != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[resolutions] value is invalid\n");
        }
    } else {
        for (i = 0; i < options->nb_resolutions; i++) {
            ret = sscanf(ptr, "%dx%d", &(options->resolutions)[i].width, &(options->resolutions)[i].height);
            if (ret != 2) {
                bv_log(h, BV_LOG_ERROR, "get member[resolutions] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }
    return 0;
}

static int get_encoding_video_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVVideoEncoderOption *config)
{
    LocalContext *localctx = h->priv_data; 

    if (bv_strcasecmp(localctx->value, "H264") == 0) {
        config->h264 = bv_mallocz(sizeof(BVVideoOption));
        if (!config->h264) {
            bv_log(h, BV_LOG_ERROR, "h264 malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->h264->codec_id = BV_CODEC_ID_H264;
        get_value_video_encoder_options(h, elem, config->h264);
    } else if (bv_strcasecmp(localctx->value, "MPEG") == 0) {
        config->mpeg = bv_mallocz(sizeof(BVVideoOption));
        if (!config->mpeg) {
            bv_log(h, BV_LOG_ERROR, "mpeg malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->mpeg->codec_id = BV_CODEC_ID_MPEG;
        get_value_video_encoder_options(h, elem, config->mpeg);
    } else if (bv_strcasecmp(localctx->value, "JPEG") == 0) {
        config->jpeg = bv_mallocz(sizeof(BVVideoOption));
        if (!config->jpeg) {
            bv_log(h, BV_LOG_ERROR, "jpeg malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->jpeg->codec_id = BV_CODEC_ID_JPEG;
        get_value_video_encoder_options(h, elem, config->jpeg);
    }
    return 0;
}

static int local_get_video_encoder_options(BVConfigContext *h, int channel, int index, BVVideoEncoderOption *config)
{
    int i = 0;
    int max = 0;
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *options = NULL;
    BVConfigObject *elem3 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoder_options");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_encoder_options] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_encoder_options] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem2, memb, "token", config->token, tmp);
    GET_VALUE(elem2, memb, "quality", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%lld..%lld]", &config->quality.min, &config->quality.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[quality] value is invalid\n");
    }

    options = bv_config_get_member(h->pdb, elem2, "options");
    if (!options) {
        bv_log(h, BV_LOG_ERROR, "get member[options] error\n");
        goto error;
    }
    max = bv_config_get_elements(h->pdb, options);
    for (i = 0; i < max; i++) {
        elem3 = bv_config_get_element(h->pdb, options, i);
        if (!elem3) {
            bv_log(h, BV_LOG_ERROR, "get element[%d] error\n", i);
            break;
        }
        GET_VALUE(elem3, memb, "encoding", localctx->value, tmp);
        get_encoding_video_encoder_options(h, elem3, config);
        bv_config_object_decref(h->pdb, elem3);
    }
    
error:
    if (options)
        bv_config_object_decref(h->pdb, options);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int get_value_audio_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVAudioEncoderOption *config)
{
    int i = 0;
    int max = 0;
    int ret = 0;
    char *ptr = NULL;
    char *saveptr = NULL;
    char string[512] = { 0 };
    char string2[512] = { 0 };
    LocalContext *localctx = h->priv_data;

    GET_VALUE(elem, memb, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "G711A") == 0) {
        config->options->codec_id = BV_CODEC_ID_G711A;
    } else if (bv_strcasecmp(localctx->value, "G711U") == 0) {
        config->options->codec_id = BV_CODEC_ID_G711U;
    } else if (bv_strcasecmp(localctx->value, "G726") == 0) {
        config->options->codec_id = BV_CODEC_ID_G726;
    } else if (bv_strcasecmp(localctx->value, "AAC") == 0) {
        config->options->codec_id = BV_CODEC_ID_AAC;
    }

    GET_VALUE(elem, memb, "bitrate", localctx->value, tmp);
    ret = sscanf(localctx->value, "(%[^)])", string);
    if (ret != 1) {
        bv_log(h, BV_LOG_ERROR, "member[bitrate] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    bv_strlcpy(string2, string, sizeof(string2));
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        max = 1;
    } else {
        do {
            ++max;
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        } while(1);
    }
    config->options->bitrate_list.nb_int = max;
    config->options->bitrate_list.items = bv_mallocz(sizeof(int64_t) * max);
    ptr = bv_strtok(string2, " ", &saveptr);
    if (!ptr) {
        bv_log(h, BV_LOG_ERROR, "bitrate copy string is wrong\n");
    } else {
        for (i = 0; i < config->options->bitrate_list.nb_int; i++) {
            ret = sscanf(ptr, "%lld", &config->options->bitrate_list.items[i]);
            if (ret != 1) {
                bv_log(h, BV_LOG_ERROR, "get member[bitrate] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }

    max = 0;
    GET_VALUE(elem, memb, "sample_rate", localctx->value, tmp);
    ret = sscanf(localctx->value, "(%[^)])", string);
    if (ret != 1) {
        bv_log(h, BV_LOG_ERROR, "member[bitrate] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    bv_strlcpy(string2, string, sizeof(string2));
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        max = 1;
    } else {
        do {
            ++max;
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        } while(1);
    }
    config->options->sample_rate_list.nb_int = max;
    config->options->sample_rate_list.items = bv_mallocz(sizeof(int64_t) * max);
    ptr = bv_strtok(string2, " ", &saveptr);
    if (!ptr) {
        bv_log(h, BV_LOG_ERROR, "sample_rate copy string is wrong\n");
    } else {
        for (i = 0; i < config->options->sample_rate_list.nb_int; i++) {
            ret = sscanf(ptr, "%lld", &config->options->sample_rate_list.items[i]);
            if (ret != 1) {
                bv_log(h, BV_LOG_ERROR, "get member[sample_rate] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }
    return 0;
}


static int local_get_audio_encoder_options(BVConfigContext *h, int channel, int index, BVAudioEncoderOption *config)
{
    int i = 0;
    int max = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *options = NULL;
    BVConfigObject *elem3 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoder_options");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoder_options] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_encoder_options] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    } 

    GET_VALUE(elem2, memb, "token", config->token, tmp);

    options = bv_config_get_member(h->pdb, elem2, "options");
    if (!options) {
        bv_log(h, BV_LOG_ERROR, "get member[options] error\n");
        goto error;
    }
    max = bv_config_get_elements(h->pdb, options);
    config->nb_options = max;
    config->options = bv_mallocz(sizeof(BVAudioOption) * config->nb_options);
    for (i = 0; i < max; i++) {
        elem3 = bv_config_get_element(h->pdb, options, i);
        if (!elem3) {
            bv_log(h, BV_LOG_ERROR, "get element[%d] error\n", i);
            break;
        }
        get_value_audio_encoder_options(h, elem3, config);
        bv_config_object_decref(h->pdb, elem3);
    }

error:
    if (options) 
        bv_config_object_decref(h->pdb, options);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_encoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem2, memb, "token", config->token, tmp);
    GET_VALUE(elem2, memb, "bitrate", NULL, config->codec_context.bit_rate);
    GET_VALUE(elem2, memb, "sample_rate", NULL, config->codec_context.sample_rate);

    GET_VALUE(elem2, memb, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "G711A") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G711A;
    } else if (bv_strcasecmp(localctx->value,"G711U") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G711U;
    } else if (bv_strcasecmp(localctx->value, "G726") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G726;
    } else if (bv_strcasecmp(localctx->value, "AAC") == 0)  {
        config->codec_context.codec_id = BV_CODEC_ID_AAC;
    }

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_set_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "set member[audio_encoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    switch (config->codec_context.codec_id) {
        case BV_CODEC_ID_G711A:
        {
            SET_VALUE(elem2, memb, "encoding", "G711A", tmp);
            break;
        }
        case BV_CODEC_ID_G711U:
        {
            SET_VALUE(elem2, memb, "encoding", "G711U", tmp);
            break;
        }
        case BV_CODEC_ID_G726:
        {
            SET_VALUE(elem2, memb, "encoding", "G726", tmp);
            break;
        }
        case BV_CODEC_ID_AAC:
        {
            SET_VALUE(elem2, memb, "encoding", "AAC", tmp);
            break;
        }
    }

    SET_VALUE(elem2, memb, "token", config->token, tmp);
    SET_VALUE(elem2, memb, "bitrate", NULL, config->codec_context.bit_rate);
    SET_VALUE(elem2, memb, "sample_rate", NULL, config->codec_context.sample_rate);

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_ptz_device(BVConfigContext *h, int channel, int index, BVPTZDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *rs232 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "PTZDevice");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[PTZDevice] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[PTZDevice] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, channel);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[channel] error\n");
        goto error;
    }
    elem2 = bv_config_get_element(h->pdb, elem, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem2, memb, "url", config->token, tmp);
    GET_VALUE(elem2, memb, "pan", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%f..%f]", &config->pan_range.min, &config->pan_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[pan] value is invalid\n");
    }

    GET_VALUE(elem2, memb, "tilt", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%f..%f]", &config->tilt_range.min, &config->tilt_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[tilt] value is invalid\n");
    }

    GET_VALUE(elem2, memb, "zoom", localctx->value, tmp);
    ret = sscanf(localctx->value, "[%f..%f]", &config->zoom_range.min, &config->zoom_range.max);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[zoom] value is invalid\n");
    }

    GET_VALUE(elem2, memb, "protocol", localctx->value, tmp);
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

    rs232 = bv_config_get_member(h->pdb, elem2, "rs232");
    if (!rs232) {
        bv_log(h, BV_LOG_ERROR, "get member[rs232] error\n");
        goto error;
    }
    GET_VALUE(rs232, memb, "data_bits", NULL, config->rs485.data_bits);
    GET_VALUE(rs232, memb, "baud_rate", NULL, config->rs485.baud_rate);
    GET_VALUE(rs232, memb, "stop_bit", NULL, config->rs485.stop_bit);
    GET_VALUE(rs232, memb, "parity", NULL, config->rs485.parity);
    GET_VALUE(rs232, memb, "flow_control", NULL, config->rs485.flow_control);

error:
    if (rs232)
        bv_config_object_decref(h->pdb, rs232);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_video_source_device(BVConfigContext *h, int index, BVVideoSourceDevice *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_source_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_source_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_source_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "chip", config->chip, tmp);
    GET_VALUE(elem, memb, "dev", config->dev, tmp);
    GET_VALUE(elem, memb, "interface", config->interface, tmp);
    GET_VALUE(elem, memb, "work_mode", config->work_mode, tmp);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_audio_source_device(BVConfigContext *h, int index, BVAudioSourceDevice *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_source_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_source_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_source_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "work_mode", config->work_mode, tmp);
    GET_VALUE(elem, memb, "chip", config->chip, tmp);
    GET_VALUE(elem, memb, "dev", config->dev, tmp);
    GET_VALUE(elem, memb, "channel_mode", NULL, config->channel_mode);
    GET_VALUE(elem, memb, "channels", NULL, config->channel_counts);
    GET_VALUE(elem, memb, "sample_format", NULL, config->sample_format);
    GET_VALUE(elem, memb, "sample_rate", NULL, config->sample_rate);
    GET_VALUE(elem, memb, "sample_points", NULL, config->sample_points);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_video_output_device(BVConfigContext *h, int index, BVVideoOutputDevice *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *display = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_output_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_output_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_output_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "chip", config->chip, tmp);
    GET_VALUE(elem, memb, "dev", config->dev, tmp);
    GET_VALUE(elem, memb, "interface", config->interface, tmp);
    GET_VALUE(elem, memb, "work_mode", config->work_mode, tmp);

    display = bv_config_get_member(h->pdb, elem, "display");
    if (!display) {
        bv_log(h, BV_LOG_ERROR, "get member[display] error\n");
        goto error;
    }
    GET_VALUE(display, memb, "left", NULL, config->display.x);
    GET_VALUE(display, memb, "top", NULL, config->display.y);
    GET_VALUE(display, memb, "width", NULL, config->display.width);
    GET_VALUE(display, memb, "height", NULL, config->display.height);

error:
    if (display)
        bv_config_object_decref(h->pdb, display);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_audio_output_device(BVConfigContext *h, int index, BVAudioOutputDevice *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_output_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_output_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_output_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "chip", config->chip, tmp);
    GET_VALUE(elem, memb, "dev", config->dev, tmp);
    GET_VALUE(elem, memb, "work_mode", config->work_mode, tmp);
    GET_VALUE(elem, memb, "channels_mode", NULL, config->channel_mode);
    GET_VALUE(elem, memb, "channels", NULL, config->channel_counts);
    GET_VALUE(elem, memb, "sample_format", NULL, config->sample_format);
    GET_VALUE(elem, memb, "sample_rate", NULL, config->sample_rate);
    GET_VALUE(elem, memb, "sample_points", NULL, config->sample_points);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_video_source(BVConfigContext *h, int index, BVVideoSource *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *capture = NULL;
    BVConfigObject *imaging = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *vision = NULL;
    BVConfigObject *elem3 = NULL;
    BVConfigObject *elem4 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_sources");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_sources] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_sources] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "video_source_device", NULL, config->video_source_device);
    GET_VALUE(elem, memb, "framerate", NULL, config->framerate);
    GET_VALUE(elem, memb, "day_to_night", localctx->value, tmp);
    config->day_capture.date_time.second = 0;
    ret = sscanf(localctx->value, "%d:%d", &config->day_capture.date_time.hour, &config->day_capture.date_time.minute);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[day_to_night] value is invalid\n");
    }

    GET_VALUE(elem, memb, "night_to_day", localctx->value, tmp);
    config->night_capture.date_time.second = 0;
    ret = sscanf(localctx->value, "%d:%d", &config->night_capture.date_time.hour, &config->night_capture.date_time.minute);
    if (ret != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[night_to_day] value is invalid\n");
    }

    capture = bv_config_get_member(h->pdb, elem, "capture_rect");
    if (!capture) {
        bv_log(h, BV_LOG_ERROR, "get member[capture_rect] error\n");
        goto error;
    }
    GET_VALUE(capture, memb, "left", NULL, config->bounds.x);
    GET_VALUE(capture, memb, "top", NULL, config->bounds.y);
    GET_VALUE(capture, memb, "width", NULL, config->bounds.width);
    GET_VALUE(capture, memb, "height", NULL, config->bounds.height);

    imaging = bv_config_get_member(h->pdb, h->pdb->root, "imaging_setting");
    if (!imaging) {
        bv_log(h, BV_LOG_ERROR, "get member[imaging_setting] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[imaging_setting] type %d\n", obj->type);
    }
    elem2 = bv_config_get_element(h->pdb, imaging, index);
    if (!elem2) {
        bv_log(h, BV_LOG_ERROR, "get member[index] error\n");
        goto error;
    }
    vision = bv_config_get_member(h->pdb, elem2, "vision_control");
    if (!vision) {
        bv_log(h, BV_LOG_ERROR, "get member[vision_control] error\n");
        goto error;
    }
    elem3 = bv_config_get_element(h->pdb, vision, 0);
    if (!elem3) {
        bv_log(h, BV_LOG_ERROR, "get element[0] error\n");
        goto error;
    }
    GET_VALUE(elem3, memb, "luminance", NULL, config->day_capture.imaging.luminance);
    GET_VALUE(elem3, memb, "contrast", NULL, config->day_capture.imaging.contrast);
    GET_VALUE(elem3, memb, "hue", NULL, config->day_capture.imaging.hue);
    GET_VALUE(elem3, memb, "saturation", NULL, config->day_capture.imaging.saturation);
    GET_VALUE(elem3, memb, "sharpness", NULL, config->day_capture.imaging.sharpness);

    elem4 = bv_config_get_element(h->pdb, vision, 1);
    if (!elem4) {
        bv_log(h, BV_LOG_ERROR, "get element[1] error\n");
        goto error;
    }
    GET_VALUE(elem4, memb, "luminance", NULL, config->night_capture.imaging.luminance);
    GET_VALUE(elem4, memb, "contrast", NULL, config->day_capture.imaging.contrast);
    GET_VALUE(elem4, memb, "hue", NULL, config->day_capture.imaging.hue);
    GET_VALUE(elem4, memb, "saturation", NULL, config->day_capture.imaging.saturation);
    GET_VALUE(elem4, memb, "sharpness", NULL, config->day_capture.imaging.sharpness);

error:
    if (elem4)
        bv_config_object_decref(h->pdb, elem4);
    if (elem3)
        bv_config_object_decref(h->pdb, elem3);
    if (vision)
        bv_config_object_decref(h->pdb, vision);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (imaging)
        bv_config_object_decref(h->pdb, imaging);
    if (capture)
        bv_config_object_decref(h->pdb, capture);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_audio_source(BVConfigContext *h, int index, BVAudioSource *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_sources");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_sources] error\n");
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_sources] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "audio_source_device", NULL, config->audio_source_device);
    GET_VALUE(elem, memb, "channels", NULL, config->channels);
    GET_VALUE(elem, memb, "input_type", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "mic") == 0) {
        config->input_type = BV_AUDIO_INPUT_TYPE_MIC;
    } else if (bv_strcasecmp(localctx->value, "line_in") == 0) {
        config->input_type = BV_AUDIO_INPUT_TYPE_LINE_IN;
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_video_output(BVConfigContext *h, int index, BVVideoOutput *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *display = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_outputs");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[video_outputs] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[video_outputs] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "video_output_device", NULL, config->video_output_device);

    display = bv_config_get_member(h->pdb, elem, "display");
    if (!display) {
        bv_log(h, BV_LOG_ERROR, "get member[display] error\n");
        goto error;
    }
    GET_VALUE(display, memb, "left", NULL, config->display.x);
    GET_VALUE(display, memb, "top", NULL, config->display.y);
    GET_VALUE(display, memb, "width", NULL, config->display.width);
    GET_VALUE(display, memb, "height", NULL, config->display.height);


error:
    if (display)
        bv_config_object_decref(h->pdb, display);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_audio_output(BVConfigContext *h, int index, BVAudioOutput *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_outputs");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[audio_outputs] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[audio_outputs] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "token", config->token, tmp);
    GET_VALUE(elem, memb, "volume", NULL, config->volume);
    GET_VALUE(elem, memb, "audio_output_device", NULL, config->audio_output_device);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_media_encoder(BVConfigContext *h, int index, BVMediaEncoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_encoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[media_encoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[media_encoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get member[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "video_source", NULL, config->video_source);
    GET_VALUE(elem, memb, "audio_source", NULL, config->audio_source);
    GET_VALUE(elem, memb, "video_channel", NULL, config->video_channel);
    GET_VALUE(elem, memb, "audio_channel", NULL, config->audio_channel);
    GET_VALUE(elem, memb, "storage_index", NULL, config->storage_index);
    GET_VALUE(elem, memb, "transfer_index", NULL, config->transfer_index);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_media_decoder(BVConfigContext *h, int index, BVMediaDecoder *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_decoders");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[media_decoders] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[media_decoders] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "video_output", NULL, config->video_output);
    GET_VALUE(elem, memb, "audio_output", NULL, config->audio_output);
    GET_VALUE(elem, memb, "video_channel", NULL, config->video_channel);
    GET_VALUE(elem, memb, "audio_channel", NULL, config->audio_channel);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_talkback(BVConfigContext *h, int index, BVTalkBack *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "talkbacks");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[talkbacks] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[talkbacks] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "media_encoder", NULL, config->media_encoder_index);
    GET_VALUE(elem, memb, "media_decoder", NULL, config->media_decoder_index);

    if ((ret = local_get_media_encoder(h, config->media_encoder_index, &config->media_encoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_encoders] in talk back error, return [%d]\n", ret);
    }
    if ((ret = local_get_media_decoder(h, config->media_decoder_index, &config->media_decoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_decoders] in talk back error, return [%d]\n", ret);
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_get_media_device(BVConfigContext *h, int index, BVMediaDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[media_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "get member[media_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }

    GET_VALUE(elem, memb, "name", config->name, tmp);
    GET_VALUE(elem, memb, "video_encode_type", NULL, config->video_encode_type);
    GET_VALUE(elem, memb, "audio_encode_type", NULL, config->audio_encode_type);
    GET_VALUE(elem, memb, "video_decode_type", NULL, config->video_decode_type);
    GET_VALUE(elem, memb, "audio_decode_type", NULL, config->audio_decode_type);

    if ((ret = local_get_media_encoder(h, index, &config->media_encoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_encoders] in media device error, return [%d]\n", ret);
    }
    if ((ret = local_get_media_decoder(h, index, &config->media_decoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_decoders] in media device error, return [%d]\n", ret);
    }

    if (config->video_encode_type == BV_MEDIA_STREAM_TYPE_IPC_VIDEO || config->audio_encode_type == BV_MEDIA_STREAM_TYPE_IPC_AUDIO) {
        config->devinfo = bv_mallocz(sizeof(BVMobileDevice));
        if (!config->devinfo) {
            bv_log(h, BV_LOG_ERROR, "malloc failed\n");
            goto error;
        }
        GET_VALUE(elem, memb, "url", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->url, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->url));
            
        GET_VALUE(elem, memb, "user", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->user, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->user));

        GET_VALUE(elem, memb, "passwd", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->pswd, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->pswd));
    
        ((BVMobileDevice *)(config->devinfo))->timeout = 5;
    } else {
        config->devinfo = NULL;
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return 0;
}

static int local_set_media_device(BVConfigContext *h, int index, BVMediaDevice *config)
{
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_devices");
    if (!obj) {
        bv_log(h, BV_LOG_ERROR, "get member[media_devices] error\n");
        return BVERROR(EINVAL);
    } else {
        bv_log(h, BV_LOG_DEBUG, "set member[media_devices] type %d\n", obj->type);
    }
    elem = bv_config_get_element(h->pdb, obj, index);
    if (!elem) {
        bv_log(h, BV_LOG_ERROR, "get element[index] error\n");
        goto error;
    }
    
    SET_VALUE(elem, memb, "name", config->name, tmp);
    if (config->video_encode_type == BV_MEDIA_STREAM_TYPE_IPC_VIDEO || config->audio_encode_type == BV_MEDIA_STREAM_TYPE_IPC_AUDIO) {
        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->user, sizeof(((BVMobileDevice *)(config->devinfo))->user));
        SET_VALUE(elem, memb, "user", localctx->value, tmp);

        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->pswd, sizeof(((BVMobileDevice *)(config->devinfo))->pswd));
        SET_VALUE(elem, memb, "passwd", localctx->value, tmp);

        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->url, sizeof(((BVMobileDevice *)(config->devinfo))->url));
        SET_VALUE(elem, memb, "url", localctx->value, tmp);
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
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
    .set_media_device           = local_set_media_device,
    .get_media_encoder          = local_get_media_encoder,
    .get_media_decoder          = local_get_media_decoder,
    .get_talkback               = local_get_talkback,
    .get_video_source_device    = local_get_video_source_device,
    .get_audio_source_device    = local_get_audio_source_device,
    .get_video_output_device    = local_get_video_output_device,
    .get_audio_output_device    = local_get_audio_output_device,
    .get_video_source           = local_get_video_source,
    .get_audio_source           = local_get_audio_source,
    .get_video_output           = local_get_video_output,
    .get_audio_output           = local_get_audio_output,
};
