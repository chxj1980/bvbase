/*************************************************************************
    > File Name: sdp.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月21日 星期二 11时59分30秒
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

#line 25 "sdp.c"

#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>
#include "bvurl.h"
#include "sdp.h"

typedef struct _SDPContext {
    const BVClass *bv_class;
    BVSDP *sdp;
} SDPContext;

/**
 * sdp:///tmp/a.sdp
 * XXX 从文件中获取sdp信息并解析、或者从字符串中解析sdp信息
 */

static int sdp_open(BVURLContext *h, const char *filename, int flags, BVDictionary **options)
{
    SDPContext *sdpctx = h->priv_data;
    char *p = NULL;
    return BVERROR(ENOSYS);
}

static int sdp_control(BVURLContext *h, int type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int sdp_close(BVURLContext *h)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(SDPContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption sdp_options[] = {
    { NULL },
};

static const BVClass sdp_class = {
    .class_name         = "sdp proto",
    .item_name          = bv_default_item_name,
    .option             = sdp_options,
    .version            = LIBBVUTIL_VERSION_INT,
};

BVURLProtocol bv_sdp_protocol = {
    .name               = "sdp",
    .url_open           = sdp_open,
#if 0
    .url_read           = sdp_read,
    .url_write          = sdp_write,
    .url_seek           = sdp_seek,
#endif
    .url_control        = sdp_control,
    .url_close          = sdp_close,
    .priv_data_size     = sizeof(SDPContext),
    .priv_class         = &sdp_class,
};
