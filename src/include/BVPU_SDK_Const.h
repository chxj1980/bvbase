#ifndef __BVPU_SDK_CONST_H__
#define __BVPU_SDK_CONST_H__


/*==========================================Global Const========================================================*/
#define BVPU_SDK_MAX_CHANNEL_COUNT 64          //PU最大通道数
#define BVPU_SDK_MAX_ID_LEN 31                 //PU/CU ID长度
#define BVPU_SDK_MAX_NAME_LEN 63              //显示用的名字长度
#define BVPU_SDK_MAX_ID_NAME_LEN (BVPU_SDK_MAX_ID_LEN > BVPU_SDK_MAX_NAME_LEN ? BVPU_SDK_MAX_ID_LEN : BVPU_SDK_MAX_NAME_LEN)
#define BVPU_SDK_MAX_PASSWORD_LEN 63          //密码长度
#define BVPU_SDK_MAX_FILE_NAME_LEN 255         //文件全路径最大长度
#define BVPU_SDK_MAX_HOST_NAME_LEN 127      //IP地址/域名最大长度
#define BVPU_SDK_MAX_SEDOMAIN_NAME_LEN 1023      //SmartEye域名最大长度
#define BVPU_SDK_LAT_LNG_UNIT 10000000.0       //经纬度单位

#define BVPU_SDK_MAX_DAYTIMESLICE_COUNT 6      //一天划分的时间片数目
#define BVPU_SDK_MAX_LANGGUAGE_COUNT 32        //支持的语言数目
#define BVPU_SDK_MAX_MOBILEPHONE_NUM_LEN 15    //手机号码长度
#define BVPU_SDK_MAX_ALARMLINKACTION_COUNT 64  //报警联动数目

//PTZ
#define BVPU_SDK_PTZ_MAX_PROTOCOL_COUNT 32 //云台支持的协议最大数目
#define BVPU_SDK_PTZ_MAX_PRESET_COUNT 256 //预置点数目
#define BVPU_SDK_PTZ_MAX_CRUISE_COUNT 32 //巡航路线数目
#define BVPU_SDK_PTZ_MAX_CRUISEPOINT_COUNT 32 //每条巡航路线最多允许的预置点数
#define BVPU_SDK_PTZ_MAX_NAME_LEN 31 //云台相关（例如预置点、巡航路线）名字
#define BVPU_SDK_PTZ_MAX_SPEED 15 //云台运动最大速度
#define BVPU_SDK_PTZ_MIN_SPEED 1  //云台运动最小速度


//SDK
#define	BVPU_SDK_USER_DATA_ARRAY_SIZE	8	//用户自定义数组的长度


typedef enum {
	BVPU_SDK_RESULT_E_FAILED = -0x10000,
	BVPU_SDK_RESULT_E_INVALIDPARAM,
	BVPU_SDK_RESULT_E_UNSUPPORTED,
	BVPU_SDK_RESULT_E_INCORRECT_STRUCT_SIZE,
    BVPU_SDK_RESULT_E_TIMEOUT,  /**< command response time out */
    BVPU_SDK_RESULT_E_GPS_NOTEXIST,
    BVPU_SDK_RESULT_E_GPS_DISABLED,  
    BVPU_SDK_RESULT_E_MEM_FAIL,

    BVPU_SDK_RESULT_E_PJ_INIT_FAIL,
    BVPU_SDK_RESULT_E_PJ_NOT_INIT,
	
	BVPU_SDK_RESULT_S_OK = 0,
	
}BVPU_SDK_Result;

typedef enum {
		BVPU_SDK_LOG_LEVEL_UNKNOW = 0,
    BVPU_SDK_LOG_LEVEL_FATAL = 1,
		BVPU_SDK_LOG_LEVEL_ALERT,
		BVPU_SDK_LOG_LEVEL_CRIT,
		BVPU_SDK_LOG_LEVEL_ERROR,
		BVPU_SDK_LOG_LEVEL_WARN,
		BVPU_SDK_LOG_LEVEL_NOTICE,
		BVPU_SDK_LOG_LEVEL_INFO,
		BVPU_SDK_LOG_LEVEL_DEBUG,
		BVPU_SDK_LOG_LEVEL_MAX_COUNT
}BVPU_SDK_Log_level;

