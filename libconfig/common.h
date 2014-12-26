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

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>

#define BV_MAX_NAME_LEN	(64)
#define BV_MAX_URL_LEN	(512)
#define BV_MAX_PRESET_NUM (256)

/**
 *	json配置文件中为了减少数据的冗余，对于公共的数据采用引用的方式,引用的数据采用字符串表示
 *	具体数据在对应的数组中的位置
 *	例如	video_sources[0][0].certification = "00/00",表示为certifications[0][0]中的认证信息.
 *	当没有引用数据则应把video_sources[0][0].certification = "FF/FF". 用FF表示没有引用数据
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

#if 0
/**
 *	采用Ffmpeg中的，还是自己定义呢？？？？
 */
enum BVCodecID {
	BV_CODEC_ID_NONE = 0,
	//video
	BV_CODEC_ID_H264,
	BV_CODEC_ID_MPEG,
	BV_CODEC_ID_JPG,
	
	//Audio
	BV_CODEC_ID_G711A,
	BV_CODEC_ID_G711U,
	BV_CODEC_ID_G726,
	BV_CODEC_ID_AAC,

	BV_CODEC_ID_UNKNOWN
};

enum BVMediaType {
    BV_MEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as BV_MEDIA_TYPE_DATA
    BV_MEDIA_TYPE_VIDEO,
    BV_MEDIA_TYPE_AUDIO,
    BV_MEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    BV_MEDIA_TYPE_SUBTITLE,
    BV_MEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    BV_MEDIA_TYPE_NB
};

typedef struct _BVCodecContext {
	enum BVMediaType codec_type;	//BV_MEDIA_TYPE_XXX
	enum BVCodecID   codec_id;
	enum BVRCModeID  mode_id;
	int width, height;
	int framerate;
	uint32_t quality;
	uint32_t bitrate;
	uint32_t gop;

	int sample_rate;
	int channel;

	uint8_t *extradata;
	int extradata_size;
} BVCodecContext;
#endif
enum BVConfigType {
    BV_CONFIG_TYPE_NONE,
    BV_CONFIG_TYPE_LOCALE,
    BV_CONFIG_TYPE_UNKNOWN,
};

typedef struct _BVBasicInfo {
	char name[BV_MAX_NAME_LEN];	//name和token放在一起用/分割
//	char token[BV_MAX_NAME_LEN];
	char url[BV_MAX_URL_LEN];
	enum BVConfigType type;
	void *any_attr;
} BVBasicInfo;

enum BVEncryptionType {
	BV_ENCRYPTION_TYPE_NONE,
	BV_ENCRYPTION_TYPE_MD5,
	BV_ENCRYPTION_TYPE_CRC,
};

typedef struct _BVCertification {
	char user_name[BV_MAX_NAME_LEN];
	char user_pswd[BV_MAX_NAME_LEN];
	char *data;		//授权信息
	int  data_size;
	enum BVEncryptionType encryption_type;
	void *any_attr;
} BVCertification;

typedef struct _BVVideoSourceDevice {
	/* data */
} BVVideoSourceDevice;

typedef struct _BVVideoOutputDevice {
	/* data */
} BVVideoOutputDevice;

typedef struct _BVAudioSourceDevice {
	/* data */
} BVAudioSourceDevice;

typedef struct _BVAudioOutputDevice {
	/* data */
} BVAudioOutputDevice;

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
	int brightness;
	int saturation;
	int contrast;
	int sharpness;
	void *any_attr;
} BVImagingSettings;

typedef struct _BVDateTime {
	int hour;
	int minute;
	int second;
} BVDateTime;

typedef struct _BVVideoCapture {
	BVImagingSettings imaging;
	BVDateTime data_time;
	void *any_attr;
} BVVideoCapture;

typedef struct _BVVideoSource {
	BVBasicInfo basic_info;
	float framerate;
	BVIntRectange bounds;
	BVVideoResolution resolution;
	BVVideoCapture day_capture;
	BVVideoCapture night_capture;
	void *any_attr;
} BVVideoSource;

typedef struct _BVVideoOutput {
	/* data */
} BVVideoOutput;

