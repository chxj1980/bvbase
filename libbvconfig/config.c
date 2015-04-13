/*************************************************************************
    > File Name: config.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月15日 星期四 16时26分53秒
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

#line 25 "config.c"

#include <libbvutil/bvstring.h>

#include "bvconfig.h"

static int init_config(BVConfigContext *s, const char *url)
{
    BVProbeData pd = {url, NULL, 0};
    int score = BV_PROBE_SCORE_RETRY;
    int score_max = score;
    BVConfig *config = NULL;
    BVConfig *config1 = NULL;
    
    while (config1 = bv_config_next(config1)) {
        if (config1->config_probe) {
            score = config1->config_probe(s, &pd);
        }
        if (score > score_max) {
            score_max = score;
            config = config1;
        } else if (score == score_max) {
            config = NULL;
        }
    }

    s->config = config;
    return config == NULL ? -1 : 0;
}

int bv_config_open(BVConfigContext **h, const char *url, BVConfig *config, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVConfigContext *s = *h;
    int ret = 0;
    if (!s && !(s = bv_config_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", __FILE__, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);

    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;
    if (config)
        s->config = config;
    else
        ret = init_config(s, url);
     if (ret < 0) {
        ret = BVERROR(EINVAL);
        goto fail;
     }
    if (s->config->priv_data_size > 0) {
        s->priv_data = bv_mallocz(s->config->priv_data_size);
        if (!s->priv_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        if (s->config->priv_class) {
            *(const BVClass **) s->priv_data = s->config->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                bv_log(s, BV_LOG_ERROR, "set dict error\n");
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }

    if (url)
        bv_strlcpy(s->url, url, sizeof(s->url));
    if (!s->config->config_open) {
        ret = BVERROR(ENOSYS); 
        goto fail;
    }

    bv_dict_free(&tmp);
    if((ret = s->config->config_open(s)) < 0) {
        goto fail;
    }
    *h = s;
    return 0;
fail:
    bv_dict_free(&tmp);
    bv_config_context_free(s);
    return ret;
}

int bv_config_close(BVConfigContext **h)
{
    BVConfigContext *s = *h;
    if (s->config && s->config->config_close)
        s->config->config_close(s);
    bv_config_context_free(s);
    *h = NULL;
    return 0;
}

int bv_config_get_device_info(BVConfigContext *s, BVDeviceInfo *devinfo)
{
    if (!s || !devinfo || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_device_info)
        return BVERROR(ENOSYS);
    return s->config->get_device_info(s, devinfo);
}

int bv_config_get_video_source_device(BVConfigContext *s, int index, BVVideoSourceDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_source_device)
        return BVERROR(ENOSYS);
    return s->config->get_video_source_device(s, index, config);
}

int bv_config_get_audio_source_device(BVConfigContext *s, int index, BVAudioSourceDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_source_device)
        return BVERROR(ENOSYS);
    return s->config->get_audio_source_device(s, index, config);
}

int bv_config_get_video_output_device(BVConfigContext *s, int index, BVVideoOutputDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_output_device)
        return BVERROR(ENOSYS);
    return s->config->get_video_output_device(s, index, config);
}

int bv_config_get_audio_output_device(BVConfigContext *s, int index, BVAudioOutputDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_output_device)
        return BVERROR(ENOSYS);
    return s->config->get_audio_output_device(s, index, config);
}

int bv_config_get_video_source(BVConfigContext *s, int index, BVVideoSource *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_source)
        return BVERROR(ENOSYS);
    return s->config->get_video_source(s, index, config);
}

int bv_config_get_video_output(BVConfigContext *s, int index, BVVideoOutput *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_output)
        return BVERROR(ENOSYS);
    return s->config->get_video_output(s, index, config);
}

int bv_config_get_audio_source(BVConfigContext *s, int index, BVAudioSource *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_source)
        return BVERROR(ENOSYS);
    return s->config->get_audio_source(s, index, config);
}

int bv_config_get_audio_output(BVConfigContext *s, int index, BVAudioOutput *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_output)
        return BVERROR(ENOSYS);
    return s->config->get_audio_output(s, index, config);
}

int bv_config_get_media_device(BVConfigContext *s, int index, BVMediaDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_media_device)
        return BVERROR(ENOSYS);
    return s->config->get_media_device(s, index, config);
}

int bv_config_set_media_device(BVConfigContext *s, int index, BVMediaDevice *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->set_media_device)
        return BVERROR(ENOSYS);
    return s->config->set_media_device(s, index, config);
}

int bv_config_get_media_encoder(BVConfigContext *s, int index, BVMediaEncoder *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_media_encoder)
        return BVERROR(ENOSYS);
    return s->config->get_media_encoder(s, index, config);
}

int bv_config_get_media_decoder(BVConfigContext *s, int index, BVMediaDecoder *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_media_decoder)
        return BVERROR(ENOSYS);
    return s->config->get_media_decoder(s, index, config);
}

int bv_config_get_talkback(BVConfigContext *s, int index, BVTalkBack *config)
{
    if (!s || !config || !s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_talkback)
        return BVERROR(ENOSYS);
    return s->config->get_talkback(s, index, config);
}

int bv_config_get_media_profiles(BVConfigContext *s, BVMediaProfile *profiles, int *max_num)
{
    if (!s || !profiles ||!s->config)
        return BVERROR(EINVAL);
    if (!s->config->get_profiles)
        return BVERROR(ENOSYS);
    return s->config->get_profiles(s, profiles, max_num);
}

int bv_config_get_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_encoder)
        return BVERROR(ENOSYS);
    return s->config->get_video_encoder(s, channel, index, config);
}

int bv_config_set_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->set_video_encoder)
        return BVERROR(ENOSYS);
    return s->config->set_video_encoder(s, channel, index, config);
}

int bv_config_get_video_encoder_options(BVConfigContext *s, int channel, int index, BVVideoEncoderOption *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->get_video_encoder_options)
        return BVERROR(ENOSYS);
    return s->config->get_video_encoder_options(s, channel, index, config);
}

int bv_config_get_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_encoder)
        return BVERROR(ENOSYS);
    return s->config->get_audio_encoder(s, channel, index, config);
}

int bv_config_set_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->set_audio_encoder)
        return BVERROR(ENOSYS);
    return s->config->set_audio_encoder(s, channel, index, config);
}

int bv_config_get_audio_encoder_options(BVConfigContext *s, int channel, int index, BVAudioEncoderOption *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->get_audio_encoder_options)
        return BVERROR(ENOSYS);
    return s->config->get_audio_encoder_options(s, channel, index, config);
}

int bv_config_get_ptz_device(BVConfigContext *s, int channel, int index, BVPTZDevice *config)
{
    if (!s || !config)
        return BVERROR(EINVAL);
    if (!s->config->get_ptz_device)
        return BVERROR(ENOSYS);
    return s->config->get_ptz_device(s, channel, index, config);
}

int bv_config_save_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset)
{
    if (!s || !preset)
        return BVERROR(EINVAL);
    if (!s->config->save_ptz_preset)
        return BVERROR(ENOSYS);
    return s->config->save_ptz_preset(s, channel, index, preset);
}

int bv_config_dele_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset)
{
    if (!s || !preset)
        return BVERROR(EINVAL);
    if (!s->config->dele_ptz_preset)
        return BVERROR(ENOSYS);
    return s->config->dele_ptz_preset(s, channel, index, preset);
}