typedef enum {
	BVPU_SDK_LOG_OUTPUT_DEVICE_UNKNOW	= -1,
	BVPU_SDK_LOG_OUTPUT_DEVICE_CONSOLE = 0,
	BVPU_SDK_LOG_OUTPUT_DEVICE_LOCAL_FILE,
	BVPU_SDK_LOG_OUTPUT_DEVICE_MAX_COUNT
}BVPU_SDK_Log_Output_Device;

typedef enum {
	BVPU_SDK_SEARCH_DEVICE_UNKNOW = 0,
	BVPU_SDK_SEARCH_DEVICE_PROTOCOL_ONVIF,
	BVPU_SDK_SEARCH_DEVICE_MAX_COUNT
}BVPU_SDK_SearchDeviceProtocol;

typedef enum {
	BVPU_SDK_DEVICE_CHANNEL_TYPE_UNKNOW = 0,
	BVPU_SDK_DEVICE_CHANNEL_TYPE_CVBS,
	BVPU_SDK_DEVICE_CHANNEL_TYPE_CMOS,
	BVPU_SDK_DEVICE_CHANNEL_TYPE_ONVIF,
	BVPU_SDK_DEVICE_CHANNEL_TYPE_AHD,
	
	BVPU_SDK_DEVICE_CHANNEL_TYPE_MAX_COUNT,
	
}BVPU_SDK_DeviceChannelType;

/*=========================================== 云台 =============================================*/

// 取gps数据格式
typedef enum _st_BVPU_SDK_GPS_OUTPUT_FORM {
    BVPU_SDK_GPS_OUTPUT_FORM_PROTOCOLBUFFER = 0,   ///< protobuf out
    BVPU_SDK_GPS_OUTPUT_FORM_RAW_ASCII      = 1,   ///< string out
}BVPU_SDK_GPS_OUTPUT_FORM;

//PTZ操作命令

//注意：所有unused参数必须设置为0。
//阈值点/巡航路线的序号从0开始，-1表示无效序号
enum {
    //方向操作
    BVPU_SDK_PTZ_COMMAND_UP,     //向上。iParam1：unused;iParam2: 速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_DOWN,   //向下。iParam1：unused;iParam2: 速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFT,  //向左。iParam1：unused;iParam2: 速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHT, //向右。iParam1：unused;iParam2: 速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFTTOP,  //左上。iParam1：垂直速度;iParam2: 水平速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHTTOP,  //右上。iParam1：垂直速度;iParam2: 水平速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFTDOWN,  //左下。iParam1：垂直速度;iParam2: 水平速度;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHTDOWN,  //右下。iParam1：垂直速度;iParam2: 水平速度;iParam3:unused

    //镜头操作
    BVPU_SDK_PTZ_COMMAND_ZOOM_INC,  //增加放大倍数。iParam1：unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_ZOOM_DEC,  //减小放大倍数。iParam1：unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_FOCUS_INC, //焦距调远。iParam1：unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_FOCUS_DEC, //焦距调近。iParam1：unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_APERTURE_INC, //光圈放大。iParam1：unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_APERTURE_DEC, //光圈缩小。iParam1：unused;iParam2: unused;iParam3:unused

    //预置点操作
    BVPU_SDK_PTZ_COMMAND_PRESET_GO,  //转到预置点。iParam1：预置点号;iParam2: 垂直速度;iParam3:水平速度
    BVPU_SDK_PTZ_COMMAND_PRESET_SET, //把当前位置设置为预置点。iParam1：预置点号;iParam2: 预置点名;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_PRESET_SETNAME, //更改预置点名字。iParam1：预置点号;iParam2: 预置点名;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_PRESET_DEL, //删除预置点。iParam1：预置点号;iParam2: unused;iParam3:unused

