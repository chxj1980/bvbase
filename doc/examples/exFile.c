#include <libbvutil/bvutil.h>
#include <libbvconfig/common.h>
#include <libbvconfig/bvconfig.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>

int main(int argc, const char *argv[])
{
    int i = 0;
    int ret = 0;
    BVVideoEncoder video_encoder;
    BVAudioEncoder audio_encoder;
    BVPTZDevice ptz_device;
    BVDeviceInfo devinfo;
    BVMediaDevice media_dev;
    BVMobileDevice mobile_dev;
    BVVideoSourceDevice vs_dev;
    BVAudioSourceDevice as_dev;
    BVVideoOutputDevice vo_dev;
    BVAudioOutputDevice ao_dev;
    BVVideoSource       vs;
    BVAudioSource       as;
    BVVideoOutput       vo;
    BVAudioOutput       ao;
    BVMediaEncoder media_encoder;
    BVMediaDecoder media_decoder;
    BVTalkBack     talkback;
    BVVideoEncoderOption video_encoder_option;
    BVAudioEncoderOption audio_encoder_option;
    BVConfigContext *config_ctx = NULL;
    BVDictionary *options = NULL;

    memset(&video_encoder, 0, sizeof(video_encoder));
    memset(&audio_encoder, 0, sizeof(audio_encoder));
    memset(&ptz_device, 0, sizeof(ptz_device));
    memset(&devinfo, 0, sizeof(devinfo));
    memset(&media_dev, 0, sizeof(media_dev));
    memset(&mobile_dev, 0, sizeof(mobile_dev));
    memset(&vs_dev, 0, sizeof(vs_dev));
    memset(&as_dev, 0, sizeof(as_dev));
    memset(&vo_dev, 0, sizeof(vo_dev));
    memset(&ao_dev, 0, sizeof(ao_dev));
    memset(&vs, 0, sizeof(vs));
    memset(&as, 0, sizeof(as));
    memset(&vo, 0, sizeof(vo));
    memset(&ao, 0, sizeof(ao));
    memset(&media_encoder, 0, sizeof(media_encoder));
    memset(&media_decoder, 0, sizeof(media_decoder));
    memset(&talkback, 0, sizeof(talkback));
    memset(&video_encoder_option, 0, sizeof(video_encoder_option));
    memset(&audio_encoder_option, 0, sizeof(audio_encoder_option));
    bv_log_set_level(BV_LOG_DEBUG);
    bv_config_register_all();
    if ((ret = bv_config_open(&config_ctx, "local://media.json", NULL, &options)) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "open config error\n");
        bv_dict_free(&options);
        return BVERROR(EINVAL);
    }
    if ((ret = bv_config_file_open(&config_ctx->pdb, "media.json", NULL, NULL)) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "open config file error\n");
        return BVERROR(EINVAL);
    }

    /**
     *  FIXME
     *  system.json
     */
#if 0
    if (bv_config_get_device_info(config_ctx, &devinfo) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get device info error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "id %s\n", devinfo.device_id);
    bv_log(config_ctx, BV_LOG_INFO, "hardware_model %s\n", devinfo.hardware_model);
