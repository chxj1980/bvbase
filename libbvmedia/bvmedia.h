/*************************************************************************
    > File Name: bvmedia.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 12时56分43秒
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

#ifndef BV_MEDIA_H
#define BV_MEDIA_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>
#include <libbvutil/dict.h>
#include <libbvutil/packet.h>
#include <libbvcodec/bvcodec.h>
#include <libbvconfig/common.h>
#include <libbvprotocol/bvio.h>

typedef struct _BVMediaContext BVMediaContext;
/**
 *  BVInputMedia BVOutputMedia flags
 */
#define BV_MEDIA_FLAGS_NOFILE       0x0001

#define BV_MEDIA_FLAGS_NOSTREAMS    0x1000

/**
 *  BVMediaContext flags
 */
#define BV_MEDIA_FLAGS_NONBLOCK     0x0001

enum BVMediaMessageType {
    BV_MEDIA_MESSAGE_TYPE_NONE = -1,
    BV_MEDIA_MESSAGE_TYPE_AUDIO_MUTE,           //静音
    BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME,         //音量调节
    BV_MEDIA_MESSAGE_TYPE_VIDEO_HIDE,           //隐藏视频
    BV_MEDIA_MESSAGE_TYPE_VIDEO_SHOW,           //显示视频
    BV_MEDIA_MESSAGE_TYPE_VIDEO_STOP,           //停止视频
    BV_MEDIA_MESSAGE_TYPE_VIDEO_PAUSE,          //暂停视频
    BV_MEDIA_MESSAGE_TYPE_VIDEO_FSFWD,          //fast forward 视频快进
    BV_MEDIA_MESSAGE_TYPE_VIDEO_REWND,          //rewind 视频快退
    BV_MEDIA_MESSAGE_TYPE_VIDEO_UPCFG,          //更改视频编解码动态属性
    BV_MEDIA_MESSAGE_TYPE_VIDEO_IMAGE,          //视频图像设置
    BV_MEDIA_MESSAGE_TYPE_VIDEO_RSKFM,          //request key frame 请求关键帧
    BV_MEDIA_MESSAGE_TYPE_OSD_CREATE,           //创建OSD叠加
    BV_MEDIA_MESSAGE_TYPE_OSD_DESTROY,          //销毁OSD
    BV_MEDIA_MESSAGE_TYPE_OSD_HIDE,             //隐藏OSD
    BV_MEDIA_MESSAGE_TYPE_OSD_SHOW,             //显示OSD
    BV_MEDIA_MESSAGE_TYPE_OSD_UPDATE,           //更新OSD数据
    BV_MEDIA_MESSAGE_TYPE_OSD_UPCFG,            //更新OSD配置
    BV_MEDIA_MESSAGE_TYPE_UNKNOW
};

typedef struct _BVInputMedia {
    const char *name;
    const char *extensions;
    const char *mime_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVInputMedia *next;
    int flags;
    int (*read_probe)(BVMediaContext *h, BVProbeData *);
    int (*read_header)(BVMediaContext *h);
    int (*read_packet)(BVMediaContext *h, BVPacket *pkt);
    int (*read_close)(BVMediaContext *h);
    int (*media_control)(BVMediaContext *h, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);
} BVInputMedia;

typedef struct _BVOutputMedia {
    const char *name;
    const char *extensions;
    const char *mime_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVOutputMedia *next;
    int flags;
    int (*write_header)(BVMediaContext *h);
    int (*write_packet)(BVMediaContext *h, BVPacket *pkt);
    int (*write_trailer)(BVMediaContext *h);
    int (*media_control)(BVMediaContext *h, enum BVMediaMessageType type, const BVControlPacket *in, BVControlPacket *out);
} BVOutputMedia;

typedef struct _BVStream {
    int index;
    BVCodecContext *codec;
    BVRational time_base;
    void *priv_data;
} BVStream;

struct _BVMediaContext {
    const BVClass *bv_class;
    BVInputMedia *imedia;
    BVOutputMedia *omedia;
    void *priv_data;
    BVIOContext *pb;
    char filename[1024];
    int flags;
    int nb_streams;
    BVStream **streams;
};

void bv_input_media_register(BVInputMedia *ifmt);

void bv_output_media_register(BVOutputMedia *media);

BVInputMedia * bv_input_media_next(BVInputMedia *ifmt);

BVOutputMedia *bv_output_media_next(const BVOutputMedia *f);

BVInputMedia *bv_input_media_find(const char *short_name);

void bv_media_register_all(void);

BVMediaContext *bv_media_context_alloc(void);

void bv_media_context_free(BVMediaContext * devctx);

int bv_input_media_open(BVMediaContext **fmt, const BVMediaChannel *channel, const char *url, BVInputMedia *media, BVDictionary **options);

BVStream * bv_stream_new(BVMediaContext *s, const BVCodec *c);

void bv_stream_free(BVMediaContext *s, BVStream *st);

int bv_input_media_read(BVMediaContext *s, BVPacket *pkt);

int bv_input_media_close(BVMediaContext **fmt);

BVOutputMedia *bv_output_media_guess(const char *short_name, const char *filename, const char *mime_type);

int bv_output_media_open(BVMediaContext **fmt, const char *url, const char *format, BVOutputMedia *media, BVDictionary **options);

int bv_output_media_write_header(BVMediaContext *s, BVDictionary **options);

int bv_output_media_write(BVMediaContext *s, BVPacket *pkt);

int bv_output_media_write_trailer(BVMediaContext *s);

int bv_output_media_close(BVMediaContext **fmt);

int bv_media_context_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

typedef struct _BVOSDConfig {
    char fontfile[128];
//    uint8_t text[1024];     //UTF-8
    int     x, y;
    int     width, height;
    int     shadowx, shadowy;
    uint8_t draw_box;       //draw box around text --- 0 1
    uint8_t draw_border;
    uint8_t draw_shadow;
    uint8_t borderw;        //边框像素的大小
    uint32_t fontsize;
    char    fontcolor[32];  //white black ...#ADDD2F 0xADDD2F read@0.5 @alpha
    char    shadowcolor[32];
    char    boxcolor[32];
    char    bordercolor[32];
    enum BVPixelFormat src_fmt;  //输入格式  创建后不可更改
    enum BVPixelFormat dst_fmt;  //输出格式  创建后不可更改
} BVOSDConfig;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MEDIA_H */
