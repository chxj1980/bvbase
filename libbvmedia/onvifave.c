/*************************************************************************
    > File Name: onvifave.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年11月04日 星期二 11时54分34秒
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

/**
 * @file
 * onvif video encode main file
 */
#include <libavformat/avformat.h>

#include <libbvutil/bvstring.h>
#include <libbvutil/time.h>
#include <libbvutil/log.h>
#include <libbvutil/opt.h>

#include <wsseapi.h>
#include <wsaapi.h>
#include "bvmedia.h"

typedef struct OnvifContext {
    BVClass *bv_class;
    char svrurl[1024];
    char *vtoken;
    char *atoken;
    int vindex;
    int aindex;
    char *user;
    char *passwd;
    enum BVCodecID vcodec_id;
    BVRational video_rate;
    int width, height;
    int framerate;
    char onvif_url[128];
    char profile_name[32];
    char profile_token[32];
    char *media_url;
    int timeout;
    struct soap *soap;
    AVFormatContext *onvif;
    BVIOContext *snapshot;
    struct SOAP_ENV__Header soap_header;
} OnvifContext;

static int onvif_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->filename, "onvifave:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

static enum BVCodecID avid_to_bvid(enum AVCodecID codec_id)
{
    int i = 0;
    struct {
        enum AVCodecID avid;
        enum BVCodecID bvid;
    }IDS[] = {
        {AV_CODEC_ID_NONE, BV_CODEC_ID_NONE},
        {AV_CODEC_ID_H264, BV_CODEC_ID_H264},
        {AV_CODEC_ID_MPEG4, BV_CODEC_ID_MPEG},
        {AV_CODEC_ID_JPEG2000, BV_CODEC_ID_JPEG},
        {AV_CODEC_ID_PCM_ALAW, BV_CODEC_ID_G711A},
        {AV_CODEC_ID_PCM_MULAW, BV_CODEC_ID_G711U},
        {AV_CODEC_ID_ADPCM_G726, BV_CODEC_ID_G726},
        {AV_CODEC_ID_AAC, BV_CODEC_ID_AAC},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].avid == codec_id)
            return IDS[i].bvid;
    }
    return BV_CODEC_ID_NONE;
}

static void set_video_info(OnvifContext *onvifctx, BVStream *bvst, AVStream *avst)
{
    bvst->time_base = (BVRational){avst->time_base.num, avst->time_base.den};
    bvst->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    bvst->codec->width = onvifctx->width;
    bvst->codec->height = onvifctx->height;
    bvst->codec->time_base = (BVRational){1, onvifctx->framerate};
    bvst->codec->bit_rate = avst->codec->bit_rate;
    bvst->codec->codec_id = avid_to_bvid(avst->codec->codec_id);
    bvst->codec->gop_size = avst->codec->gop_size;
    bvst->codec->profile  = avst->codec->profile;
    if (avst->codec->extradata_size > 0) {
        bvst->codec->extradata = bv_memdup(avst->codec->extradata, avst->codec->extradata_size);
        if (bvst->codec->extradata) {
            bvst->codec->extradata_size = avst->codec->extradata_size;
        }
    }
}

static void set_audio_info(OnvifContext *onvifctx, BVStream *bvst, AVStream *avst)
{
    bvst->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    bvst->codec->sample_rate = avst->codec->sample_rate;
    bvst->time_base = (BVRational){avst->time_base.num, avst->time_base.den};
    bvst->codec->channels = avst->codec->channels;
    bvst->codec->sample_fmt = (enum BVSampleFormat )avst->codec->sample_fmt;
    bvst->codec->codec_id = avid_to_bvid(avst->codec->codec_id);
}

