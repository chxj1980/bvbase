/*************************************************************************
    > File Name: exConfig.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月15日 星期四 16时25分01秒
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

#include <libbvutil/bvutil.h>
#include <libbvconfig/common.h>
#include <libbvconfig/bvconfig.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>

#define MEMSET_STRUCT(x) memset(&(x), 0, sizeof(x))

static void dump_profiles(BVMediaProfile *profile)
{
    int i = 0;
    bv_log(NULL, BV_LOG_INFO, "dump profiles start >>>>>>>>>>>>>>>>\n");
    bv_log(NULL, BV_LOG_INFO, "profile name %s\n", profile->token);
    bv_log(NULL, BV_LOG_INFO, "media source name %s\n", profile->video_source->token);
    bv_log(NULL, BV_LOG_INFO, "media info video codec %d\n", profile->video_encoder->codec_context.codec_id);
    bv_log(NULL, BV_LOG_INFO, "media info video size %dx%d\n", profile->video_encoder->codec_context.width, profile->video_encoder->codec_context.height);

    bv_log(NULL, BV_LOG_INFO, "media info audio samplerate %d\n", profile->audio_encoder->codec_context.sample_rate);
    bv_log(NULL, BV_LOG_INFO, "dump profiles end <<<<<<<<<<<<<<<<<<\n");
}

static void free_profiles(BVMediaProfile *profile)
{
    bv_free(profile->video_source);
    bv_free(profile->audio_source);
    bv_free(profile->video_encoder);
    bv_free(profile->audio_encoder);
}

static void dump_video_encoder(BVVideoEncoder *encoder)
{
    bv_log(NULL, BV_LOG_INFO, "dump video encoder start >>>>>>>>>>>>>>>>\n");
    bv_log(NULL, BV_LOG_INFO, "video token %s\n", encoder->token);
    bv_log(NULL, BV_LOG_INFO, "video type %d\n", encoder->type);
    bv_log(NULL, BV_LOG_INFO, "video id %d\n", encoder->codec_context.codec_id);
    bv_log(NULL, BV_LOG_INFO, "video size %dx%d\n", encoder->codec_context.width, encoder->codec_context.height);
    bv_log(NULL, BV_LOG_INFO, "video framerate %d\n", encoder->codec_context.time_base.den / encoder->codec_context.time_base.num);
    bv_log(NULL, BV_LOG_INFO, "dump video encoder end <<<<<<<<<<<<<<<<<<\n");
}

static void free_video_encoder_option(BVVideoEncoderOption *video_encoder_option)
{
    if (video_encoder_option->h264) {
        free(video_encoder_option->h264->resolutions);
        free(video_encoder_option->h264);
        video_encoder_option->h264 = NULL;
    }
    if (video_encoder_option->mpeg) {
        free(video_encoder_option->mpeg->resolutions);
        free(video_encoder_option->mpeg);
        video_encoder_option->mpeg = NULL;
    }
    if (video_encoder_option->jpeg) {
        free(video_encoder_option->jpeg->resolutions);
        free(video_encoder_option->jpeg);
        video_encoder_option->jpeg = NULL;
    }
}
int main(int argc, const char *argv[])
{
    int ret = 0;
    int i = 0;
    BVMediaProfile profiles[5];
    BVVideoEncoder video_encoder;
    BVVideoEncoderOption video_encoder_option;
    BVDeviceInfo devinfo;
    BVConfigContext *config_context = NULL; 
    BVDictionary *options = NULL;

    MEMSET_STRUCT(video_encoder);
    MEMSET_STRUCT(video_encoder_option);
    MEMSET_STRUCT(devinfo);

    bv_config_register_all();
    bv_dict_set(&options, "user", "admin", 0);
    bv_dict_set(&options, "passwd", "12345", 0);
    bv_dict_set(&options, "timeout", "5", 0);
    memset(&devinfo, 0, sizeof(devinfo));
    if ((ret = bv_config_open(&config_context, "onvif_cfg://192.168.6.149:80/onvif/device_service", NULL, &options)) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open config error");
        bv_dict_free(&options);
        return BVERROR(EINVAL);
    }
#if 1
    if (bv_config_get_device_info(config_context, &devinfo) < 0) {
        bv_log(config_context, BV_LOG_ERROR, "get device info error\n");
    }
    bv_log(config_context, BV_LOG_INFO, "id %s\n", devinfo.device_id);
    bv_log(config_context, BV_LOG_INFO, "videosource %d\n", devinfo.video_sources);
    bv_log(config_context, BV_LOG_INFO, "audioSource %d\n", devinfo.audio_sources);
    int  max_num = 5;
    if (bv_config_get_media_profiles(config_context, profiles, &max_num) < 0) {
        bv_log(config_context, BV_LOG_ERROR, "get media profiles error\n");
    }
    for (i = 0; i < max_num; i++) {
        dump_profiles(profiles + i);
//        free_profiles(profiles + i);
    }

    strcpy(video_encoder.token, profiles[0].video_encoder->token);
    if (bv_config_get_video_encoder(config_context, 1, 1, &video_encoder) < 0) {
        bv_log(config_context, BV_LOG_ERROR, "get encoder config error\n");
    }
    dump_video_encoder(&video_encoder);

    strcpy(video_encoder_option.token, profiles[0].video_encoder->token);
    bv_log(config_context, BV_LOG_INFO, "video_encoder_option token %s\n", video_encoder_option.token);
    if (bv_config_get_video_encoder_options(config_context, 1, 1, &video_encoder_option) < 0) {
        bv_log(config_context, BV_LOG_ERROR, "get encoder config options error\n");
    }
    bv_log(config_context, BV_LOG_INFO, "video quality range %lld .. %lld\n", video_encoder_option.quality.min, video_encoder_option.quality.max);
    video_encoder.codec_context.bit_rate = 1000;
    video_encoder.codec_context.gop_size = 30;
    video_encoder.codec_context.time_base = (BVRational) {1 , 10};
    video_encoder.codec_context.quality = (int)video_encoder_option.quality.max - 1;
    bv_log(config_context, BV_LOG_INFO, "quality %d\n", video_encoder.codec_context.quality); 
    if (bv_config_set_video_encoder(config_context, 1, 1, &video_encoder)) {
        bv_log(config_context, BV_LOG_ERROR, "set encoder config error\n");
    }

    free_video_encoder_option(&video_encoder_option);

#endif

    for (i = 0; i < max_num; i++) {
        free_profiles(profiles + i);
    }

    bv_dict_free(&options);
    bv_config_close(&config_context);
    return 0;
}
