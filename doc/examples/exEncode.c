/*************************************************************************
    > File Name: exEncode.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月25日 星期三 20时30分06秒
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
    bv_media_register_all();
    bv_protocol_register_all();


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
    strcpy(videv.chip, "tw2866");
    strcpy(videv.dev, "/dev/tw2865dev");
    pkt_in.data = &videv;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_VIUDEV, &pkt_in, &pkt_out) < 0) {
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
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_VOUDEV, &pkt_in, &pkt_out) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "vodev config error\n");
    }

//aidev init
    strcpy(aidev.token, "0");
    aidev.channel_mode = 1;
    aidev.channel_counts = 16;
    aidev.sample_format = 16;
    aidev.sample_rate = 8000;
    aidev.sample_points = 320;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    strcpy(aidev.chip, "tw2866");
    strcpy(aidev.dev, "/dev/tw2865dev");
    pkt_in.data = &aidev;
    if (bv_system_control(sysctx, BV_SYS_MESSAGE_TYPE_AIMDEV, &pkt_in, NULL) < 0) {
        bv_log(sysctx, BV_LOG_ERROR, "aidev config error\n");
    }
#if 1
    strcpy(aidev.token, "1");
    aidev.channel_mode = 1;
    aidev.channel_counts = 2; 
    aidev.sample_format = 16;
    aidev.sample_rate = 8000;
    aidev.sample_points = 320;
    strcpy(aodev.work_mode, "I2S_SLAVE");
    strcpy(aidev.chip, "tlv320aic23");
    strcpy(aidev.dev, "/dev/tlv320aic23");
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
    BVMediaContext *avictx = NULL; 
    BVMediaContext *avictx1 = NULL; 

    bv_dict_set(&opn, "atoken", "0/0", 0);
    bv_dict_set(&opn, "achip", "tw2866", 0);
    bv_dict_set(&opn, "adev", "/dev/tw2865dev", 0);
    if (bv_input_media_open(&avictx, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }
//设置aichn 声音
    int volume = 80;
    pkt_in.data = &volume;
    pkt_in.size = 1;
    if (bv_media_context_control(avictx, BV_MEDIA_MESSAGE_TYPE_AUDIO_VOLUME, &pkt_in, NULL) < 0) {
        bv_log(avictx, BV_LOG_ERROR, "set audio volume error\n");
    }

    bv_dict_set(&opn, "atoken", "1/0", 0);
    bv_dict_set(&opn, "achip", "tlv320aic23", 0);
    bv_dict_set(&opn, "adev", "/dev/tlv320aic23", 0);
    if (bv_input_media_open(&avictx1, NULL, "hisavi://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    BVMediaContext *avectx1 = NULL; 
    bv_dict_set(&opn, "atoken", "1/0/0", 0);
    bv_dict_set(&opn, "achip", "tlv320aic23", 0);
    bv_dict_set(&opn, "adev", "/dev/tlv320aic23", 0);
    bv_dict_set_int(&opn, "acodec_id", BV_CODEC_ID_G711A, 0);
    if (bv_input_media_open(&avectx1, NULL, "hisave://", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open input media error\n");
    }

    int i = 0;
    BVPacket pkt;
    BVIOContext *ioctx = NULL;
    if (bv_io_open(&ioctx, "/tmp/xx.g711", BV_IO_FLAG_WRITE, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }

    while ( i < 200) {
        bv_packet_init(&pkt);
        if (bv_input_media_read(avectx1, &pkt) > 0 ) {
            bv_log(avectx1, BV_LOG_ERROR, "audio pkt size %d\n", pkt.size);
            bv_io_write(ioctx, pkt.data, pkt.size);
            bv_packet_free(&pkt);
            i ++;
        }
    }

    bv_io_close(ioctx);
    bv_input_media_close(&avectx1);
    bv_input_media_close(&avictx1);
    bv_input_media_close(&avictx);
    bv_system_exit(&sysctx);
    return 0;
}
