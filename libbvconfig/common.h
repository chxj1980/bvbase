/*************************************************************************
    > File Name: common.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月11日 星期四 13时43分24秒
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

#ifndef BV_COMMON_H
#define BV_COMMON_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#include <libbvcodec/bvcodec.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>

#define BV_MAX_NAME_LEN   (128)
#define BV_MAX_URL_LEN    (512)
#define BV_MAX_PRESET_NUM (256)

/**
 *    json配置文件中为了减少数据的冗余，对于公共的数据采用引用的方式,引用的数据采用字符串表示
 *    具体数据在对应的数组中的位置
 *    例如    video_sources[0][0].certification = "00/00",表示为certifications[0][0]中的认证信息.
 *    当没有引用数据则应把video_sources[0][0].certification = "FF/FF". 用FF表示没有引用数据
 */

enum BVFileType {
    BV_FILE_TYPE_NONE = 0,
    BV_FILE_TYPE_AUDIO = (1 << 0),
    BV_FILE_TYPE_VIDEO = (1 << 1),
    BV_FILE_TYPE_RECORD = BV_FILE_TYPE_AUDIO | BV_FILE_TYPE_VIDEO,
    BV_FILE_TYPE_GPS = (1 << 2),
    BV_FILE_TYPE_LOG = (1 << 3),
    BV_FILE_TYPE_FIRMWARE = (1 << 4),
};

enum BVConfigType {
    BV_CONFIG_TYPE_NONE = -1,
    BV_CONFIG_TYPE_LOCAL,
    BV_CONFIG_TYPE_ONVIF,
    BV_CONFIG_TYPE_UNKNOWN,
};

typedef struct _BVBasicInfo {
    char name[BV_MAX_NAME_LEN];    //name和token放在一起用/分割采用三级命名localname/remotename/remotetoken/ localename LocalAudioS_00_00
    char url[BV_MAX_URL_LEN];
    enum BVConfigType type;
    void *any_attr;
} BVBasicInfo;

enum BVEncryptionType {
    BV_ENCRYPTION_TYPE_NONE,
    BV_ENCRYPTION_TYPE_MD5,
    BV_ENCRYPTION_TYPE_CRC,
};

typedef struct _BVIntRange {
    int64_t min;
    int64_t max;
} BVIntRange;

typedef struct _BVFloatRange {
    float min;
    float max;
} BVFloatRange;

typedef struct _BVIntList {
    int nb_int;
    int64_t *items;
} BVIntList;

typedef struct _BVFloatList {
    int nb_float;
    float *items;
} BVFloatList;

typedef struct _BVCertification {
    char server[BV_MAX_URL_LEN];
    char user_name[BV_MAX_NAME_LEN];
    char user_pswd[BV_MAX_NAME_LEN];
    char *data;        //授权信息
    int  data_size;
    enum BVEncryptionType encryption_type;
    void *any_attr;
} BVCertification;

typedef struct _BVIntRectange {
    int x;
    int y;
    int width;
    int height;
} BVIntRectange;

//视频分辨率
typedef struct _BVVideoResolution {
    int width;
    int height;
} BVVideoResolution;

typedef struct _BVImagingSettings {
    uint8_t luminance;  //亮度
    uint8_t contrast;   //对比度
    uint8_t sharpness;  //锐度
    uint8_t hue;        //色调
    uint8_t saturation; //饱和度
    void *any_attr;
} BVImagingSettings;

typedef struct _BVDateTime {
    int hour;
    int minute;
    int second;
} BVDateTime;

typedef struct _BVVideoSourceDevice {
    char token[BV_MAX_NAME_LEN];
    char chip[BV_MAX_NAME_LEN];
    char dev[BV_MAX_NAME_LEN];
    uint8_t video_sources;
    char interface[BV_MAX_NAME_LEN];    //BT656 BT601 BT1120P BT1120I
    char work_mode[BV_MAX_NAME_LEN];     //for BT656 4D1 4HALFD1 2D1 
} BVVideoSourceDevice;