static int set_avcodec_info(BVMediaContext *s, OnvifContext *onvifctx)
{
    AVFormatContext *onvif = onvifctx->onvif;
    int i;
    BVStream *bvst = NULL;
    AVStream *avst = NULL;
    for (i = 0; i < onvif->nb_streams; i++) {
        avst = onvif->streams[i];
        if (avst->codec->codec_type == AVMEDIA_TYPE_VIDEO && onvifctx->vtoken) {
            bvst = bv_stream_new(s, NULL);
            set_video_info(onvifctx, bvst, avst);
            onvifctx->vindex = bvst->index;
        } else if (avst->codec->codec_type == AVMEDIA_TYPE_AUDIO && onvifctx->atoken) {
            bvst = bv_stream_new(s, NULL);
            set_audio_info(onvifctx, bvst, avst);
            onvifctx->aindex = bvst->index;
        } else {
            bv_log(s, BV_LOG_WARNING, "Drop CodecType %d\n", avst->codec->codec_type);
        }
    }
    return 0;
}
#define ONVIF_TMO (-5000)
#define MEMSET_STRUCT(X)    memset(&(X), 0, sizeof((X)));
static struct soap *bv_soap_new(OnvifContext *onvifctx)
{
    struct soap *soap = NULL;
    int timeout = onvifctx->timeout;
    soap = soap_new();
    if (soap == NULL) {
        return NULL;
    }
    soap_set_namespaces(soap, namespaces);
    if (!timeout) {
        timeout = ONVIF_TMO;
    }
    soap->recv_timeout = timeout;
    soap->send_timeout = timeout;
    soap->connect_timeout = timeout;
    return soap;
}

static void bv_soap_free(struct soap *soap)
{
    if (soap == NULL) {
        return;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
}

static int onvif_stream_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    int ret = -1;
    char *p = NULL;
    char *q = NULL;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetStreamUriResponse trt__GetStreamUriResponse;
    struct _trt__GetStreamUri trt__GetStreamUri;
    struct tt__StreamSetup StreamSetup;    /* required element of type tt:StreamSetup */
    struct tt__Transport Transport;    /* required element of type tt:Transport */
    struct SOAP_ENV__Header header;
    if (!onvifctx->media_url) {
        return BVERROR(ENOSYS);
    }
    trt__GetStreamUri.ProfileToken = onvifctx->profile_token;
    trt__GetStreamUri.StreamSetup = &StreamSetup;
    StreamSetup.Transport = &Transport;
    StreamSetup.Stream = tt__StreamType__RTP_Unicast;
    Transport.Protocol = tt__TransportProtocol__RTSP;
    Transport.Tunnel = NULL;
    StreamSetup.__size = 1;
    StreamSetup.__any = NULL;
    StreamSetup.__anyAttribute = NULL;
    soap_default_SOAP_ENV__Header(soap, &header);
    soap->header = &header;

    header.wsa__MessageID = (char *)soap_wsa_rand_uuid(soap);

    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }

    retval = soap_call___trt__GetStreamUri(soap, onvifctx->media_url , NULL, &trt__GetStreamUri, &trt__GetStreamUriResponse);
    if (retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get media url error\n");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    }  else {
        bv_log(NULL, BV_LOG_INFO, "get uri %s\n", trt__GetStreamUriResponse.MediaUri->Uri);
        q = trt__GetStreamUriResponse.MediaUri->Uri;
        if (onvifctx->user && onvifctx->passwd) {
            char tmp[128] = { 0 };
            bv_sprintf(tmp, sizeof(tmp), "%s:%s@", onvifctx->user, onvifctx->passwd);
            p = bv_sinsert(q, "://", tmp); 
            bv_strlcpy(onvifctx->onvif_url, p, sizeof(onvifctx->onvif_url));
            bv_free(p);
         } else {
            bv_strlcpy(onvifctx->onvif_url, q, sizeof(onvifctx->onvif_url));
        }
        ret = 0;
    }
    return ret;
}

