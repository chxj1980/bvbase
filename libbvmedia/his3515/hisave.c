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

#line 25 "hisave.c"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <libbvutil/bvstring.h>
#include <libbvutil/log.h>
#include <libbvutil/opt.h>
#include <libbvutil/time.h>

#include <libbvmedia/bvmedia.h>

//FIXME c99 not support asm
#define asm __asm__

//His3515 headers
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_aenc.h"
#include "mpi_venc.h"

#include "drv.h"

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

#define MAX_PACKETS (8)

typedef struct HisAVEContext {
    BVClass *bv_class;
    char *vtoken;   //videv/vichn/vechn
    int videv;
    int vichn;
    int vechn;
    int vindex;
    int vfd;
    enum BVCodecID vcodec_id;
    enum BVRCModeID mode_id;
    int width, height;
    int quality;
    int bit_rate;
    int gop_size;
    int framerate;
    VENC_PACK_S *pstPack;
    BVPacket *packet;

    char *atoken;
    int aidev;
    int aichn;
    int aechn;
    int aindex;
    int afd;
    enum BVCodecID acodec_id;
    int abit_rate;      //audio bit_rate;
    int sample_rate;
    int channels;
    int blocked;
} HisAVEContext;

typedef struct IntTable {
    int a;
    int b;
} IntTable;

static int his_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->filename, "hisave:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

static int destroy_audio_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    HI_MPI_AENC_UnBindAi(hisctx->aechn, hisctx->aidev, hisctx->aichn);
    HI_MPI_AENC_DestroyChn(hisctx->aechn);
    return 0;
}

static int destroy_video_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    bv_free(hisctx->pstPack);
    s32Ret = HI_MPI_VENC_StopRecvPic(hisctx->vechn);
    s32Ret = HI_MPI_VENC_UnRegisterChn(hisctx->vechn);
    s32Ret = HI_MPI_VENC_DestroyChn(hisctx->vechn);
    s32Ret = HI_MPI_VENC_UnbindInput(hisctx->vechn);
    s32Ret = HI_MPI_VENC_DestroyGroup(hisctx->vechn);
    return 0;
}

static int create_audio_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    AENC_CHN_ATTR_S stChnAttr; 
    AENC_ATTR_G711_S stG711Attr;
    AENC_ATTR_G726_S stG726Attr;
    AENC_ATTR_LPCM_S stLpcmAttr;
    HI_S32 s32Ret = HI_FAILURE;

    BBCLEAR_STRUCT(stChnAttr);
    BBCLEAR_STRUCT(stG711Attr);
    BBCLEAR_STRUCT(stG726Attr);
    BBCLEAR_STRUCT(stLpcmAttr);

    if (sscanf(hisctx->atoken, "%d/%d/%d", &hisctx->aidev, &hisctx->aichn, &hisctx->aechn) != 3) {
        bv_log(s, BV_LOG_ERROR, "atoken param error %s\n", hisctx->atoken);
        return BVERROR(EINVAL);
    }

    //FIXME
    switch (hisctx->acodec_id) {
        case BV_CODEC_ID_G726:
        {
            stChnAttr.enType = PT_G726;
            if (hisctx->abit_rate == 16000) {
                stG726Attr.enG726bps = MEDIA_G726_16K;
            } else {
                stG726Attr.enG726bps = MEDIA_G726_32K;
            }
            stChnAttr.pValue = &stG726Attr;
            break;
        }
        case BV_CODEC_ID_LPCM:
        {
            stChnAttr.enType = PT_LPCM;
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
        default:
        {
            stChnAttr.enType = PT_G711U;
            stChnAttr.pValue = &stG711Attr;
            break;
        }
    }
    stChnAttr.u32BufSize = 30;
    s32Ret = HI_MPI_AENC_CreateChn(hisctx->aechn, &stChnAttr);
    BREAK_WHEN_SDK_FAILED("create aenc channel error", s32Ret);
    s32Ret = HI_MPI_AENC_BindAi(hisctx->aechn, hisctx->aidev, hisctx->aichn);
    BREAK_WHEN_SDK_FAILED("bind aichn error", s32Ret);
    
    hisctx->afd = HI_MPI_AENC_GetFd(hisctx->aechn);
    bv_log(s, BV_LOG_DEBUG, "create audio encode channel %s success\n", hisctx->atoken);
    return 0;
fail:
    return BVERROR(EIO);
}

