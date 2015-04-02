/*************************************************************************
    > File Name: hisavd.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月13日 星期五 17时37分12秒
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

#line 25 "hisavd.c"

#include <libbvmedia/bvmedia.h>

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
#include "mpi_vdec.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_adec.h"


/**
 *  His3515 Audio Video Decoder
 */

#define BREAK_WHEN_SDK_FAILED(comment, s32Ret) \
    do { \
        if (s32Ret != HI_SUCCESS) { \
            bv_log(s, BV_LOG_ERROR, comment " 0x%X\n", s32Ret); \
            goto fail; \
        } \
    }while(0)

typedef struct HisAVDContext {
    const BVClass *bv_class;
    char *vtoken;
    char *atoken;
    int vindex;
    int vodev;
    int vochn;
    int vdchn;
    int vpacked;

    int aindex;
    int aodev;
    int aochn;
    int adchn;
    int apacked;
    int abuf_size;
    uint8_t *abuf;
    uint8_t *abuf_ptr;
    uint8_t *abuf_end;
} HisAVDContext;

typedef struct IntTable {
    int a,b;
    int c;
} IntTable;

static int destroy_video_decode_channel(BVMediaContext *s)
{
    HisAVDContext *hisctx = s->priv_data;
    HI_MPI_VDEC_StopRecvStream(hisctx->vdchn);
    HI_MPI_VDEC_UnbindOutput(hisctx->vdchn);
    HI_MPI_VDEC_DestroyChn(hisctx->vdchn);
    return 0;
}

static int destroy_audio_decode_channel(BVMediaContext *s)
{
    HisAVDContext *hisctx = s->priv_data;
    HI_MPI_AO_UnBindAdec(hisctx->aodev, hisctx->aochn, hisctx->adchn);
    HI_MPI_ADEC_DestroyChn(hisctx->adchn); 
    return 0;
}

