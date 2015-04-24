/*************************************************************************
    > File Name: rtsp.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月21日 星期二 11时58分42秒
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

#line 25 "rtsp.c"

//TODO

#include "bvurl.h"

typedef struct _RTSPContext {
    const BVClass *bv_class;
} RTSPContext;

static int rtsp_open(BVURLContext *h, const char *filename, int flags, BVDictionary **options)
{
    return BVERROR(ENOSYS);
}

static int rtsp_control(BVURLContext *h, int type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int rtsp_close(BVURLContext *h)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(RTSPContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption rtsp_options[] = {
    { NULL },
};

static const BVClass rtsp_class = {
    .class_name         = "rtsp proto",
    .item_name          = bv_default_item_name,
    .option             = rtsp_options,
    .version            = LIBBVUTIL_VERSION_INT,
};

BVURLProtocol bv_rtsp_protocol = {
    .name               = "rtsp",
    .url_open           = rtsp_open,
#if 0
    .url_read           = rtsp_read,
    .url_write          = rtsp_write,
    .url_seek           = rtsp_seek,
#endif
    .url_control        = rtsp_control,
    .url_close          = rtsp_close,
    .priv_data_size     = sizeof(RTSPContext),
    .priv_class         = &rtsp_class,
};