typedef struct _BVVideoOutputDevice {
    char token[BV_MAX_NAME_LEN];
    char chip[BV_MAX_NAME_LEN];
    char dev[BV_MAX_NAME_LEN];
    uint8_t video_outputs;
    char interface[BV_MAX_NAME_LEN];    //CVBS HDMI
    char work_mode[BV_MAX_NAME_LEN];    //PAL NTSC AUTO
    BVIntRectange display;
} BVVideoOutputDevice;

typedef struct _BVAudioSourceDevice {
    char token[BV_MAX_NAME_LEN];
    char chip[BV_MAX_NAME_LEN];
    char dev[BV_MAX_NAME_LEN];
    uint8_t channel_mode;
    uint8_t channel_counts;
    uint8_t audio_sources;
    uint8_t sample_format;
    int sample_rate;
    int sample_points;

    char work_mode[BV_MAX_NAME_LEN];     //I2S_SALVE I2S_MASTER 
} BVAudioSourceDevice;

typedef struct _BVAudioOutputDevice {
    char token[BV_MAX_NAME_LEN];
    char chip[BV_MAX_NAME_LEN];
    char dev[BV_MAX_NAME_LEN];
    uint8_t channel_mode;
    uint8_t channel_counts;
    uint8_t audio_outputs;
    uint8_t sample_format;
    int sample_rate;
    int sample_points;
    char work_mode[BV_MAX_NAME_LEN];     //I2S_SLAVE I2S_MASTER 
} BVAudioOutputDevice;

typedef struct _BVVideoCapture {
    BVImagingSettings imaging;
    BVDateTime date_time;
    void *any_attr;
} BVVideoCapture;

typedef struct _BVVideoSource {
    char token[BV_MAX_NAME_LEN];
    float framerate;
    BVIntRectange bounds;
    BVVideoResolution resolution;
    BVVideoCapture day_capture;
    BVVideoCapture night_capture;
    void *any_attr;
} BVVideoSource;

typedef struct _BVVideoOutput {
    char token[BV_MAX_NAME_LEN];
    BVIntRectange display;
} BVVideoOutput;

typedef struct _BVAudioOutput {
    char token[BV_MAX_NAME_LEN];
    int volume;
} BVAudioOutput;

enum BVAudioInputType {
    BV_AUDIO_INPUT_TYPE_MIC,
    BV_AUDIO_INPUT_TYPE_LINE_IN,
};

typedef struct _BVAudioSource {
    char token[BV_MAX_NAME_LEN];
    int channels;
    int volume;
    enum BVAudioInputType input_type;
    void *any_attr;
} BVAudioSource;

enum BVMobileDeviceType {
    BV_MOBILE_DEVICE_TYPE_NONE = 0,
    BV_MOBILE_DEVICE_TYPE_NVS = (1 << 0),
    BV_MOBILE_DEVICE_TYPE_NVT = (1 << 1),
    BV_MOBILE_DEVICE_TYPE_NVA = (1 << 2),
    BV_MOBILE_DEVICE_TYPE_NVD = (1 << 3),

    BV_MOBILE_DEVICE_TYPE_UNKNOWN,
};

typedef struct _BVMobileDevice {
    enum BVMobileDeviceType type;
    char url[1024];
    char user[64];
    char pswd[64];
    int  timeout;
} BVMobileDevice;

enum BVMediaDeviceType {
    BV_MEDIA_DEVICE_TYPE_NONE,
    BV_MEDIA_DEVICE_TYPE_CVBS,
    BV_MEDIA_DEVICE_TYPE_IPC,
    BV_MEDIA_DEVICE_TYPE_UNKNOW,
};

typedef struct _BVMediaDevice {
    char name[BV_MAX_NAME_LEN];
    enum BVMediaDeviceType type; 
    void *devinfo;
} BVMediaDevice;