//FIXME Not Important
static int create_video_decode_channel(BVMediaContext *s, BVStream *stream)
{
    HisAVDContext *hisctx = s->priv_data;
    VDEC_CHN_ATTR_S stChnAttr;
    VDEC_ATTR_H264_S stH264Attr;
    HI_S32 s32Ret = HI_FAILURE;

    if (!hisctx->vtoken) {
        bv_log(s, BV_LOG_ERROR, "must set vtoken before create video decode channel\n");
        return BVERROR(EINVAL);
    }
    if (sscanf(hisctx->vtoken, "%d/%d/%d", &hisctx->vodev, &hisctx->vochn, &hisctx->vdchn) != 3) {
        bv_log(s, BV_LOG_ERROR, "vtoken param error\n");
        return BVERROR(EINVAL);
    }
    BBCLEAR_STRUCT(stChnAttr);
    BBCLEAR_STRUCT(stH264Attr);
    switch (stream->codec->codec_id) {
        case BV_CODEC_ID_H264:
        {
            stH264Attr.enMode = H264D_MODE_STREAM;
            if (hisctx->vpacked) {
                stH264Attr.enMode = H264D_MODE_FRAME;
            }
            stH264Attr.u32PicWidth  = stream->codec->width;
            stH264Attr.u32PicHeight = stream->codec->height;
            stH264Attr.u32Priority = 0;
            stH264Attr.u32RefFrameNum = 2;
            stChnAttr.enType = PT_H264;
            stChnAttr.u32BufSize = stH264Attr.u32PicWidth * stH264Attr.u32PicHeight *2;
            stChnAttr.pValue = &stH264Attr;
            break;
        }
        case BV_CODEC_ID_MPEG:
        {
            break;
        }
        case BV_CODEC_ID_JPEG:
        {
            break;
        }
        default:
        {
            bv_log(s, BV_LOG_ERROR, "not support this codec type\n");
            return BVERROR(EINVAL);
        }
    }
    s32Ret = HI_MPI_VDEC_CreateChn(hisctx->vdchn, &stChnAttr, NULL);
    BREAK_WHEN_SDK_FAILED("create video decode channel error", s32Ret);

    s32Ret = HI_MPI_VDEC_BindOutput(hisctx->vdchn, hisctx->vodev, hisctx->vochn);
    BREAK_WHEN_SDK_FAILED("bind output device error", s32Ret);
    
    s32Ret = HI_MPI_VDEC_StartRecvStream(hisctx->vdchn);
    BREAK_WHEN_SDK_FAILED("start receive stream error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "create video decode channel %s success\n", hisctx->vtoken);
    return 0;
fail:
    destroy_video_decode_channel(s);
    return BVERROR(EIO);
}

static int get_sample_format_size(enum BVSampleFormat fmt)
{
    int i = 0;
    IntTable fmts[] = {
        {BV_SAMPLE_FMT_U8, BV_SAMPLE_FMT_U8, 8},
        {BV_SAMPLE_FMT_S16, BV_SAMPLE_FMT_S16P, 16},
        {BV_SAMPLE_FMT_S32, BV_SAMPLE_FMT_S32P, 32},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(fmts); i++) {
        if ((fmts[i].a == fmt) || (fmts[i].b == fmt))
           return fmts[i].c; 
    }
    return 16;
}

static int create_audio_decode_channel(BVMediaContext *s, BVStream *stream)
{
    HisAVDContext *hisctx = s->priv_data;
    BVCodecContext *codec = stream->codec;
    int framerate = 25;
    int buf_size = 320;
    int sample_fmt = 16;
    ADEC_CHN_ATTR_S stChnAttr;
    
    ADEC_ATTR_LPCM_S stLpcmAttr;
    ADEC_ATTR_G711_S stG711Attr;
    ADEC_ATTR_G726_S stG726Attr;
    ADEC_ATTR_AAC_S  stAACAttr;
    ADEC_ATTR_ADPCM_S stAdpcmAttr;

    HI_S32 s32Ret = HI_FAILURE;
    if (!hisctx->atoken) {
        bv_log(s, BV_LOG_ERROR, "must set atoken before create audio decode channel\n");
        return BVERROR(EINVAL);
    }
    if (sscanf(hisctx->atoken, "%d/%d/%d", &hisctx->aodev, &hisctx->aochn, &hisctx->adchn) != 3) {
        bv_log(s, BV_LOG_ERROR, "atoken param error\n");
        return BVERROR(EINVAL);
    }

    BBCLEAR_STRUCT(stLpcmAttr);
    BBCLEAR_STRUCT(stG711Attr);
    BBCLEAR_STRUCT(stG726Attr);
    BBCLEAR_STRUCT(stChnAttr);

    HI_MPI_ADEC_DestroyChn(hisctx->adchn);
    
    switch (stream->codec->codec_id) {
        case BV_CODEC_ID_LPCM:
        {
            stChnAttr.enType = PT_LPCM;
            //stChnAttr.enMode = ADEC_MODE_PACK;
            stChnAttr.pValue = &stLpcmAttr;
            break;
        }
        case BV_CODEC_ID_G711A:
        {
            stChnAttr.enType = PT_G711A;
            stChnAttr.pValue = &stG711Attr;
            break;
        }
        case BV_CODEC_ID_G711U:
        {
            stChnAttr.enType = PT_G711U;
            stChnAttr.pValue = &stG711Attr;
            break;
        }
        case BV_CODEC_ID_G726:
        {
            stG726Attr.enG726bps = MEDIA_G726_32K;
            stChnAttr.enType = PT_G726;
            stChnAttr.pValue = &stG726Attr;
            break;
        }
        case BV_CODEC_ID_ADPCM:
        {
            stChnAttr.enType = PT_ADPCMA;
            stAdpcmAttr.enADPCMType = ADPCM_TYPE_IMA; //NOTICE
            stChnAttr.pValue = &stAdpcmAttr;
            break;
        }
        case BV_CODEC_ID_AAC:
        {
            stChnAttr.enType = PT_AAC;
            stChnAttr.pValue = &stAACAttr;
            break;
        }
        default:
        {
            bv_log(s, BV_LOG_ERROR, "not support this codec type\n");
            return BVERROR(EINVAL);
        }
    }
    stChnAttr.enMode = ADEC_MODE_STREAM;
    if (hisctx->apacked) {
        stChnAttr.enMode = ADEC_MODE_PACK;
    }
    stChnAttr.u32BufSize = 25;
  
    s32Ret = HI_MPI_ADEC_CreateChn(hisctx->adchn, &stChnAttr);
    BREAK_WHEN_SDK_FAILED("create adec channel error", s32Ret);
    
    s32Ret = HI_MPI_AO_BindAdec(hisctx->aodev, hisctx->aochn, hisctx->adchn);
    BREAK_WHEN_SDK_FAILED("bind adec channel error", s32Ret);

    sample_fmt = get_sample_format_size(codec->sample_fmt);
    if (!codec->time_base.den || !codec->time_base.num) {
        bv_log(s, BV_LOG_ERROR, "codec time base error using default 25\n");
    } else {
        framerate = codec->time_base.den / codec->time_base.num;
    }
    if (!framerate) {
        framerate = 25;
    }
    buf_size = codec->sample_rate * sample_fmt * codec->channels / (framerate * 8);
    if (codec->codec_id == BV_CODEC_ID_LPCM) {
        hisctx->abuf_size = buf_size;
    } else if (codec->codec_id == BV_CODEC_ID_G726){
        hisctx->abuf_size = buf_size / 4;
    } else {
        hisctx->abuf_size = buf_size / 2;
    }
    hisctx->abuf = bv_mallocz(hisctx->abuf_size);
    if (!hisctx->abuf) {
        bv_log(s, BV_LOG_ERROR, "alloc audio buf size error\n");
    }
    hisctx->abuf_ptr = hisctx->abuf;
    hisctx->abuf_end = hisctx->abuf + hisctx->abuf_size;
    bv_log(s, BV_LOG_DEBUG, "create audio decode channel %s success\n", hisctx->atoken);
    return 0;
fail:
    destroy_audio_decode_channel(s);
    return BVERROR(EINVAL);
}

static int his_write_trailer(BVMediaContext *s);

static int his_write_header(BVMediaContext *s)
{
    HisAVDContext *hisctx = s->priv_data;
    BVStream *stream = NULL;
    int ret = 0;
    int i = 0;
    for (i = 0; i < s->nb_streams; i++) {
        stream = s->streams[i];
        if (stream->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
            if (create_video_decode_channel(s, stream) < 0) {
                bv_log(s, BV_LOG_ERROR, "create video decode channel error\n");
                ret = BVERROR(EINVAL);
                break;
            }
            hisctx->vindex = i;
        } else if(stream->codec->codec_type == BV_MEDIA_TYPE_AUDIO) {
            if (create_audio_decode_channel(s, stream) < 0) {
                bv_log(s, BV_LOG_ERROR, "create audio decode channel error\n");
                ret = BVERROR(EINVAL);
                break;
            }
            hisctx->aindex = i;
        } else {
            bv_log(s, BV_LOG_WARNING, "his3515 not support this media type\n");
        }
    }
    if (ret < 0) {
        his_write_trailer(s);
        return ret;
    }
    bv_log(s, BV_LOG_DEBUG, "create audio video decode channel success\n");
    return ret;
}

static int write_video_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVDContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    VDEC_STREAM_S stStream;
    stStream.pu8Addr = pkt->data;
    stStream.u32Len  = pkt->size;
    stStream.u64PTS  = pkt->pts;
    s32Ret = HI_MPI_VDEC_SendStream(hisctx->vdchn, &stStream, HI_IO_BLOCK);
    BREAK_WHEN_SDK_FAILED("send video stream error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "write video stream ok\n");
    return pkt->size;
fail:
    return BVERROR(EIO);
}

static int write_audio_packet_internal(BVMediaContext *s, uint8_t *data, int size)
{
    HisAVDContext *hisctx = s->priv_data;
    AUDIO_STREAM_S stStream;
    HI_S32 s32Ret = HI_FAILURE;
    if (size <= 0) {
        return 0;
    }
    BBCLEAR_STRUCT(stStream);
    if (s->streams[hisctx->aindex]->codec->codec_id != BV_CODEC_ID_LPCM) {
        HI_S16 HiAudioHead[2] = {0x01<<8, 0};//海思音频流头
        HiAudioHead[1] = size / sizeof(HI_S16);
        stStream.u32Len = 4;
        stStream.u64TimeStamp = 0;
        stStream.pStream = (uint8_t *)HiAudioHead;
        stStream.u32Seq = 0;

        s32Ret = HI_MPI_ADEC_SendStream(hisctx->adchn, &stStream, HI_IO_BLOCK);
        BREAK_WHEN_SDK_FAILED("send audio stream header error", s32Ret);
        bv_log(s, BV_LOG_DEBUG, "write audio stream header ok\n");
    }
    BBCLEAR_STRUCT(stStream);
    stStream.u32Len = size;
    stStream.u64TimeStamp = 0;
    stStream.pStream = data;
    stStream.u32Seq = 0;

    s32Ret = HI_MPI_ADEC_SendStream(hisctx->adchn, &stStream, HI_IO_BLOCK);
    BREAK_WHEN_SDK_FAILED("send audio stream error", s32Ret);
    return size;
fail:
    return BVERROR(EIO);
}

static int write_audio_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVDContext *hisctx = s->priv_data;
    int size = pkt->size;
    uint8_t *buffer = pkt->data;
    if (hisctx->apacked) {
        return write_audio_packet_internal(s, buffer, size);
    }

    while (size > 0) {
        int len = BBMIN(hisctx->abuf_end - hisctx->abuf_ptr, size);
        memcpy(hisctx->abuf_ptr, buffer, len);
        hisctx->abuf_ptr += len;

        if (hisctx->abuf_ptr  >= hisctx->abuf_end) {
            if (write_audio_packet_internal(s, hisctx->abuf, hisctx->abuf_ptr - hisctx->abuf) < 0) {
                bv_log(s, BV_LOG_ERROR, "write audio stream error\n");
                goto fail;
            }
            hisctx->abuf_ptr = hisctx->abuf;
        }
        buffer += len;
        size -= len;
    }
    bv_log(s, BV_LOG_DEBUG, "write audio stream ok\n");
    return pkt->size;
fail:
    return BVERROR(EIO);
}

