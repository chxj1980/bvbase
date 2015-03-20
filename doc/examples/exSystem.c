/*************************************************************************
    > File Name: exSystem.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月12日 星期四 14时45分28秒
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

#include <libbvsystem/bvsystem.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/packet.h>
#include <libbvutil/log.h>
#include <libbvconfig/common.h>
#include <libbvmedia/bvmedia.h>
#include <libbvprotocol/bvio.h>
#include <libbvprotocol/bvurl.h>
#include <libbvutil/time.h>

int main(int argc, const char *argv[])
{
    BVSystemContext *sysctx = NULL;
    BVSystem *sys = NULL;
    BVDictionary *opn = NULL;
    BVControlPacket pkt_in;
    BVControlPacket pkt_out;
    BVVideoSourceDevice videv;
    BVVideoOutputDevice vodev;

    BVAudioSourceDevice aidev;
    BVAudioOutputDevice aodev;

    bv_log_set_level(BV_LOG_DEBUG);
    bv_system_register_all();
    sys = bv_system_find_system(BV_SYSTEM_TYPE_HIS3515);
    if (!sys) {
        bv_log(NULL, BV_LOG_ERROR, "Not find HIS3515 system");
        return -1;
    }

    if (bv_system_init(&sysctx, sys, NULL) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "system init error\n");
        return -1;
    }
//videv init
    strcpy(videv.token, "0");
    strcpy(videv.interface, "BT656");
    strcpy(videv.work_mode, "4D1");
    pkt_in.data = &videv;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_VIUDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "videv config error\n");
    }

//vodev init
    strcpy(vodev.token, "2");
    strcpy(vodev.interface, "CVBS");
    strcpy(vodev.work_mode, "PAL");
    vodev.display.x = 0;
    vodev.display.y = 0;
    vodev.display.width = 704;
    vodev.display.height = 576;
    pkt_in.data = &vodev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_VOUDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "vodev config error\n");
    }

//aidev init
    strcpy(aidev.token, "0");
    aidev.channel_counts = 1;
    aidev.sample_format = 16;
    aidev.sample_rate = 8000;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    pkt_in.data = &aidev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AIMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aidev config error\n");
    }

//aodev init
    strcpy(aodev.token, "0");
    aodev.channel_counts = 1;
    aodev.sample_format = 16;
    aodev.sample_rate = 8000;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    pkt_in.data = &aodev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AOMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aodev config error\n");
    }

    bv_dict_free(&opn);
    BVMediaContext *mc = NULL; 
    BVMediaContext *mc2 = NULL; 
    BVMediaContext *mc3 = NULL; 
    BVMediaContext *mc4 = NULL; 
    BVMediaContext *mc5 = NULL; 
    BVMediaContext *mc6 = NULL; 
    BVMediaContext *mc7 = NULL; 
    BVMediaContext *mc8 = NULL; 
    BVMediaContext *mc9 = NULL; 
    BVMediaContext *mc10 = NULL; 
    BVMediaContext *mc11 = NULL; 
    BVMediaContext *mc12 = NULL; 
    BVMediaContext *mc13 = NULL; 

    BVStream *st = NULL;

    bv_media_register_all();
    bv_protocol_register_all();

//aichn vichn init 
    BVImagingSettings imaging;
    bv_dict_set(&opn, "vtoken", "0/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0", 0);
    if (bv_input_media_open(&mc3, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
// 设置图像参数
    imaging.luminance = 80;
    imaging.contrast = 10;
    imaging.saturation = 120;
    imaging.hue = 120;
    imaging.sharpness = 120;
    pkt_in.size = 1;
    pkt_in.data = &imaging;
    if (bv_media_context_control(mc3, BV_MEDIA_MESSAGE_TYPE_VIDEO_IMAGE, &pkt_in, NULL) < 0) {
        bv_log(mc3, BV_LOG_ERROR, "set video imaging error\n");
    }
//设置aichn 声音
    int volume = 80;
    pkt_in.data = &volume;
    pkt_in.size = 1;
    if (bv_media_context_control(mc3, BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME, &pkt_in, NULL) < 0) {
        bv_log(mc3, BV_LOG_ERROR, "set audio volume error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1", 0); 
    bv_dict_set(&opn, "atoken", "0/1", 0);
    if (bv_input_media_open(&mc6, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2", 0); 
    bv_dict_set(&opn, "atoken", "0/2", 0);
    if (bv_input_media_open(&mc7, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3", 0); 
    bv_dict_set(&opn, "atoken", "0/3", 0);
    if (bv_input_media_open(&mc7, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

//编码通道的创建
    bv_dict_set(&opn, "vtoken", "0/0/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0/0", 0);
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_H264, 0);
    bv_dict_set_int(&opn, "mode_id", BV_RC_MODE_ID_CBR, 0);
    bv_dict_set_int(&opn, "width", 704, 0);
    bv_dict_set_int(&opn, "height", 576, 0);
    bv_dict_set_int(&opn, "gop_size", 100, 0);
    bv_dict_set_int(&opn, "framerate", 15, 0);
    bv_dict_set_int(&opn, "bit_rate", 4096, 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    if (bv_input_media_open(&mc4, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1/1", 0); 
    bv_dict_set(&opn, "atoken", "0/1/1", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&mc5, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2/2", 0); 
    bv_dict_set(&opn, "atoken", "0/2/2", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G726, 0);
    if (bv_input_media_open(&mc8, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3/3", 0); 
    bv_dict_set(&opn, "atoken", "0/3/3", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    if (bv_input_media_open(&mc9, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set_int(&opn, "width", 352, 0);
    bv_dict_set_int(&opn, "height", 288, 0);
    bv_dict_set(&opn, "vtoken", "0/0/10", 0); 
    bv_dict_set(&opn, "atoken", "0/0/10", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&mc10, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1/11", 0); 
    bv_dict_set(&opn, "atoken", "0/1/11", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G726, 0);
    if (bv_input_media_open(&mc11, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2/12", 0); 
    bv_dict_set(&opn, "atoken", "0/2/12", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    if (bv_input_media_open(&mc12, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3/13", 0); 
    bv_dict_set(&opn, "atoken", "0/3/13", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&mc13, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    BVPacket pkt;
    int i = 0;
    bv_packet_init(&pkt);

    BVIOContext *ioctx = NULL;
    if (bv_io_open(&ioctx, "/tmp/xx.264", BV_IO_FLAG_WRITE, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }
    int flags = 0;
    while ( 1 ) {
        if (bv_input_media_read(mc4, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            if (i < 1000) {
                bv_io_write(ioctx, pkt.data, pkt.size);
            }
            if ( i == 1000) {
                bv_io_close(ioctx);
            }
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc5, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc8, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc9, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc10, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc11, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc12, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(mc13, &pkt) > 0 ) {
    //        bv_log(mc4, BV_LOG_ERROR, "pkt size %d pts %lld index %d\n", pkt.size, pkt.pts, pkt.stream_index);
            flags = 1;
            bv_packet_free(&pkt);
        }

     //   bv_usleep(10000);
        if (flags)
            i ++;
    }
    bv_input_media_close(&mc4);
    i = 0;
    bv_dict_set(&opn, "vtoken", "2/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0", 0);

    if (bv_output_media_open(&mc, NULL, "hisavo", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output media error\n");
        bv_dict_free(&opn);
        return 0;
    }
    st = bv_stream_new(mc, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->width = 704;
    st->codec->height = 576;
    st->time_base = (BVRational){1, 1000000};
    st = bv_stream_new(mc, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->sample_rate = 8000;
    st->time_base = (BVRational){1, 1000000};
    st->codec->codec_id = BV_CODEC_ID_G711A;
    st->codec->channels = 1;

    if (bv_output_media_write_header(mc, NULL) < 0) {
        bv_log(mc, BV_LOG_ERROR, "write header error\n");
        goto close;
    }

    bv_dict_set(&opn, "vtoken", "2/0/1", 0); 
    bv_dict_set(&opn, "atoken", "0/0/1", 0);
    if (bv_output_media_open(&mc2, NULL, "hisavd", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output media error\n");
        bv_dict_free(&opn);
        return 0;
    }
    st = bv_stream_new(mc2, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->codec_id = BV_CODEC_ID_H264;
    st->codec->width = 704;
    st->codec->height = 576;
    st->time_base = (BVRational){1, 1000000};
    st = bv_stream_new(mc2, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->sample_rate = 8000;
    st->codec->codec_id = BV_CODEC_ID_LPCM;
    st->codec->channels = 1;

    if (bv_output_media_write_header(mc2, NULL) < 0) {
        bv_log(mc, BV_LOG_ERROR, "write header error\n");
        goto close;
    }

    BVPacket apkt;
    BVPacket vpkt;
    apkt.stream_index = 1;
    apkt.size = 320;
    apkt.data = bv_malloc(320);
    vpkt.stream_index = 0;
    vpkt.size = 4096;
    vpkt.data = bv_malloc(4096);
    vpkt.data[0] = vpkt.data[1] = vpkt.data[2] = 0;
    vpkt.data[3] = 1;
    while (i < 10) {
        apkt.pts = bv_gettime();
        bv_output_media_write(mc2, &apkt);
        bv_usleep(10000);
        vpkt.pts = bv_gettime();
        bv_output_media_write(mc2, &vpkt);
        bv_usleep(10000);
        i++;
    }
    bv_free(apkt.data);
    bv_free(vpkt.data);

close:
    bv_dict_free(&opn);
 //   bv_output_media_write_trailer(mc);
//    bv_output_media_close(&mc);

    while (1) {
        bv_usleep(100000);
    }
    bv_system_exit(&sysctx);
    return 0;
}