typedef struct _BVVideoOption {
    enum BVCodecID codec_id;
    int nb_resolutions;
    BVVideoResolution *resolutions;
    BVIntRange framerate_range;
    BVIntRange gop_range;
    BVIntRange bitrate_range;
} BVVideoOption;

typedef struct _BVVideoEncoderOption {
    char token[BV_MAX_NAME_LEN];
    BVIntRange quality;
    BVVideoOption *h264;
    BVVideoOption *mpeg;
    BVVideoOption *jpeg;
    void *any_attr;
} BVVideoEncoderOption;

typedef struct _BVVideoEncoder {
    char token[BV_MAX_NAME_LEN];
    bool enable;
    BVCodecContext codec_context;
 //   BVVideoEncoderOption *option;
    void *any_attr;
} BVVideoEncoder;

typedef struct _BVAudioOption {
    enum BVCodecID codec_id;
    BVIntList bitrate_list;
    BVIntList sample_rate_list;
} BVAudioOption;

typedef struct _BVAudioEncoderOption {
    char token[BV_MAX_NAME_LEN];
    int nb_options;
    BVAudioOption *options;
    void *any_attr;
} BVAudioEncoderOption;

typedef struct _BVAudioEncoder {
    char token[BV_MAX_NAME_LEN];
    bool enable;
    BVCodecContext codec_context;
    void *any_attr;
} BVAudioEncoder;

typedef struct _BVVideoDecoder {
    char token[BV_MAX_NAME_LEN];
} BVVideoDecoder;

typedef struct _BVAudioDecoder {
    char token[BV_MAX_NAME_LEN];
} BVAudioDecoder;

enum BVEncodeChannelType {
    BV_CHANNEL_TYPE_STORAGE = 0,
    BV_CHANNEL_TYPE_PREVIEW,
    BV_CHANNEL_TYPE_PICTURE,

    BV_CHANNEL_TYPE_UNKNOWN
};

typedef struct _BVEncodeChannelConfig {
    enum BVEncodeChannelType type;
    BVAudioEncoder *audio_encoder;
    BVVideoEncoder *video_encoder;
} BVEncodeChannelConfig;

typedef struct _BVDecodeChannelConfig {
    BVAudioDecoder *audio_decoder;
    BVVideoDecoder *video_decoder;
} BVDecodeChannelConfig;

typedef struct _BVInputChannelConfig {
    BVAudioSource *audio_source;
    BVVideoSource *video_source;
} BVSourceChannelConfig;

typedef struct _BVOutputChannelConfig {
    BVAudioOutput *audio_output;
    BVVideoOutput *video_output;
} BVOutputChannelConfig;

enum BVMediaChannelType {
    BV_MEDIA_CHANNEL_TYPE_NONE,
    BV_MEDIA_CHANNEL_TYPE_SOURCE,   //audio video source
    BV_MEDIA_CHANNEL_TYPE_OUTPUT,   //audio video output 
    BV_MEDIA_CHANNEL_TYPE_ENCODE,
    BV_MEDIA_CHANNEL_TYPE_DECODE,
    BV_MEDIA_CHANNEL_TYPE_UNKNOWN,
};

typedef struct _BVMediaChannel {
    enum BVMediaChannelType type;
    void *payload;
} BVMediaChannel;

