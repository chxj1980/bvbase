/*************************************************************************
    > File Name: bvconfig.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月23日 星期四 11时48分59秒
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

#ifndef BV_CONFIG_H
#define BV_CONFIG_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 *    @file
 *    @ingroup libconfig
 *    libconfig external API header
 */

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/dict.h>

#include "common.h"
#include "bvcfile.h"

typedef struct _BVConfigContext {
    const BVClass *bv_class;
    struct _BVConfig *config;
    void *priv_data;
    BVConfigFileContext *pdb;
    char url[1024];
} BVConfigContext;

#define BV_CONFIG_FLAGS_NOFILE      0x0001

#define BV_CONFIG_FLAGS_NETWORK     0x1000

typedef struct _BVConfig {
    const char *name;
    enum BVConfigType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVConfig *next;
    int flags;
//////////////////////////////////////////////////////////////////////
    int (*config_probe)(BVConfigContext *s, BVProbeData *p);
    int (*config_open)(BVConfigContext *s);
    int (*config_close)(BVConfigContext *s);
    int (*get_device_info)(BVConfigContext *s, BVDeviceInfo *devinfo);
    int (*get_video_source_device)(BVConfigContext *s, int index, BVVideoSourceDevice *config);
    int (*get_audio_source_device)(BVConfigContext *s, int index, BVAudioSourceDevice *config);
    int (*get_video_output_device)(BVConfigContext *s, int index, BVVideoOutputDevice *config);
    int (*get_audio_output_device)(BVConfigContext *s, int index, BVAudioOutputDevice *config);
    int (*get_video_source)(BVConfigContext *s, int index, BVVideoSource *config);
    int (*get_video_output)(BVConfigContext *s, int index, BVVideoOutput *config);
    int (*get_audio_source)(BVConfigContext *s, int index, BVAudioSource *config);
    int (*get_audio_output)(BVConfigContext *s, int index, BVAudioOutput *config);
    int (*get_media_device)(BVConfigContext *s, int index, BVMediaDevice *config);
    int (*set_media_device)(BVConfigContext *s, int index, BVMediaDevice *config);
    int (*get_media_encoder)(BVConfigContext *s, int index, BVMediaEncoder *config);
    int (*get_media_decoder)(BVConfigContext *s, int index, BVMediaDecoder *config);
    int (*get_talkback)(BVConfigContext *s, int index, BVTalkBack *config);
    int (*get_profiles)(BVConfigContext *s, BVMediaProfile *profiles, int *max_num);
    int (*get_video_encoder)(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);
    int (*set_video_encoder)(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);
    int (*get_video_encoder_options)(BVConfigContext *s, int channel, int index, BVVideoEncoderOption *config);
    int (*get_audio_encoder)(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);
    int (*set_audio_encoder)(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);
    int (*get_audio_encoder_options)(BVConfigContext *s, int channel, int index, BVAudioEncoderOption *config);
    int (*get_ptz_device)(BVConfigContext *s, int channel, int index, BVPTZDevice *config);
    int (*save_ptz_preset)(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);
    int (*dele_ptz_preset)(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);
} BVConfig;

void bv_config_register_all(void);
int bv_config_register(BVConfig * cfg);
BVConfig *bv_config_next(BVConfig * cfg);
BVConfig *bv_config_find(enum BVConfigType type);
BVConfig *bv_config_find_by_name(const char *cfg_name);
BVConfigContext *bv_config_context_alloc(void);
void bv_config_context_free(BVConfigContext * s);

int bv_config_open(BVConfigContext **h, const char *url, BVConfig *config, BVDictionary **options);

int bv_config_close(BVConfigContext **h);

//////////////////////////////////////////////////////////////////////

int bv_config_get_device_info(BVConfigContext *s, BVDeviceInfo *devinfo);

int bv_config_get_video_source_device(BVConfigContext *s, int index, BVVideoSourceDevice *config);

int bv_config_get_audio_source_device(BVConfigContext *s, int index, BVAudioSourceDevice *config);

int bv_config_get_video_output_device(BVConfigContext *s, int index, BVVideoOutputDevice *config);

int bv_config_get_audio_output_device(BVConfigContext *s, int index, BVAudioOutputDevice *config);

int bv_config_get_video_source(BVConfigContext *s, int index, BVVideoSource *config);

int bv_config_get_video_output(BVConfigContext *s, int index, BVVideoOutput *config);

int bv_config_get_audio_source(BVConfigContext *s, int index, BVAudioSource *config);

int bv_config_get_audio_output(BVConfigContext *s, int index, BVAudioOutput *config);

int bv_config_get_media_device(BVConfigContext *s, int index, BVMediaDevice *config);

int bv_config_set_media_device(BVConfigContext *s, int index, BVMediaDevice *config);

int bv_config_get_media_encoder(BVConfigContext *s, int index, BVMediaEncoder *config);

int bv_config_get_media_decoder(BVConfigContext *s, int index, BVMediaDecoder *config);

int bv_config_get_talkback(BVConfigContext *s, int index, BVTalkBack *config);

int bv_config_get_media_profiles(BVConfigContext *s, BVMediaProfile *profiles, int *max_num);

int bv_config_get_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);

int bv_config_set_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);

int bv_config_get_video_encoder_options(BVConfigContext *s, int channel, int index, BVVideoEncoderOption *config);

int bv_config_get_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);

int bv_config_set_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);

int bv_config_get_audio_encoder_options(BVConfigContext *s, int channel, int index, BVAudioEncoderOption *config);

int bv_config_get_ptz_device(BVConfigContext *s, int channel, int index, BVPTZDevice *config);

int bv_config_save_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);

int bv_config_dele_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_CONFIG_H */