static int get_video_rate_control_mode(int mode)
{
    int i = 0;
    IntTable ctrl_modes[] = {
        { BV_RC_MODE_ID_VBR, RC_MODE_VBR},
        { BV_RC_MODE_ID_CBR, RC_MODE_CBR},
        { BV_RC_MODE_ID_ABR, RC_MODE_ABR},
        { BV_RC_MODE_ID_BUTT, RC_MODE_BUTT},
        { BV_RC_MODE_ID_FIXQP, RC_MODE_FIXQP}
    };
    for (i = 0; i < BV_ARRAY_ELEMS(ctrl_modes); i++) {
        if (ctrl_modes[i].a == mode)
           return ctrl_modes[i].b; 
    }
    return RC_MODE_CBR;
}

static int create_h264_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    VENC_CHN_ATTR_S stChnAttr;
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_RC_S stH264Rc;
    HI_S32 s32Ret = HI_FAILURE;
    int mode = VIDEO_STD_PAL;
    int framerate = 25;

    BBCLEAR_STRUCT(stH264Attr);
    BBCLEAR_STRUCT(stH264Rc);

    VideoInGetStd(hisctx->videv, hisctx->vichn, &mode);
    if (mode == VIDEO_STD_NTSC) {
        framerate = 30;
    }
    stH264Attr.bByFrame = HI_TRUE;
    stH264Attr.bField = HI_FALSE;
    stH264Attr.bMainStream = HI_TRUE;
    stH264Attr.bVIField = HI_FALSE;
    stH264Attr.enRcMode = get_video_rate_control_mode(hisctx->mode_id);
    stH264Attr.s32Minutes = 10;
    stH264Attr.u32Bitrate = hisctx->bit_rate;
    stH264Attr.u32BufSize = 704 * 576 * 4;
    stH264Attr.u32Gop = hisctx->gop_size;
    stH264Attr.u32PicHeight = hisctx->height;
    stH264Attr.u32PicWidth = hisctx->width;
    stH264Attr.u32PicLevel = hisctx->quality;
    stH264Attr.u32ViFramerate = framerate;
    stH264Attr.u32TargetFramerate = hisctx->framerate;
    
    stChnAttr.enType = PT_H264;
    stChnAttr.pValue = &stH264Attr;

    s32Ret = HI_MPI_VENC_CreateChn(hisctx->vechn, &stChnAttr, NULL);
    BREAK_WHEN_SDK_FAILED("create video encode channel error", s32Ret);
    s32Ret = HI_MPI_VENC_GetH264eRcPara(hisctx->vechn, &stH264Rc);
    BREAK_WHEN_SDK_FAILED("get H264 RcPara error", s32Ret);
    stH264Rc.s32MinQP = 28; //24	32
    stH264Rc.s32MaxQP = 38; //28	38
    stH264Rc.s32IOsdMaxQp = 45;
    stH264Rc.s32POsdMaxQp = 50;

    s32Ret = HI_MPI_VENC_SetH264eRcPara(hisctx->vechn, &stH264Rc);
    BREAK_WHEN_SDK_FAILED("set H264 RcPara error", s32Ret);

    s32Ret = HI_MPI_VENC_RegisterChn(hisctx->vechn, hisctx->vechn);
    BREAK_WHEN_SDK_FAILED("register video encode channel error", s32Ret);

    s32Ret = HI_MPI_VENC_StartRecvPic(hisctx->vechn);
    BREAK_WHEN_SDK_FAILED("video encode channel start recieve pic error", s32Ret);
    hisctx->vfd = HI_MPI_VENC_GetFd(hisctx->vechn); 
    return 0;
fail:
    return BVERROR(EIO);
}

static int create_mpeg_encode_channel(BVMediaContext *s)
{
    return BVERROR(ENOSYS);
}

