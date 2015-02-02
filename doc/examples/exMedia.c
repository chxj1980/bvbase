/*************************************************************************
	> File Name: exMedia.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2015年01月05日 星期一 09时39分09秒
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <libbvutil/bvutil.h>
#include <libbvmedia/bvmedia.h>
#include <libbvprotocol/bvio.h>
#include <libbvprotocol/bvurl.h>

int main(int argc, const char *argv[])
{
    BVMediaContext *mc = NULL;
    BVMediaContext *out = NULL;
    BVDictionary *opn = NULL;
    BVDictionary *iopn = NULL;
    BVStream *st = NULL;
    char filename[1024] = { 0 };
    bv_media_register_all(); 
    bv_protocol_register_all();
    bv_log_set_level(BV_LOG_INFO);
//    av_log_set_level(BV_LOG_DEBUG);
#if 1
    bv_dict_set(&opn, "user", "admin", 0);
    bv_dict_set(&opn, "passwd", "12345", 0);
    bv_dict_set(&opn, "token", "Profile_1", 0);
    bv_dict_set(&opn, "timeout", "2", 0);
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_H264, 0);
#else
    bv_dict_set(&opn, "token", "Profile_1", 0);
#endif

    //sprintf(filename, "%s", "file:///tmp/00_20120412_031132_");
    sprintf(filename, "%s", "bvfs://tmp/00_20120412_031132_");
    sprintf(filename + strlen(filename), "%ld.dav", time(NULL));
    if (bv_input_media_open(&mc, NULL, "onvifave://192.168.6.149:80/onvif/device_service", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open media error\n");
        return -1;
    }
    if (bv_output_media_open(&out, filename, "dav", NULL, NULL) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output file error\n");
        goto close1;
    }
    int i = 0;
    BVPacket pkt;

    for (i = 0; i < mc->nb_streams; i++) {
        if (mc->streams[i]->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
            bv_log(mc, BV_LOG_INFO, "stream count %d\n", mc->nb_streams);
            bv_log(mc, BV_LOG_INFO, "codec time base %d/%d\n", mc->streams[i]->codec->time_base.num, mc->streams[i]->codec->time_base.den);
            bv_log(mc, BV_LOG_INFO, "stream time base %d/%d\n", mc->streams[i]->time_base.num, mc->streams[i]->time_base.den);
            bv_log(mc, BV_LOG_INFO, "gop size %d\n", mc->streams[i]->codec->gop_size);
            bv_log(mc, BV_LOG_INFO, "extradata size %d\n", mc->streams[i]->codec->extradata_size);
            bv_log(mc, BV_LOG_INFO, "codec ID %d\n", mc->streams[i]->codec->codec_id);
            bv_log(mc, BV_LOG_INFO, "video size %dX%d\n", mc->streams[i]->codec->width, mc->streams[i]->codec->height);
            bv_log(mc, BV_LOG_INFO, "video profile %d\n", mc->streams[i]->codec->profile);
        } else if (mc->streams[i]->codec->codec_type == BV_MEDIA_TYPE_AUDIO) {
            bv_log(mc, BV_LOG_INFO, "audio sample_rate %d\n", mc->streams[i]->codec->sample_rate);
            bv_log(mc, BV_LOG_INFO, "audio stream time base %d/%d\n", mc->streams[i]->time_base.num, mc->streams[i]->time_base.den);
            bv_log(mc, BV_LOG_INFO, "audio channels %d\n", mc->streams[i]->codec->channels);
            bv_log(mc, BV_LOG_INFO, "audio type %d\n", mc->streams[i]->codec->codec_id);
            bv_log(mc, BV_LOG_INFO, "audio sample_fmt %d\n", mc->streams[i]->codec->sample_fmt);
        }
    }
    for (i = 0; i < mc->nb_streams; i++) {
        if (mc->streams[i]->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
            st = bv_stream_new(out, NULL);
            st->time_base = (BVRational){1, 1000000};
            st->codec->time_base = (BVRational){1, 25};
            st->codec->width = 1280;
            st->codec->height = 960;
            st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
            st->codec->codec_id = BV_CODEC_ID_H264;
        } else if (mc->streams[i]->codec->codec_type == BV_MEDIA_TYPE_AUDIO) {
            st = bv_stream_new(out, NULL);
            st->codec->sample_rate = 8000;
            st->time_base = (BVRational){1, 1000000};
            st->codec->codec_id = BV_CODEC_ID_G711A;
            st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
            st->codec->channels = 1;
        }
    }
    if (!(out->omedia->flags & BV_MEDIA_FLAGS_NOFILE)) {
        bv_dict_set_int(&iopn, "file_type", 1, 0);
        bv_dict_set_int(&iopn, "storage_type", 2, 0);
        bv_dict_set_int(&iopn, "channel_num", 2, 0);

        if (bv_io_open(&out->pb, filename, BV_IO_FLAG_WRITE, NULL, &iopn) < 0) {
            bv_log(out, BV_LOG_ERROR, "open file error\n");
            goto close2;
        }
    }
    if (bv_output_media_write_header(out, NULL) < 0) {
        bv_log(out, BV_LOG_ERROR, "write header error\n");
        goto close2;
    }
    while (i < 2000) {
       bv_packet_init(&pkt); 
       if (bv_input_media_read(mc, &pkt) < 0)
           continue;
       st = mc->streams[pkt.stream_index];
       bv_log(mc, BV_LOG_DEBUG, "Before pkt size %d pts %llu strindex %d\n", pkt.size, pkt.pts, pkt.stream_index);
       pkt.pts = bv_rescale_q(pkt.pts, mc->streams[pkt.stream_index]->time_base, out->streams[pkt.stream_index]->time_base);
       //pkt.pts = (pkt.pts * mc->streams[pkt.stream_index]->time_base.num * out->streams[pkt.stream_index]->time_base.den) / mc->streams[pkt.stream_index]->time_base.den;
       bv_log(mc, BV_LOG_DEBUG, "After pkt size %d  pts %llu strindex %d\n", pkt.size, pkt.pts, pkt.stream_index);
       if (bv_output_media_write(out, &pkt)) {
            bv_log(out, BV_LOG_ERROR, "write packet error\n");
       }
       i ++;
       bv_packet_free(&pkt);
    }
close2:
    bv_output_media_write_trailer(out);
    if (!(out->omedia->flags & BV_MEDIA_FLAGS_NOFILE)) {
        bv_dict_free(&iopn);
        bv_io_close(out->pb);
    }
    bv_output_media_close(&out);
close1:
    bv_dict_free(&opn);
    bv_input_media_close(&mc);
    return 0;
}
