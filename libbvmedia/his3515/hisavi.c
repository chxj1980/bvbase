/*************************************************************************
    > File Name: hisavi.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月13日 星期五 16时48分39秒
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

#line 25 "hisavi.c"

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

//#include "drv.h"

/**
 *  His3515 Audio Video Input Device Channel
 */

#define BREAK_WHEN_SDK_FAILED(comment, s32Ret) \
    do { \
        if (s32Ret != HI_SUCCESS) { \
            bv_log(s, BV_LOG_ERROR, comment " 0x%X\n", s32Ret); \
            goto fail; \
        } \
    }while(0)

typedef struct HisAVIContext {
    const BVClass *bv_class;
    BVMediaDriverContext *vdriver;
    char *vtoken;
    char *vchip;
    char *vdev;
    int videv;
    int vichn;
    int vindex;
    int width;
    int height;
    int framerate;
    enum BVPixelFormat pix_fmt;
    BVMediaDriverContext *adriver;
    char *atoken;
    char *achip;
    char *adev;
    int aidev;
    int aichn;
    int aindex;
    int sample_rate;
    int channels;
} HisAVIContext;

typedef struct IntTable {
    int a;
    int b;
} IntTable;

static int his_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->filename, "hisavi:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

static int destroy_audio_input_channel(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    s32Ret = HI_MPI_AI_DisableChn(hisctx->aidev, hisctx->aichn);
    BREAK_WHEN_SDK_FAILED("disable aichn error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "disable audio input channel success\n");
    return 0;
fail:
    return BVERROR(EIO);
}

static int destroy_video_input_channel(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    s32Ret = HI_MPI_VI_DisableChn(hisctx->videv, hisctx->vichn);
    BREAK_WHEN_SDK_FAILED("disable vichn error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "disable video input channel success\n");
    return 0;
fail:
    return BVERROR(EIO);
}
static int audio_set_volume(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

static int create_audio_input_channel(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    BVControlPacket pkt_in;
    int volume = 70;
    if (sscanf(hisctx->atoken, "%d/%d", &hisctx->aidev, &hisctx->aichn) != 2) {
        bv_log(s, BV_LOG_ERROR, "atoken param error\n");
        return BVERROR(EINVAL);
    }
    //设置音量
    pkt_in.data = &volume;
    pkt_in.size = 1;
    audio_set_volume(s, &pkt_in, NULL);

    s32Ret = HI_MPI_AI_EnableChn(hisctx->aidev, hisctx->aichn);
    BREAK_WHEN_SDK_FAILED("enable aichn error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "create audio input channel %s success\n", hisctx->atoken);
    return 0;
fail:
    return BVERROR(EIO);
}

static int get_video_pix_format(int pix_fmt)
{
    int i = 0;
    IntTable pix_fmts[] = {
        { BV_PIX_FMT_YUV420P, PIXEL_FORMAT_YUV_SEMIPLANAR_420},
        { BV_PIX_FMT_YUV422P, PIXEL_FORMAT_YUV_SEMIPLANAR_422},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(pix_fmts); i++) {
        if (pix_fmts[i].a == pix_fmt)
           return pix_fmts[i].b; 
    }
    return PIXEL_FORMAT_YUV_SEMIPLANAR_420;
}

static int create_video_input_channel(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    VI_CHN_ATTR_S stChnAttr;
    BVControlPacket pkt_in;
    BVVideoSourceFormat source_format;
    HI_S32 s32Ret = HI_FAILURE;
    if (sscanf(hisctx->vtoken, "%d/%d", &hisctx->videv, &hisctx->vichn) != 2) {
        bv_log(s, BV_LOG_ERROR, "vtoken param error\n");
        return BVERROR(EINVAL);
    }
    bv_strlcpy(source_format.token, hisctx->vtoken, sizeof(source_format.token));
    source_format.format = BV_VIDEO_FORMAT_PAL;
    pkt_in.data = &source_format;
    pkt_in.size = 1;
    if (bv_media_driver_control(hisctx->vdriver, BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_SOURCE_GET_FORMAT, &pkt_in, NULL) < 0) {
        bv_log(s, BV_LOG_ERROR, "get video source format error\n");
    }
    if (source_format.format == BV_VIDEO_FORMAT_NTSC) {
        hisctx->framerate = 30;
        hisctx->height = 240;
    }
    //FIXME
    stChnAttr.bChromaResample = HI_FALSE;
    stChnAttr.bDownScale = HI_FALSE;
    stChnAttr.bHighPri = HI_FALSE;
    stChnAttr.enCapSel = VI_CAPSEL_BOTH;
    stChnAttr.enViPixFormat = get_video_pix_format(hisctx->pix_fmt);
    stChnAttr.stCapRect.s32X = 2;
    stChnAttr.stCapRect.s32Y = 2;
    stChnAttr.stCapRect.u32Height = hisctx->height;
    stChnAttr.stCapRect.u32Width = hisctx->width;
    s32Ret = HI_MPI_VI_SetChnAttr(hisctx->videv, hisctx->vichn, &stChnAttr);
    BREAK_WHEN_SDK_FAILED("set vichn error", s32Ret);
    s32Ret = HI_MPI_VI_SetSrcFrameRate(hisctx->videv, hisctx->vichn, hisctx->framerate);
    BREAK_WHEN_SDK_FAILED("set vichn source frame rate error", s32Ret);
    s32Ret = HI_MPI_VI_SetFrameRate(hisctx->videv, hisctx->vichn, hisctx->framerate);
    BREAK_WHEN_SDK_FAILED("set vichn frame rate error", s32Ret);

    s32Ret = HI_MPI_VI_EnableChn(hisctx->videv, hisctx->vichn);
    BREAK_WHEN_SDK_FAILED("enable vichn error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "create video input channel %s success\n", hisctx->vtoken);
    return 0;
fail:
    destroy_video_input_channel(s);
    return BVERROR(EIO);
}

static int add_audio_input_stream(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    BVStream *st = bv_stream_new(s, NULL);
    hisctx->aindex = st->index;
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->codec_id = BV_CODEC_ID_PCM;
    st->codec->sample_rate = hisctx->sample_rate;
    st->codec->channels = hisctx->channels;
    st->time_base = (BVRational) {1, 1000000};
    return 0;
}

static int add_video_input_stream(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;
    BVStream *st = bv_stream_new(s, NULL);
    hisctx->vindex = st->index;
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->codec_id = BV_CODEC_ID_REWVIDEO;
    st->codec->time_base = (BVRational) {1, hisctx->framerate};
    st->time_base = (BVRational) {1, 1000000};
    st->codec->width = hisctx->width;
    st->codec->height = hisctx->height;
    return 0;
}

static bv_cold int his_read_close(BVMediaContext *s);

static bv_cold int his_read_header(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;

    if (hisctx->atoken) {
        if (bv_media_driver_open(&hisctx->adriver, hisctx->adev, hisctx->achip, NULL, NULL) < 0) {
            bv_log(s, BV_LOG_ERROR, "open audio driver %s path %s error\n", hisctx->achip, hisctx->adev);
            goto fail;
        }
        if (create_audio_input_channel(s) < 0) {
            goto fail;
        }
        add_audio_input_stream(s);
    }
    if (hisctx->vtoken) {
        if (bv_media_driver_open(&hisctx->vdriver, hisctx->vdev, hisctx->vchip, NULL, NULL) < 0) {
            bv_log(s, BV_LOG_ERROR, "open video driver %s path %s error\n", hisctx->vchip, hisctx->vdev);
            goto fail;
        }
        if (create_video_input_channel(s) < 0) {
            goto fail;
        }
        add_video_input_stream(s);
    }
    return 0;
fail:
    his_read_close(s);
    return BVERROR(EIO);
}

static bv_cold int his_read_packet(BVMediaContext *s, BVPacket *pkt)
{
    return BVERROR(ENOSYS);
}

static bv_cold int his_read_close(BVMediaContext *s)
{
    HisAVIContext *hisctx = s->priv_data;

    bv_media_driver_close(&hisctx->adriver);
    if (hisctx->aindex != -1) {
        destroy_audio_input_channel(s);
    }
    
    bv_media_driver_close(&hisctx->vdriver);
    if (hisctx->vindex != -1) {
        destroy_video_input_channel(s);
    }
    return 0;
}

static int audio_set_volume(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    HisAVIContext *hisctx = s->priv_data;
    int volume = *((int *)pkt_in->data);
    BVControlPacket pkt;
    BVAudioSourceVolume source_volume;
    int ret = 0;
    //设置音量
    bv_strlcpy(source_volume.token, hisctx->atoken, sizeof(source_volume.token));
    source_volume.volume = volume;
    pkt.data = &source_volume;
    pkt.size = 1;
    ret = bv_media_driver_control(hisctx->adriver, BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_VOLUME, &pkt, NULL);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "set audio source volume error\n");
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

static int audio_set_aec(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    HisAVIContext *hisctx = s->priv_data;
    int aodev, aochn;
    HI_S32 s32Ret = HI_FALSE;
    BVAudioOutput *audio_output = (BVAudioOutput *) pkt_in->data;
    if (sscanf(audio_output->token, "%d/%d", &aodev, &aochn) != 2) {
        bv_log(s, BV_LOG_ERROR, "audio_output token error\n");
        return BVERROR(EINVAL);
    }

    s32Ret = HI_MPI_AI_EnableAec(hisctx->aidev, hisctx->aichn, aodev, aochn);
    BREAK_WHEN_SDK_FAILED("enable aec error", s32Ret);
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int video_set_imaging(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    HisAVIContext *hisctx = s->priv_data;
    BVImagingSettings *imaging = pkt_in->data;
    BVVideoSourceImaging source_imaging;
    BVControlPacket pkt;
    int ret = 0;
    bv_strlcpy(source_imaging.token, hisctx->vtoken, sizeof(source_imaging.token));
    source_imaging.imaging = *imaging;
    pkt.data = &source_imaging;
    pkt.size = 1;

    ret = bv_media_driver_control(hisctx->vdriver, BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_SOURCE_SET_IMAGING, &pkt, NULL);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "set video imaging error\n");
    }
    return ret; 
}

static bv_cold int his_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaMessageType type;
        int (*control)(BVMediaContext *s, const BVControlPacket *, BVControlPacket *);
    } media_control[] = {
        { BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME, audio_set_volume},
        { BV_MEDIA_MESSAGE_TYPE_AUDIO_MUTE, audio_set_mute},
        { BV_MEDIA_MESSAGE_TYPE_AUDIO_AEC, audio_set_aec},
        { BV_MEDIA_MESSAGE_TYPE_VIDEO_IMAGE, video_set_imaging},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(media_control); i++) {
        if (media_control[i].type == type)
           return media_control[i].control(s, pkt_in, pkt_out); 
    }

    bv_log(s, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(HisAVIContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "vchip", "", OFFSET(vchip), BV_OPT_TYPE_STRING, {.str = "tw2866"}, 0, 0, DEC},
    { "vdev", "",  OFFSET(vdev), BV_OPT_TYPE_STRING, {.str = "/dev/tw2865dev"}, 0, 0, DEC},
    { "vindex", "", OFFSET(vindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    { "width", "", OFFSET(width), BV_OPT_TYPE_INT, {.i64= 704}, -1, INT_MAX, DEC},
    { "height", "", OFFSET(height), BV_OPT_TYPE_INT, {.i64= 288}, -1, INT_MAX, DEC},
    { "framerate", "", OFFSET(framerate), BV_OPT_TYPE_INT, {.i64 = 25}, 2, 30, DEC},
    { "pix_fmt", "", OFFSET(pix_fmt), BV_OPT_TYPE_INT, {.i64 = BV_PIX_FMT_YUV420P}, BV_PIX_FMT_YUV420P, BV_PIX_FMT_YUV422P, DEC},
    { "atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "achip", "", OFFSET(achip), BV_OPT_TYPE_STRING, {.str = "tlv320aic23"}, 0, 0, DEC},
    { "adev", "",  OFFSET(adev), BV_OPT_TYPE_STRING, {.str = "/dev/tlv320aic23"}, 0, 0, DEC},
    { "aindex", "", OFFSET(aindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    { "sample_rate", "", OFFSET(sample_rate), BV_OPT_TYPE_INT, {.i64 = 8000}, -1, INT_MAX, DEC},
    { "channels", "", OFFSET(channels), BV_OPT_TYPE_INT, {.i64 = 1}, -1, INT_MAX, DEC},
    { NULL }
};

static const BVClass his_class = {
    .class_name         = "hisavi indev",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEMUXER,
};

BVInputMedia bv_hisavi_demuxer = {
    .name               = "hisavi",
    .priv_class         = &his_class,
    .priv_data_size     = sizeof(HisAVIContext),
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .read_probe         = his_probe,
    .read_header        = his_read_header,
    .read_packet        = his_read_packet,
    .read_close         = his_read_close,
    .media_control      = his_media_control,
};
