/*************************************************************************
    > File Name: hisave.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月30日 星期五 15时23分42秒
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

#include <libbvutil/bvstring.h>
#include <libbvutil/log.h>
#include <libbvutil/opt.h>

#include <libbvmedia/bvmedia.h>

typedef struct HisAVEContext {
    BVClass *bv_class;
} HisAVEContext;

#define OFFSET(x) offset(HisAVEContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL },
};

static const BVClass hisave_class = {
    .class_name = "hisave indev",
    .item_name = bv_default_item_name,
    .option = options,
    .version = LIBBVUTIL_VERSION_INT,
    .category = BV_CLASS_CATEGORY_DEMUXER,
};

static int his_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->file_name, "hisave:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

static bv_cold int his_read_header(BVMediaContext *s)
{
    return BVERROR(ENOSYS);
}

static bv_cold int his_read_packet(BVMediaContext *s, BVPacket *pkt)
{
    return BVERROR(ENOSYS);
}

static bv_cold int his_read_close(BVMediaContext *s)
{
    return BVERROR(ENOSYS);
}

static bv_cold int his_control(BVMediaContext *s, int type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

BVInputMedia bv_hisave_demuxer = {
    .name               = "hisave",
    .priv_data_size     = sizeof(HisAVEContext),
    .read_probe         = his_probe,
    .read_header        = his_read_header,
    .read_packet        = his_read_packet,
    .read_close         = his_read_close,
    .control_message    = his_control,
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .priv_class         = &his_class,
};
