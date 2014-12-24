/*************************************************************************
	> File Name: stream.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年11月12日 星期三 10时00分22秒
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

#include <stdio.h>
#include <string.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
#include <libavutil/parseutils.h>

#include "stream.h"

enum BVStreamDirection {
	BV_STREAM_DIRECTION_NONE = 0,
	BV_STREAM_DIRECTION_INPUT = (1 << 0),
	BV_STREAM_DIRECTION_OUTPUT = (1 << 1),

	BV_STREAM_DIRECTION_UNKNOWN
};

struct BVFormatContext {
	enum BVStreamDirection direction;
	int nb_formats;
	AVFormatContext **formats;
};

struct _BVSStream {
	int index;			//format index
	int nb_streams;
	BVStream **streams;
};

static BVFormatContext * bvformat_alloc_context()
{
    BVFormatContext *ic;
    ic = av_malloc(sizeof(BVFormatContext));
    if (!ic) {
		av_log(NULL, AV_LOG_ERROR, "alloc BVFormatContext Error NoMEM\n");
		return ic;
	}
	ic->direction = BV_STREAM_DIRECTION_NONE;
	ic->nb_formats = 0;
	ic->formats = NULL;
    return ic;
}

static AVFormatContext * bvformat_new_format(BVFormatContext *s)
{
    AVFormatContext *st = NULL;
    AVFormatContext **formats;

    if (s->nb_formats >= INT_MAX/sizeof(*formats))
        return NULL;
    formats = av_realloc_array(s->formats, s->nb_formats + 1, sizeof(*formats));
    if (!formats)
        return NULL;
    s->formats = formats;
#if 1
	st = avformat_alloc_context();
    if (!st)
        return NULL;
#endif
    s->formats[s->nb_formats++] = st;

    return st;
}

static void bvformat_free_context(BVFormatContext *s)
{
	int i;
	if (!s)
		return ;
	for (i = s->nb_formats - 1; i >= 0; i--) {
		if (s->direction & BV_STREAM_DIRECTION_INPUT) {
			av_log(NULL, AV_LOG_INFO, "close input %d\n", i);
			avformat_close_input(&s->formats[i]);
		}
    }
	av_freep(&s->formats);
	av_free(s);
	return ;
}

void bv_stream_init()
{
	av_register_all();	//调用了 avcodec_register_all();
	avformat_network_init();
	avdevice_register_all();
	return ;
}

void bv_stream_deinit()
{
	avformat_network_deinit();
	return ;
}

static enum AVCodecID bvcodec_to_avcodec(enum BVCodecID bvcodec_id)
{
	switch(bvcodec_id) {
		case BV_CODEC_ID_H264:
			return AV_CODEC_ID_H264;
		case BV_CODEC_ID_JPG:
			return AV_CODEC_ID_MJPEG;
		case BV_CODEC_ID_MPEG:
			return AV_CODEC_ID_MPEG4;
		/*Audio**/
		case BV_CODEC_ID_AAC:
			return AV_CODEC_ID_AAC;
		case BV_CODEC_ID_G711A:
			return AV_CODEC_ID_PCM_ALAW;
		case BV_CODEC_ID_G711U:
			return AV_CODEC_ID_PCM_MULAW;
		case BV_CODEC_ID_G726:
			return AV_CODEC_ID_ADPCM_G726;
		case BV_CODEC_ID_UNKNOWN:
		case BV_CODEC_ID_NONE:
			break;
	}
	return AV_CODEC_ID_NONE;
}