static int create_jpeg_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    VENC_CHN_ATTR_S stChnAttr;
    VENC_ATTR_JPEG_S stJpegAttr;
    HI_S32 s32Ret = HI_FAILURE;

    BBCLEAR_STRUCT(stJpegAttr);

    stJpegAttr.bByFrame = HI_TRUE;
    stJpegAttr.bVIField = HI_TRUE;
    stJpegAttr.u32BufSize = hisctx->width * hisctx->height * 2;
    stJpegAttr.u32ImageQuality = hisctx->quality;
    stJpegAttr.u32MCUPerECS = 0;
    stJpegAttr.u32PicHeight = hisctx->height;
    stJpegAttr.u32PicWidth  = hisctx->width;
    stJpegAttr.u32Priority = 0;
    
    stChnAttr.enType = PT_JPEG;
    stChnAttr.pValue = &stJpegAttr;

    s32Ret = HI_MPI_VENC_CreateChn(hisctx->vechn, &stChnAttr, NULL);
    BREAK_WHEN_SDK_FAILED("create video encode channel error", s32Ret);
    s32Ret = HI_MPI_VENC_SetMaxStreamCnt(hisctx->vechn, 1);
    BREAK_WHEN_SDK_FAILED("set jpeg max stream count error", s32Ret);
    s32Ret = HI_MPI_VENC_RegisterChn(hisctx->vechn, hisctx->vechn);
    BREAK_WHEN_SDK_FAILED("register video encode channel error", s32Ret);
    s32Ret = HI_MPI_VENC_SetMaxStreamCnt(hisctx->vechn, hisctx->framerate);
    BREAK_WHEN_SDK_FAILED("set jpeg max stream count error", s32Ret);

    s32Ret = HI_MPI_VENC_StartRecvPic(hisctx->vechn);
    BREAK_WHEN_SDK_FAILED("video encode channel start recieve pic error", s32Ret);
    hisctx->vfd = HI_MPI_VENC_GetFd(hisctx->vechn); 
    return 0;
fail:
    return BVERROR(EIO);
}

static int create_video_encode_channel(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;

    HI_S32 s32Ret = HI_FAILURE; 
    if (sscanf(hisctx->vtoken, "%d/%d/%d", &hisctx->videv, &hisctx->vichn, &hisctx->vechn) !=3) {
        bv_log(s, BV_LOG_ERROR, "vtoken param error %s\n", hisctx->vtoken);
        return BVERROR(EINVAL);
    }

    s32Ret = HI_MPI_VENC_CreateGroup(hisctx->vechn);
    BREAK_WHEN_SDK_FAILED("create venc group error", s32Ret);

    s32Ret = HI_MPI_VENC_BindInput(hisctx->vechn, hisctx->videv, hisctx->vichn);
    BREAK_WHEN_SDK_FAILED("venc bind input error", s32Ret);
    
    switch (hisctx->vcodec_id) {
        case BV_CODEC_ID_MPEG:
        {
            s32Ret = create_mpeg_encode_channel(s);
            break;
        }
        case BV_CODEC_ID_JPEG:
        {
            s32Ret = create_jpeg_encode_channel(s);
            break;
        }
        case BV_CODEC_ID_H264:
        default:
        {
            s32Ret = create_h264_encode_channel(s);
            break;
        }
    }
    if (s32Ret < 0) {
        goto fail;
    }
    bv_log(s, BV_LOG_DEBUG, "create video encode channel %s success\n", hisctx->vtoken);
    return 0;
fail:
    destroy_video_encode_channel(s);
    return BVERROR(EIO);
}

static int add_audio_encode_stream(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    BVStream *st = bv_stream_new(s, NULL);
    hisctx->aindex = st->index;
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->codec_id = hisctx->acodec_id;
    st->time_base = (BVRational) {1, 1000000};
    st->codec->sample_rate = hisctx->sample_rate;
    st->codec->bit_rate = hisctx->abit_rate;
    st->codec->channels = hisctx->channels;
    return 0;
}

static int add_video_encode_stream(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    BVStream *st = bv_stream_new(s, NULL);
    hisctx->pstPack = (VENC_PACK_S *) bv_malloc(sizeof(VENC_PACK_S) * MAX_PACKETS);
    hisctx->vindex = st->index;
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->width = hisctx->width;
    st->codec->height = hisctx->height;
    st->time_base = (BVRational) {1, 1000000};
    st->codec->time_base = (BVRational) {1, hisctx->framerate};
    return 0;
}

static int read_video_packet(BVMediaContext *s, BVPacket *pkt);