    //巡航路线操作
    BVPU_SDK_PTZ_COMMAND_CRUISE_GO,//启动巡航。iParam1：巡航路线号;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_STOP,//停止巡航。iParam1：巡航路线号;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_SET,//设置整个巡航路线。iParam1：巡航路线号;iParam2: BVCU_PUCFG_CRUISE指针;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_DEL,//删除巡航路线。iParam1：巡航路线号;iParam2: unused;iParam3:unused

    //辅助功能操作
    BVPU_SDK_PTZ_COMMAND_AUX,//打开/关闭辅助功能开关，Param1：辅助号;iParam2: 0-关闭,1-开启;iParam3:unused

    //锁操作
    //如果锁定超过60秒后，用户没有手工解除锁定，Server会自动解除锁定。
    BVPU_SDK_PTZ_COMMAND_LOCK,//锁定/解锁云台。iParam1：unused;iParam2: unused;iParam3:unused
};

/*============================================SDK Command/Subcommand=========================================================*/
typedef enum{
    BVPU_SDK_COMMAND_UNKNOWN  = 0,
    BVPU_SDK_COMMAND_QUERY,
    BVPU_SDK_COMMAND_CONTROL,    
    BVPU_SDK_COMMAND_SUBSCRIBE,

    BVPU_SDK_COMMAND_RESERVED = 0x80000000,//保留供内部使用
}BVPU_SDK_Command;