static int set_vcodec_info(AVCodecContext *vcodec, BVCodecContext *bvcodec)
{
	vcodec->width = bvcodec->width;
	vcodec->height = bvcodec->height;
	vcodec->time_base.den = bvcodec->framerate;
	vcodec->time_base.num = 1;
#if 1
	vcodec->gop_size = bvcodec->gop;
	vcodec->bit_rate = bvcodec->bitrate;
	vcodec->compression_level = bvcodec->quality;
	switch(bvcodec->mode_id) {
		case BV_RC_MODE_ID_CBR:
			vcodec->bit_rate_tolerance = 1;
			break;
		case BV_RC_MODE_ID_VBR:
			vcodec->bit_rate_tolerance = 2;
			break;
		case BV_RC_MODE_ID_ABR:
			vcodec->bit_rate_tolerance = 3;
			break;
		case BV_RC_MODE_ID_FIXQP:
		case BV_RC_MODE_ID_BUTT:
		default:
			vcodec->bit_rate_tolerance = 1;
	}
#endif
	vcodec->codec_id = bvcodec_to_avcodec(bvcodec->codec_id);
	if (vcodec->codec_id != AV_CODEC_ID_H264) {
		printf("error codecid\n");
	}
	vcodec->codec_type = AVMEDIA_TYPE_VIDEO;
	if (bvcodec->extradata_size > 0) {
		vcodec->extradata = av_malloc(bvcodec->extradata_size);
		memcpy(vcodec->extradata, bvcodec->extradata, bvcodec->extradata_size);
	}
	vcodec->extradata_size = bvcodec->extradata_size;
	return 0;
}

static int set_acodec_info(AVCodecContext *acodec, BVCodecContext *bvcodec)
{
	acodec->sample_rate = bvcodec->sample_rate;
	acodec->bit_rate = bvcodec->bitrate;
	acodec->channels = bvcodec->channel;
	acodec->codec_id = bvcodec_to_avcodec(bvcodec->codec_id);
	acodec->codec_type = AVMEDIA_TYPE_AUDIO;
	return 0;
}

int onvif_open_input(BVFormatContext *s, const BVEncodeChannel *channel)
{
	AVFormatContext *avfmt;
	AVStream *stream;
	AVCodecContext *codec;
	char filename[1024];
	if (!(avfmt = bvformat_new_format(s))) {
		return AVERROR(ENOMEM);
	}

	if (channel->video_stream) {
		stream = avformat_new_stream(avfmt, NULL);
		codec = stream->codec;
		if (set_vcodec_info(codec, &channel->video_stream->codec) < 0) {
			return -1;
		}
	}

	if (channel->audio_stream) {
		stream = avformat_new_stream(avfmt, NULL);
		codec = stream->codec;
		if (set_acodec_info(codec, &channel->audio_stream->codec) < 0) {
			return -1;
		}
	}
	snprintf(filename, 1024, "%s/?token=%s", channel->url, channel->profile_token);

	return avformat_open_input(&avfmt, filename, NULL, NULL);
}

int hisi_open_input(BVFormatContext *s, const BVEncodeChannel *channel)
{
	AVFormatContext *avfmt;
	AVStream *stream;
	AVCodecContext *codec;
	char filename[1024];
	if (channel->video_stream) {
		if (!(avfmt = bvformat_new_format(s))) {
			return AVERROR(ENOMEM);
		}

		stream = avformat_new_stream(avfmt, NULL);
		codec = stream->codec;
		if (set_vcodec_info(codec, &channel->video_stream->codec) < 0) {
			return -1;
		}
		snprintf(filename, 1024, "%s/?token=%s", channel->video_stream->name, channel->video_stream->token);
		if (avformat_open_input(&avfmt, filename, NULL, NULL) < 0) {
			return -1;
		}
	}

	if (channel->audio_stream) {
		if (!(avfmt = bvformat_new_format(s))) {
			return AVERROR(ENOMEM);
		}

		stream = avformat_new_stream(avfmt, NULL);
		codec = stream->codec;
		if (set_acodec_info(codec, &channel->audio_stream->codec) < 0) {
			return -1;
		}
		snprintf(filename, 1024, "%s/?token=%s", channel->audio_stream->name, channel->audio_stream->token);
		if (avformat_open_input(&avfmt, filename, NULL, NULL) < 0) {
			return -1;
		}
	}

	return 0;
}

static int channel_open_input(BVFormatContext *s, const BVEncodeChannel *channel)
{
	if (!channel->video_stream && !channel->audio_stream) {
		return AVERROR(EINVAL);
	}

	if (strstr(channel->url, "onvifve")) {
		return onvif_open_input(s, channel);
	} 
	
	if(strstr(channel->url, "hisive")) {
		return hisi_open_input(s, channel);
	} 

	av_log(NULL, AV_LOG_ERROR, "Not support This Channel Type\n");
	return AVERROR(EINVAL);
}

