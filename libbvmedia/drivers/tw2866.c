/*************************************************************************
    > File Name: tw2866.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月23日 星期一 15时31分02秒
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

#line 25 "tw2866.c"

#include <libbvmedia/bvmedia.h>

#include "tw28xx.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

typedef struct Tw2866Driver {
    const BVClass *bv_class;
    int fd;
} Tw2866Driver;

static int tw2866_driver_open(BVMediaDriverContext *s)
{
    Tw2866Driver *driver = s->priv_data;
    bv_log(s, BV_LOG_DEBUG, "run here tw2866 driver open\n");
    driver->fd = open(s->filename, O_RDONLY);
    if (driver->fd < 0) {
        return BVERROR(EIO);
    }
    return 0;
}

static int audio_source_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tw2866Driver *driver = s->priv_data;
    BVAudioSourceVolume *source_volume = pkt_in->data;
    tw286x_audio_in_volume  involume;
    int aidev, aichn;
    int ret = 0;
    if (sscanf(source_volume->token, "%d/%d", &aidev, &aichn) != 2) {
        bv_log(s, BV_LOG_ERROR, "audio source token error %s\n", source_volume->token);
        return BVERROR(EINVAL);
    }
    involume.chip   = aidev;//iAiDev;
    involume.chn    = aichn;
    involume.volume = source_volume->volume * 15 / 100;

    ret = ioctl(driver->fd, TW286X_SET_AUDIO_IN_VOLUME, &involume);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "ioctl TW286X_SET_AUDIO_IN_VOLUME err !!!\n");
    }
    return ret;
}

static int audio_output_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tw2866Driver *driver = s->priv_data;
    BVAudioOutputVolume *output_volume = pkt_in->data;
    tw286x_audio_da_volume  davolume;
    int aodev, aochn;
    int ret = 0;

    if (sscanf(output_volume->token, "%d/%d", &aodev, &aochn) != 2) {
        bv_log(s, BV_LOG_ERROR, "audio output token error %s\n", output_volume->token);
        return BVERROR(EINVAL);
    }
    davolume.chip   = aodev;
    davolume.volume = output_volume->volume * 15 / 100;

    ret = ioctl(driver->fd, TW286X_SET_AUDIO_DA_VOLUME, &davolume);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "ioctl TW286X_SET_AUDIO_DA_VOLUME err !!!\n");
    }

    bv_log(s, BV_LOG_INFO, "ioctl TW286X_SET_AUDIO_DA_VOLUME success!!!\n");
    return ret;
}

static int audio_source_set_sample(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tw2866Driver *driver = s->priv_data;
    int audio_source_samplerate = *(int *)pkt_in->data;
    tw286x_audio_samplerate samplerate;
    int ret = 0;
    switch (audio_source_samplerate) {
        case 8000:
            samplerate = TW286X_SAMPLE_RATE_8000;
            break;
        case 16000:
            samplerate = TW286X_SAMPLE_RATE_16000;
            break;
        case 32000:
            samplerate = TW286X_SAMPLE_RATE_32000;
            break;
        case 44100:
            samplerate = TW286X_SAMPLE_RATE_44100;
            break;
        case 48000:
            samplerate = TW286X_SAMPLE_RATE_48000;
            break;
        default:
            bv_log(s, BV_LOG_ERROR, "not support audio_source_samplerate %d\n", audio_source_samplerate);
            return -1;
    }
    ret = ioctl(driver->fd, TW286X_SET_SAMPLE_RATE, &samplerate);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "ioctl TW286X_SET_SAMPLE_RATE %d err !!! \n", audio_source_samplerate);
    }

    return ret;
}

static int video_source_set_imaging(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tw2866Driver *driver = s->priv_data;
    BVVideoSourceImaging *video_source_imaging = pkt_in->data;
    tw286x_image_adjust stParam;
    int videv, vichn;
    int ret = 0;

    if (sscanf(video_source_imaging->token, "%d/%d", &videv, &vichn) != 2) {
        bv_log(s, BV_LOG_ERROR, "video source token %s error", video_source_imaging->token);
        return BVERROR(EINVAL);
    }
    
    BBCLEAR_STRUCT(stParam);
    stParam.chip = videv;
    stParam.chn  = vichn;
    stParam.brightness = video_source_imaging->imaging.luminance;
    stParam.saturation = video_source_imaging->imaging.saturation;
    stParam.contrast   = video_source_imaging->imaging.contrast;
    stParam.hue        = video_source_imaging->imaging.hue;
    stParam.item_sel   = TW286X_SET_BRIGHT | TW286X_SET_SATURATION | TW286X_SET_CONTRAST | TW286X_SET_HUE;

    ret = ioctl(driver->fd, TW286X_SET_IMAGE_ADJUST, &stParam);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "set video source imaging error\n");
    }

    return ret;
}

static int tw2866_driver_control(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaDriverMessageType type;
        int (*control)(BVMediaDriverContext *s, const BVControlPacket *, BVControlPacket *);
    } driver_control[] = {
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_VOLUME, audio_source_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUTPUT_SET_VOLUME, audio_output_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_SAMPLE, audio_source_set_sample},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_SOURCE_SET_IMAGING, video_source_set_imaging},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(driver_control); i++) {
        if (driver_control[i].type == type)
           return driver_control[i].control(s, pkt_in, pkt_out); 
    }

    bv_log(s, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

static int tw2866_driver_close(BVMediaDriverContext *s)
{
    Tw2866Driver *driver = s->priv_data;
    if (driver->fd >= 0) {
        close(driver->fd);
    }
    return 0;
}

#define OFFSET(x) offsetof(Tw2866Driver, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};
static const BVClass tw2866_class = {
    .class_name         = "tw2866 driver",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVMediaDriver bv_tw2866_driver = {
    .name               = "tw2866",
    .type               = BV_MEDIA_DRIVER_TYPE_VIDEO | BV_MEDIA_DRIVER_TYPE_AUDIO,
    .priv_class         = &tw2866_class,
    .priv_data_size     = sizeof(Tw2866Driver),
    .driver_open        = tw2866_driver_open,
    .driver_control     = tw2866_driver_control,
    .driver_close       = tw2866_driver_close,
};
