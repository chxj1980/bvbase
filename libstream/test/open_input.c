/*************************************************************************
	> File Name: open_input.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年11月25日 星期二 14时41分33秒
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
#include <stdlib.h>
#include <stream.h>
#include <time.h>
#include <string.h>

static void set_default_channel_info(BVEncodeChannel *chn)
{
	strncpy(chn->name, "chn1", sizeof(chn->name) -1);
	strcpy(chn->url, "onvifave://192.168.6.149:8899/onvif/device_service");
	strcpy(chn->profile_name, "profile_0000");
	strcpy(chn->profile_token, "000");
	chn->video_stream = malloc(sizeof(BVStream));
	chn->video_stream->index = 0;
	chn->video_stream->codec.codec_type = BV_MEDIA_TYPE_VIDEO;
	chn->video_stream->codec.codec_id = BV_CODEC_ID_H264;
	chn->video_stream->codec.width = 1280;
	chn->video_stream->codec.height = 720;
	chn->video_stream->codec.framerate = 25;
	chn->video_stream->codec.gop = 100;
	chn->video_stream->codec.bitrate = 2000;

	chn->audio_stream = malloc(sizeof(BVStream));
	chn->audio_stream->index = 1;
	chn->audio_stream->codec.codec_type = BV_MEDIA_TYPE_AUDIO;
	chn->audio_stream->codec.codec_id = BV_CODEC_ID_G711A;
	chn->audio_stream->codec.bitrate = 32000;
	chn->audio_stream->codec.sample_rate = 8000;
	chn->audio_stream->codec.channel = 1;
}


static void set_default_stream_info(BVSStream *sstream)
{
	BVCodecContext codec;
	codec.codec_type = BV_MEDIA_TYPE_VIDEO;
	codec.codec_id = BV_CODEC_ID_H264;
	codec.width = 1280;
	codec.height = 720;
	codec.framerate = 25;
	codec.gop = 100;
	codec.bitrate = 2000;
	bv_new_stream(sstream, &codec);
}

int main(int argc, const char *argv[])
{
	bv_stream_init();
	BVFormatContext *fmtctx = NULL;
	BVFormatContext *outfmt = NULL;
	BVSStream *sstream;
	BVEncodeChannel channel;
	set_default_channel_info(&channel);

	if (bv_format_open_input(&fmtctx, &channel, NULL) < 0) {
		printf("open input error\n");
		goto fail;
	}
	bv_sstream_init(&sstream);
	set_default_stream_info(sstream);
	if (bv_format_open_output(&outfmt, NULL, argv[1], sstream) < 0) {
		printf("open output error");
		goto fail;
	}

	bv_sstream_free(&sstream);
	
	BVSAVPacket spkt;
	BVSAVPacket dpkt;
	time_t start_time = time (NULL);
	time_t end_time = time(NULL);

	while (end_time - start_time < 50)
	{
		end_time = time(NULL);
		bv_spacket_init(&spkt);
		if(bv_format_read(fmtctx, &spkt) < 0)
			continue;

		if (bv_format_write(outfmt, &spkt) < 0) {
			printf("write file error\n");
		}
		bv_spacket_free(&spkt);
	}

fail:
	bv_format_close_input(&fmtctx);
	bv_format_close_output(&outfmt);

	free(channel.video_stream);
	free(channel.audio_stream);

	bv_stream_deinit();
	return 0;
}
