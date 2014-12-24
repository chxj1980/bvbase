#ifndef __BVPU_SDK_CONST_H__
#define __BVPU_SDK_CONST_H__


/*==========================================Global Const========================================================*/
#define BVPU_SDK_MAX_CHANNEL_COUNT 64          //PU���ͨ����
#define BVPU_SDK_MAX_ID_LEN 31                 //PU/CU ID����
#define BVPU_SDK_MAX_NAME_LEN 63              //��ʾ�õ����ֳ���
#define BVPU_SDK_MAX_ID_NAME_LEN (BVPU_SDK_MAX_ID_LEN > BVPU_SDK_MAX_NAME_LEN ? BVPU_SDK_MAX_ID_LEN : BVPU_SDK_MAX_NAME_LEN)
#define BVPU_SDK_MAX_PASSWORD_LEN 63          //���볤��
#define BVPU_SDK_MAX_FILE_NAME_LEN 255         //�ļ�ȫ·����󳤶�
#define BVPU_SDK_MAX_HOST_NAME_LEN 127      //IP��ַ/������󳤶�
#define BVPU_SDK_MAX_SEDOMAIN_NAME_LEN 1023      //SmartEye������󳤶�
#define BVPU_SDK_LAT_LNG_UNIT 10000000.0       //��γ�ȵ�λ

#define BVPU_SDK_MAX_DAYTIMESLICE_COUNT 6      //һ�컮�ֵ�ʱ��Ƭ��Ŀ
#define BVPU_SDK_MAX_LANGGUAGE_COUNT 32        //֧�ֵ�������Ŀ
#define BVPU_SDK_MAX_MOBILEPHONE_NUM_LEN 15    //�ֻ����볤��
#define BVPU_SDK_MAX_ALARMLINKACTION_COUNT 64  //����������Ŀ

//PTZ
#define BVPU_SDK_PTZ_MAX_PROTOCOL_COUNT 32 //��̨֧�ֵ�Э�������Ŀ
#define BVPU_SDK_PTZ_MAX_PRESET_COUNT 256 //Ԥ�õ���Ŀ
#define BVPU_SDK_PTZ_MAX_CRUISE_COUNT 32 //Ѳ��·����Ŀ
#define BVPU_SDK_PTZ_MAX_CRUISEPOINT_COUNT 32 //ÿ��Ѳ��·����������Ԥ�õ���
#define BVPU_SDK_PTZ_MAX_NAME_LEN 31 //��̨��أ�����Ԥ�õ㡢Ѳ��·�ߣ�����
#define BVPU_SDK_PTZ_MAX_SPEED 15 //��̨�˶�����ٶ�
#define BVPU_SDK_PTZ_MIN_SPEED 1  //��̨�˶���С�ٶ�


//SDK
#define	BVPU_SDK_USER_DATA_ARRAY_SIZE	8	//�û��Զ�������ĳ���


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

/*=========================================== ��̨ =============================================*/

// ȡgps���ݸ�ʽ
typedef enum _st_BVPU_SDK_GPS_OUTPUT_FORM {
    BVPU_SDK_GPS_OUTPUT_FORM_PROTOCOLBUFFER = 0,   ///< protobuf out
    BVPU_SDK_GPS_OUTPUT_FORM_RAW_ASCII      = 1,   ///< string out
}BVPU_SDK_GPS_OUTPUT_FORM;

//PTZ��������

//ע�⣺����unused������������Ϊ0��
//��ֵ��/Ѳ��·�ߵ���Ŵ�0��ʼ��-1��ʾ��Ч���
enum {
    //�������
    BVPU_SDK_PTZ_COMMAND_UP,     //���ϡ�iParam1��unused;iParam2: �ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_DOWN,   //���¡�iParam1��unused;iParam2: �ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFT,  //����iParam1��unused;iParam2: �ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHT, //���ҡ�iParam1��unused;iParam2: �ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFTTOP,  //���ϡ�iParam1����ֱ�ٶ�;iParam2: ˮƽ�ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHTTOP,  //���ϡ�iParam1����ֱ�ٶ�;iParam2: ˮƽ�ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_LEFTDOWN,  //���¡�iParam1����ֱ�ٶ�;iParam2: ˮƽ�ٶ�;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_RIGHTDOWN,  //���¡�iParam1����ֱ�ٶ�;iParam2: ˮƽ�ٶ�;iParam3:unused

