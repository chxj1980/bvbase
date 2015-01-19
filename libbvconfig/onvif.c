/*************************************************************************
    > File Name: onvif.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月30日 星期四 17时58分08秒
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

#include <libbvutil/bvstring.h>

#include <wsseapi.h>
#include <wsaapi.h>

#include "bvconfig.h"

typedef struct OnvifContext {
    const BVClass *bv_class;
    int timeout;
    char svrurl[1024];
    char *media_url;
    char *device_url;
    char *ptz_url;
    char *deviceio_url;
    char *user;
    char *passwd;
    struct soap *soap;
} OnvifContext;

#define ONVIF_TMO (-5000)
#define MEMSET_STRUCT(X)    memset(&(X), 0, sizeof((X)));

static struct soap *bv_soap_new(OnvifContext *onvifctx)
{
    struct soap *soap = NULL;
    int timeout = onvifctx->timeout;
    soap = soap_new();
    if (soap == NULL) {
        return NULL;
    }
    soap_set_namespaces(soap, namespaces);
    if (!timeout) {
        timeout = ONVIF_TMO;
    }
    bv_log(onvifctx, BV_LOG_INFO, "timeout %d\n", timeout);
    soap->recv_timeout = timeout;
    soap->send_timeout = timeout;
    soap->connect_timeout = timeout;
    return soap;
}

static void bv_soap_free(struct soap *soap)
{
    if (soap == NULL) {
        return;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
}

//FIXME
//熊迈的IPC GetServices 返回NULL
static int bv_onvif_service_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    int i = 0;
    struct soap *soap = onvifctx->soap;
    struct _tds__GetServices tds__GetServices;
    struct _tds__GetServicesResponse tds__GetServicesResponse;

    MEMSET_STRUCT(tds__GetServices);
    MEMSET_STRUCT(tds__GetServicesResponse);
    tds__GetServices.IncludeCapability = xsd__boolean__false_;

    retval = soap_call___tds__GetServices(soap, onvifctx->svrurl, NULL, &tds__GetServices, &tds__GetServicesResponse);

    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get Services URI error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    }
    for (i = 0; i < tds__GetServicesResponse.__sizeService; i++) {
       if (bv_strcasecmp(SOAP_NAMESPACE_OF_tds, tds__GetServicesResponse.Service[i].Namespace) == 0) {
           onvifctx->device_url = bv_strdup(tds__GetServicesResponse.Service[i].XAddr);
       }

       if (bv_strcasecmp(SOAP_NAMESPACE_OF_trt, tds__GetServicesResponse.Service[i].Namespace) == 0) {
            onvifctx->media_url = bv_strdup(tds__GetServicesResponse.Service[i].XAddr);
       }

       if (bv_strcasecmp(SOAP_NAMESPACE_OF_tptz, tds__GetServicesResponse.Service[i].Namespace) == 0) {
           onvifctx->ptz_url = bv_strdup(tds__GetServicesResponse.Service[i].XAddr);
       }

       if (bv_strcasecmp(SOAP_NAMESPACE_OF_tmd, tds__GetServicesResponse.Service[i].Namespace) == 0) {
           onvifctx->deviceio_url = bv_strdup(tds__GetServicesResponse.Service[i].XAddr);
       }
    } 

    return 0;
}

static int onvif_probe(BVConfigContext *h, BVProbeData *p)
{
    if (bv_strstart(p->filename, "onvif_cfg:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

//url likes onvif_cfg://192.168.6.149:80/onvif/device_service
static int onvif_open(BVConfigContext *h)
{
    int ret;
    char *p;
    OnvifContext *onvifctx = h->priv_data;
    if (h->url[0] == '\0') {
        bv_log(h, BV_LOG_ERROR, "file name must set\n");
        return BVERROR(EINVAL);
    }
    p = bv_sreplace(h->url, "onvif_cfg", "http");
    if (!p) {
        return BVERROR(ENOMEM);
    }
    bv_strlcpy(onvifctx->svrurl, p, sizeof(onvifctx->svrurl));
    bv_free(p);

    bv_log(h, BV_LOG_DEBUG, "svr %s\n", onvifctx->svrurl);
    onvifctx->soap = bv_soap_new(onvifctx);
    if (!onvifctx->soap)
        return BVERROR(ENOMEM);
    if (bv_onvif_service_uri(onvifctx)) {
        ret = BVERROR(EINVAL);
        goto fail;
    }

    return 0;
fail:
    bv_soap_free(onvifctx->soap);
    return ret;
}

static int onvif_close(BVConfigContext *s)
{
    OnvifContext *onvifctx = s->priv_data;
    if (onvifctx->soap)
        bv_soap_free(onvifctx->soap);
    return 0;
}

//onvif device info can check needed authorized
static int bv_onvif_get_device_info(OnvifContext *onvifctx, BVDeviceInfo *devinfo)
{
    struct soap *soap = onvifctx->soap;
    int retval;
    struct _tds__GetDeviceInformation tds__GetDeviceInformation;
    struct _tds__GetDeviceInformationResponse tds__GetDeviceInformationResponse; 
    MEMSET_STRUCT(tds__GetDeviceInformation);
    MEMSET_STRUCT(tds__GetDeviceInformationResponse);
    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }
    retval = soap_call___tds__GetDeviceInformation(soap, onvifctx->svrurl, NULL, &tds__GetDeviceInformation, &tds__GetDeviceInformationResponse);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get DeviceInformation URI error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        //FIXME http NotAuthorized
        return BVERROR_HTTP_UNAUTHORIZED;
    }
    bv_strlcpy(devinfo->device_model, tds__GetDeviceInformationResponse.Model, sizeof(devinfo->device_model));
    bv_strlcpy(devinfo->manufacturer_name, tds__GetDeviceInformationResponse.Manufacturer, sizeof(devinfo->manufacturer_name));
    bv_strlcpy(devinfo->firware_version, tds__GetDeviceInformationResponse.FirmwareVersion, sizeof(devinfo->firware_version));
    bv_strlcpy(devinfo->device_id, tds__GetDeviceInformationResponse.SerialNumber, sizeof(devinfo->device_id));
    bv_strlcpy(devinfo->hardware_model, tds__GetDeviceInformationResponse.HardwareId, sizeof(devinfo->hardware_model));
    return 0;
}

static int bv_onvif_get_device_io(OnvifContext *onvifctx, BVDeviceInfo *devinfo)
{
    int retval = SOAP_OK;
    struct soap *soap = onvifctx->soap;
    struct _tmd__GetServiceCapabilities tmd__GetServiceCapabilities;
    struct _tmd__GetServiceCapabilitiesResponse tmd__GetServiceCapabilitiesResponse;
    MEMSET_STRUCT(tmd__GetServiceCapabilities);
    MEMSET_STRUCT(tmd__GetServiceCapabilitiesResponse);

    retval = soap_call___tmd__GetServiceCapabilities(soap, onvifctx->deviceio_url, NULL, &tmd__GetServiceCapabilities, &tmd__GetServiceCapabilitiesResponse);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get DeviceIO Capabilities error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    }
    devinfo->video_sources = tmd__GetServiceCapabilitiesResponse.Capabilities->VideoSources;
    devinfo->audio_sources = tmd__GetServiceCapabilitiesResponse.Capabilities->AudioSources;
    devinfo->video_outputs = tmd__GetServiceCapabilitiesResponse.Capabilities->VideoOutputs;
    devinfo->audio_outputs = tmd__GetServiceCapabilitiesResponse.Capabilities->AudioOutputs;
    devinfo->relay_outputs = tmd__GetServiceCapabilitiesResponse.Capabilities->RelayOutputs;
    devinfo->serial_ports  = tmd__GetServiceCapabilitiesResponse.Capabilities->SerialPorts;

    return 0;
}

static int onvif_get_device_info(BVConfigContext *h, BVDeviceInfo *devinfo)
{
    OnvifContext *onvifctx = h->priv_data;
    int retval = 0;

    retval = bv_onvif_get_device_info(onvifctx, devinfo);
    if (retval) {
        bv_log(h, BV_LOG_ERROR, "get onvif device info error\n");
        return retval;
    }

    retval = bv_onvif_get_device_io(onvifctx, devinfo);
    if (retval) {
        bv_log(h, BV_LOG_ERROR, "get onvif device IO info error\n");
        return retval;
    }
    if (onvifctx->ptz_url) {
        devinfo->ptz_count = 1;
    }

    return  0;
}

static enum BVCodecID onvif_audio_to_bvcodeid(enum tt__AudioEncoding codecid)
{
    int i = 0;
    struct {
        enum tt__AudioEncoding oaid;
        enum BVCodecID bvid;
    }IDS[] = {
        {tt__AudioEncoding__G711, BV_CODEC_ID_G711U},
        {tt__AudioEncoding__G726, BV_CODEC_ID_G726},
        {tt__AudioEncoding__AAC,  BV_CODEC_ID_AAC},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].oaid == codecid)
            return IDS[i].bvid;
    }
    return BV_CODEC_ID_NONE;
}

static enum BVCodecID onvif_video_to_bvcodeid(enum tt__VideoEncoding codecid)
{
    int i = 0;
    struct {
        enum tt__VideoEncoding ovid;
        enum BVCodecID bvid;
    }IDS[] = {
        {tt__VideoEncoding__JPEG, BV_CODEC_ID_JPEG},
        {tt__VideoEncoding__MPEG4, BV_CODEC_ID_MPEG},
        {tt__VideoEncoding__H264, BV_CODEC_ID_H264},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].ovid == codecid)
            return IDS[i].bvid;
    }
    return BV_CODEC_ID_NONE;
}

static int onvif_h264_profile(enum tt__H264Profile profile)
{
    int i = 0;
    struct {
        enum tt__H264Profile ofile;
        int profile;
    }IDS[] = {
        {tt__H264Profile__Baseline, BV_PROFILE_H264_BASELINE},
        {tt__H264Profile__Main, BV_PROFILE_H264_MAIN},
        {tt__H264Profile__Extended, BV_PROFILE_H264_EXTENDED},
        {tt__H264Profile__High, BV_PROFILE_H264_HIGH},
     };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].ofile == profile)
            return IDS[i].profile;
    }
    return BV_PROFILE_H264_BASELINE;
}

static int onvif_mpeg_profile(enum tt__Mpeg4Profile profile)
{
    int i = 0;
    struct {
        enum tt__Mpeg4Profile ofile;
        int profile;
    }IDS[] = {
        {tt__Mpeg4Profile__SP, BV_PROFILE_MPEG4_SIMPLE},
        {tt__Mpeg4Profile__ASP, BV_PROFILE_MPEG4_ADVANCED_SIMPLE},
     };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].ofile == profile)
            return IDS[i].profile;
    }
    return BV_PROFILE_MPEG4_SIMPLE;
}
static int set_video_source(BVVideoSource *video_source, struct tt__VideoSourceConfiguration *VideoSourceConfiguration)
{
    video_source->type = BV_CONFIG_TYPE_ONVIF;
    video_source->bounds = (BVIntRectange){VideoSourceConfiguration->Bounds->x, VideoSourceConfiguration->Bounds->y, VideoSourceConfiguration->Bounds->width, VideoSourceConfiguration->Bounds->height};
    return 0;
}

static int set_audio_source(BVAudioSource *audio_source, struct tt__AudioSourceConfiguration *AudioSourceConfiguration)
{
    audio_source->type = BV_CONFIG_TYPE_ONVIF;
    audio_source->channels = 1;
    return 0;
}

static int set_video_encoder(BVVideoEncoder *video_encoder, struct tt__VideoEncoderConfiguration *VideoEncoderConfiguration)
{
    video_encoder->type = BV_CONFIG_TYPE_ONVIF;
    video_encoder->codec_context.codec_type = BV_MEDIA_TYPE_VIDEO;
    video_encoder->codec_context.codec_id = onvif_video_to_bvcodeid(VideoEncoderConfiguration->Encoding);
    video_encoder->codec_context.width = VideoEncoderConfiguration->Resolution->Width;
    video_encoder->codec_context.height = VideoEncoderConfiguration->Resolution->Height;
    video_encoder->codec_context.quality = (int)VideoEncoderConfiguration->Quality;
    video_encoder->codec_context.time_base = (BVRational) {1, VideoEncoderConfiguration->RateControl->FrameRateLimit};
    video_encoder->codec_context.bit_rate = VideoEncoderConfiguration->RateControl->BitrateLimit;
    if (video_encoder->codec_context.codec_id == BV_CODEC_ID_H264) {
        video_encoder->codec_context.gop_size = VideoEncoderConfiguration->H264->GovLength;
        video_encoder->codec_context.profile = onvif_h264_profile(VideoEncoderConfiguration->H264->H264Profile);
    } else {
        video_encoder->codec_context.gop_size = VideoEncoderConfiguration->H264->GovLength;
        video_encoder->codec_context.profile = onvif_mpeg_profile(VideoEncoderConfiguration->MPEG4->Mpeg4Profile);
    }
    video_encoder->codec_context.gop_size = VideoEncoderConfiguration->RateControl->EncodingInterval;
    return 0;
}

static int set_audio_encoder(BVAudioEncoder *audio_encoder, struct tt__AudioEncoderConfiguration *AudioEncoderConfiguration)
{
    audio_encoder->type = BV_CONFIG_TYPE_ONVIF;
    audio_encoder->codec_context.bit_rate = AudioEncoderConfiguration->Bitrate;
    audio_encoder->codec_context.sample_rate = AudioEncoderConfiguration->SampleRate;
    audio_encoder->codec_context.codec_id = onvif_audio_to_bvcodeid(AudioEncoderConfiguration->Encoding);
    audio_encoder->codec_context.channels = 1;
    audio_encoder->codec_context.codec_type = BV_MEDIA_TYPE_AUDIO;
    return 0;
}

static int set_profile(BVConfigContext *h, BVMediaProfile *profile, struct tt__Profile *Profile)
{
    BVVideoSource *video_source =  NULL;
    BVAudioSource *audio_source = NULL;
    BVVideoEncoder *video_encoder = NULL;
    BVAudioEncoder *audio_encoder = NULL;
    bv_sprintf(profile->token, sizeof(profile->token), "%s/%s", Profile->Name, Profile->token);
    if (Profile->VideoSourceConfiguration) {
       profile->video_source = bv_mallocz(sizeof(*profile->video_source));
       video_source = profile->video_source;
       bv_sprintf(video_source->token, sizeof(video_source->token), "%s/%s/%s", profile->token, Profile->VideoSourceConfiguration->Name, Profile->VideoSourceConfiguration->token);
       set_video_source(video_source, Profile->VideoSourceConfiguration);
    }

    if (Profile->AudioSourceConfiguration) {
        profile->audio_source = bv_mallocz(sizeof(*profile->audio_source));
        audio_source = profile->audio_source;
        bv_sprintf(audio_source->token, sizeof(audio_source->token), "%s/%s/%s", profile->token, Profile->AudioSourceConfiguration->Name, Profile->AudioSourceConfiguration->token);
        set_audio_source(audio_source, Profile->AudioSourceConfiguration);
    }

    if (Profile->AudioEncoderConfiguration) {
        profile->audio_encoder = bv_mallocz(sizeof(*profile->audio_encoder));
        audio_encoder = profile->audio_encoder;
        bv_sprintf(audio_encoder->token, sizeof(audio_encoder->token), "%s/%s/%s", profile->token, Profile->AudioEncoderConfiguration->Name, Profile->AudioEncoderConfiguration->token);
        set_audio_encoder(audio_encoder, Profile->AudioEncoderConfiguration);
    }

    if (Profile->VideoEncoderConfiguration) {
        profile->video_encoder = bv_mallocz(sizeof(*profile->video_encoder));
        video_encoder = profile->video_encoder;
        bv_sprintf(video_encoder->token, sizeof(video_encoder->token), "%s/%s/%s", profile->token, Profile->VideoEncoderConfiguration->Name, Profile->VideoEncoderConfiguration->token);
        set_video_encoder(video_encoder, Profile->VideoEncoderConfiguration);
    }

    if (Profile->PTZConfiguration) {
        //FIXME NotSupport
    }
    return 0;
}

static int onvif_get_profiles(BVConfigContext *h, BVMediaProfile *profile, int *max_num)
{
    OnvifContext *onvifctx = h->priv_data;
    int retval = 0;
    int i = 0;
    int count = 0;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetProfiles Profiles;
    struct _trt__GetProfilesResponse ProfilesResponse;
    MEMSET_STRUCT(ProfilesResponse);
    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }
    retval = soap_call___trt__GetProfiles(soap, onvifctx->media_url, NULL, &Profiles, &ProfilesResponse);
    if(retval != SOAP_OK) {
        *max_num = 0;
        bv_log(NULL, BV_LOG_INFO, "get MediaService URI error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    }
    count = *max_num > ProfilesResponse.__sizeProfiles ? ProfilesResponse.__sizeProfiles : *max_num;
    *max_num = count;
    for (i = 0; i < count; i++) {
        set_profile(h, profile + i, ProfilesResponse.Profiles + i);
    }
    return 0;
}

static struct tt__VideoEncoderConfiguration * get_video_encoder(BVConfigContext *h, BVVideoEncoder *config)
{
    OnvifContext *onvifctx = h->priv_data;
    int retval = 0;
    struct soap *soap = onvifctx->soap;
    int size = 0;
    char profile_token[32] = { 0 };
    char configs_token[32] = { 0 };
    char *p, *q, *r;
    struct _trt__GetVideoEncoderConfiguration request;
    struct _trt__GetVideoEncoderConfigurationResponse response;
    if (!onvifctx->media_url) {
        return NULL;
    }
    MEMSET_STRUCT(request);
    MEMSET_STRUCT(response);
    p = bv_strsub(config->token, "/", 1);
    q = bv_strsub(config->token, "/", 2);
    r = bv_strsub(config->token, "/", 3);
    if (!p || !q || !r) {
        retval = BVERROR(EINVAL);
        bv_log(h, BV_LOG_ERROR, "config token is error\n");
        return NULL;
    }
    size = sizeof(profile_token);
    size = size > q - p ? q - p : size;
    bv_strlcpy(profile_token, p, size);     //notify bv_strlcpy() copy (size - 1) characters
    size = sizeof(configs_token);
    bv_strlcpy(configs_token, r, size);     //notify bv_strlcpy() copy (size - 1) characters

    bv_log(onvifctx, BV_LOG_INFO, "token %s\n", configs_token);
    request.ConfigurationToken = configs_token;

    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }

    retval = soap_call___trt__GetVideoEncoderConfiguration(soap, onvifctx->media_url, NULL, &request, &response);

    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get video encoder error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return NULL;
    }
    return response.Configuration;
}

static int onvif_get_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    struct tt__VideoEncoderConfiguration *configuration = get_video_encoder(h, config);
    if (!configuration) {
        return BVERROR(EINVAL);
    }
    set_video_encoder(config, configuration);
    return 0;
}

static int onvif_set_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    OnvifContext *onvifctx = h->priv_data;
    int retval = 0;
    struct soap *soap = onvifctx->soap;
    enum BVCodecID codec_id;
    struct _trt__SetVideoEncoderConfiguration request;
    struct _trt__SetVideoEncoderConfigurationResponse response;
    struct tt__H264Configuration h264_config;
    struct tt__Mpeg4Configuration mpeg_config;
    if (!onvifctx->media_url) {
        return BVERROR(ENOSYS);
    }
    bv_log(h, BV_LOG_ERROR, "run here %s %d\n", __func__, __LINE__);
    MEMSET_STRUCT(request);
    MEMSET_STRUCT(response);
    request.Configuration = get_video_encoder(h, config);
    if (!request.Configuration) {
        return BVERROR(EINVAL);
    }
 //   request.Configuration->Encoding
    request.Configuration->Quality = config->codec_context.quality;
    request.Configuration->RateControl->BitrateLimit = config->codec_context.bit_rate;
    request.Configuration->RateControl->FrameRateLimit = config->codec_context.time_base.den / config->codec_context.time_base.num;
    codec_id = onvif_video_to_bvcodeid(request.Configuration->Encoding);
    if (codec_id != config->codec_context.codec_id) {
        bv_log(h, BV_LOG_WARNING, "codec changed>>>>>>>>>>>>>>>>>>\n");
        if (codec_id == BV_CODEC_ID_H264) {
            request.Configuration->H264 = NULL;
        } else {
            request.Configuration->MPEG4 = NULL;
        } 
    }

    if (config->codec_context.codec_id == BV_CODEC_ID_H264) {
        if (!request.Configuration->H264)
            request.Configuration->H264 = &h264_config;
        request.Configuration->H264->GovLength = config->codec_context.gop_size;
//        request.Configuration->H264->H264Profile = 
    } else {
        if (!request.Configuration->MPEG4)
            request.Configuration->MPEG4 = &mpeg_config;
        request.Configuration->MPEG4->GovLength = config->codec_context.gop_size;
    }
    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }
    retval = soap_call___trt__SetVideoEncoderConfiguration(soap, onvifctx->media_url, NULL, &request, &response);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get video encoder options error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    }

    return 0;
}

//Notify config token 
// profile_name/profile_token/config_name/config_token
static int onvif_get_video_encoder_options(BVConfigContext *h, int channel, int index, BVVideoEncoderOption *config)
{
    int retval;
    int i = 0;
    int size = 0;
    char profile_token[32] = { 0 };
    char options_token[32] = { 0 };
    char *p, *q, *r;
    OnvifContext *onvifctx = h->priv_data;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetVideoEncoderConfigurationOptions Options;
    struct _trt__GetVideoEncoderConfigurationOptionsResponse OptionsResponse;
    MEMSET_STRUCT(Options);
    MEMSET_STRUCT(OptionsResponse);
    //profile_token
    p = bv_strsub(config->token, "/", 1);
    q = bv_strsub(config->token, "/", 2);
    r = bv_strsub(config->token, "/", 3);
    if (!p || !q || !r) {
        retval = BVERROR(EINVAL);
        bv_log(h, BV_LOG_ERROR, "config token is error\n");
        return retval;
    }
    size = sizeof(profile_token);
    size = size > q - p ? q - p : size;
    bv_strlcpy(profile_token, p, size);     //notify bv_strlcpy() copy (size - 1) characters
    size = sizeof(options_token);
    bv_strlcpy(options_token, r, size);     //notify bv_strlcpy() copy (size - 1) characters
    
    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvifctx->user, onvifctx->passwd);
    }
    
    Options.ProfileToken = profile_token;
    Options.ConfigurationToken = options_token;
    retval = soap_call___trt__GetVideoEncoderConfigurationOptions(soap, onvifctx->media_url, NULL, &Options, &OptionsResponse);
    if(retval != SOAP_OK) {
        bv_log(NULL, BV_LOG_INFO, "get video encoder options error");
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    }
    config->quality.min =  OptionsResponse.Options->QualityRange->Min;
    config->quality.max =  OptionsResponse.Options->QualityRange->Max;

    if (OptionsResponse.Options->H264) {
        if (!config->h264) {
            config->h264 = bv_malloc(sizeof(BVVideoOption));
            if (!config->h264) {
                retval = BVERROR(ENOMEM);
                return retval;
            }
        }
        config->h264->codec_id = BV_CODEC_ID_H264;
        config->h264->nb_resolutions = OptionsResponse.Options->H264->__sizeResolutionsAvailable;
        config->h264->resolutions = bv_malloc_array(config->h264->nb_resolutions, sizeof(BVVideoResolution));
        for (i = 0; i < config->h264->nb_resolutions; i++) {
            config->h264->resolutions[i].width  = OptionsResponse.Options->H264->ResolutionsAvailable[i].Width;
            config->h264->resolutions[i].height = OptionsResponse.Options->H264->ResolutionsAvailable[i].Height;
        }
        config->h264->framerate_range.min = OptionsResponse.Options->H264->FrameRateRange->Min;
        config->h264->framerate_range.max = OptionsResponse.Options->H264->FrameRateRange->Max;
        config->h264->gop_range.min = OptionsResponse.Options->H264->GovLengthRange->Min;
        config->h264->gop_range.max = OptionsResponse.Options->H264->GovLengthRange->Max;
        if (OptionsResponse.Options->Extension && OptionsResponse.Options->Extension->H264) {
            config->h264->bitrate_range.min = OptionsResponse.Options->Extension->H264->BitrateRange->Min;
            config->h264->bitrate_range.max = OptionsResponse.Options->Extension->H264->BitrateRange->Max;
        }
    }

    if (OptionsResponse.Options->MPEG4) {
        if (!config->mpeg) {
            config->mpeg = bv_malloc(sizeof(BVVideoOption));
            if (!config->mpeg) {
                retval = BVERROR(ENOMEM);
                bv_free(config->h264);
                return retval;
            }
        }
        config->mpeg->codec_id = BV_CODEC_ID_MPEG;
        config->mpeg->nb_resolutions = OptionsResponse.Options->MPEG4->__sizeResolutionsAvailable;
        config->mpeg->resolutions = bv_malloc_array(config->mpeg->nb_resolutions, sizeof(BVVideoResolution));
        for (i = 0; i < config->mpeg->nb_resolutions; i++) {
            config->mpeg->resolutions[i].width  = OptionsResponse.Options->MPEG4->ResolutionsAvailable[i].Width;
            config->mpeg->resolutions[i].height = OptionsResponse.Options->MPEG4->ResolutionsAvailable[i].Height;
        }
        config->mpeg->framerate_range.min = OptionsResponse.Options->MPEG4->FrameRateRange->Min;
        config->mpeg->framerate_range.max = OptionsResponse.Options->MPEG4->FrameRateRange->Max;
        config->mpeg->gop_range.min = OptionsResponse.Options->MPEG4->GovLengthRange->Min;
        config->mpeg->gop_range.max = OptionsResponse.Options->MPEG4->GovLengthRange->Max;
        if (OptionsResponse.Options->Extension && OptionsResponse.Options->Extension->MPEG4) {
            config->mpeg->bitrate_range.min = OptionsResponse.Options->Extension->MPEG4->BitrateRange->Min;
            config->mpeg->bitrate_range.max = OptionsResponse.Options->Extension->MPEG4->BitrateRange->Max;
        }
    }

    if (OptionsResponse.Options->JPEG) {
        if (!config->jpeg) {
            config->jpeg = bv_malloc(sizeof(BVVideoOption));
            if (!config->jpeg) {
                retval = BVERROR(ENOMEM);
                bv_free(config->h264);
                bv_free(config->mpeg);
                return retval;
            }
        }
        config->jpeg->codec_id = BV_CODEC_ID_MPEG;
        config->jpeg->nb_resolutions = OptionsResponse.Options->JPEG->__sizeResolutionsAvailable;
        config->jpeg->resolutions = bv_malloc_array(config->jpeg->nb_resolutions, sizeof(BVVideoResolution));
        for (i = 0; i < config->jpeg->nb_resolutions; i++) {
            config->jpeg->resolutions[i].width  = OptionsResponse.Options->JPEG->ResolutionsAvailable[i].Width;
            config->jpeg->resolutions[i].height = OptionsResponse.Options->JPEG->ResolutionsAvailable[i].Height;
        }
        config->jpeg->framerate_range.min = OptionsResponse.Options->JPEG->FrameRateRange->Min;
        config->jpeg->framerate_range.max = OptionsResponse.Options->JPEG->FrameRateRange->Max;
        if (OptionsResponse.Options->Extension && OptionsResponse.Options->Extension->JPEG) {
            config->jpeg->bitrate_range.min = OptionsResponse.Options->Extension->JPEG->BitrateRange->Min;
            config->jpeg->bitrate_range.max = OptionsResponse.Options->Extension->JPEG->BitrateRange->Max;
        }
    }

    return 0;
}

#define OFFSET(x) offsetof(OnvifContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"timeout", "read write time out", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 =  ONVIF_TMO}, INT_MIN, INT_MAX, DEC},
    {"user", "user name", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "user password", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"media_url", "user password", OFFSET(media_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"ptz_url", "user password", OFFSET(ptz_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"device_url", "user password", OFFSET(device_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"deviceio_url", "user password", OFFSET(deviceio_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {NULL}
};

static const BVClass onvif_class = {
    .class_name     = "onvif config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfig bv_onvif_config = {
    .name               = "onvif",
    .type               = BV_CONFIG_TYPE_ONVIF,
    .flags              = BV_CONFIG_FLAGS_NOFILE | BV_CONFIG_FLAGS_NETWORK,
    .priv_data_size     = sizeof(OnvifContext),
    .priv_class         = &onvif_class,
    .config_probe       = onvif_probe,
    .config_open        = onvif_open,
    .config_close       = onvif_close,
    .get_device_info    = onvif_get_device_info,
    .get_profiles       = onvif_get_profiles,
    .get_video_encoder  = onvif_get_video_encoder,
    .set_video_encoder  = onvif_set_video_encoder,
    .get_video_encoder_options = onvif_get_video_encoder_options,
};
