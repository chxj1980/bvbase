/*************************************************************************
    > File Name: rtp.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月21日 星期二 11时58分13秒
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

#line 25 "rtp.c"

//TODO

#include "bvurl.h"

typedef struct _RTPContext {
    const BVClass *bv_class;
} RTPContext;

static int rtp_open(BVURLContext *h, const char *filename, int flags, BVDictionary **options)
{
    return BVERROR(ENOSYS);
}

static int rtp_control(BVURLContext *h, int type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int rtp_close(BVURLContext *h)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(RTPContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption rtp_options[] = {
    { NULL },
};

static const BVClass rtp_class = {
    .class_name         = "rtp proto",
    .item_name          = bv_default_item_name,
    .option             = rtp_options,
    .version            = LIBBVUTIL_VERSION_INT,
};

BVURLProtocol bv_rtp_protocol = {
    .name               = "rtp",
    .url_open           = rtp_open,
#if 0
    .url_read           = rtp_read,
    .url_write          = rtp_write,
    .url_seek           = rtp_seek,
#endif
    .url_control        = rtp_control,
    .url_close          = rtp_close,
    .priv_data_size     = sizeof(RTPContext),
    .priv_class         = &rtp_class,
};