static int onvif_snapshot_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    int ret = -1;
    char *p = NULL;
    char *q = NULL;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetSnapshotUri trt__GetSnapshotUri;
    struct _trt__GetSnapshotUriResponse trt__GetSnapshotUriResponse;

    if (!onvifctx->media_url) {
        return BVERROR(ENOSYS);
    }
    trt__GetSnapshotUri.ProfileToken = onvifctx->profile_token;

    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }
    retval = soap_call___trt__GetSnapshotUri(soap, onvifctx->media_url, NULL, &trt__GetSnapshotUri, &trt__GetSnapshotUriResponse);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get SnapshotUri error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        q = trt__GetSnapshotUriResponse.MediaUri->Uri;
        if (onvifctx->user && onvifctx->passwd) {
            char tmp[128] = { 0 };
            bv_sprintf(tmp, sizeof(tmp), "%s:%s@", onvifctx->user, onvifctx->passwd);
            p = bv_sinsert(q, "://", tmp); 
            bv_strlcpy(onvifctx->onvif_url, p, sizeof(onvifctx->onvif_url));
            bv_free(p);
         } else {
            bv_strlcpy(onvifctx->onvif_url, q, sizeof(onvifctx->onvif_url));
        }
        ret = 0;
        bv_log(onvifctx, BV_LOG_INFO, "snapshot url %s\n", onvifctx->onvif_url);
    }
    return ret;
}

static int onvif_service_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    struct soap *soap = onvifctx->soap;
    struct _tds__GetCapabilities request;
    struct _tds__GetCapabilitiesResponse response;
    enum tt__CapabilityCategory Category = tt__CapabilityCategory__Media;

    MEMSET_STRUCT(request);
    MEMSET_STRUCT(response);

    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }

    request.Category = &Category;
    request.__sizeCategory = 1;
    retval = soap_call___tds__GetCapabilities(soap, onvifctx->svrurl, NULL, &request, &response);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get MediaService URI error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        onvifctx->media_url = bv_strdup(response.Capabilities->Media->XAddr);
    }
    bv_log(onvifctx, BV_LOG_INFO, "Get MediaService URI %s\n", onvifctx->media_url);
    return 0;
}

static int onvif_stream_open(BVMediaContext *s)
{
    int ret = 0;
    OnvifContext *onvifctx = s->priv_data;
    ret = onvif_stream_uri(onvifctx);
    if (ret < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "onvif get stream uri error\n");
        return ret;
    }

    /**
     *  RTSP 流的处理协议的解析等等 现在使用FFmpeg中的，已有的功能。
     *  抓拍流是HTTP的协议也是用FFmpeg中的
     *  自己开发估计耗时太久。
     *  FIXME
     *      依赖 libavformat libavcodec libavutil
     *  以后可能RTSP协议处理会添加在libbvprotocol中
     */
    if(avformat_open_input(&onvifctx->onvif, onvifctx->onvif_url, NULL, NULL) < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "open onvif stream error\n");
        return BVERROR(EIO);
    }
#if 0
    if (avformat_find_stream_info(onvifctx->onvif, NULL) < 0) {
        avformat_close_input(&onvifctx->onvif);
        bv_log(onvifctx, BV_LOG_ERROR, "get onvif stream info error\n");
        return -1;
    }
#endif
    av_dump_format(onvifctx->onvif, 0, onvifctx->onvif_url, 0);
    set_avcodec_info(s, onvifctx);

    return 0;
}

static int onvif_snapshot_open(BVMediaContext *s)
{
    int ret = 0;
    OnvifContext *onvifctx = s->priv_data;
    BVStream *bvst = NULL;
    ret = onvif_snapshot_uri(onvifctx);
    if (ret < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "onvif get snapshot uri error\n");
        return ret;
    }

    bvst = bv_stream_new(s, NULL);
    if (!bvst) {
        return BVERROR(ENOMEM);
    }
    onvifctx->vindex = 0;
    bvst->time_base = (BVRational){1, 1000000};
    bvst->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    bvst->codec->codec_id   = onvifctx->vcodec_id;
    bvst->codec->width      = onvifctx->width;
    bvst->codec->height     = onvifctx->height;
    return 0;
}