//FIXME
static int get_video_extradata(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    BVCodecParserContext *parser = NULL;
    BVCodecContext *codec = s->streams[hisctx->vindex]->codec;
    BVPacket packet;
    int ret = 0;
    if (hisctx->vcodec_id != BV_CODEC_ID_H264)
        return 0;
    parser = bv_codec_parser_init(hisctx->vcodec_id);
    if (!parser) {
        return BVERROR(ENOSYS);
    }
    //get extradata from packet
    while (1) {
        bv_packet_init(&packet);
        if(read_video_packet(s, &packet) < 0) {
            continue;
        }

        if (!(packet.flags & BV_PKT_FLAG_KEY)) {
            bv_packet_free(&packet);
            continue;
        }
        if (bv_codec_parser_parse(parser, codec,packet.data, packet.size, NULL, NULL) < 0) {
            bv_log(s, BV_LOG_ERROR, "parser H264 extradata error\n");
            bv_packet_free(&packet);
            ret = BVERROR(EIO);
        }
        break;
     }
    if (ret == 0) {
        hisctx->packet = (BVPacket *)bv_mallocz(sizeof(BVPacket));
        if (!hisctx->packet) {
            bv_log(s, BV_LOG_ERROR, "alloc packet for extradata error\n");
            bv_packet_free(&packet);
            bv_codec_parser_exit(parser);
            return BVERROR(ENOMEM);
        }
        bv_packet_copy(hisctx->packet, &packet);
        bv_packet_free(&packet);
        bv_log(s, BV_LOG_DEBUG, "extradata_size %d\n", codec->extradata_size);
    }
    bv_codec_parser_exit(parser);
    return ret;
}

static bv_cold int his_read_close(BVMediaContext *s);

static bv_cold int his_read_header(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    if (hisctx->atoken) {
        if (create_audio_encode_channel(s) < 0) {
            goto fail;
        }
        add_audio_encode_stream(s);
    }

    if (hisctx->vtoken) {
        if (create_video_encode_channel(s) < 0) {
            goto fail;
        }
        add_video_encode_stream(s);
        if (hisctx->vcodec_id == BV_CODEC_ID_H264) {
            if (get_video_extradata(s) < 0) {
                bv_log(s, BV_LOG_ERROR, "get video extradata error\n");
                goto fail;
            }
        }
    }
    return 0;
fail:
    his_read_close(s);
    return BVERROR(ENOSYS);
}

static int read_audio_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVEContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    AUDIO_STREAM_S stStream;
    s32Ret = HI_MPI_AENC_GetStream(hisctx->aechn, &stStream, HI_IO_NOBLOCK);
    BREAK_WHEN_SDK_FAILED("get audio encode stream error", s32Ret);
    if (bv_packet_new(pkt, stStream.u32Len) < 0) {
        HI_MPI_AENC_ReleaseStream(hisctx->aechn, &stStream);
        return BVERROR(ENOMEM);
    }
    if (hisctx->acodec_id != BV_CODEC_ID_LPCM) {
        memcpy(pkt->data, stStream.pStream + 4, stStream.u32Len - 4);
        pkt->size = stStream.u32Len - 4;
    } else {
        memcpy(pkt->data, stStream.pStream, stStream.u32Len);
        pkt->size = stStream.u32Len;
    }
    pkt->pts = stStream.u64TimeStamp;
    pkt->dts = pkt->pts;
    pkt->flags |= BV_PKT_FLAG_KEY;
    pkt->stream_index = hisctx->aindex;
    HI_MPI_AENC_ReleaseStream(hisctx->aechn, &stStream);
    return pkt->size;
fail:
    return BVERROR(EIO);
}

static int read_h264_video_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVEContext *hisctx = s->priv_data;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret = HI_FAILURE;
    int len = 0, i = 0;
    
    s32Ret = HI_MPI_VENC_Query(hisctx->vechn, &stStat);
    BREAK_WHEN_SDK_FAILED("get video encode status error", s32Ret);
    if (stStat.u32CurPacks == 0) {
        return BVERROR(EAGAIN);
    }
    stStream.u32PackCount = stStat.u32CurPacks;
    stStream.pstPack = hisctx->pstPack;

    s32Ret = HI_MPI_VENC_GetStream(hisctx->vechn, &stStream, HI_IO_NOBLOCK);
    BREAK_WHEN_SDK_FAILED("get video encode stream error", s32Ret);

    for (i = 0; i < stStream.u32PackCount; i++) {
        len += stStream.pstPack[i].u32Len[0];
        if (stStream.pstPack[i].u32Len[1])
            len += stStream.pstPack[i].u32Len[1];
    }
    if (bv_packet_new(pkt, len) < 0) {
        HI_MPI_VENC_ReleaseStream(hisctx->vechn, &stStream);
        return BVERROR(ENOMEM);
    }
    pkt->size = len;
    len = 0;
    for (i = 0; i < stStream.u32PackCount; i++) {
        memcpy(pkt->data + len, stStream.pstPack[i].pu8Addr[0], stStream.pstPack[i].u32Len[0]);
        len += stStream.pstPack[i].u32Len[0];
        if (stStream.pstPack[i].u32Len[1]) {
            memcpy(pkt->data + len, stStream.pstPack[i].pu8Addr[1], stStream.pstPack[i].u32Len[1]);
            len += stStream.pstPack[i].u32Len[1];
        }
    }
    pkt->pts = stStream.pstPack[0].u64PTS;
    pkt->dts = pkt->pts;
    pkt->stream_index = hisctx->vindex;
    if (stStream.u32PackCount == 4)
        pkt->flags |= BV_PKT_FLAG_KEY;

    HI_MPI_VENC_ReleaseStream(hisctx->vechn, &stStream);
    
    return pkt->size;