typedef enum{
    BVPU_SDK_SUBCOMMAND_UNKNOWN  = 0,
    
    /*===================Query=================*/
    BVPU_SDK_SUBCOMMAND_QUERY_FIRST = 0x01,   
    BVPU_SDK_SUBCOMMAND_CAPABILITIES_DEVICE,	/*TODO:设备能力集；（包括硬件能力和软件能力）(不允许设置，因为是否支持是由SDK内部决定的，不是由调用者决定的)
    																						因为如果不支持新的能力，该参数中可能/以不填写相应的项，所以需要使用XML/ProtoBuf/JSON之类的语言描述，（暂定使用ProtoBuf，因为Server和PU之间使用的ProtoBuf作为负载数据封装格式，所以统一使用ProtoBuf可以减少一次负载封装格式的转换）
    																			
    																			
    																			
    																					*/
    BVPU_SDK_SUBCOMMAND_CAPABILITIES_PTZ,			/*设备PTZ能力集；是否支持旋转、对焦、光圈等等能力；输入参数：输出参数：*/
    
    BVPU_SDK_SUBCOMMAND_SEARCH_DEVICE,	/*搜索设备；输入类型：BVPU_SDK_SearchLocalNetworkDevice；输出类型：BVPU_SDK_LocalNetworkDevice数组*/
    BVPU_SDK_SUBCOMMAND_DEVICE_INFO,	/*设备信息；输入类型:无；输出类型：BVCU_PUCFG_DeviceInfo;BVCU_PUCFG_DeviceInfo中需要增加 模拟通道数、IP通道数、模拟报警输入输出数、IP报警输																			入输出数等等*/
    
    BVPU_SDK_SUBCOMMAND_STORAGE_MEDIA,

    BVPU_SDK_SUBCOMMAND_GPSDATA,
    
    /*===================Control================*/
    BVPU_SDK_SUBCOMMAND_CONTROL_FIRST = 0x20000000,
    
    BVPU_SDK_SUBCOMMAND_POWEROFF,
    BVPU_SDK_SUBCOMMAND_RESTART,
    BVPU_SDK_SUBCOMMAND_MANUALRECORD, 

    BVPU_SDK_SUBCOMMAND_EXPORT_CONFIG,
    BVPU_SDK_SUBCOMMAND_IMPORT_CONFIG,
    BVPU_SDK_SUBCOMMAND_RESET_CONFIG,

    BVPU_SDK_SUBCOMMAND_UPGRADE,

    BVPU_SDK_SUBCOMMAND_STORAGE_FORMAT,
    
    /*===============Query/Control===========*/
    /*注意：query/control类型的命令，通常query和control的输入类型和输出类型是对称的，例如
    BVPU_SDK_SUBCOMMAND_PTZATTR命令做Query时，输入类型无，输出类型是BVPU_SDK_PUCFG_PTZAttr，做Control
    时，输入类型BVPU_SDK_PUCFG_PTZAttr，输出类型无。以下注释中，只表明Control的参数，如果没有特别说明，     
    Query的参数输入/输出类型就是Control的输出/输入类型
    */
    BVPU_SDK_SUBCOMMAND_CONTROL_QUERY_FIRST = 0x40000000,
    
    BVPU_SDK_SUBCOMMAND_SDK_LOG,    /*设置SDK的log等级和log的输出设备；输入类型：BVPU_SDK_LogSetting；输出类型：无；*/
    
    BVPU_SDK_SUBCOMMAND_AVCHANNEL_DEVICE_INFO,	/*设置音视频通道的设备属性；输入类型：BVPU_SDK_ChannelDeviceInfo；输出类型：无；
    																						备注：例如把search 到的一个device设置成本sdk的一个channel.
    																									如果该通道是使用IPC，需要通过该命令设置该通道ipc的地址、端口、用户名、密码等等信息。
    																					*/
    BVPU_SDK_SUBCOMMAND_ENCODER_CHANNEL, /* BVPU_SDK_EncoderChannel */ 

    BVPU_SDK_SUBCOMMAND_SERVERS = 0x40000100,
    BVPU_SDK_SUBCOMMAND_ETHERNET,
    BVPU_SDK_SUBCOMMAND_WIFI,
    BVPU_SDK_SUBCOMMAND_RADIONETWORK,
    BVPU_SDK_SUBCOMMAND_SERIALPORT,
    BVPU_SDK_SUBCOMMAND_GPS,
    BVPU_SDK_SUBCOMMAND_TIMEZONE,
    BVPU_SDK_SUBCOMMAND_SYSTEM_TIME,

    BVPU_SDK_SUBCOMMAND_REGION_LIST,
    BVPU_SDK_SUBCOMMAND_REGION_GET,
    BVPU_SDK_SUBCOMMAND_REGION_ADD,
    BVPU_SDK_SUBCOMMAND_REGION_SET,
    BVPU_SDK_SUBCOMMAND_REGION_DEL, 

    BVPU_SDK_SUBCOMMAND_LINKACTION_ADD,
    BVPU_SDK_SUBCOMMAND_LINKACTION_MOD,
    BVPU_SDK_SUBCOMMAND_LINKACTION_DEL,
    BVPU_SDK_SUBCOMMAND_LINKACTION_LIST,
    BVPU_SDK_SUBCOMMAND_LINKACTION_GET,

    BVPU_SDK_SUBCOMMAND_STORAGE_SCHEDULE,
    BVPU_SDK_SUBCOMMAND_STORAGE_RULE,
    
    /*==================Subscribe===============*/
    


    BVPU_SDK_SUBCOMMAND_RESERVED = 0x80000000,//保留供内部使用
}BVPU_SDK_Subcommand;

/*==========================================const for Media Stream========================================================*/
/**/
/*IP通道/IP报警输入/IP报警输出*/
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_CHANNEL  0   //音视频通道	/*先模拟通道后IP通道*/
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_CHANNEL  0x00FFFF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_GPS      0x010000 //GPS设备数据
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_GPS      0x0100FF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_TSP      0x010100 //透明串口设备数据
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_TSP      0x0101FF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_CUSTOM   0xF00000 //自定义设备数据
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_CUSTOM   0xF000FF

#define BVPU_SDK_VIDEO_QUALITY_COUNT         (6)

