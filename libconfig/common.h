/*************************************************************************
	> File Name: common.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年11月07日 星期五 14时11分41秒
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
/**
 * includes all data structs
 * 结构体应可扩展，同时不影响以前的功能。所以采用定义一个基本的结构体，然后在
 * 其中添加指针。
 * typedef struct BVServerInfo {
 *     char server_name[128];
 *     uint16_t server_port;
 *     uint16_t protocol_type;
 *
 *	   //extension info
 *    struct BVServerExtInfo *ext_info;
 * }BVServerInfo;
 *
 * struct BVServerExtInfo {
 *    char user_name[128];
 *    char user_pwd[64];
 * };
 *
 */
#include <stdint.h>

#define BV_MAX_NAME_LEN (32)
#define BV_MAX_PSWD_LEN (16)
#define BV_MAX_URL_LEN (256)
#define BV_MAX_CHN_NUM (64)

enum BVStreamType {
	BV_STREAM_TYPE_NONE = 0,
	BV_STREAN_TYPE_VIDEO = (1 << 0),
	BV_STREAM_TYPE_AUDIO = (1 << 1),

	BV_STREAM_TYPE_UNKNOWN
};

enum BVChannelType {
	BV_CHANNEL_TYPE_STORAGE = 0,
	BV_CHANNEL_TYPE_PREVIEW,
	BV_CHANNEL_TYPE_PICTURE,

	BV_CHANNEL_TYPE_UNKNOWN
};

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

enum BVRCModeID {
	BV_RC_MODE_ID_VBR = 0, /* VBR must be 0 for compatible with 3511 */
    BV_RC_MODE_ID_CBR,
    BV_RC_MODE_ID_ABR,
    BV_RC_MODE_ID_FIXQP,
    BV_RC_MODE_ID_BUTT,
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
enum BVAudioInputType {
	BV_AUDIO_INPUT_MIC,
	BV_AUDIO_INPUT_LINEIN,
};

enum BVPTZProtocol {
	BV_PTZ_PROTO_NONE = 0,
	BV_PTZ_PROTO_USERDEFINED,
	BV_PTZ_PROTO_PELCO_D,
	BV_PTZ_PROTO_PELCO_P,
	BV_PTZ_PROTO_SAMSUNG,
	BV_PTZ_PROTO_VISCA,
	BV_PTZ_PROTO_YAAN,
};

struct VideoSizeAbbr {
	const char *abbr;
	int width;
	int height;
};

typedef struct _BVDateTime {
	int hour;
	int minute;
	int second;
} BVDateTime;

typedef struct _BVDate {
	int year;
	int month;
	int day;
} BVDate;

typedef struct _BVImageRect {
	int left;
	int top;
	int width;
	int height;
} BVImageRect;


typedef struct _BVVideoInputUnit {

} BVVideoInputUnit;

typedef struct _BVVideoOutUnit {

} BVVideoOutUnit;

typedef struct _BVAudioInputUnit {

} BVAudioInputUnit;

typedef struct _BVAudioOutUnit {

} BVAudioOutUnit;

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
	
	struct BVDeviceExtInfo *ext_info;	
} BVDeviceInfo;

typedef struct _BVDeviceExtInfo {
	uint8_t language;
	uint8_t temprrature_count;
	uint8_t voltage_count;
	uint8_t spedd_count;
} BVDeviceExtInfo;

typedef struct _BVTimeZone {
	int time_zone;
} BVTimeZone;

typedef struct _BVVideoChipInfo {
	char input_mode[BV_MAX_NAME_LEN];
	char work_mode[BV_MAX_NAME_LEN];
} BVVideoChip;

typedef struct _BVSystemConfigInfo {
	char media_fact_config[BV_MAX_NAME_LEN];
	char dev_fact_config[BV_MAX_NAME_LEN];
	char custom_fact_config[BV_MAX_NAME_LEN];

	char media_config[BV_MAX_NAME_LEN];
	char dev_config[BV_MAX_NAME_LEN];
	char custom_config[BV_MAX_NAME_LEN];
} BVSystemConfigInfo;

typedef struct _BVVideoCapture {
	uint8_t luminance;		//亮度
	uint8_t contrast;		//对比度
	uint8_t hue;			//色彩
	uint8_t satuature;		//饱和度
	BVDateTime start_time;	//开始时间
	BVDateTime end_time;	//结束时间
} BVVideoCapture;

typedef struct _BVVideoInputSource {
	char name[BV_MAX_NAME_LEN];
	char token[BV_MAX_NAME_LEN];
	char size[BV_MAX_NAME_LEN];
	uint32_t framerate;
	BVImageRect capture_rect;	//采集区域
	BVVideoCapture day_capture;
	BVVideoCapture night_capture;
} BVVideoInputSource;

typedef struct _BVAudioInputSource {
	char name[BV_MAX_NAME_LEN];
	char token[BV_MAX_NAME_LEN];
	enum BVAudioInputType input_type;
	uint8_t channel;
	uint8_t volume;
	uint8_t sample_rate;	//KHZ
	uint8_t bit_depth;		//采样精度
} BVAudioInputSource;

typedef struct _BVStream {
	char name[BV_MAX_NAME_LEN];
	char token[BV_MAX_NAME_LEN];
	int index;
	enum BVStreamType stream_type;		//流类型 视频流 音频流
	BVCodecContext codec;
} BVStream;

typedef struct _BVVideoLive {
	uint8_t channel_id;
	uint8_t video_input_source;
	uint8_t video_output_source;
	BVImageRect display_rect;
} BVVideoLive;

typedef struct _BVAudioDecode {
	enum BVCodecID codec_id;
} BVAudioDecode;

typedef struct _BVEncodeChannel {
	char name[BV_MAX_NAME_LEN];
	char url[BV_MAX_URL_LEN];	//onvif onvif IPC url 海思平台videv/vichn hisi://00/01
	char remote_channel;
	char user[BV_MAX_NAME_LEN];
	char passwd[BV_MAX_PSWD_LEN];
	char profile_name[BV_MAX_NAME_LEN];
	char profile_token[BV_MAX_NAME_LEN];	//onvif IPC profile token 海思平台编码通道号05
	uint8_t video_source;
	uint8_t status;
	uint8_t channel_id;
	enum BVChannelType channel_type;	//通道类型 存储 网传 抓拍
	enum BVStreamType stream_type;		//流类型 视频流 音频流
	enum BVOSDType osd_type;
	BVStream *video_stream;
	BVStream *audio_stream;
} BVEncodeChannel;

typedef struct _BVOSD {
	char name[BV_MAX_NAME_LEN];
	char token[BV_MAX_NAME_LEN];
	enum BVOSDType osd_type;
	char path[BV_MAX_NAME_LEN];
	uint8_t font_size;
	uint8_t font_format;
	BVImageRect osd_rect;
} BVOSD;

typedef struct _BVPTZ {
	char name[BV_MAX_NAME_LEN];
	char token[BV_MAX_NAME_LEN];
	char url[BV_MAX_URL_LEN];
	enum BVPTZProtocol protocol;
} BVPTZ;

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
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_COMMON_H */