//url like onvifave://192.168.6.149:8899/onvif/device_serveces
static bv_cold int onvif_read_header(BVMediaContext * s)
{
    int ret;
    char *p, *q, *r;
    int size;
    OnvifContext *onvifctx = s->priv_data;
    if (!onvifctx->vtoken && !onvifctx->atoken) {
        bv_log(s, BV_LOG_ERROR, "must set onvifave token url\n");
        return BVERROR(EINVAL);
    }
    if (onvifctx->vtoken) {
        q = onvifctx->vtoken;
    } else {
        q= onvifctx->atoken;
    }
    p = bv_strsub(q, "/", 1);
    r = bv_strsub(q, "/", 2);
    if (!p || !r) {
        bv_log(s, BV_LOG_ERROR, "onvifave token error\n");
        return BVERROR(EINVAL);
    }
    size = sizeof(onvifctx->profile_name);
    size = size > p - q ? p - q : size;
    bv_strlcpy(onvifctx->profile_name, q, p - q);
    size = sizeof(onvifctx->profile_token);
    size = size > r - p ? r - p : size;
    bv_strlcpy(onvifctx->profile_token, p, size);
    p = bv_sreplace(s->filename, "onvifave", "http");
    if (!p) {
        return BVERROR(ENOMEM);
    }
    bv_strlcpy(onvifctx->svrurl, p, sizeof(onvifctx->svrurl));
    bv_free(p);

    onvifctx->soap = bv_soap_new(onvifctx);
    if (!onvifctx->soap)
        return BVERROR(ENOMEM);

    if (onvif_service_uri(onvifctx)) {
        ret = BVERROR(EINVAL);
        goto fail;
    }
    bv_log(s, BV_LOG_DEBUG, "svr %s\n", onvifctx->svrurl);
    if (onvifctx->vcodec_id == BV_CODEC_ID_H264 || onvifctx->vcodec_id == BV_CODEC_ID_MPEG) {
        ret = onvif_stream_open(s);
    } else if (onvifctx->vcodec_id == BV_CODEC_ID_JPEG) {
        ret = onvif_snapshot_open(s);
    } else {
        bv_log(onvifctx, BV_LOG_ERROR, "video codec id error\n");
        ret = BVERROR(EINVAL);
        goto fail;
    }

    if(ret < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "onvif get stream uri error\n");
        goto fail;
    }
    return 0;
fail:
    bv_soap_free(onvifctx->soap);
    return ret;
}

static int copy_data(BVPacket *pkt, AVPacket *rpkt, int index)
{
    if (bv_packet_new(pkt, rpkt->size) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "NoMem at %s %d\n", __FILE__, __LINE__);
        return BVERROR(ENOMEM);
    }
    memcpy(pkt->data, rpkt->data, rpkt->size);
    pkt->size = rpkt->size;
    pkt->pts = rpkt->pts;
    pkt->dts = rpkt->dts;
    if (rpkt->flags & AV_PKT_FLAG_KEY)
        pkt->flags |= BV_PKT_FLAG_KEY;
    pkt->stream_index = index;
    return 0;
}

static int onvif_read_stream(BVMediaContext *s, BVPacket *pkt) 
{
    OnvifContext *onvifctx = s->priv_data;
    AVStream *avstream = NULL;
    AVPacket rpkt;
    int size = 0;
    if (onvifctx->onvif == NULL) {
        return BVERROR(EINVAL);
    }
    av_init_packet(&rpkt);
    if(av_read_frame(onvifctx->onvif, &rpkt) < 0) {
        return BVERROR(EIO);
    }
    size = rpkt.size;
    avstream = onvifctx->onvif->streams[rpkt.stream_index];
    if ((avstream->codec->codec_type == AVMEDIA_TYPE_VIDEO) && (onvifctx->vindex != -1)) {
        copy_data(pkt, &rpkt, onvifctx->vindex);
    } else if ((avstream->codec->codec_type == AVMEDIA_TYPE_AUDIO) && (onvifctx->aindex != -1)) {
        copy_data(pkt, &rpkt, onvifctx->aindex);
    } else {
        size = BVERROR(EAGAIN);
    }
    av_free_packet(&rpkt);
    return size;
}

