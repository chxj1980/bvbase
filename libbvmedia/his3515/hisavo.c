/*************************************************************************
    > File Name: hisavo.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月13日 星期五 17时38分28秒
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

#line 25 "hisavo.c"

#include <libbvmedia/bvmedia.h>
#include <libbvmedia/driver.h>
#include <libbvutil/bvstring.h>

//FIXME c99 not support asm
#define asm __asm__

//His3515 headers
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_aio.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_ai.h"
#include "mpi_ao.h"

/**
 *  His3515 Audio Video Output Device Channel
 */

#define BREAK_WHEN_SDK_FAILED(comment, s32Ret) \
    do { \
        if (s32Ret != HI_SUCCESS) { \
            bv_log(s, BV_LOG_ERROR, comment " 0x%X\n", s32Ret); \
            goto fail; \
        } \
    }while(0)

typedef struct HisAVOContext {
    const BVClass *bv_class;
    BVMediaDriverContext *vdriver;
    char *vtoken;
    char *vchip;
    char *vdev;
    BVMediaDriverContext *adriver;
    char *atoken;
    char *achip;
    char *adev;
    int vodev;
    int vochn;
    int vindex;
    int aodev;
    int aochn;
    int aindex;
} HisAVOContext;

static int create_video_output_channel(BVMediaContext *s, BVStream *stream)
{
    HisAVOContext *hisctx = s->priv_data;
    VO_CHN_ATTR_S stVoChnAttr;
    HI_S32 s32Ret = HI_FAILURE;
    if (!hisctx->vtoken) {
        bv_log(s, BV_LOG_ERROR, "must set vtoken before create video output channel\n");
        return BVERROR(EINVAL);
    }

    if (sscanf(hisctx->vtoken, "%d/%d", &hisctx->vodev, &hisctx->vochn) != 2) {
        bv_log(s, BV_LOG_ERROR, "vtoken param error\n");
        return BVERROR(EINVAL);
    }
//FIXME x y may be seted
    BBCLEAR_STRUCT(stVoChnAttr);
    stVoChnAttr.stRect.s32X = 0;
    stVoChnAttr.stRect.s32Y = 0;
    stVoChnAttr.stRect.u32Width = stream->codec->width;
    stVoChnAttr.stRect.u32Height = stream->codec->height;
    stVoChnAttr.u32Priority = 0;
    stVoChnAttr.bZoomEnable = HI_TRUE;
    stVoChnAttr.bDeflicker = HI_FALSE;

    s32Ret = HI_MPI_VO_SetChnAttr(hisctx->vodev, hisctx->vochn, &stVoChnAttr);
    BREAK_WHEN_SDK_FAILED("set vochn attr error", s32Ret);
    
    s32Ret = HI_MPI_VO_EnableChn(hisctx->vodev, hisctx->vochn);
    BREAK_WHEN_SDK_FAILED("enable vochn error", s32Ret);

    s32Ret = HI_MPI_VO_ChnHide(hisctx->vodev, hisctx->vochn);
    BREAK_WHEN_SDK_FAILED("hide vochn error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "create video output channel %s success\n", hisctx->vtoken);
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int audio_set_volume(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

static int create_audio_output_channel(BVMediaContext *s, BVStream *stream)
{
    HisAVOContext *hisctx = s->priv_data;
    int volume = 80;
    BVControlPacket pkt_in;
    HI_S32 s32Ret = HI_FAILURE;
    if (!hisctx->atoken) {
        bv_log(s, BV_LOG_ERROR, "must set atoken before create audio output channel\n");
        return BVERROR(EINVAL);
    }
    if (sscanf(hisctx->atoken, "%d/%d", &hisctx->aodev, &hisctx->aochn) != 2) {
        bv_log(s, BV_LOG_ERROR, "atoken param error\n");
        return BVERROR(EINVAL);
    }
    pkt_in.data = &volume;
    pkt_in.size = 1;
    audio_set_volume(s, &pkt_in, NULL);
    //AudioOutVolumeSet(hisctx->aodev, hisctx->aochn, 80);
    s32Ret = HI_MPI_AO_EnableChn(hisctx->aodev, hisctx->aochn);
    BREAK_WHEN_SDK_FAILED("enable aochn error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "create audio output channel %s success\n", hisctx->atoken);
    return 0;
fail:
    return BVERROR(EINVAL);
}

/**
 *  FIXME enable audio video output channel
 *  Caller must add stream before call this function
 */
static int his_write_header(BVMediaContext *s)
{
    HisAVOContext *hisctx = s->priv_data;
    BVStream *stream = NULL;
    int ret = 0;
    int i = 0;
    for (i = 0; i < s->nb_streams; i++) {
        stream = s->streams[i];
        if (stream->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
            if (create_video_output_channel(s, stream) < 0) {
                bv_log(s, BV_LOG_ERROR, "create video output channel error\n");
                ret = BVERROR(EINVAL);
                break;
            }
            hisctx->vindex = i;
        } else if (stream->codec->codec_type == BV_MEDIA_TYPE_AUDIO) {
            if (bv_media_driver_open(&hisctx->adriver, hisctx->adev, hisctx->achip, NULL, NULL) < 0) {
                bv_log(s, BV_LOG_ERROR, "open audio driver %s path %s error\n", hisctx->achip, hisctx->adev);
                ret = BVERROR(EINVAL);
                break;
            }
            if (create_audio_output_channel(s, stream) < 0) {
                bv_log(s, BV_LOG_ERROR, "create audio output channel error\n");
                ret = BVERROR(EINVAL);
                break;
            }
            hisctx->aindex = i;
        } else {
            bv_log(s, BV_LOG_ERROR, "His3515 Not Support This Stream Type\n");
        }
    }
    return ret;
}

static int his_write_packet(BVMediaContext *s, BVPacket *pkt)
{
    return BVERROR(ENOSYS);
}

static int his_write_trailer(BVMediaContext *s)
{
    return BVERROR(ENOSYS);
}

static int audio_set_volume(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    HisAVOContext *hisctx = s->priv_data;
    BVControlPacket pkt;
    BVAudioOutputVolume output_volume;
    int volume = *((int *)pkt_in->data);
    int ret = 0;

    bv_strlcpy(output_volume.token, hisctx->atoken, sizeof(output_volume.token));
    output_volume.volume = volume;
    pkt.data = &output_volume;
    pkt.size = 1;
    ret = bv_media_driver_control(hisctx->adriver, BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUTPUT_SET_VOLUME, &pkt, NULL); 
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "set audio output volume error\n");
    }

    return ret;
}

static int audio_set_mute(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    BVControlPacket pkt;
    int volume = 0;
    pkt.data = &volume;
    return audio_set_mute(s, &pkt, NULL);
}

static int video_set_imaging(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}


static int his_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaMessageType type;
        int (*control)(BVMediaContext *s, const BVControlPacket *, BVControlPacket *);
    } media_control[] = {
        { BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME, audio_set_volume},
        { BV_MEDIA_MESSAGE_TYPE_AUDIO_MUTE, audio_set_mute},
        { BV_MEDIA_MESSAGE_TYPE_VIDEO_IMAGE, video_set_imaging},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(media_control); i++) {
        if (media_control[i].type == type)
           return media_control[i].control(s, pkt_in, pkt_out); 
    }

    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(HisAVOContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "vindex", "", OFFSET(vindex), BV_OPT_TYPE_INT, {.i64 = -1}, -1, 128, DEC},
    { "vchip", "", OFFSET(vchip), BV_OPT_TYPE_STRING, {.str = "tw2866"}, 0, 0, DEC},
    { "vdev", "",  OFFSET(vdev), BV_OPT_TYPE_STRING, {.str = "/dev/tw2865dev"}, 0, 0, DEC},
    { "atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "aindex", "", OFFSET(aindex), BV_OPT_TYPE_INT, {.i64 = -1}, -1, 128, DEC},
    { "achip", "", OFFSET(achip), BV_OPT_TYPE_STRING, {.str = "tlv320aic23"}, 0, 0, DEC},
    { "adev", "",  OFFSET(adev), BV_OPT_TYPE_STRING, {.str = "/dev/tlv320aic23"}, 0, 0, DEC},
    {NULL}
};

static const BVClass his_class = {
    .class_name         = "hisavo muxer",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_MUXER,
};

BVOutputMedia bv_hisavo_muxer = {
    .name               = "hisavo",
    .mime_type          = "video/x-bsvideo",
    .priv_class         = &his_class,
    .priv_data_size     = sizeof(HisAVOContext),
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .write_header       = his_write_header,
    .write_packet       = his_write_packet,
    .write_trailer      = his_write_trailer,
    .media_control      = his_media_control,
};

