/*************************************************************************
	> File Name: bvformat.h
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

#ifndef BV_FORMAT_H
#define BV_FORMAT_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>
#include <libbvutil/packet.h>
#include <libbvcodec/bvcodec.h>
#include <libbvconfig/common.h>

struct _BVFormatContext;

typedef struct _BVProbeData {
    const char *filename;
    void *buf;
    int buf_size;
    const char *mime_type;
} BVProbeData;

typedef struct _BVInputFormat {
    const char *name;
    const char *extensions;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVInputFormat *next;
    int (*read_probe)(BVProbeData *);
    int (*read_head)(struct _BVFormatContext *h);
    int (*read_packet)(struct _BVFormatContext *h, BVPacket *pkt);
    int (*read_close)(struct _BVFormatContext *h);
    int (*control_message)(struct _BVFormatContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVInputFormat;

typedef struct _BVOutputFormat {
    const char *name;
    const char *extensions;
    const char *mime_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVOutputFormat *next;
    int (*write_head)(struct _BVFormatContext *h);
    int (*write_packet)(struct _BVFormatContext *h, BVPacket *pkt);
    int (*write_trailer)(struct _BVFormatContext *h);
    int (*control_message)(struct _BVFormatContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVOutputFormat;

typedef struct _BVStream {
    int index;
    BVCodecContext *codec;
} BVStream;

typedef struct _BVFormatContext {
    BVClass *bv_class;
    BVInputFormat *iformat;
    BVOutputFormat *oformat;
    void *priv_data;
    char file_name[1024];
    int nb_streams;
    BVStream **stream;
    BVChannel *channel;
} BVFormatContext;

void bv_input_format_register(BVInputFormat *ifmt);

void bv_output_format_register(BVOutputFormat *format);

BVInputFormat * bv_input_format_next(BVInputFormat *ifmt);

BVOutputFormat *bv_output_format_next(const BVOutputFormat *f);

BVInputFormat *bv_input_format_find(const char *short_name);

void bv_format_register_all(void);

BVFormatContext *bv_format_context_alloc(void);

void bv_format_context_free(BVFormatContext * devctx);

int bv_input_format_open(BVFormatContext **fmt, const BVChannel *channel, const char *url, BVInputFormat *format, BVDictionary **options);

BVOutputFormat *bv_output_format_guess(const char *short_name, const char *filename, const char *mime_type);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_FORMAT_H */
