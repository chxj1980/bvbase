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
#include <libbvutil/packet.h>
#include <libbvcodec/bvcodec.h>
#include <libbvconfig/common.h>
#include <libbvprotocol/bvio.h>

struct _BVMediaContext;

#define BV_MEDIA_FLAG_NOFILE    1

typedef struct _BVInputMedia {
    const char *name;
    const char *extensions;
    const char *mime_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVInputMedia *next;
    int flags;
    int (*read_probe)(struct _BVMediaContext *h, BVProbeData *);
    int (*read_header)(struct _BVMediaContext *h);
    int (*read_packet)(struct _BVMediaContext *h, BVPacket *pkt);
    int (*read_close)(struct _BVMediaContext *h);
    int (*control_message)(struct _BVMediaContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVInputMedia;

typedef struct _BVOutputMedia {
    const char *name;
    const char *extensions;
    const char *mime_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVOutputMedia *next;
    int (*write_head)(struct _BVMediaContext *h);
    int (*write_packet)(struct _BVMediaContext *h, BVPacket *pkt);
    int (*write_trailer)(struct _BVMediaContext *h);
    int (*control_message)(struct _BVMediaContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVOutputMedia;

typedef struct _BVStream {
    int index;
    BVCodecContext *codec;
    void *priv_data;
} BVStream;

typedef struct _BVMediaContext {
    const BVClass *bv_class;
    BVInputMedia *imedia;
    BVOutputMedia *omedia;
    void *priv_data;
    BVIOContext *pb;
    char filename[1024];
    int nb_streams;
    BVStream **streams;
    BVChannel *channel;
} BVMediaContext;

void bv_input_media_register(BVInputMedia *ifmt);

void bv_output_media_register(BVOutputMedia *media);

BVInputMedia * bv_input_media_next(BVInputMedia *ifmt);

BVOutputMedia *bv_output_media_next(const BVOutputMedia *f);

BVInputMedia *bv_input_media_find(const char *short_name);

void bv_media_register_all(void);

BVMediaContext *bv_media_context_alloc(void);

void bv_media_context_free(BVMediaContext * devctx);

int bv_input_media_open(BVMediaContext **fmt, const BVChannel *channel, const char *url, BVInputMedia *media, BVDictionary **options);

BVOutputMedia *bv_output_media_guess(const char *short_name, const char *filename, const char *mime_type);

BVStream * bv_stream_new(BVMediaContext *s, const BVCodec *c);

void bv_stream_free(BVMediaContext *s, BVStream *st);

int bv_input_media_read(BVMediaContext *s, BVPacket *pkt);

int bv_input_media_close(BVMediaContext **fmt);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MEDIA_H */