static int uri_open_input(BVFormatContext *s, const char *uri)
{
	AVFormatContext *avfmt;
	if (!(avfmt = bvformat_new_format(s))) {
		return AVERROR(ENOMEM);
	}
	if (avformat_open_input(&avfmt, uri, NULL, NULL) < 0) {
		return -1;
	}
	if (avio_open(&avfmt->pb, uri, AVIO_FLAG_READ | AVIO_FLAG_DIRECT) < 0) {
		return -1;
	}
	return 0;
}

int bv_format_open_input(BVFormatContext **fmtctx, const BVEncodeChannel *channel, const char *uri)
{
	int ret = -1;
	BVFormatContext *s = *fmtctx;

	if (!channel && !uri) {
		return AVERROR(EINVAL);
	}

	if (!s && !(s = bvformat_alloc_context())) {
		return AVERROR(ENOMEM);
	}

	s->direction = BV_STREAM_DIRECTION_INPUT;

	if (channel) {
		ret = channel_open_input(s, channel);
	} else {
		ret = uri_open_input(s, uri);
	}

	if (ret < 0) {
		bvformat_free_context(s);
		s = NULL;
	}
	*fmtctx = s;
	return ret;
}

static BVAVPacket *bv_alloc_packet(int size)
{
	BVAVPacket *pkt = av_mallocz(sizeof(BVAVPacket));
	if (!pkt) {
		av_log(NULL, AV_LOG_ERROR, "ENOMEM");
		return NULL;
	}
	pkt->type = BV_MEDIA_TYPE_UNKNOWN;
	if (size > 0) {
		pkt->data = av_malloc(size);
		if (!pkt->data) {
			av_free(pkt);
			return NULL;
		}
		pkt->size = size;
	}

	return pkt;
}

static void bv_free_packet(BVAVPacket *pkt)
{
	if (!pkt)
		return;
	if (pkt->data)
		av_free(pkt->data);
	av_free(pkt);
}

void bv_spacket_init(BVSAVPacket *spkt)
{
	spkt->nb_pkts = 0;
	spkt->pkts = NULL;
}

static BVAVPacket *bv_packet_new(BVSAVPacket *spkt, int size)
{
	BVAVPacket *pkt;
	BVAVPacket **pkts;

	if (spkt->nb_pkts == INT_MAX/sizeof(*pkts))
		return NULL;

	pkts = av_realloc_array(spkt->pkts, spkt->nb_pkts + 1, sizeof(*pkts));
	if (!pkts)
		return NULL;
	spkt->pkts = pkts;

	pkt = bv_alloc_packet(size);
	if (!pkt)
		return NULL;
	spkt->pkts[spkt->nb_pkts ++] = pkt;

	return pkt;
}

void bv_spacket_free(BVSAVPacket *spkt)
{
	int i = 0;
	BVAVPacket *pkt;
	for (i = spkt->nb_pkts - 1; i >= 0; i--) {
		pkt = spkt->pkts[i];
		bv_free_packet(pkt);
	}
	av_freep(&spkt->pkts);
	return ;
}

static void copy_avpkt_data(BVAVPacket *bpkt, AVPacket *pkt)
{
	bpkt->pts = pkt->pts;
	bpkt->dts = pkt->dts;
	memcpy(bpkt->data, pkt->data, bpkt->size);
	bpkt->flags = pkt->flags;
}