static int his_write_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVDContext *hisctx = s->priv_data;
    if (pkt->stream_index == hisctx->vindex) {
        return write_video_packet(s, pkt);
    } else {
        return write_audio_packet(s, pkt);
    }
    return BVERROR(EINVAL);
}

static int his_write_trailer(BVMediaContext *s)
{   
    HisAVDContext *hisctx = s->priv_data;
    if (hisctx->vindex != -1) {
        destroy_video_decode_channel(s);
    }
    if (hisctx->aindex != -1) {
        if (!hisctx->apacked) {
            write_audio_packet_internal(s, hisctx->abuf, hisctx->abuf_ptr - hisctx->abuf);
        }
        destroy_audio_decode_channel(s);
        bv_free(hisctx->abuf);
    }
    return 0;
}

static int his_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(HisAVDContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "vindex", "", OFFSET(vindex), BV_OPT_TYPE_INT, {.i64 = -1}, -1, 128, DEC},
    { "vpacked", "", OFFSET(vpacked), BV_OPT_TYPE_INT, {.i64 = 1}, 0, 1, DEC},
    { "atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "aindex", "", OFFSET(aindex), BV_OPT_TYPE_INT, {.i64 = -1}, -1, 128, DEC},
    { "apacked", "", OFFSET(apacked), BV_OPT_TYPE_INT, {.i64 = 1}, -1, 128, DEC},
    {NULL}
};

static const BVClass his_class = {
    .class_name         = "hisavd muxer",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_MUXER,
};

BVOutputMedia bv_hisavd_muxer = {
    .name               = "hisavd",
    .mime_type          = "video/x-bsvideo",
    .priv_class         = &his_class,
    .priv_data_size     = sizeof(HisAVDContext),
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .write_header       = his_write_header,
    .write_packet       = his_write_packet,
    .write_trailer      = his_write_trailer,
    .media_control      = his_media_control,
};

