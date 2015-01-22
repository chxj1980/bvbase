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
    BV_CODEC_ID_H264 = 0x64,
    BV_CODEC_ID_MPEG,
    BV_CODEC_ID_JPEG,

    //Audio
    BV_CODEC_ID_G711A = 0x258,
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

#define BV_PROFILE_UNKNOWN -99
#define BV_PROFILE_RESERVED -100

#define BV_PROFILE_AAC_MAIN 0
#define BV_PROFILE_AAC_LOW  1
#define BV_PROFILE_AAC_SSR  2
#define BV_PROFILE_AAC_LTP  3
#define BV_PROFILE_AAC_HE   4
#define BV_PROFILE_AAC_HE_V2 28
#define BV_PROFILE_AAC_LD   22
#define BV_PROFILE_AAC_ELD  38
#define BV_PROFILE_MPEG2_AAC_LOW 128
#define BV_PROFILE_MPEG2_AAC_HE  131

#define BV_PROFILE_DTS         20
#define BV_PROFILE_DTS_ES      30
#define BV_PROFILE_DTS_96_24   40
#define BV_PROFILE_DTS_HD_HRA  50
#define BV_PROFILE_DTS_HD_MA   60

#define BV_PROFILE_MPEG2_422    0
#define BV_PROFILE_MPEG2_HIGH   1
#define BV_PROFILE_MPEG2_SS     2
#define BV_PROFILE_MPEG2_SNR_SCALABLE  3
#define BV_PROFILE_MPEG2_MAIN   4
#define BV_PROFILE_MPEG2_SIMPLE 5

#define BV_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
#define BV_PROFILE_H264_INTRA        (1<<11) // 8+3; constraint_set3_flag

#define BV_PROFILE_H264_BASELINE             66
#define BV_PROFILE_H264_CONSTRAINED_BASELINE (66|BV_PROFILE_H264_CONSTRAINED)
#define BV_PROFILE_H264_MAIN                 77
#define BV_PROFILE_H264_EXTENDED             88
#define BV_PROFILE_H264_HIGH                 100
#define BV_PROFILE_H264_HIGH_10              110
#define BV_PROFILE_H264_HIGH_10_INTRA        (110|BV_PROFILE_H264_INTRA)
#define BV_PROFILE_H264_HIGH_422             122
#define BV_PROFILE_H264_HIGH_422_INTRA       (122|BV_PROFILE_H264_INTRA)
#define BV_PROFILE_H264_HIGH_444             144
#define BV_PROFILE_H264_HIGH_444_PREDICTIVE  244
#define BV_PROFILE_H264_HIGH_444_INTRA       (244|BV_PROFILE_H264_INTRA)
#define BV_PROFILE_H264_CAVLC_444            44

#define BV_PROFILE_VC1_SIMPLE   0
#define BV_PROFILE_VC1_MAIN     1
#define BV_PROFILE_VC1_COMPLEX  2
#define BV_PROFILE_VC1_ADVANCED 3

#define BV_PROFILE_MPEG4_SIMPLE                     0
#define BV_PROFILE_MPEG4_SIMPLE_SCALABLE            1
#define BV_PROFILE_MPEG4_CORE                       2
#define BV_PROFILE_MPEG4_MAIN                       3
#define BV_PROFILE_MPEG4_N_BIT                      4
#define BV_PROFILE_MPEG4_SCALABLE_TEXTURE           5
#define BV_PROFILE_MPEG4_SIMPLE_FACE_ANIMATION      6
#define BV_PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE     7
#define BV_PROFILE_MPEG4_HYBRID                     8
#define BV_PROFILE_MPEG4_ADVANCED_REAL_TIME         9
#define BV_PROFILE_MPEG4_CORE_SCALABLE             10
#define BV_PROFILE_MPEG4_ADVANCED_CODING           11
#define BV_PROFILE_MPEG4_ADVANCED_CORE             12
#define BV_PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE 13
#define BV_PROFILE_MPEG4_SIMPLE_STUDIO             14
#define BV_PROFILE_MPEG4_ADVANCED_SIMPLE           15

#define BV_PROFILE_JPEG2000_CSTREAM_RESTRICTION_0   0
#define BV_PROFILE_JPEG2000_CSTREAM_RESTRICTION_1   1
#define BV_PROFILE_JPEG2000_CSTREAM_NO_RESTRICTION  2
#define BV_PROFILE_JPEG2000_DCINEMA_2K              3
#define BV_PROFILE_JPEG2000_DCINEMA_4K              4

#define BV_PROFILE_HEVC_MAIN                        1
#define BV_PROFILE_HEVC_MAIN_10                     2
#define BV_PROFILE_HEVC_MAIN_STILL_PICTURE          3
#define BV_PROFILE_HEVC_REXT                        4

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
