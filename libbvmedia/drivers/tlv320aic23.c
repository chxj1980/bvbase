/*************************************************************************
    > File Name: tlv320aic23.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月07日 星期二 13时22分59秒
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

#line 25 "tlv320aic23.c"

#include <libbvmedia/bvmedia.h>

#include "tlv320aic23.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

typedef struct Tlv320Aic23Driver {
    const BVClass *bv_class;
    int fd;
} Tlv320Aic23Driver;

static int tlv320aic23_driver_open(BVMediaDriverContext *s)
{
    Tlv320Aic23Driver *driver = s->priv_data;
    bv_log(s, BV_LOG_DEBUG, "run here tlv320aic23 driver open\n");
    driver->fd = open(s->filename, O_RDONLY);
    if (driver->fd < 0) {
        return BVERROR(EIO);
    }
    return 0;
}

static int audio_source_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tlv320Aic23Driver *driver = s->priv_data;
    BVAduioSourceVolume *source_volume = pkt_in->data;
    int cmd = AIC23IOC_S_AD_RIGHT_VOLUME;
    int aidev, aichn;
    int ret = 0;
    if (sscanf(source_volume->token, "%d/%d", &aidev, &aichn) != 2) {
        bv_log(s, BV_LOG_ERROR, "audio source token error %s\n", source_volume->token);
        return BVERROR(EINVAL);
    }
    if (aichn == 0) {
        cmd = AIC23IOC_S_AD_LEFT_VOLUME;
    }
    ret = ioctl(driver->fd, cmd, &source_volume->volume);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "ioctl AIC23IOC_S_AD_VOLUME err !!!\n");
    }
    return ret;
}

static int audio_output_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tlv320Aic23Driver *driver = s->priv_data;
    BVAudioOutputVolume *output_volume = pkt_in->data;
    int cmd = AIC23IOC_S_DA_RIGHT_VOLUME;
    int aodev, aochn;
    int ret = 0;
    if (sscanf(output_volume->token, "%d/%d", &aodev, &aochn) != 2) {
        bv_log(s, BV_LOG_ERROR, "audio output token error %s\n", output_volume->token);
        return BVERROR(EINVAL);
    }
    if (aochn == 0) {
        cmd = AIC23IOC_S_DA_LEFT_VOLUME;
    }
    ret = ioctl(driver->fd, cmd, &output_volume->volume);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "ioctl AIC23IOC_S_DA_VOLUME err !!!\n");
    }
    return ret;
}

static int audio_source_set_sample(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    Tlv320Aic23Driver *driver = s->priv_data;
    int audio_source_samplerate = *(int *)pkt_in->data;
    int ret = 0;
    ret = ioctl(driver->fd, AIC23IOC_S_SAMPLE_RATE, &audio_source_samplerate);
    if (ret < 0) {
        bv_log(NULL, BV_LOG_ERROR, "ioctl AIC23IOC_S_SAMPLE_RATE %d err !!!\n", audio_source_samplerate);
    }
    return ret;
}

static int tlv320aic23_driver_control(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaDriverMessageType type;
        int (*control)(BVMediaDriverContext *s, const BVControlPacket *, BVControlPacket *);
    } driver_control[] = {
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_VOLUME, audio_source_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUTPUT_SET_VOLUME, audio_output_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_SAMPLE, audio_source_set_sample},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(driver_control); i++) {
        if (driver_control[i].type == type)
           return driver_control[i].control(s, pkt_in, pkt_out); 
    }

    bv_log(s, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

static int tlv320aic23_driver_close(BVMediaDriverContext *s)
{
    Tlv320Aic23Driver *driver = s->priv_data;
    bv_log(s, BV_LOG_DEBUG, "run here tlv320aic23 driver close\n");
    if (driver->fd >= 0) {
        close(driver->fd);
    }
    return 0;
}

#define OFFSET(x) offsetof(Tlv320Aic23Driver, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};
static const BVClass tlv320aic23_class = {
    .class_name         = "tlv320aic23 driver",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVMediaDriver bv_tlv320aic23_driver = {
    .name               = "tlv320aic23",
    .type               = BV_MEDIA_DRIVER_TYPE_AUDIO,
    .priv_class         = &tlv320aic23_class,
    .priv_data_size     = sizeof(Tlv320Aic23Driver),
    .driver_open        = tlv320aic23_driver_open,
    .driver_control     = tlv320aic23_driver_control,
    .driver_close       = tlv320aic23_driver_close,
};
