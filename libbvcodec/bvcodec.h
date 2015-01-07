/*************************************************************************
    > File Name: bvcodec.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 16时38分55秒
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

#ifndef BV_CODEC_H
#define BV_CODEC_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/log.h>
#include <libbvutil/opt.h>
#include <libbvutil/pixfmt.h>
#include <libbvutil/samplefmt.h>

enum BVCodecID {
    BV_CODEC_ID_NONE = 0,
    //video
    BV_CODEC_ID_H264,
    BV_CODEC_ID_MPEG,
    BV_CODEC_ID_JPEG,

    //Audio
    BV_CODEC_ID_G711A,
    BV_CODEC_ID_G711U,
    BV_CODEC_ID_G726,
    BV_CODEC_ID_AAC,

    BV_CODEC_ID_UNKNOWN
};

enum BVRCModeID {
    BV_RC_MODE_ID_VBR = 0, /* VBR must be 0 for compatible with 3511 */
    BV_RC_MODE_ID_CBR,
    BV_RC_MODE_ID_ABR,
    BV_RC_MODE_ID_FIXQP,
    BV_RC_MODE_ID_BUTT,
};

typedef struct _BVCodec {
    const char *name;
    enum BVCodecID id;
    enum BVMediaType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVCodec *next;
    //FIXME
    //编解码
} BVCodec;

typedef struct _BVCodecContext {
    const BVClass *bv_class;
    const BVCodec *codec;
    void *priv_data;
    enum BVMediaType codec_type;    //BV_MEDIA_TYPE_XXX
    enum BVCodecID   codec_id;
    enum BVRCModeID  mode_id;
    int  width, height;
    int coded_width, coded_height;
    BVRational time_base;
    int quality;
    int bit_rate;
    int gop_size;
    enum BVPixelFormat pix_fmt;

    int sample_rate;
    enum BVSampleFormat sample_fmt;  ///< sample format
    int channels;

    uint8_t *extradata;
    int extradata_size;
    int profile;
} BVCodecContext;

BVCodecContext * bv_codec_context_alloc(const BVCodec *c);

void bv_codec_context_free(BVCodecContext * codec);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_CODEC_H */
