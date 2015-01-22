/*************************************************************************
    > File Name: davmux.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月21日 星期三 14时28分16秒
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

#include "bvmedia.h"

#include "dav.h"

typedef struct DavMuxContext {
    /* data */
} DavMuxContext;

static int dav_write_header(BVMediaContext *s)
{
    DavMuxContext *davctx = s->priv_data;
    return BVERROR(ENOSYS);
}

static int dav_write_packet(BVMediaContext *s, BVPacket *pkt)
{
    DavMuxContext *davctx = s->priv_data;
    return BVERROR(ENOSYS);
}

static int dav_write_trailer(BVMediaContext *s)
{
    DavMuxContext *davctx = s->priv_data;
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(DavMuxContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
//    {"vcodec_id", "", OFFSET(vcodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_H264}, 0, INT_MAX, DEC},
 //   {"video_rate", "", OFFSET(video_rate), BV_OPT_TYPE_VIDEO_RATE, {.str = "25"}, 0, 0, DEC},

    {NULL}
};

static const BVClass dav_class = {
    .class_name = "dav muxer",
    .item_name = bv_default_item_name,
    .option = options,
    .version = LIBBVUTIL_VERSION_INT,
    .category = BV_CLASS_CATEGORY_MUXER,
};

BVOutputMedia bv_dav_muxer = {
    .name           = "dav",
    .extensions     = "dav",
    .mime_type      = "video/x-bsvideo",
    .priv_data_size = sizeof(DavMuxContext),
    .priv_class     = &dav_class,
    .write_header   = dav_write_header,
    .write_packet   = dav_write_packet,
    .write_trailer  = dav_write_trailer,
};