typedef struct _BVDeviceInfo {
    char device_model[BV_MAX_NAME_LEN];            //设备型号
    char device_type[BV_MAX_NAME_LEN];            //设备类型
    char software_version[BV_MAX_NAME_LEN];        //软件版本
    char hardware_version[BV_MAX_NAME_LEN];        //硬件版本
    char hardware_model[BV_MAX_NAME_LEN];        //硬件型号
    char manufacturer_id[BV_MAX_NAME_LEN];        //厂商ID
    char manufacturer_name[BV_MAX_NAME_LEN];    //厂商名字
    char device_id[BV_MAX_NAME_LEN];            //设备ID
    char device_name[BV_MAX_NAME_LEN];            //设备名字
    char firware_version[BV_MAX_NAME_LEN];          //固件版本

    uint8_t cpu_usage;                            //CPU使用率
    uint8_t memory_usage;                        //内存使用率
    uint8_t wifi_count;
    uint8_t wireless_count;
    uint8_t channel_count;
    uint8_t video_dev_count;
    uint8_t audio_dev_count;
    uint8_t video_sources;
    uint8_t video_outputs;
    uint8_t audio_sources;
    uint8_t audio_outputs;
    uint8_t relay_outputs;
    uint8_t serial_ports;
    uint8_t alert_in_count;
    uint8_t alert_out_count;
    uint8_t ptz_count;
    uint8_t gps_count;
    uint8_t storage_count;
    uint8_t support_sms;
    uint8_t support_call;
    uint16_t preset_count;
    uint16_t cruise_count;
    
    void *any_attr;
} BVDeviceInfo;

typedef struct _BVDeviceExtInfo {
    uint8_t language;
    uint8_t temprrature_count;
    uint8_t voltage_count;
    uint8_t spedd_count;
    void *any_attr;
} BVDeviceExtInfo;

enum BVOSDTimeFormat {
    BV_OSD_TIME_FORMAT_INVALID = 0,//不叠加时间
    BV_OSD_TIME_FORMAT_1,//YYYY-MM-DD hh:mm:ss
    BV_OSD_TIME_FORMAT_2,//YYYY-MM-DD 星期W hh:mm:ss
    BV_OSD_TIME_FORMAT_3,//DD日MM月YYYY年 hh:mm:ss
    BV_OSD_TIME_FORMAT_4,//DD日MM月YYYY年 星期W hh:mm:ss
    BV_OSD_TIME_FORMAT_5,//MM月DD日YYYY年 hh:mm:ss
    BV_OSD_TIME_FORMAT_6,//MM月DD日YYYY年 星期W hh:mm:ss
    BV_OSD_TIME_FORMAT_7,//YYYY年MM月DD日 hh:mm:ss
    BV_OSD_TIME_FORMAT_8,//YYYY年MM月DD日 星期W hh:mm:ss
    BV_OSD_TIME_FORMAT_9,//DD-MM-YYYY hh:mm:ss
    BV_OSD_TIME_FORMAT_10,//DD-MM-YYYY 星期W hh:mm:ss
    BV_OSD_TIME_FORMAT_11,//MM-DD-YYYY hh:mm:ss
    BV_OSD_TIME_FORMAT_12,//MM-DD-YYYY 星期W hh:mm:ss
};

enum BVOSDType {
    BV_OSD_TIME_NONE = 0,
    BV_OSD_TYPE_TIME = (1 << 0),
    BV_OSD_TYPE_TEXT = (1 << 1),
    BV_OSD_TYPE_GPS  = (1 << 2),
    BV_OSD_TYPE_ALARM = (1 << 3),
    BV_OSD_TYPE_NAME = (1 << 4),
    BV_OSD_TYPE_LOGO = (1 << 5),
    BV_OSD_TYPE_UNKNOWN
};

typedef struct _BVVector1D {
    float x;
} BVVector1D;

typedef struct _BVVector2D {
    float x;
    float y;
} BVVector2D;

typedef struct _BVPTZVector {
    bool pan_tilt_enable;
    bool zoom_enable;
    BVVector2D pan_tilt;
    BVVector1D zoom;
} BVPTZVector;

typedef struct _BVPTZContinuousMove {
    int64_t duration;
    BVPTZVector velocity;
} BVPTZContinuousMove;

typedef struct _BVPTZAbsoluteMove {
    BVPTZVector position;
    BVPTZVector speed;
} BVPTZAbsoluteMove;

typedef struct _BVPTZRelativeMove {
    BVPTZVector translation;
    BVPTZVector speed;
} BVPTZRelativeMove;