///
/// codec id
///
typedef enum  BVPU_SDK_SAVCodec_ID{
    SAVCODEC_ID_NONE , /**<  none codec */
    SAVCODEC_ID_FIRST_VIDEO = 0x00001,  /**< first video */
    SAVCODEC_ID_MJPEG  = 0x00008,   /**< mjpeg */
    SAVCODEC_ID_H264  = 0x0001C,    /**< h264, our use now */
    /*audio*/
    SAVCODEC_ID_FIRST_AUDIO = 0x10000,  /**< first audio */
    SAVCODEC_ID_G726  = 0x1100B,    /**< g726 our use now */
    SAVCODEC_ID_AAC   = 0x15002,    /**< aac */
	SAVCODEC_ID_G711A  = 0x10007,  /**< g711a Our use now */
    /*subtile*/
    SAVCODEC_ID_FIRST_SUBTITLE = 0x17000,   /**< subtitle */
    SAVCODEC_ID_TEXT  = 0x17002,     /**< text */
    
    /*custom data*/
    SAVCODEC_ID_FIRST_CUSTOMDATA = 0xFFFF0000, /**< custom */
    SAVCODEC_ID_CUSTOMDATA1 =  SAVCODEC_ID_FIRST_CUSTOMDATA,
}BVPU_SDK_SAVCodec_ID;

typedef enum BVPU_SDK_STREAM_TYPE {
    BVPU_STREAM_VIDEO = 0x01,   /**< video */
    BVPU_STREAM_AUDIO = 0x02,   /**< audio */
} BVPU_SDK_STREAM_TYPE;

typedef enum BVPU_SDK_VIDEO_FORMAT {
    BVPU_SDK_VIDEOFORMAT_UNKNOWN = 0,  ///< unkown format, set to invalid
    BVPU_SDK_VIDEOFORMAT_PAL,          ///< pal
    BVPU_SDK_VIDEOFORMAT_NTSC,         ///< ntsc
    BVPU_SDK_VIDEOFORMAT_CUSTOM,//自定义 ///< custom define
} BVPU_SDK_VIDEO_FORMAT;

// 时间显示格式
typedef enum BVPU_SDK_TIMEFORMAT{
    BVPU_OSD_TIMEFORMAT_INVALID = 0, ///<   不叠加时间
    BVPU_OSD_TIMEFORMAT_1,           ///<   YYYY-MM-DD hh:mm:ss
    BVPU_OSD_TIMEFORMAT_2,           ///<   YYYY-MM-DD 星期W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_3,           ///<   DD日MM月YYYY年 hh:mm:ss
    BVPU_OSD_TIMEFORMAT_4,           ///<   DD日MM月YYYY年 星期W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_5,           ///<   MM月DD日YYYY年 hh:mm:ss
    BVPU_OSD_TIMEFORMAT_6,           ///<   MM月DD日YYYY年 星期W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_7,           ///<   YYYY年MM月DD日 hh:mm:ss
    BVPU_OSD_TIMEFORMAT_8,           ///<   YYYY年MM月DD日 星期W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_9,           ///<   DD-MM-YYYY hh:mm:ss
    BVPU_OSD_TIMEFORMAT_10,          ///<   DD-MM-YYYY 星期W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_11,          ///<   MM-DD-YYYY hh:mm:ss
    BVPU_OSD_TIMEFORMAT_12,          ///<   MM-DD-YYYY 星期W hh:mm:ss
} BVPU_SDK_TIMEFORMAT;

enum BVPU_SDK_Overlay {
    BVPU_SDK_OVERLAY_TIME = (1<<0),  //叠加 时间
    BVPU_SDK_OVERLAY_TEXT = (1<<1),  //叠加 自定义文字
    BVPU_SDK_OVERLAY_GPS =  (1<<2),  //叠加 GPS信息
    BVPU_SDK_OVERLAY_ALARM = (1<<3), //叠加 事件信息
    BVPU_SDK_OVERLAY_NAME = (1<<4),  //叠加 通道名
};

//encoder设备支持的流类型
typedef enum{
    BVPU_SDK_ENCODERSTREAMTYPE_INVALID = -1,
    BVPU_SDK_ENCODERSTREAMTYPE_STORAGE,//存储流
    BVPU_SDK_ENCODERSTREAMTYPE_PREVIEW,//传输流
    BVPU_SDK_ENCODERSTREAMTYPE_PICTURE,//图片流
    BVPU_SDK_ENCODERSTREAMTYPE_MAX_COUNT,
}BVPU_SDK_EncoderStreamType;

