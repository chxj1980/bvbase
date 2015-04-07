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
#include <pthread.h>

static void *snapshot(void *args)
{
    BVDictionary *opn = NULL;
    BVPacket pkt;
    int i = 0;
    bv_packet_init(&pkt);
    int flags = 1;

    BVMediaContext *snp1 = NULL; 
    BVMediaContext *snp2 = NULL; 
    BVMediaContext *snp3 = NULL; 
    BVMediaContext *snp4 = NULL; 

    BVIOContext *ioctx = NULL;
    if (bv_io_open(&ioctx, "/tmp/xx.jpg", BV_IO_FLAG_WRITE, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }

    bv_dict_set(&opn, "vtoken", "0/0/24", 0); 
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_JPEG, 0);
    bv_dict_set_int(&opn, "framerate", 5, 0);

    if (bv_input_media_open(&snp1, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/0/25", 0); 
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_JPEG, 0);
    bv_dict_set_int(&opn, "framerate", 2, 0);

    if (bv_input_media_open(&snp2, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
    bv_dict_set(&opn, "vtoken", "0/0/26", 0); 
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_JPEG, 0);
    bv_dict_set_int(&opn, "framerate", 3, 0);

    if (bv_input_media_open(&snp3, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/0/27", 0); 
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_JPEG, 0);
    bv_dict_set_int(&opn, "framerate", 4, 0);

    if (bv_input_media_open(&snp4, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
    while (1) {
        if (bv_input_media_read(snp1, &pkt) > 0 ) {
            if (flags) {
                bv_io_write(ioctx, pkt.data, pkt.size);
                bv_io_close(ioctx);
                flags = 0;
            }
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(snp2, &pkt) > 0 ) {
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(snp3, &pkt) > 0 ) {
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(snp4, &pkt) > 0 ) {
            bv_packet_free(&pkt);
        }
        usleep(500000);
    }
    return 0;
}


int main(int argc, const char *argv[])
{
    BVSystemContext *sysctx = NULL;
    BVSystem *sys = NULL;
    BVDictionary *opn = NULL;
    BVControlPacket pkt_in;
    BVControlPacket pkt_out;
    BVVideoSourceDevice videv;
    BVVideoOutputDevice vodev;

    pthread_t snapshot_t;

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
    aidev.channel_mode = 1;
    aidev.channel_counts = 16;
    aidev.sample_format = 16;
    aidev.sample_rate = 8000;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    pkt_in.data = &aidev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AIMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aidev config error\n");
    }
#if 0
    strcpy(aidev.token, "1");
    aidev.channel_mode = 1;
    aidev.channel_counts = 2; 
    aidev.sample_format = 16;
    aidev.sample_rate = 8000;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    pkt_in.data = &aidev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AIMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aidev config error\n");
    }
#endif
//aodev init
    strcpy(aodev.token, "0");
    aodev.channel_mode = 1;
    aodev.channel_counts = 2;
    aodev.sample_format = 16;
    aodev.sample_rate = 8000;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    pkt_in.data = &aodev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AOMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aodev config error\n");
    }

    bv_dict_free(&opn);
    BVMediaContext *avoctx = NULL; 
    BVMediaContext *avdctx = NULL; 
    BVMediaContext *avictx = NULL; 
    BVMediaContext *avictx1 = NULL; 
    BVMediaContext *avictx2 = NULL; 
    BVMediaContext *avictx3 = NULL; 
    BVMediaContext *avictx4 = NULL; 
    BVMediaContext *avectx1 = NULL; 
    BVMediaContext *avectx2 = NULL; 
    BVMediaContext *avectx4 = NULL; 
    BVMediaContext *avectx5 = NULL; 
    BVMediaContext *avectx6 = NULL; 
    BVMediaContext *avectx7 = NULL; 
    BVMediaContext *avectx8 = NULL; 
    BVMediaContext *avectx9 = NULL; 
    BVMediaContext *avectx10 = NULL; 

    BVStream *st = NULL;

    bv_codec_register_all();
    bv_media_register_all();
    bv_protocol_register_all();

//aichn vichn init 
    BVImagingSettings imaging;
    bv_dict_set(&opn, "vtoken", "0/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0", 0);
    if (bv_input_media_open(&avictx, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
// 设置图像参数
    imaging.luminance = 128;
    imaging.contrast = 128;
    imaging.saturation = 128;
    imaging.hue = 120;
    imaging.sharpness = 120;
    pkt_in.size = 1;
    pkt_in.data = &imaging;
    if (bv_media_context_control(avictx, BV_MEDIA_MESSAGE_TYPE_VIDEO_IMAGE, &pkt_in, NULL) < 0) {
        bv_log(avictx, BV_LOG_ERROR, "set video imaging error\n");
    }
//设置aichn 声音
    int volume = 80;
    pkt_in.data = &volume;
    pkt_in.size = 1;
    if (bv_media_context_control(avictx, BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME, &pkt_in, NULL) < 0) {
        bv_log(avictx, BV_LOG_ERROR, "set audio volume error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1", 0); 
    bv_dict_set(&opn, "atoken", "0/1", 0);
    if (bv_input_media_open(&avictx1, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2", 0); 
    bv_dict_set(&opn, "atoken", "0/2", 0);
    if (bv_input_media_open(&avictx2, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3", 0); 
    bv_dict_set(&opn, "atoken", "0/3", 0);
    if (bv_input_media_open(&avictx3, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
    bv_dict_free(&opn);
    bv_dict_set(&opn, "atoken", "1/0", 0);
    if (bv_input_media_open(&avictx4, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

//编码通道的创建
    int blocked = 1;
    bv_dict_set(&opn, "vtoken", "0/0/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0/0", 0);
    bv_dict_set_int(&opn, "vcodec_id", BV_CODEC_ID_H264, 0);
    bv_dict_set_int(&opn, "mode_id", BV_RC_MODE_ID_VBR, 0);
    bv_dict_set_int(&opn, "width", 704, 0);
    bv_dict_set_int(&opn, "height", 576, 0);
    bv_dict_set_int(&opn, "gop_size", 100, 0);
    bv_dict_set_int(&opn, "framerate", 25, 0);
    bv_dict_set_int(&opn, "bit_rate", 4096, 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    bv_dict_set_int(&opn, "blocked", blocked, 0);
    if (bv_input_media_open(&avectx1, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1/1", 0); 
    bv_dict_set(&opn, "atoken", "0/1/1", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&avectx2, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2/2", 0); 
    bv_dict_set(&opn, "atoken", "0/2/2", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G726, 0);
    if (bv_input_media_open(&avectx4, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3/3", 0); 
    bv_dict_set(&opn, "atoken", "0/3/3", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    if (bv_input_media_open(&avectx5, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
#if 1
    //bv_dict_set_int(&opn, "width", 352, 0);
    //bv_dict_set_int(&opn, "height", 288, 0);
    bv_dict_set(&opn, "vtoken", "0/0/10", 0); 
    bv_dict_set(&opn, "atoken", "0/0/10", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&avectx6, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/1/11", 0); 
    bv_dict_set(&opn, "atoken", "0/1/11", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G726, 0);
    if (bv_input_media_open(&avectx7, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/2/12", 0); 
    bv_dict_set(&opn, "atoken", "0/2/12", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_LPCM, 0);
    if (bv_input_media_open(&avectx8, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    bv_dict_set(&opn, "vtoken", "0/3/13", 0); 
    bv_dict_set(&opn, "atoken", "0/3/13", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&avectx9, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
    bv_dict_free(&opn);
#if 0
    bv_dict_set(&opn, "vtoken", "0/3/14", 0); 
    bv_dict_set(&opn, "atoken", "1/0/14", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&avectx10, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
#endif
#endif

    pthread_create(&snapshot_t, NULL, snapshot , NULL);

    BVPacket pkt;
    int i = 0;
    bv_packet_init(&pkt);

    BVIOContext *ioctx = NULL;
    BVIOContext *ioctx2 = NULL;
    if (bv_io_open(&ioctx, "/tmp/xx.264", BV_IO_FLAG_WRITE, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }
    if (bv_io_open(&ioctx2, "/tmp/xx.pcm", BV_IO_FLAG_WRITE, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }

    int flags = 0;
    while ( i < 500 + i) {
        if (bv_input_media_read(avectx1, &pkt) > 0 ) {
            flags = 1;
            if (i < 400) {
                if (avectx1->streams[pkt.stream_index]->codec->codec_type == BV_MEDIA_TYPE_VIDEO) {
                    bv_io_write(ioctx, pkt.data, pkt.size);
                } else {
                    //bv_log(avectx1, BV_LOG_ERROR, "audio pkt size %d\n", pkt.size);
                    bv_io_write(ioctx2, pkt.data, pkt.size);
                }
            }
            if ( i == 400) {
                bv_io_close(ioctx);
            }
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx2, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx4, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx5, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }

        if (bv_input_media_read(avectx6, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx7, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx8, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
        if (bv_input_media_read(avectx9, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
#if 0
        if (bv_input_media_read(avectx10, &pkt) > 0 ) {
            flags = 1;
            bv_packet_free(&pkt);
        }
#endif
        if (blocked == 0) {
            bv_usleep(10000);
        }
        if (flags)
            i ++;
    }
    bv_input_media_close(&avectx1);
    bv_input_media_close(&avectx2);
    bv_input_media_close(&avectx4);
    bv_input_media_close(&avectx5);
    bv_input_media_close(&avectx6);
    bv_input_media_close(&avectx7);
    bv_input_media_close(&avectx8);
    bv_input_media_close(&avectx9);

    i = 0;
    bv_dict_set(&opn, "vtoken", "2/0", 0); 
    bv_dict_set(&opn, "atoken", "0/0", 0);

    if (bv_output_media_open(&avoctx, NULL, "hisavo", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output media error\n");
        bv_dict_free(&opn);
        return 0;
    }
    st = bv_stream_new(avoctx, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->width = 704;
    st->codec->height = 576;
    st->time_base = (BVRational){1, 1000000};
    st = bv_stream_new(avoctx, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->sample_rate = 8000;
    st->time_base = (BVRational){1, 1000000};
    st->codec->codec_id = BV_CODEC_ID_G711A;
    st->codec->channels = 1;

    if (bv_output_media_write_header(avoctx, NULL) < 0) {
        bv_log(avoctx, BV_LOG_ERROR, "write header error\n");
        goto close;
    }

    bv_dict_set(&opn, "vtoken", "2/0/1", 0); 
    bv_dict_set(&opn, "atoken", "0/0/1", 0);
    bv_dict_set_int(&opn, "apacked", 1, 0);
    if (bv_output_media_open(&avdctx, NULL, "hisavd", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output media error\n");
        bv_dict_free(&opn);
        return 0;
    }
    st = bv_stream_new(avdctx, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st->codec->codec_id = BV_CODEC_ID_H264;
    st->codec->width = 704;
    st->codec->height = 576;
    st->time_base = (BVRational){1, 1000000};
    st = bv_stream_new(avdctx, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;
    st->codec->sample_rate = 8000;
    st->codec->codec_id = BV_CODEC_ID_LPCM;
    st->codec->channels = 1;

    if (bv_output_media_write_header(avdctx, NULL) < 0) {
        bv_log(avoctx, BV_LOG_ERROR, "write header error\n");
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
        bv_output_media_write(avdctx, &apkt);
        bv_usleep(10000);
        vpkt.pts = bv_gettime();
        bv_output_media_write(avdctx, &vpkt);
        bv_usleep(10000);
        i++;
    }
    bv_free(apkt.data);
    bv_free(vpkt.data);

close:
    bv_dict_free(&opn);
    bv_output_media_write_trailer(avoctx);
    bv_output_media_close(&avoctx);

#if 0
    while (1) {
        bv_usleep(100000);
    }
#endif
    bv_system_exit(&sysctx);
    return 0;
}