#endif

    if (bv_config_get_video_encoder(config_ctx, 3, 2, &video_encoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video encoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "encoding %d\n", video_encoder.codec_context.codec_id);
    bv_log(config_ctx, BV_LOG_INFO, "bitrate %d\n", video_encoder.codec_context.bit_rate);
    video_encoder.codec_context.codec_id = BV_CODEC_ID_H264;
    video_encoder.codec_context.bit_rate = 512;

    if (bv_config_set_video_encoder(config_ctx, 3, 2, &video_encoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "set video encoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "encoding %d\n", video_encoder.codec_context.codec_id);
    bv_log(config_ctx, BV_LOG_INFO, "bitrate %d\n", video_encoder.codec_context.bit_rate);

    if (bv_config_get_audio_encoder(config_ctx, 2, 1, &audio_encoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get audio encoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "encoding %d\n", audio_encoder.codec_context.codec_id);
    bv_log(config_ctx, BV_LOG_INFO, "sample %d\n", audio_encoder.codec_context.sample_rate);
    audio_encoder.codec_context.codec_id = BV_CODEC_ID_G711A;
    audio_encoder.codec_context.sample_rate = 8000;

    if (bv_config_set_audio_encoder(config_ctx, 0, 0, &audio_encoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "set audio encoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "encoding %d\n", audio_encoder.codec_context.codec_id);
    bv_log(config_ctx, BV_LOG_INFO, "sample %d\n", audio_encoder.codec_context.sample_rate);
    
    if (bv_config_get_ptz_device(config_ctx, 0, 0, &ptz_device) < 0) { 
        bv_log(config_ctx, BV_LOG_ERROR, "get ptz device error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "protocol %d\n", ptz_device.protocol);
    bv_log(config_ctx, BV_LOG_INFO, "data_bits %d\n", ptz_device.rs485.data_bits);
    bv_log(config_ctx, BV_LOG_INFO, "pan min %f max %f\n", ptz_device.pan_range.min, ptz_device.pan_range.max);

    if (bv_config_get_media_encoder(config_ctx, 0, &media_encoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get media encoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "video_source %d\n", media_encoder.video_source);
    bv_log(config_ctx, BV_LOG_INFO, "audio_source %d\n", media_encoder.audio_source);

    if (bv_config_get_media_decoder(config_ctx, 0, &media_decoder) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get media decoder error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "video_output %d\n", media_decoder.video_output);
    bv_log(config_ctx, BV_LOG_INFO, "audio_output %d\n", media_decoder.audio_output);

    if (bv_config_get_talkback(config_ctx, 0, &talkback) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get talk back error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "media_encoder_index %d\n", talkback.media_encoder_index);
    bv_log(config_ctx, BV_LOG_INFO, "storage_index %d\n", talkback.media_encoder.storage_index);

    if (bv_config_get_media_device(config_ctx, 0, &media_dev) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get media device error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "video_source %d\n", media_dev.media_encoder.video_source);
    bv_log(config_ctx, BV_LOG_INFO, "video_output %d\n", media_dev.media_decoder.video_output);
    bv_log(config_ctx, BV_LOG_INFO, "url %s\n", ((BVMobileDevice *)(media_dev.devinfo))->url);
    bv_log(config_ctx, BV_LOG_INFO, "user %s\n", ((BVMobileDevice *)(media_dev.devinfo))->user);
    bv_free(media_dev.devinfo);

    media_dev.devinfo = bv_mallocz(sizeof(BVMobileDevice));
    if (!media_dev.devinfo) {
        bv_log(config_ctx, BV_LOG_ERROR, ">>>>>>>>>>malloc failed\n");
    }
    strncpy(((BVMobileDevice *)(media_dev.devinfo))->pswd, "12345", sizeof(((BVMobileDevice *)(media_dev.devinfo))->pswd));
    strncpy(((BVMobileDevice *)(media_dev.devinfo))->user, "admin", sizeof(((BVMobileDevice *)(media_dev.devinfo))->user));
    strncpy(((BVMobileDevice *)(media_dev.devinfo))->url, "http://192.168.6.149:80/onvif/device_service", 
                        sizeof(((BVMobileDevice *)(media_dev.devinfo))->url));
    if (bv_config_set_media_device(config_ctx, 0, &media_dev) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "set media devie error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "passwd %s\n", ((BVMobileDevice *)(media_dev.devinfo))->pswd);
    bv_log(config_ctx, BV_LOG_INFO, "user %s\n", ((BVMobileDevice *)(media_dev.devinfo))->user);
    if (media_dev.devinfo) {
        bv_free(media_dev.devinfo);
    }

    if (bv_config_get_video_source_device(config_ctx, 0, &vs_dev) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video source devices error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "interface %s\n", vs_dev.interface);
    bv_log(config_ctx, BV_LOG_INFO, "work_mode %s\n", vs_dev.work_mode);

    if (bv_config_get_audio_source_device(config_ctx, 0, &as_dev) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get audio source devices error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "channels %d\n", as_dev.channel_counts);
    bv_log(config_ctx, BV_LOG_INFO, "sample_rate %d\n", as_dev.sample_rate);

    if (bv_config_get_video_output_device(config_ctx, 0, &vo_dev) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video output devices error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "interface %s\n", vo_dev.interface);
    bv_log(config_ctx, BV_LOG_INFO, "work_mode %s\n", vo_dev.work_mode);

    if (bv_config_get_video_source(config_ctx, 0, &vs) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video source error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "framerate %f\n", vs.framerate);
    bv_log(config_ctx, BV_LOG_INFO, "width %d height %d\n", vs.bounds.width, vs.bounds.height);
    bv_log(config_ctx, BV_LOG_INFO, "day_capture hour %d minute %d second %d\n", vs.day_capture.date_time.hour, 
                                        vs.day_capture.date_time.minute, vs.day_capture.date_time.second);
    bv_log(config_ctx, BV_LOG_INFO, "day_capture hour %d minute %d second %d\n", vs.day_capture.date_time.hour, 
                                        vs.day_capture.date_time.minute, vs.day_capture.date_time.second);

    if (bv_config_get_audio_source(config_ctx, 0, &as) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get audio source error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "channels %d\n", as.channels);
    bv_log(config_ctx, BV_LOG_INFO, "input_type %d\n", as.input_type);

    if (bv_config_get_video_encoder_options(config_ctx, 0, 0, &video_encoder_option) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video encoder option error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "quality min %lld max %lld\n", video_encoder_option.quality.min, video_encoder_option.quality.max);
    if (video_encoder_option.h264) {
        bv_log(config_ctx, BV_LOG_INFO, "nb_resolutions %d\n", video_encoder_option.h264->nb_resolutions);
        bv_log(config_ctx, BV_LOG_INFO, "framerate_range min %lld max %lld\n", video_encoder_option.h264->framerate_range.min, 
                                            video_encoder_option.h264->framerate_range.max);
        bv_log(config_ctx, BV_LOG_INFO, "resolutions %dx%d\n", (video_encoder_option.h264->resolutions)[0].width, 
                                            (video_encoder_option.h264->resolutions)[0].height);
        bv_log(config_ctx, BV_LOG_INFO, "resolutions %dx%d\n", (video_encoder_option.h264->resolutions)[1].width, 
                                            (video_encoder_option.h264->resolutions)[1].height);
        bv_free(video_encoder_option.h264->resolutions);
        bv_free(video_encoder_option.h264);
    }

    if (bv_config_get_audio_encoder_options(config_ctx, 0, 0, &audio_encoder_option) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get audio encoder option error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "nb_options %d\n", audio_encoder_option.nb_options);
    if (audio_encoder_option.options) {
        bv_log(config_ctx, BV_LOG_INFO, "codec_id %d\n", audio_encoder_option.options->codec_id);
        if (audio_encoder_option.options->bitrate_list.items) {
            bv_log(config_ctx, BV_LOG_INFO, "nb_int %d\n", audio_encoder_option.options->bitrate_list.nb_int);
            for (i = 0; i < audio_encoder_option.options->bitrate_list.nb_int; i++) {
                bv_log(config_ctx, BV_LOG_INFO, "items[0] %lld\n", audio_encoder_option.options->bitrate_list.items[i]);
            }
            bv_free(audio_encoder_option.options->bitrate_list.items);
        }
        if (audio_encoder_option.options->sample_rate_list.items) {
            bv_log(config_ctx, BV_LOG_INFO, "nb_int %d\n", audio_encoder_option.options->sample_rate_list.nb_int);
            for (i = 0; i < audio_encoder_option.options->sample_rate_list.nb_int; i++) {
                bv_log(config_ctx, BV_LOG_INFO, "items[0] %lld\n", audio_encoder_option.options->sample_rate_list.items[i]);
            }
            bv_free(audio_encoder_option.options->sample_rate_list.items);
        }
        bv_free(audio_encoder_option.options);
    }

    if (bv_config_get_video_output(config_ctx, 0, &vo) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get video output error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "token %s\n", vo.token);
    bv_log(config_ctx, BV_LOG_INFO, "display %dx%d\n", vo.display.width, vo.display.height);

    if (bv_config_get_audio_output(config_ctx, 0, &ao) < 0) {
        bv_log(config_ctx, BV_LOG_ERROR, "get audio output error\n");
    }
    bv_log(config_ctx, BV_LOG_INFO, "token %s\n", ao.token);
    bv_log(config_ctx, BV_LOG_INFO, "volume %d\n", ao.volume);

    bv_log(config_ctx, BV_LOG_INFO, ">>>>>>>>>>>>>>>>>>>>>>>>>>end\n");
    bv_config_file_close(&config_ctx->pdb);
    bv_config_close(&config_ctx);
    return 0;
}