fail:
    return BVERROR(EIO);
}

static int read_jpeg_video_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVEContext *hisctx = s->priv_data;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret = HI_FAILURE;
    int len = 0, i = 0;
    const uint8_t JpegSOI[2] = {0xFF, 0xD8};
    const uint8_t JpegEOI[2] = {0xFF, 0xD9};
    
    s32Ret = HI_MPI_VENC_Query(hisctx->vechn, &stStat);
    BREAK_WHEN_SDK_FAILED("get video encode status error", s32Ret);
    if (stStat.u32CurPacks == 0) {
        return BVERROR(EAGAIN);
    }
    stStream.u32PackCount = stStat.u32CurPacks;
    stStream.pstPack = hisctx->pstPack;

    s32Ret = HI_MPI_VENC_GetStream(hisctx->vechn, &stStream, HI_IO_NOBLOCK);
    BREAK_WHEN_SDK_FAILED("get video encode stream error", s32Ret);
    
    len = sizeof(JpegSOI) + sizeof(JpegEOI);

    for (i = 0; i < stStream.u32PackCount; i++) {
        len += stStream.pstPack[i].u32Len[0];
        if (stStream.pstPack[i].u32Len[1])
            len += stStream.pstPack[i].u32Len[1];
    }
    if (bv_packet_new(pkt, len) < 0) {
        HI_MPI_VENC_ReleaseStream(hisctx->vechn, &stStream);
        return BVERROR(ENOMEM);
    }
    pkt->size = len;
    memcpy(pkt->data, JpegSOI, sizeof(JpegSOI));
    len = sizeof(JpegSOI);
    for (i = 0; i < stStream.u32PackCount; i++) {
        memcpy(pkt->data + len, stStream.pstPack[i].pu8Addr[0], stStream.pstPack[i].u32Len[0]);
        len += stStream.pstPack[i].u32Len[0];
        if (stStream.pstPack[i].u32Len[1]) {
            memcpy(pkt->data + len, stStream.pstPack[i].pu8Addr[1], stStream.pstPack[i].u32Len[1]);
            len += stStream.pstPack[i].u32Len[1];
        }
    }
    memcpy(pkt->data + len, JpegEOI, sizeof(JpegEOI));
    pkt->pts = stStream.pstPack[0].u64PTS;
    pkt->dts = pkt->pts;
    pkt->stream_index = hisctx->vindex;
    pkt->flags |= BV_PKT_FLAG_KEY;

    HI_MPI_VENC_ReleaseStream(hisctx->vechn, &stStream);
    
    return pkt->size;
fail:
    return BVERROR(EIO);
}

static int read_video_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVEContext *hisctx = s->priv_data;
    if (hisctx->packet) {
        bv_packet_copy(pkt, hisctx->packet); 
        bv_packet_free(hisctx->packet);
        bv_freep(&hisctx->packet);
        hisctx->packet = NULL;
        return pkt->size;
    }
    if (hisctx->vcodec_id == BV_CODEC_ID_H264) {
        return read_h264_video_packet(s, pkt);
    }

    if (hisctx->vcodec_id == BV_CODEC_ID_JPEG) {
        return read_jpeg_video_packet(s, pkt);
    }

    return BVERROR(EIO);
}