/*媒体方向*/
typedef enum{
    BVPU_SDK_MEDIADIR_VIDEOSEND = (1<<0),
    BVPU_SDK_MEDIADIR_VIDEORECV = (1<<1),
    BVPU_SDK_MEDIADIR_AUDIOSEND = (1<<2),
    BVPU_SDK_MEDIADIR_AUDIORECV = (1<<3),
    BVPU_SDK_MEDIADIR_TALKONLY  = (BVPU_SDK_MEDIADIR_AUDIOSEND | BVPU_SDK_MEDIADIR_AUDIORECV),
    BVPU_SDK_MEDIADIR_DATASEND  = (1<<4),
    BVPU_SDK_MEDIADIR_DATARECV  = (1<<5),
}BVPU_SDK_MediaDir;

//视频码率控制
typedef enum {
    BVPU_SDK_RATECONTROL_CBR = 0,
    BVPU_SDK_RATECONTROL_VBR,
    BVPU_SDK_RATECONTROL_CVBR,
    BVPU_SDK_RATECONTROL_ABR,
}BVPU_SDK_RateControl;

typedef enum{
    BVPU_SDK_MEDIASTREAM_COMMAND_UNKNOWN  = 0,
    BVPU_SDK_MEDIASTREAM_COMMAND_QUERY,
    BVPU_SDK_MEDIASTREAM_COMMAND_CONTROL,    

    BVPU_SDK_MEDIASTREAM_COMMAND_RESERVED = 0x80000000,//保留供内部使用
}BVPU_SDK_MediaStreamCommand;

typedef enum {
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_UNKNOW = 0,
	
	/*Control*/
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_PRODUCE_I_FRAME,	/*立即产生视频关键帧；输入参数：无*/
		
	/*Query*/
	
	/*Control/Query*/
	/*注意：query/control类型的命令，通常query和control的输入类型和输出类型是对称的，例如
    BVPU_SDK_MEDIASTREAM_SUBCOMMAND_AUDIOIN_VOLUME命令做Query时，输入类型无，输出类型是BVPU_SDK_PUCFG_PTZAttr，做Control
    时，输入类型BVPU_SDK_PUCFG_PTZAttr，输出类型无。以下注释中，只表明Control的参数，如果没有特别说明，     
    Query的参数输入/输出类型就是Control的输出/输入类型
    */
    BVPU_SDK_MEDIASTREAM_SUBCOMMAND_MANUAL_ADJUST_VIDEO_ENCODER,	/*临时调整video 的编码参数，主要用于调用者调整传输流的传输策略；输入参数：BVPU_SDK_MediaStreamVideoEncoderDynamicParam*/
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_UPDATE_STREAM_DIR,	/*媒体流的媒体方向；输入参数：BVPU_SDK_MediaDir*/
	
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_MAX_CONUT,
	
}BVPU_SDK_MediaStreamSubcommand;
/*========================================================================================================*/

/*====================================TODO：所有的Hardware Interface========================================================*/

typedef enum {
	BVPU_SDK_HARDWARE_DEVICETYPE_UNKNOW = 0,
	
	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_VIDEOIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_VIDEOOUT,
	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_AUDIOIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_AUDIOOUT,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_VIDEOIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_VIDEOOUT,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_AUDIOIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_AUDIOOUT,

	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_ALARMIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_ALARMOUT,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_ALARMIN,
	BVPU_SDK_HARDWARE_DEVICETYPE_IP_ALARMOUT,

	BVPU_SDK_HARDWARE_DEVICETYPE_WIFI,
	BVPU_SDK_HARDWARE_DEVICETYPE_MOBILE_NETWORK,
	BVPU_SDK_HARDWARE_DEVICETYPE_ETHERNET,
	
	BVPU_SDK_HARDWARE_DEVICETYPE_GPS,
	
	BVPU_SDK_HARDWARE_DEVICETYPE_SERIALPORT,
	
	BVPU_SDK_HARDWARE_DEVICETYPE_STORAGE,

	BVPU_SDK_HARDWARE_DEVICETYPE_MAX_COUNT
}BVPU_SDK_HardwareDeviceType;



#endif