static int onvif_read_snapshot(BVMediaContext *s, BVPacket *pkt)
{
    BVIOContext *ioctx = NULL;
    OnvifContext *onvifctx = s->priv_data;
    int64_t size = 0LL;
    if (bv_io_open(&ioctx, onvifctx->onvif_url, BV_IO_FLAG_READ, NULL, NULL) < 0) {
        bv_log(s, BV_LOG_ERROR, "open snapshot url %s error\n", onvifctx->onvif_url);
        return BVERROR(EIO);
    }

    if ((size = bv_io_size(ioctx)) < 0) {
        bv_log(s, BV_LOG_ERROR, "get snapshot image size error\n");
        return BVERROR(EIO);
    }
    if (bv_packet_new(pkt, size) < 0) {
        return BVERROR(ENOMEM);
    }
    pkt->stream_index = onvifctx->vindex;
    pkt->flags |= BV_PKT_FLAG_KEY;
    pkt->pts = bv_gettime_relative();
    if (bv_io_read(ioctx, pkt->data, size) != size) {
        bv_log(s, BV_LOG_ERROR, "read snapshot image error\n");
        bv_packet_free(pkt);
    }
    bv_io_close(ioctx);
    return pkt->size;
}

static int onvif_read_packet(BVMediaContext *s, BVPacket *pkt)
{
    OnvifContext *onvifctx = s->priv_data;

    if (onvifctx->vcodec_id == BV_CODEC_ID_JPEG) {
        //snapshot
        return onvif_read_snapshot(s, pkt);
    }

    return onvif_read_stream(s, pkt);
}

static bv_cold int onvif_read_close(BVMediaContext * s)
{
    OnvifContext *onvifctx = s->priv_data;
    if (onvifctx->vcodec_id != BV_CODEC_ID_JPEG) {
        avformat_close_input(&onvifctx->onvif);
    }
    bv_soap_free(onvifctx->soap);
    return 0;
}

static bv_cold int onvif_media_control(BVMediaContext *s, enum BVMediaMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return 0;
}

#define OFFSET(x) offsetof(OnvifContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"vtoken", "", OFFSET(vtoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"vindex", "", OFFSET(vindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    {"atoken", "", OFFSET(atoken), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"aindex", "", OFFSET(aindex), BV_OPT_TYPE_INT, {.i64= -1}, -1, 128, DEC},
    {"user", "", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"vcodec_id", "", OFFSET(vcodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_H264}, 0, INT_MAX, DEC},
    { "framerate", "", OFFSET(framerate), BV_OPT_TYPE_INT, {.i64 = 25}, 0, 120, DEC},
    { "width", "", OFFSET(width), BV_OPT_TYPE_INT, {.i64 = 1280}, 0, INT_MAX, DEC},
    { "height", "", OFFSET(height), BV_OPT_TYPE_INT, {.i64 = 720}, 0, INT_MAX, DEC},
    {"media_url", "", OFFSET(media_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"timeout", "", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 = -5000}, INT_MIN, INT_MAX, DEC},

    {NULL}
};

static const BVClass onvif_class = {
    .class_name         = "onvifave indev",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEMUXER,
};

BVInputMedia bv_onvifave_demuxer = {
    .name               = "onvifave",
    .priv_class         = &onvif_class,
    .priv_data_size     = sizeof(OnvifContext),
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .read_probe         = onvif_probe,
    .read_header        = onvif_read_header,
    .read_packet        = onvif_read_packet,
    .read_close         = onvif_read_close,
    .media_control      = onvif_media_control,
};