static bv_cold int his_read_packet(BVMediaContext *s, BVPacket *pkt)
{
    HisAVEContext *hisctx = s->priv_data;
	struct timeval tv;
    int ret = 0;
	fd_set fds;
    FD_ZERO(&fds);
    if (hisctx->atoken && hisctx->afd >= 0) {
        FD_SET(hisctx->afd, &fds);
    }
    if (hisctx->vtoken && hisctx->vfd >= 0) {
        FD_SET(hisctx->vfd, &fds);
    }
    BBCLEAR_STRUCT(tv);
    if (hisctx->blocked) {
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
    }
    ret = select(BBMAX(hisctx->afd, hisctx->vfd) + 1, &fds, NULL, NULL, &tv);
    if (ret < 0) {
        bv_log(s, BV_LOG_ERROR, "select error %d\n", ret);
        return BVERROR(errno);
    }
    if (ret == 0) {
        //bv_log(s, BV_LOG_DEBUG, "select timeout\n");
        return BVERROR(EAGAIN);
    }

    if (hisctx->atoken && hisctx->afd >= 0 && FD_ISSET(hisctx->afd, &fds)) {
        return read_audio_packet(s, pkt);
    }

    if (hisctx->vtoken && hisctx->vfd >= 0 && FD_ISSET(hisctx->vfd, &fds)) {
        return read_video_packet(s, pkt);
    }
    return BVERROR(EIO);
}

static bv_cold int his_read_close(BVMediaContext *s)
{
    HisAVEContext *hisctx = s->priv_data;
    if (hisctx->atoken) {
        destroy_audio_encode_channel(s);
    }
    if (hisctx->packet) {
        bv_packet_free(hisctx->packet);
        bv_freep(&hisctx->packet);
    }
    if (hisctx->vtoken) {
        destroy_video_encode_channel(s);
    }
    return 0;
}

static int video_update_config(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int video_request_key_frame(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int osd_create(BVMediaContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static bv_cold int his_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaMessageType type;
        int (*control)(BVMediaContext *s, const BVControlPacket *, BVControlPacket *);
    } media_control[] = {
        { BV_MEDIA_MESSAGE_TYPE_VIDEO_UPCFG, video_update_config},
        { BV_MEDIA_MESSAGE_TYPE_VIDEO_RSKFM, video_request_key_frame},
        { BV_MEDIA_MESSAGE_TYPE_OSD_CREATE, osd_create},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(media_control); i++) {
        if (media_control[i].type == type)
           return media_control[i].control(s, pkt_in, pkt_out); 
    }

    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(HisAVEContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "vindex", "", OFFSET(vindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    { "vcodec_id", "", OFFSET(vcodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_H264}, 0, INT_MAX, DEC},
    { "mode_id", "", OFFSET(mode_id), BV_OPT_TYPE_INT, {.i64 = BV_RC_MODE_ID_CBR}, 0, 255, DEC},
    { "width", "", OFFSET(width), BV_OPT_TYPE_INT, {.i64 = 704}, 0, INT_MAX, DEC},
    { "height", "", OFFSET(height), BV_OPT_TYPE_INT, {.i64 = 576}, 0, INT_MAX, DEC},
    { "quality", "", OFFSET(quality), BV_OPT_TYPE_INT, {.i64 = 3}, 0, INT_MAX, DEC},
    { "bit_rate", "", OFFSET(bit_rate), BV_OPT_TYPE_INT, {.i64 = 1024}, 0, INT_MAX, DEC},
    { "gop_size", "", OFFSET(gop_size), BV_OPT_TYPE_INT, {.i64 = 100}, 0, INT_MAX, DEC},
    { "framerate", "", OFFSET(framerate), BV_OPT_TYPE_INT, {.i64 = 25}, 0, 120, DEC},
    { "atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    { "aindex", "", OFFSET(aindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    { "acodec_id", "", OFFSET(acodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_G711A}, 0, INT_MAX, DEC},
    { "abit_rate", "", OFFSET(abit_rate), BV_OPT_TYPE_INT, {.i64 = 32000}, 0, INT_MAX, DEC},
    { "sample_rate", "", OFFSET(sample_rate), BV_OPT_TYPE_INT, {.i64 = 8000}, 0, INT_MAX, DEC},
    { "channels", "", OFFSET(channels), BV_OPT_TYPE_INT, {.i64 = 1}, 0, 2, DEC},
    { "blocked", "", OFFSET(blocked), BV_OPT_TYPE_INT, {.i64 = 0}, 0, 2, DEC},
    { NULL },
};

static const BVClass his_class = {
    .class_name         = "hisave indev",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEMUXER,
};

BVInputMedia bv_hisave_demuxer = {
    .name               = "hisave",
    .priv_class         = &his_class,
    .priv_data_size     = sizeof(HisAVEContext),
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .read_probe         = his_probe,
    .read_header        = his_read_header,
    .read_packet        = his_read_packet,
    .read_close         = his_read_close,
    .media_control      = his_media_control,
};