    //��ͷ����
    BVPU_SDK_PTZ_COMMAND_ZOOM_INC,  //���ӷŴ�����iParam1��unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_ZOOM_DEC,  //��С�Ŵ�����iParam1��unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_FOCUS_INC, //�����Զ��iParam1��unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_FOCUS_DEC, //���������iParam1��unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_APERTURE_INC, //��Ȧ�Ŵ�iParam1��unused;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_APERTURE_DEC, //��Ȧ��С��iParam1��unused;iParam2: unused;iParam3:unused

    //Ԥ�õ����
    BVPU_SDK_PTZ_COMMAND_PRESET_GO,  //ת��Ԥ�õ㡣iParam1��Ԥ�õ��;iParam2: ��ֱ�ٶ�;iParam3:ˮƽ�ٶ�
    BVPU_SDK_PTZ_COMMAND_PRESET_SET, //�ѵ�ǰλ������ΪԤ�õ㡣iParam1��Ԥ�õ��;iParam2: Ԥ�õ���;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_PRESET_SETNAME, //����Ԥ�õ����֡�iParam1��Ԥ�õ��;iParam2: Ԥ�õ���;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_PRESET_DEL, //ɾ��Ԥ�õ㡣iParam1��Ԥ�õ��;iParam2: unused;iParam3:unused

    //Ѳ��·�߲���
    BVPU_SDK_PTZ_COMMAND_CRUISE_GO,//����Ѳ����iParam1��Ѳ��·�ߺ�;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_STOP,//ֹͣѲ����iParam1��Ѳ��·�ߺ�;iParam2: unused;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_SET,//��������Ѳ��·�ߡ�iParam1��Ѳ��·�ߺ�;iParam2: BVCU_PUCFG_CRUISEָ��;iParam3:unused
    BVPU_SDK_PTZ_COMMAND_CRUISE_DEL,//ɾ��Ѳ��·�ߡ�iParam1��Ѳ��·�ߺ�;iParam2: unused;iParam3:unused

    //�������ܲ���
    BVPU_SDK_PTZ_COMMAND_AUX,//��/�رո������ܿ��أ�Param1��������;iParam2: 0-�ر�,1-����;iParam3:unused

    //������
    //�����������60����û�û���ֹ����������Server���Զ����������
    BVPU_SDK_PTZ_COMMAND_LOCK,//����/������̨��iParam1��unused;iParam2: unused;iParam3:unused
};

/*============================================SDK Command/Subcommand=========================================================*/
typedef enum{
    BVPU_SDK_COMMAND_UNKNOWN  = 0,
    BVPU_SDK_COMMAND_QUERY,
    BVPU_SDK_COMMAND_CONTROL,    
    BVPU_SDK_COMMAND_SUBSCRIBE,

    BVPU_SDK_COMMAND_RESERVED = 0x80000000,//�������ڲ�ʹ��
}BVPU_SDK_Command;