typedef struct _BVAudioOutput {
	/* data */
} BVAudioOutput;

enum BVAudioInputType {
	BV_AUDIO_INPUT_TYPE_MIC,
	BV_AUDIO_INPUT_TYPE_LINEIN,
};

typedef struct _BVAudioSource {
	BVBasicInfo basic_info;
	int channels;
	enum BVAudioInputType input_type;
	void *any_attr;
} BVAudioSource;

enum BVRCModeID {
	BV_RC_MODE_ID_VBR = 0, /* VBR must be 0 for compatible with 3511 */
    BV_RC_MODE_ID_CBR,
    BV_RC_MODE_ID_ABR,
    BV_RC_MODE_ID_FIXQP,
    BV_RC_MODE_ID_BUTT,
};

typedef struct _BVVideoEncoder {
	BVBasicInfo basic_info;
	enum BVRCModeID mode_id;
	AVCodecContext codec_context;
	void *any_attr;
} BVVideoEncoder;

typedef struct _BVAudioEncoder {
	BVBasicInfo basic_info;
	AVCodecContext codec_context;
	void *any_attr;
} BVAudioEncoder;

typedef struct _BVVideoDecoder {
	/* data */
} BVVideoDecoder;

typedef struct _BVAudioDecoder {
	/* data */
} BVAudioDecoder;

typedef struct _BVMediaProfile {
	BVBasicInfo basic_info;
	char video_source_name[BV_MAX_NAME_LEN];
	BVVideoSource *video_source;
	char audio_source_name[BV_MAX_NAME_LEN];
	BVAudioSource *audio_source;
	char video_encoder_name[BV_MAX_NAME_LEN];
	BVVideoEncoder *video_encoder;
	char audio_encoder_name[BV_MAX_NAME_LEN];
	BVAudioEncoder *audio_encoder;
} BVMediaProfile;

enum BVChannelType {
	BV_CHANNEL_TYPE_STORAGE = 0,
	BV_CHANNEL_TYPE_PREVIEW,
	BV_CHANNEL_TYPE_PICTURE,

	BV_CHANNEL_TYPE_UNKNOWN
};

typedef struct _BVChannel {
	int index;
	enum BVChannelType type;
	char media_profile_name[BV_MAX_NAME_LEN];
	BVMediaProfile profile;
} BVChannel;

typedef struct _BVDeviceInfo {
	char device_model[BV_MAX_NAME_LEN];			//设备型号
	char device_type[BV_MAX_NAME_LEN];			//设备类型
	char software_version[BV_MAX_NAME_LEN];		//软件版本
	char hardware_version[BV_MAX_NAME_LEN];		//硬件版本
	char hardware_model[BV_MAX_NAME_LEN];		//硬件型号
	char manufacturer_id[BV_MAX_NAME_LEN];		//厂商ID
	char manufacturer_name[BV_MAX_NAME_LEN];	//厂商名字
	char device_id[BV_MAX_NAME_LEN];			//设备ID
	char device_name[BV_MAX_NAME_LEN];			//设备名字

	uint8_t cpu_usage;							//CPU使用率
	uint8_t memory_usage;						//内存使用率
	uint8_t wifi_count;
	uint8_t wireless_count;
	uint8_t channel_count;
	uint8_t video_dev_count;
	uint8_t audio_dev_count;
	uint8_t video_in_count;
	uint8_t video_out_count;
	uint8_t audio_in_count;
	uint8_t audio_out_count;
	uint8_t serial_port_count;
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

typedef struct _BVRS232 {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bit;
    uint8_t parity;
    uint8_t flow_control;
} BVRS232;

typedef struct _BVPTZPreset {
    int index;
    char url[BV_MAX_NAME_LEN];
} BVPTZPreset;

typedef struct _BVPTZ {
    int index;
	BVBasicInfo basic_info;
	enum BVPTZProtocol protocol;
    BVRS232 rs232;
    uint32_t preset_num;
    BVPTZPreset preset[BV_MAX_PRESET_NUM];
} BVPTZ;

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

#if 1
typedef struct _BVNetWorkInfo {
	uint32_t work_mode;		//wifi 3G/4G ethernet
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
#endif
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_COMMON_H */


