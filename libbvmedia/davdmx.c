/*************************************************************************
    > File Name: davdmx.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月17日 星期二 17时25分55秒
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

#line 25 "davdmx.c"

#include "bvmedia.h"

/**
 *  read stream info from dav file 
 */

typedef struct DavDeMuxContext {
    const BVClass *bv_class;
} DavDeMuxContext;

static int dav_probe(BVMediaContext *s, BVProbeData *p)
{
    return 0;
}

static int dav_read_header(BVMediaContext *s)
{
    return BVERROR(ENOSYS);
}

static int dav_read_packet(BVMediaContext *s, BVPacket *pkt)
{
    return BVERROR(ENOSYS);
}

static int dav_read_close(BVMediaContext *s) 
{
    return BVERROR(ENOSYS);
}

static int dav_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(DavDeMuxContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass dav_class = {
    .class_name         = "dav demuxer",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEMUXER,
};

BVInputMedia bv_dav_demuxer = {
    .name               = "dav",
    .extensions         = "dav",
    .mime_type          = "video/x-bsvideo",
    .priv_class         = &dav_class,
    .priv_data_size     = sizeof(DavDeMuxContext),
    .read_probe         = dav_probe,
    .read_header        = dav_read_header,
    .read_packet        = dav_read_packet,
    .read_close         = dav_read_close,
    .media_control      = dav_media_control,
};