typedef enum{
    BVPU_SDK_SUBCOMMAND_UNKNOWN  = 0,
    
    /*===================Query=================*/
    BVPU_SDK_SUBCOMMAND_QUERY_FIRST = 0x01,   
    BVPU_SDK_SUBCOMMAND_CAPABILITIES_DEVICE,	/*TODO:�豸��������������Ӳ�����������������(���������ã���Ϊ�Ƿ�֧������SDK�ڲ������ģ������ɵ����߾�����)
    																						��Ϊ�����֧���µ��������ò����п���/�Բ���д��Ӧ���������Ҫʹ��XML/ProtoBuf/JSON֮����������������ݶ�ʹ��ProtoBuf����ΪServer��PU֮��ʹ�õ�ProtoBuf��Ϊ�������ݷ�װ��ʽ������ͳһʹ��ProtoBuf���Լ���һ�θ��ط�װ��ʽ��ת����
    																			
    																			
    																			
    																					*/
    BVPU_SDK_SUBCOMMAND_CAPABILITIES_PTZ,			/*�豸PTZ���������Ƿ�֧����ת���Խ�����Ȧ�ȵ�������������������������*/
    
    BVPU_SDK_SUBCOMMAND_SEARCH_DEVICE,	/*�����豸���������ͣ�BVPU_SDK_SearchLocalNetworkDevice��������ͣ�BVPU_SDK_LocalNetworkDevice����*/
    BVPU_SDK_SUBCOMMAND_DEVICE_INFO,	/*�豸��Ϣ����������:�ޣ�������ͣ�BVCU_PUCFG_DeviceInfo;BVCU_PUCFG_DeviceInfo����Ҫ���� ģ��ͨ������IPͨ������ģ�ⱨ�������������IP������																			��������ȵ�*/
    
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
    /*ע�⣺query/control���͵����ͨ��query��control���������ͺ���������ǶԳƵģ�����
    BVPU_SDK_SUBCOMMAND_PTZATTR������Queryʱ�����������ޣ����������BVPU_SDK_PUCFG_PTZAttr����Control
    ʱ����������BVPU_SDK_PUCFG_PTZAttr����������ޡ�����ע���У�ֻ����Control�Ĳ��������û���ر�˵����     
    Query�Ĳ�������/������;���Control�����/��������
    */
    BVPU_SDK_SUBCOMMAND_CONTROL_QUERY_FIRST = 0x40000000,
    
    BVPU_SDK_SUBCOMMAND_SDK_LOG,    /*����SDK��log�ȼ���log������豸���������ͣ�BVPU_SDK_LogSetting��������ͣ��ޣ�*/
    
    BVPU_SDK_SUBCOMMAND_AVCHANNEL_DEVICE_INFO,	/*��������Ƶͨ�����豸���ԣ��������ͣ�BVPU_SDK_ChannelDeviceInfo��������ͣ��ޣ�
    																						��ע�������search ����һ��device���óɱ�sdk��һ��channel.
    																									�����ͨ����ʹ��IPC����Ҫͨ�����������ø�ͨ��ipc�ĵ�ַ���˿ڡ��û���������ȵ���Ϣ��
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
    


    BVPU_SDK_SUBCOMMAND_RESERVED = 0x80000000,//�������ڲ�ʹ��
}BVPU_SDK_Subcommand;

/*==========================================const for Media Stream========================================================*/
/**/
/*IPͨ��/IP��������/IP�������*/
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_CHANNEL  0   //����Ƶͨ��	/*��ģ��ͨ����IPͨ��*/
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_CHANNEL  0x00FFFF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_GPS      0x010000 //GPS�豸����
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_GPS      0x0100FF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_TSP      0x010100 //͸�������豸����
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MAX_TSP      0x0101FF
#define BVPU_SDK_SUBDEV_INDEXMAJOR_MIN_CUSTOM   0xF00000 //�Զ����豸����
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
    BVPU_SDK_VIDEOFORMAT_CUSTOM,//�Զ��� ///< custom define
} BVPU_SDK_VIDEO_FORMAT;

// ʱ����ʾ��ʽ
typedef enum BVPU_SDK_TIMEFORMAT{
    BVPU_OSD_TIMEFORMAT_INVALID = 0, ///<   ������ʱ��
    BVPU_OSD_TIMEFORMAT_1,           ///<   YYYY-MM-DD hh:mm:ss
    BVPU_OSD_TIMEFORMAT_2,           ///<   YYYY-MM-DD ����W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_3,           ///<   DD��MM��YYYY�� hh:mm:ss
    BVPU_OSD_TIMEFORMAT_4,           ///<   DD��MM��YYYY�� ����W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_5,           ///<   MM��DD��YYYY�� hh:mm:ss
    BVPU_OSD_TIMEFORMAT_6,           ///<   MM��DD��YYYY�� ����W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_7,           ///<   YYYY��MM��DD�� hh:mm:ss
    BVPU_OSD_TIMEFORMAT_8,           ///<   YYYY��MM��DD�� ����W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_9,           ///<   DD-MM-YYYY hh:mm:ss
    BVPU_OSD_TIMEFORMAT_10,          ///<   DD-MM-YYYY ����W hh:mm:ss
    BVPU_OSD_TIMEFORMAT_11,          ///<   MM-DD-YYYY hh:mm:ss
    BVPU_OSD_TIMEFORMAT_12,          ///<   MM-DD-YYYY ����W hh:mm:ss
} BVPU_SDK_TIMEFORMAT;