typedef struct _BVPTZStop {
    bool pan_tilt;
    bool zoom;
} BVPTZStop;

enum BVPTZProtocol {
    BV_PTZ_PROTO_NONE = 0,
    BV_PTZ_PROTO_USERDEFINED,
    BV_PTZ_PROTO_PELCO_D,
    BV_PTZ_PROTO_PELCO_P,
    BV_PTZ_PROTO_SAMSUNG,
    BV_PTZ_PROTO_VISCA,
    BV_PTZ_PROTO_YAAN,
};

enum BVRS232ControlMode {
    BV_RS232_CONTROL_MODE_NONE = -1,
    BV_RS232_CONTROL_MODE_CONSOLE,                      //控制台
    BV_RS232_CONTROL_MODE_TRANSPARENT_CHANNEL,          //透明通道
    BV_RS232_CONTROL_MODE_UNKNOWN
};

typedef struct _BVRS232 {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bit;
    uint8_t parity;
    uint8_t flow_control;
    enum BVRS232ControlMode control_mode;
} BVRS232;

typedef struct _BVRS485 {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bit;
    uint8_t parity;
    uint8_t flow_control;
    uint8_t address;
    //FIXME 解码器类型
} BVRS485;

typedef struct _BVPTZPreset {
    int index;
    int flags;  //预置位已用为1
    char name[BV_MAX_NAME_LEN];
    char token[BV_MAX_NAME_LEN];
} BVPTZPreset;

typedef struct _BVPTZGotoPreset {
    char token[BV_MAX_NAME_LEN];
    BVPTZVector speed;
} BVPTZGotoPreset;

typedef struct _BVPTZDevice {
    char token[BV_MAX_NAME_LEN];
    enum BVPTZProtocol protocol;
    BVRS485 rs485;
    BVFloatRange pan_range;
    BVFloatRange tilt_range;
    BVFloatRange zoom_range;
    uint16_t max_preset;
    uint16_t nb_presets;
    BVPTZPreset *presets;
    void *any_attr;
} BVPTZDevice;

enum BVNetInterface {
    BV_NET_INTERFACE_NONE = 0,
    BV_NET_INTERFACE_ETHERNET = 1 << 1,
    BV_NET_INTERFACE_WIFI = 1 << 2,
    BV_NET_INTERFACE_WIRLESS = 1 << 3,
    BV_NET_INTERFACE_UNKNOWN
};

enum BVNetPrimary {
    BV_NET_PRIMARY_ONLY_ETHERNET,
    BV_NET_PRIMARY_ONLY_WIFI,
    BV_NET_PRIMARY_ONLY_4G,
    BV_NET_PRIMARY_ONLY_3G,
    BV_NET_PRIMARY_WIFI_THAN_WIRELESS,
    BV_NET_PRIMARY_UNKNOWN,
};

typedef struct _BVNetWorkInfo {
    uint32_t work_mode;        //wifi 3G/4G ethernet
    enum BVNetPrimary primary;
    uint8_t enalbe_router;
} BVNetWorkInfo;

typedef struct _BVEthernetInfo {
    uint8_t enable_dhcp;
    char address[16];
    char submask[16];
    char gateway[16];
} BVEthernetInfo;

typedef struct _BVDNSInfo {
    uint8_t auto_dns;
    char master_dns[16];
    char slaver_dns[16];
} BVDNSInfo;

typedef struct _BVPPPOE {
    uint8_t enable;
    char user[16];
    char passwd[16];
} BVPPPOE;

typedef struct _BVMediaProfile {
    char token[BV_MAX_NAME_LEN];
    BVVideoSource *video_source;
    BVAudioSource *audio_source;
    BVVideoEncoder *video_encoder;
    BVAudioEncoder *audio_encoder;
    BVPTZDevice    *ptz_device;
} BVMediaProfile;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_COMMON_H */