int bv_format_read(BVFormatContext *fmtctx, BVSAVPacket *spkt)
{
	AVFormatContext *st;
	AVStream *stream;
	AVPacket pkt;
	BVAVPacket *bpkt;
	int i = 0;
	if (!fmtctx || !spkt || !(fmtctx->direction & BV_STREAM_DIRECTION_INPUT)) {
		av_log(NULL, AV_LOG_ERROR, "param error\n");
		return AVERROR(EINVAL);
	}

	if (fmtctx->nb_formats <= 0) {
		av_log(NULL, AV_LOG_ERROR, "format error\n");
		return AVERROR(EINVAL);
	}

	for (i = 0; i < fmtctx->nb_formats; ++i) {
		st = fmtctx->formats[i];
		av_init_packet(&pkt);
		if (av_read_frame(st, &pkt) < 0)
			continue;
		stream = st->streams[pkt.stream_index];
		bpkt = bv_packet_new(spkt, pkt.size);
		copy_avpkt_data(bpkt, &pkt);
		bpkt->type = (enum BVMediaType)stream->codec->codec_type;
		av_free_packet(&pkt);
	}
	return 0;
}

int bv_format_close_input(BVFormatContext **fmtctx)
{
	BVFormatContext *st ;
	if (!fmtctx)
		return -1;
	st = *fmtctx;
	bvformat_free_context(st);
	*fmtctx = NULL;
	return 0;
}

void bv_sstream_init(BVSStream **sstream)
{
	BVSStream *st = *sstream;
	if (!st) {
		av_log(NULL, AV_LOG_ERROR, "sstream not NULL\n");
		return;
	}
	st = av_malloc(sizeof(BVSStream));
	st->nb_streams = 0;
	*sstream = st;
}

static BVStream * bv_alloc_stream(BVCodecContext *codec)
{
	BVStream *st = av_malloc(sizeof(BVStream));
	if (!st)
		return NULL;
	if (codec) {
		st->codec = *codec ;
		if (codec->extradata_size) {
			st->codec.extradata = av_malloc(codec->extradata_size);
			memcpy(st->codec.extradata, codec->extradata, codec->extradata_size);
		}
	}
	return st;
}

BVStream * bv_new_stream(BVSStream *sstream, BVCodecContext *codec)
{
	BVStream *st;
	BVStream **sts;
	
	if (sstream->nb_streams == INT_MAX/sizeof(*sts))
		return NULL;

	sts = av_realloc_array(sstream->streams, sstream->nb_streams + 1, sizeof(*sts));
	if (!sts)
		return NULL;
	sstream->streams = sts;

	st = bv_alloc_stream(codec);
	if (!st)
		return NULL;
	st->index = sstream->nb_streams;
	sstream->streams[sstream->nb_streams ++] = st;

	return st;
}

void bv_sstream_free(BVSStream **sstream)
{
	int i = 0;
	BVSStream *sst;
	if (!sstream || !(sst = *sstream)) {
		printf("NULL error\n");
		return ;
	}
	BVStream *st;
	for (i = sst->nb_streams - 1; i >= 0; i--) {
		st = sst->streams[i];
		av_free(st);
	}
	av_freep(&sst->streams);
	av_free(sst);
	return ;
}

static int uri_open_output(BVFormatContext *s, const char *uri, const BVSStream *sstream)
{
	AVFormatContext *avfmt;
	AVStream *stream;
	AVCodecContext *codec;
	int i;
	int ret ;
	if (!(avfmt = bvformat_new_format(s))) {
		return AVERROR(ENOMEM);
	}
	printf("source %p", avfmt);
	if (avformat_alloc_output_context2(&avfmt, NULL, NULL, uri) < 0) {
		return AVERROR(EINVAL);	
	}
	printf("dest %p\n", avfmt);

	if (sstream) {
		for (i = 0; i < sstream->nb_streams; i++) {
			stream = avformat_new_stream(avfmt, NULL);
			if (!stream) {
				return AVERROR(ENOMEM);
			}
			codec = stream->codec;
			if (sstream->streams[i]->codec.codec_type == BV_MEDIA_TYPE_VIDEO) {
				set_vcodec_info(codec, &sstream->streams[i]->codec);
			} else if (sstream->streams[i]->codec.codec_type == BV_MEDIA_TYPE_AUDIO) {
				set_acodec_info(codec, &sstream->streams[i]->codec);
			} else {
				continue;
			}
			if (avfmt->oformat->flags & AVFMT_GLOBALHEADER)
				stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
			stream->codec->codec_tag = 0;
#if 0
			stream->time_base.den = sstream->streams[i]->codec.framerate;
			stream->time_base.num = 1;
#endif
		}
	}
	av_dict_set(&avfmt->metadata,"title", "BesoVideo", 0);
	av_dict_set(&avfmt->metadata,"author", "albert", 0);
	av_dump_format(avfmt, 0, uri, 1);

	if (!(avfmt->oformat->flags & AVFMT_NOFILE)) {
		ret = avio_open(&avfmt->pb, uri, AVIO_FLAG_WRITE);
		if (ret < 0) {
			fprintf(stderr, "Could not open output file '%s'", uri);
			return -1;
		}
	}
	printf("dest %p\n", avfmt);
#if 0
	s->formats[s->nb_formats - 1] = avfmt;
#endif
	ret = avformat_write_header(avfmt, NULL);
	return ret;
}