enum BVPU_SDK_Overlay {
    BVPU_SDK_OVERLAY_TIME = (1<<0),  //���� ʱ��
    BVPU_SDK_OVERLAY_TEXT = (1<<1),  //���� �Զ�������
    BVPU_SDK_OVERLAY_GPS =  (1<<2),  //���� GPS��Ϣ
    BVPU_SDK_OVERLAY_ALARM = (1<<3), //���� �¼���Ϣ
    BVPU_SDK_OVERLAY_NAME = (1<<4),  //���� ͨ����
};

//encoder�豸֧�ֵ�������
typedef enum{
    BVPU_SDK_ENCODERSTREAMTYPE_INVALID = -1,
    BVPU_SDK_ENCODERSTREAMTYPE_STORAGE,//�洢��
    BVPU_SDK_ENCODERSTREAMTYPE_PREVIEW,//������
    BVPU_SDK_ENCODERSTREAMTYPE_PICTURE,//ͼƬ��
    BVPU_SDK_ENCODERSTREAMTYPE_MAX_COUNT,
}BVPU_SDK_EncoderStreamType;

/*ý�巽��*/
typedef enum{
    BVPU_SDK_MEDIADIR_VIDEOSEND = (1<<0),
    BVPU_SDK_MEDIADIR_VIDEORECV = (1<<1),
    BVPU_SDK_MEDIADIR_AUDIOSEND = (1<<2),
    BVPU_SDK_MEDIADIR_AUDIORECV = (1<<3),
    BVPU_SDK_MEDIADIR_TALKONLY  = (BVPU_SDK_MEDIADIR_AUDIOSEND | BVPU_SDK_MEDIADIR_AUDIORECV),
    BVPU_SDK_MEDIADIR_DATASEND  = (1<<4),
    BVPU_SDK_MEDIADIR_DATARECV  = (1<<5),
}BVPU_SDK_MediaDir;

//��Ƶ���ʿ���
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

    BVPU_SDK_MEDIASTREAM_COMMAND_RESERVED = 0x80000000,//�������ڲ�ʹ��
}BVPU_SDK_MediaStreamCommand;

typedef enum {
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_UNKNOW = 0,
	
	/*Control*/
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_PRODUCE_I_FRAME,	/*����������Ƶ�ؼ�֡�������������*/
		
	/*Query*/
	
	/*Control/Query*/
	/*ע�⣺query/control���͵����ͨ��query��control���������ͺ���������ǶԳƵģ�����
    BVPU_SDK_MEDIASTREAM_SUBCOMMAND_AUDIOIN_VOLUME������Queryʱ�����������ޣ����������BVPU_SDK_PUCFG_PTZAttr����Control
    ʱ����������BVPU_SDK_PUCFG_PTZAttr����������ޡ�����ע���У�ֻ����Control�Ĳ��������û���ر�˵����     
    Query�Ĳ�������/������;���Control�����/��������
    */
    BVPU_SDK_MEDIASTREAM_SUBCOMMAND_MANUAL_ADJUST_VIDEO_ENCODER,	/*��ʱ����video �ı����������Ҫ���ڵ����ߵ����������Ĵ�����ԣ����������BVPU_SDK_MediaStreamVideoEncoderDynamicParam*/
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_UPDATE_STREAM_DIR,	/*ý������ý�巽�����������BVPU_SDK_MediaDir*/
	
	BVPU_SDK_MEDIASTREAM_SUBCOMMAND_MAX_CONUT,
	
}BVPU_SDK_MediaStreamSubcommand;
/*========================================================================================================*/

/*====================================TODO�����е�Hardware Interface========================================================*/

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
