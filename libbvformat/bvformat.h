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
#include <libbvcodec/bvcodec.h>

struct BVFormatContext;

typedef struct _BVInputFormat {
    const char *name;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVInputFormat *next;
    int (*read_head)(struct BVFormatContext *h);
    int (*read_packet)(struct BVFormatContext *h, BVPacket *pkt);
    int (*read_close)(struct BVFormatContext *h);
    int (*control_message)(struct BVFormatContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVInputFormat;

typedef struct _BVOutputFormat {
    const char *name;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVOutputFormat *next;
    int (*write_head)(struct BVFormatContext *h);
    int (*write_packet)(struct BVFormatContext *h, BVPacket *pkt);
    int (*write_trailer)(struct BVFormatContext *h);
    int (*control_message)(struct BVFormatContext *h, int type, BVControlPacket *in, BVControlPacket *out);
} BVOutputFormat;

typedef struct _BVStream {
    int index;
    BVCodecContext *codec;
} BVStream;

typedef struct _BVFormatContext {
    BVClass *bv_class;
    BVInputFormat *ifmt;
    BVOutputFormat *ofmt;
    int priv_data_size;
    char file_name[1024];
    int nb_streams;
    BVStream **stream;
} BVFormatContext;


int bv_register_input_format(BVInputFormat *ifmt);
BVInputFormat * bv_input_format_next(BVInputFormat *ifmt);

void bv_format_register_all(void);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_FORMAT_H */
