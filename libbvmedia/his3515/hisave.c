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
    char *vtoken;   //videv/vichn/vechn
    enum BVCodecID vcodec_id;
    enum BVRCModeID mode_id;
    int width, height;
    int quality;
    int bit_rate;
    int gop_size;
    int src_framerate;
    int framerate;

    char *atoken;
    enum BVCodecID acodec_id;
    int sample_rate;
    int channels;
} HisAVEContext;

#define OFFSET(x) offsetof(HisAVEContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "vcodec_id", "", OFFSET(vcodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_H264}, 0, INT_MAX, DEC},
    { "mode_id", "", OFFSET(mode_id), BV_OPT_TYPE_INT, {.i64 = BV_RC_MODE_ID_CBR}, 0, 255, DEC},
    { "width", "", OFFSET(width), BV_OPT_TYPE_INT, {.i64 = 704}, 0, INT_MAX, DEC},
    { "height", "", OFFSET(height), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "quality", "", OFFSET(quality), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "bit_rate", "", OFFSET(bit_rate), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "gop_size", "", OFFSET(gop_size), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "src_framerate", "", OFFSET(src_framerate), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "framerate", "", OFFSET(framerate), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "acodec_id", "", OFFSET(acodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_G711A}, 0, INT_MAX, DEC},
    { "sample_rate", "", OFFSET(sample_rate), BV_OPT_TYPE_INT, {.i64 = 8000}, 0, INT_MAX, DEC},
    { "channels", "", OFFSET(channels), BV_OPT_TYPE_INT, {.i64 = 1}, 0, 2, DEC},
    { NULL },
};

static const BVClass his_class = {
    .class_name = "hisave indev",
    .item_name = bv_default_item_name,
    .option = options,
    .version = LIBBVUTIL_VERSION_INT,
    .category = BV_CLASS_CATEGORY_DEMUXER,
};

static int his_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->filename, "hisave:", NULL))
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
