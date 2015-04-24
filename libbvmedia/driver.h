/*************************************************************************
    > File Name: driver.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月08日 星期三 13时36分42秒
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

#ifndef BV_MEDIA_DRIVER_H
#define BV_MEDIA_DRIVER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvconfig/common.h>
#include <libbvutil/packet.h>

enum BVVideoFormat {
    BV_VIDEO_FORMAT_NONE = -1,
    BV_VIDEO_FORMAT_AUTIO,
    BV_VIDEO_FORMAT_NTSC,
    BV_VIDEO_FORMAT_PAL,
    BV_VIDEO_FORMAT_UNKNOWN,
};

typedef struct _BVAudioSourceVolume {
    char token[BV_MAX_NAME_LEN];
    int volume;     //0 - 100
} BVAudioSourceVolume;

typedef struct _BVAudioOutputVolume {
    char token[BV_MAX_NAME_LEN];
    int volume;     //0 - 100
} BVAudioOutputVolume;

typedef struct _BVVideoSourceImaging {
    char token[BV_MAX_NAME_LEN];
    BVImagingSettings imaging;
} BVVideoSourceImaging;

typedef struct _BVVideoSourceFormat {
    char token[BV_MAX_NAME_LEN];
    enum BVVideoFormat format;
} BVVideoSourceFormat;

enum BVMediaDriverMessageType {
    BV_MEDIA_DRIVER_MESSAGE_TYPE_NONE = -1,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_VOLUME,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUTPUT_SET_VOLUME,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_SAMPLE,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUTPUT_SET_SAMPLE,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_SOURCE_SET_IMAGING,
    BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_SOURCE_GET_FORMAT,   //NTSC PAL 
};

enum BVMediaDriverType {
    BV_MEDIA_DRIVER_TYPE_NONE = 0,
    BV_MEDIA_DRIVER_TYPE_VIDEO = 1,
    BV_MEDIA_DRIVER_TYPE_AUDIO = 2,
    BV_MEDIA_DRIVER_TYPE_UNKNOWN,
};

enum BVMediaDriverID {
    BV_MEDIA_DRIVER_ID_NONE,
    BV_MEDIA_DRIVER_ID_TW2866,
    BV_MEDIA_DRIVER_ID_TLV320,
    BV_MEDIA_DRIVER_ID_UNKNOWN,
};

typedef struct _BVMediaDriverContext {
    const BVClass *bv_class;
    struct _BVMediaDriver *driver;
    void *priv_data;
    char filename[1024];
} BVMediaDriverContext;

typedef struct _BVMediaDriver {
    const char *name;
    enum BVMediaDriverID id;
    enum BVMediaDriverType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVMediaDriver *next;
    int (*driver_open)(BVMediaDriverContext *s);
    int (*driver_control)(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);
    int (*driver_close)(BVMediaDriverContext *s);
} BVMediaDriver;

/**
 *  BVMediaDriver 
 */
void bv_media_driver_register(BVMediaDriver *driver);

BVMediaDriver *bv_media_driver_next(const BVMediaDriver *driver);

BVMediaDriverContext *bv_media_driver_context_alloc(void);

void bv_media_driver_context_free(BVMediaDriverContext *s);

int bv_media_driver_open(BVMediaDriverContext **s, const char *url, const char *short_name, BVMediaDriver *driver, BVDictionary **options);

int bv_media_driver_close(BVMediaDriverContext **s);

int bv_media_driver_control(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MEDIA_DRIVER_H */
