/*************************************************************************
    > File Name: davmux.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月21日 星期三 14时28分16秒
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

#include "bvmedia.h"
#include "dav.h"

#include <time.h>

typedef struct DavMuxContext {
    uint8_t channel;
    int width;
    int height;
    int fps;
    VideoEncodeType video_type;
    AudioEncodeType audio_type;
    AudioSampleRate sample_rate;
    int channels;       //音频通道数
    uint32_t vframeseq;
    uint32_t aframeseq;
} DavMuxContext;

static VideoEncodeType get_video_type(enum BVCodecID codec_id)
{
    int i = 0;
    struct {
        enum BVCodecID codec_id;
        VideoEncodeType vtype;
    }IDS[] = {
        {BV_CODEC_ID_H264, VIDEO_ENCODE_H264},
        {BV_CODEC_ID_MPEG, VIDEO_ENCODE_MPEG4},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].codec_id == codec_id) {
            return IDS[i].vtype;
        }
    }

    return VIDEO_ENCODE_H264;
}

static int set_video_info(DavMuxContext *davctx, BVStream *stream)
{
    davctx->width = stream->codec->width;
    davctx->height = stream->codec->height;
    davctx->fps = stream->codec->time_base.num / stream->codec->time_base.den;
    davctx->video_type = get_video_type(stream->codec->codec_id);
    return 0;
}

static AudioSampleRate get_audio_sample_rate(int sample_rate)
{
    int i = 0;
    struct {
        int sample_rate;
        AudioSampleRate SampleRate;
    }sample_rates[] = {
        {4000, SAMPLE_FREQ_4000},
        {8000, SAMPLE_FREQ_8000},
        {11025, SAMPLE_FREQ_11025},
        {16000, SAMPLE_FREQ_16000},
        {20000, SAMPLE_FREQ_20000},
        {22050, SAMPLE_FREQ_22050},
        {32000, SAMPLE_FREQ_32000},
        {44100, SAMPLE_FREQ_44100},
        {48000, SAMPLE_FREQ_48000},
    };

    for (i = 0; i < BV_ARRAY_ELEMS(sample_rates); i++) {
        if (sample_rates[i].sample_rate == sample_rate) {
            return sample_rates[i].SampleRate; 
        } 
    }
    return SAMPLE_FREQ_8000;
}

static AudioEncodeType get_audio_type(enum BVCodecID codec_id)
{
    int i = 0;
    struct {
        enum BVCodecID codec_id;
        AudioEncodeType EncodeType;
    }IDS[] = {
        {BV_CODEC_ID_G711A, AUDIO_ENCODE_G711A},
        {BV_CODEC_ID_G711U, AUDIO_ENCODE_G711U},
        {BV_CODEC_ID_G726,  AUDIO_ENCODE_G726},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].codec_id == codec_id) {
            return IDS[i].EncodeType;
        }
    }

    return AUDIO_ENCODE_G711A;
}

static int set_audio_info(DavMuxContext *davctx, BVStream *stream)
{
    davctx->channels = stream->codec->channels;
    davctx->sample_rate = get_audio_sample_rate(stream->codec->sample_rate);
    davctx->audio_type  = get_audio_type(stream->codec->codec_id);
    return 0;
}

static int dav_write_header(BVMediaContext *s)
{
    DavMuxContext *davctx = s->priv_data;
    BVStream *stream = NULL;
    int i = 0;
    if (!s->pb) {
        bv_log(s, BV_LOG_ERROR, "file pb is NULL, open protocol first\n");
        return BVERROR(EINVAL);
    }
    for (i = 0; i < s->nb_streams; i++) {
        stream = s->streams[i];
       if (stream->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
            set_video_info(davctx, stream);
       } else if (stream->codec->codec_type == BV_MEDIA_TYPE_AUDIO) {
            set_audio_info(davctx, stream);
       } else {
            bv_log(s, BV_LOG_ERROR, "stream %d type error\n", i);
       }
    }
    return 0;
}

static void SetFrameHeaderTime(FrameHeader *pstFrameHeader)
{
    time_t tCurTime = time(NULL);
    struct tm TM ;
	localtime_r(&tCurTime,&TM);

    pstFrameHeader->stDateTime.second = TM.tm_sec;
    pstFrameHeader->stDateTime.minute = TM.tm_min;
    pstFrameHeader->stDateTime.hour =   TM.tm_hour;
    pstFrameHeader->stDateTime.day =    TM.tm_mday;
    pstFrameHeader->stDateTime.month =  TM.tm_mon + 1;
    pstFrameHeader->stDateTime.year =   TM.tm_year + 1900 - 2000;
}

static unsigned char U8CRC(unsigned char *pData, int iSize)
{
    unsigned char u8crc = 0x00;
    register int i;
    for(i = 0 ; i < iSize; i++)
    {
        u8crc += pData[i];
    }
    return u8crc;
}

static int set_video_frame(DavMuxContext *davctx, Frame *pframe, BVPacket *pkt)
{
    strncpy(pframe->stFrameHeader.sHeaderFlag, "ZLAV",sizeof(pframe->stFrameHeader.sHeaderFlag));
    pframe->stFrameHeader.iChannel = davctx->channel;

    pframe->stFrameHeader.enFrameChildType = FRAME_CHILD_TYPE;
    pframe->stFrameHeader.iChildFrameSeq = 0;
    pframe->stFrameHeader.iFrameSeq = davctx->vframeseq;

    SetFrameHeaderTime(&pframe->stFrameHeader);

    pframe->stFrameHeader.iMSTimeStamp = (unsigned short)(pkt->pts / 1000 % 65535);

    if(pkt->flags & BV_PKT_FLAG_KEY){
        pframe->stFrameHeader.enFrameType = FRAME_TYPE_I_SLICE;
        pframe->stFrameHeader.iAddedDataLen = sizeof(IDRFrameAddedHeader);

        pframe->stIDRFrameHeader.stImageSize.iFlag = 0x80;
        pframe->stIDRFrameHeader.stImageSize.enFiledFlag = FILED_TYPE_ONLY_ONE;
        pframe->stIDRFrameHeader.stImageSize.iWidth = davctx->width >> 3;
        pframe->stIDRFrameHeader.stImageSize.iHeight = davctx->height >> 3;
        pframe->stIDRFrameHeader.stPlayBackType.iFlag = 0x81;
        pframe->stIDRFrameHeader.stPlayBackType.iReserve = 0x00;
        pframe->stIDRFrameHeader.stPlayBackType.enCodedType = davctx->video_type;
        pframe->stIDRFrameHeader.stPlayBackType.iFps = davctx->fps;
    }else{
        pframe->stFrameHeader.enFrameType = FRAME_TYPE_P_SLICE;
        pframe->stFrameHeader.iAddedDataLen = 0;
    }

    pframe->stFrameHeader.iFrameLen = pkt->size + sizeof(FrameHeader) + pframe->stFrameHeader.iAddedDataLen + sizeof(FrameEnder);

    pframe->stFrameHeader.u8Crc = U8CRC((unsigned char *)&pframe->stFrameHeader, sizeof(FrameHeader) - 1);

    strncpy(pframe->stFrameEnder.sEnderFlag, "zlav",sizeof(pframe->stFrameEnder.sEnderFlag));
    pframe->stFrameEnder.iFrameLen = pframe->stFrameHeader.iFrameLen;

    return 0;
}

static int set_audio_frame(DavMuxContext *davctx, Frame *pframe, BVPacket *pkt)
{
    strncpy(pframe->stFrameHeader.sHeaderFlag, "ZLAV",sizeof(pframe->stFrameHeader.sHeaderFlag));
    pframe->stFrameHeader.iChannel = davctx->channels;
    pframe->stFrameHeader.enFrameType = FRAME_TYPE_AUDIO;
    pframe->stFrameHeader.enFrameChildType = FRAME_CHILD_TYPE;
    pframe->stFrameHeader.iChildFrameSeq = 0;
    pframe->stFrameHeader.iFrameSeq = davctx->aframeseq;

    pframe->stFrameHeader.iFrameLen = sizeof(pframe->stFrameHeader) + sizeof(AudioFrameAddHeader) + sizeof(FrameEnder) + pkt->size;

    SetFrameHeaderTime(&pframe->stFrameHeader); 

    pframe->stFrameHeader.iMSTimeStamp =  (unsigned short)((pkt->pts / 1000) % 65535);
    pframe->stFrameHeader.iAddedDataLen = sizeof(AudioFrameAddHeader);
    pframe->stFrameHeader.u8Crc = U8CRC((unsigned char *)&pframe->stFrameHeader, sizeof(FrameHeader) -1);

    pframe->stAudioFrameHeader.iFlag = 0x83;
    pframe->stAudioFrameHeader.iChannelCount = davctx->channels;
    pframe->stAudioFrameHeader.enCodedType = davctx->audio_type;
    pframe->stAudioFrameHeader.enSampleRate = davctx->sample_rate;

    strncpy(pframe->stFrameEnder.sEnderFlag, "zlav",sizeof(pframe->stFrameEnder.sEnderFlag));
    pframe->stFrameEnder.iFrameLen = pframe->stFrameHeader.iFrameLen;

    return 0;
}

static int write_video(BVMediaContext *s, BVPacket *pkt)
{
    Frame frame;
    DavMuxContext *davctx = s->priv_data;
    set_video_frame(davctx, &frame, pkt);
    davctx->vframeseq ++;
    // frame_header data frame_tail 
    bv_io_write(s->pb, (const uint8_t *) &frame.stFrameHeader, sizeof(FrameHeader));
    if (frame.stFrameHeader.iAddedDataLen) {
        bv_io_write(s->pb, (const uint8_t *) &frame.stIDRFrameHeader, sizeof(IDRFrameAddedHeader));
    }

    bv_io_write(s->pb, pkt->data, pkt->size);
    bv_io_write(s->pb, (const uint8_t *)&frame.stFrameEnder, sizeof(FrameEnder));
    return 0;
}

static int write_audio(BVMediaContext *s, BVPacket *pkt)
{
    Frame frame;
    DavMuxContext *davctx = s->priv_data;
    set_audio_frame(davctx, &frame, pkt);
    davctx->aframeseq ++;
    // frame_header data frame_tail 
    //
    bv_io_write(s->pb, (const uint8_t *) &frame.stFrameHeader, sizeof(FrameHeader));
    bv_io_write(s->pb, (const uint8_t *) &frame.stAudioFrameHeader, sizeof(AudioFrameAddHeader));

    bv_io_write(s->pb, pkt->data, pkt->size);
    bv_io_write(s->pb, (const uint8_t *)&frame.stFrameEnder, sizeof(FrameEnder));

    return 0;
}

static int dav_write_packet(BVMediaContext *s, BVPacket *pkt)
{
    int stream_type = BV_MEDIA_TYPE_UNKNOWN;
    int ret = 0;
    stream_type = s->streams[pkt->stream_index]->codec->codec_type;
    if (stream_type != BV_MEDIA_TYPE_VIDEO && stream_type != BV_MEDIA_TYPE_AUDIO) {
        bv_log(s, BV_LOG_ERROR, "stream %d type error\n", pkt->stream_index);
        return BVERROR(EINVAL);
    }
    if (!s->pb) {
        return BVERROR(EINVAL);
    }
    if (stream_type == BV_MEDIA_TYPE_VIDEO) {
        ret = write_video(s, pkt);
    }
    
    if (stream_type == BV_MEDIA_TYPE_AUDIO) {
        ret = write_audio(s, pkt); 
    }
    return ret;
}

static int dav_write_trailer(BVMediaContext *s)
{
//    DavMuxContext *davctx = s->priv_data;
    if (!s->pb)
        return BVERROR(EINVAL);
    bv_io_flush(s->pb);
    return 0;
}

#define OFFSET(x) offsetof(DavMuxContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "channel", "", OFFSET(channel), BV_OPT_TYPE_INT, {.i64 = 0}, 0, 255, DEC },

    { NULL }
};

static const BVClass dav_class = {
    .class_name         = "dav muxer",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_MUXER,
};

BVOutputMedia bv_dav_muxer = {
    .name               = "dav",
    .extensions         = "dav",
    .mime_type          = "video/x-bsvideo",
    .priv_class         = &dav_class,
    .priv_data_size     = sizeof(DavMuxContext),
    .write_header       = dav_write_header,
    .write_packet       = dav_write_packet,
    .write_trailer      = dav_write_trailer,
};