int bv_format_open_output(BVFormatContext **fmtctx, void * decchn, const char *uri, const BVSStream *sstream)
{
	int ret = -1;
	BVFormatContext *s = *fmtctx;

	if (!decchn && !uri) {
		return AVERROR(EINVAL);
	}

	if (!s && !(s = bvformat_alloc_context())) {
		return AVERROR(ENOMEM);
	}

	s->direction = BV_STREAM_DIRECTION_OUTPUT;

	if (decchn) {
//		ret = channel_open_input(s, channel);
	} else {
		ret = uri_open_output(s, uri, sstream);
	}

	if (ret < 0) {
		bv_format_close_output(&s);
		s = NULL;
	}
	*fmtctx = s;

	return ret;
}
int bv_format_write(BVFormatContext *fmtctx, BVSAVPacket *spkt)
{
	AVFormatContext *st;
	AVPacket pkt;
	BVAVPacket *bpkt;
	int i = 0;
	int j = 0;
	int x = 0;
	static int flags = 1;
	if (!fmtctx || !spkt || !(fmtctx->direction & BV_STREAM_DIRECTION_OUTPUT)) {
		av_log(NULL, AV_LOG_ERROR, "param error\n");
		printf("%p %p\n", fmtctx, spkt);
		return AVERROR(EINVAL);
	}

	if (fmtctx->nb_formats <= 0) {
		av_log(NULL, AV_LOG_ERROR, "format error\n");
		return AVERROR(EINVAL);
	}

	for (i = 0; i < spkt->nb_pkts; ++i) {
		bpkt = spkt->pkts[i];
		if (flags) {
			if (!(bpkt->flags & BV_PKT_FLAG_KEY)){
				printf("not key frame\n");
				continue;
			}
			flags = 0;
		}
		for (j = 0; j < fmtctx->nb_formats; j++) {
			st = fmtctx->formats[j];
			for (x = 0; x < st->nb_streams; x++) {
				if (bpkt->type == (enum BVMediaType)st->streams[x]->codec->codec_type) {
					av_new_packet(&pkt, bpkt->size);
					pkt.pts = bpkt->pts;
					pkt.dts = bpkt->dts;
					pkt.flags = bpkt->flags;
					pkt.stream_index = x;
					memcpy(pkt.data, bpkt->data, bpkt->size);
					if (av_write_frame(st, &pkt) < 0) {
						av_log(NULL, AV_LOG_ERROR, "write packet error\n");
					}
					//av_log(NULL, AV_LOG_WARNING, "pts %lld dts %lld\n", pkt.pts, pkt.dts);
					av_free_packet(&pkt);
					return 0;
				}
			}
		}
	}
	av_log(NULL, AV_LOG_ERROR, "write file error\n");
	return 0;
}

int bv_format_close_output(BVFormatContext **fmtctx)
{
	BVFormatContext *st ;
	AVFormatContext *ofmt_ctx;
	int i = 0;
	if (!fmtctx || !(st = *fmtctx))
		return -1;
	for (i = 0; i < st->nb_formats; i++) {
		ofmt_ctx = st->formats[i];
		av_write_trailer(ofmt_ctx);
		if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
			avio_close(ofmt_ctx->pb);
		avformat_free_context(ofmt_ctx);
	}
	av_freep(&st->formats);
	av_freep(fmtctx);
	return 0;
}
