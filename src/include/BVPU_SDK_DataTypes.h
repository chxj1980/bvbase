#ifndef __BVPU_SDK_DATATYPES_H__
#define __BVPU_SDK_DATATYPES_H__


/*===================================================included files====================================================================*/
#include "BVPU_SDK_Const.h"

#include "BVPU_SDK_Event.h"

/*===================================================Common Structs====================================================================*/

typedef void *	BVPU_SDK_HHandle;
typedef BVPU_SDK_HHandle BVPU_SDK_HMediaStreamHandle;


/*TODO：deviceinfo参见海康客户端SDK中的NET_DVR_DEVICECFG_V40*/


typedef struct _st_BVPU_SDK_VideoColorParam {
		unsigned char cBrightness;  //亮度,取值范围[0,255]
    unsigned char cContrast;    //对比度,取值范围[0,255]
    unsigned char cSaturation;  //饱和度,取值范围[0,255]
    unsigned char cHue;         //色调,取值范围[0,255]
} BVPU_SDK_VideoColorParam;
//
///
///一天中的一个时间片
/// Start time and end time
/// @warning 注意：所有BVCU_DayTimeSlice stWeek[7][...]等表示一周中的时间片，stWeek[0]表示周日，stWeek[1]表示周一,...stWeek[6]表示周六
///
typedef struct _BVPU_SDK_DayTimeSlice{
    char cHourBegin;         ///< hour begin
    char cMinuteBegin;       ///< minute begin
    char cSecondBegin;       ///< second begin
    char cHourEnd;           ///< hour end
    char cMinuteEnd;         ///< minute end
    char cSecondEnd;         ///< second end
    char cReserved[2];       //对齐
}BVPU_SDK_DayTimeSlice;

///
///墙上时间
///
typedef struct _BVPU_SDK_WallTime{
    short iYear;             ///< year
    char  iMonth;            ///< month
    char  iDay;              ///< day
    char  iHour;             ///< hour
    char  iMinute;           ///< minute
    char  iSecond;           ///< second
    char  cReserved[1];      //对齐
}BVPU_SDK_WallTime;

///
/// ImageSize
///
typedef struct _BVPU_SDK_ImageSize{
    int iWidth;   ///< width
    int iHeight;  ///< height
}BVPU_SDK_ImageSize;

///
/// Position with (x,y) (left, top)
///
typedef struct _BVPU_SDK_ImagePos{
    int iLeft;    ///< left
    int iTop;     ///< top
}BVPU_SDK_ImagePos;

///
/// Contain (x,y)(width, height)
///
typedef struct _BVPU_SDK_ImageRect{
    int iLeft;    ///< left
    int iTop;     ///< top
    int iWidth;   ///< width
    int iHeight;  ///< height
}BVPU_SDK_ImageRect;


typedef struct _st_BVPU_SDK_OnvifConnectInfo {
        // The rtsp url
        char service_url[BVPU_SDK_MAX_HOST_NAME_LEN];
		char szServerAddress[BVPU_SDK_MAX_HOST_NAME_LEN+1];
		int	iPort;
		char szUserName[BVPU_SDK_MAX_NAME_LEN+1];
		char szUserPassword[BVPU_SDK_MAX_PASSWORD_LEN+1];
		int	iSupportAnonymousLogin;	/*是否匿名登录*/
}BVPU_SDK_OnvifConnectInfo;

/////
///// Device Capability
/////

typedef struct _st_BVPU_SDK_DeviceCapability {
} BVPU_SDK_DeviceCapability;

typedef struct _st_BVPU_SDK_PTZCapability {
} BVPU_SDK_PTZCapability;


/////
///// Device Media Capability
/////

typedef struct _st_BVPU_SDK_MediaAudioEncoderCapability {
    BVPU_SDK_SAVCodec_ID iVideoCodecAll[4];  // 支持的音频编码
    char iChannelCount[4];   // 声道数
    char iBitsPerSample[4];  // 采样精度
    char iSamplesPerSec[8];  // 采样率
    char iBitRate[8];        // 码率
} BVPU_SDK_MediaAudioEncoderCapability;

typedef struct _st_BVPU_SDK_MediaVideoEncoderCapability {
    int   iMaxFrameRate;
    BVPU_SDK_ImageSize stVideoResolution[8];
    BVPU_SDK_SAVCodec_ID iVideoCodecAll[4]; // 支持的视频编码参数
    int   iKbpsLimitMin[BVPU_SDK_VIDEO_QUALITY_COUNT];
    int   iKbpsLimitMax;
} BVPU_SDK_MediaVideoEncoderCapability;

// 每个通道的音视频编码能力
typedef struct _st_BVPU_SDK_MediaEncoderCapability {
    int   iChannel;
    // 视频部分
    BVPU_SDK_MediaVideoEncoderCapability stMediaVideoEncoderCapability;

    // 音频部分
    BVPU_SDK_MediaAudioEncoderCapability stMediaAudioEncoderCapability;

} BVPU_SDK_MediaEncoderCapability;

// 某媒体设备的能力
// 针对通道
typedef struct _st_BVPU_SDK_DeviceMediaCapability {
    int     iSize;     // sizeof(BVPU_SDK_DeviceMediaCapabilities)

    int     iCounts;   // How many
    
    BVPU_SDK_MediaEncoderCapability *stMediaEncoderCapability;
} BVPU_SDK_DeviceMediaCapability;



/*================================================ onvif inference ============================================== */
/*=============================================structs of Global Param===========================================================*/
typedef struct _st_BVPU_SDK_GlobalParam {
	/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_GlobalParam）*/
	int	iSize;
	
	/**
		*用于处理库内部的全局异常、报警事件和收到的命令等等
		*iEventCode	对应的全局事件，见BVPU_SDK_EVENT_xxxx；
		*pParamBuf	全局事件对应的参数。
		*iParamBufSize 参数缓冲区的大小
		*pUserData指向用户自定义数组szUserData[4]
		*TODO:如果以后Event太多了的话，可以考虑参考海康的做法，把Event分类，每一类有一个Event的回调。
		**/
	void (*OnEvent)(BVPU_SDK_Event iEventCode, void *pParamBuf, int iParamBufSize, int *pUserData);
	
	/*用户自定义的数据，在OnEvent中通过参数传入*/
	int szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
	
} BVPU_SDK_GlobalParam;

/*==========================================structs of SubCommand=============================================================*/

typedef struct _st_BVPU_SDK_LogSetting {
	/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_Log_Setting)*/
	int	iSize;
		/*SDK的输出log等级，BVPU_SDK_LOG_LEVELxxx */
	BVPU_SDK_Log_level iLogLevel;
	
	/*SDK的输出log设备，BVPU_SDK_LOG_OUTPUT_DEVICE_xxx*/
	BVPU_SDK_Log_Output_Device iLogOutputDevice;

    // TODO: Log file name if output to file
}BVPU_SDK_LogSetting;

typedef struct _st_BVPU_SDK_SearchLocalNetworkDevice {
	/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_SearchDevice)*/
	int	iSize;
	
	/*搜索协议的类型，BVPU_SDK_SearchDeviceProtocol*/
	BVPU_SDK_SearchDeviceProtocol iSearchDeviceProtocolType;
	
	/*搜索设备的最长时间，默认为5s，超过这个时间没有搜索到设备就失败*/
	int iTimeOut;
	
}BVPU_SDK_SearchLocalNetworkDevice;

typedef struct _st_BVPU_SDK_RemoteServerCfg {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_RemoteServerCfg)*/
		int iSize;
				
		char szServerName[BVPU_SDK_MAX_NAME_LEN+1];
		char szServerAddress[BVPU_SDK_MAX_HOST_NAME_LEN+1];
		int	iPort;
		char szUserName[BVPU_SDK_MAX_NAME_LEN+1];
		char szUserPassword[BVPU_SDK_MAX_PASSWORD_LEN+1];
		int	iSupportAnonymousLogin;	/*是否支持匿名登录*/
}BVPU_SDK_RemoteServerCfg;

typedef struct _st_BVPU_SDK_LocalNetworkDevice {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_LocalNetDeviceList)*/
		int iSize;
		
        // Max support 16 devices.
		BVPU_SDK_RemoteServerCfg stLocalNetworkDeviceCfg[16];
		
}BVPU_SDK_LocalNetworkDevice;

typedef struct _st_BVPU_SDK_ChannelDeviceInfo {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_DeviceChannel)*/
		int iSize;
		
		int	iChannel;
		int	iChannelType;/*通道类型，BVPU_SDK_DEVICE_CHANNEL_TYPE_xxx*/
		
		/*当iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_CVBS时，使用这个配置*/
		
		/*当iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_COMS时，使用这个配置*/
		
		/*当iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_ONVIF时，使用这个配置*/
		BVPU_SDK_OnvifConnectInfo stOnvifConnectInfo;
		
		/*当iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_AHD时，使用这个配置*/
		
}BVPU_SDK_ChannelDeviceInfo;
/*==========================================structs of Media Stream========================================================*/

///
/// 音频编码器参数
///
typedef struct _BVPU_SDK_AudioEncoderParam{
    BVPU_SDK_SAVCodec_ID iCodecID;              ///< 编码器ID
    char iChannelCount;           ///< 当前使用的声道索引
    char iBitsPerSample;          ///< 当前使用的采样精度索引
    char iSamplesPerSec;          ///< 当前使用的采样率索引
    char iBitRate;                ///< 当前使用的码率索引
}BVPU_SDK_AudioEncoderParam;

///
/// 编码器压缩参数
///
typedef struct _BVPU_SDK_EncoderParam{
    BVPU_SDK_DayTimeSlice stTime; //时间片。不同的时间可以采用不同的编码参数 (目前只有一个，可忽略)

    //视频编码属性
    int bVideoEnable;//0-禁止，1-使能
    BVPU_SDK_SAVCodec_ID iVideoCodec;//
    BVPU_SDK_RateControl iRateControl;//码率控制类型
    BVPU_SDK_ImageRect iImageRect;//
    int iFramesPerSec;//单位1/1000帧。例如25fps，需要设置为25*1000。该帧率不能超过BVCU_PUCFG_VideoIn.iFPSMax
    int iKeyFrameInterval;//关键帧间隔
    int iImageQuality;//视频编码质量，取值范围1~BVCU_VIDEO_QUALITY_COUNT
    int iKbpsLimitCurrent;//码率限制当前值。
	int iOverlay;//叠加信息标志。BVCU_PUCFG_SNAPSHOTPARAM_OVERLAY_*的组合

    //音频编码属性
    int bAudioEnable;//0-禁止，1-使能
    BVPU_SDK_AudioEncoderParam stAudioCodec;
}BVPU_SDK_EncoderParam;

///
/// Encoder Stream Param
///
typedef struct _BVPU_SDK_EncoderStreamParam{
    int iCount;//编码器配置的时间片个数
    //BVCU_PUCFG_EncoderParam* pstParams;//编码器配置，每个成员对应一个时间片段的设置
    BVPU_SDK_EncoderParam pstParams[2];//编码器配置，每个成员对应一个时间片段的设置,目前仅支持1个
    int iStreamType;//流类型。BVCU_ENCODERSTREAMTYPE_*
    int bEnableTransfer;//是否允许传输。0-不允许，1-允许
}BVPU_SDK_EncoderStreamParam;

///
/// 编码器通道
///
typedef struct  _BVPU_SDK_EncoderChannel{
    char szName[BVPU_SDK_MAX_NAME_LEN+1];
    int iCount;//支持的码流个数。只读
    //BVCU_PUCFG_EncoderStreamParam* pParams;
    BVPU_SDK_EncoderStreamParam pParams[4];
    char iVideoInIndex;//摄像头索引，-1表示不支持。只读
    char iAudioInIndex;//音频输入索引，-1表示不支持。只读
    char iAudioOutIndex;//音频输出索引，-1表示不支持。只读
    char iPTZIndex; //云台索引。-1表示不支持。只读
    char cReserved[16];
}BVPU_SDK_EncoderChannel;

/*直通*/
typedef struct _st_BVPU_SDK_LiveVideoChannelChange {
    int   bStart;  // 0: stop live video, 1: start livevideo, -1: 4/1画面的切换功能 
    int   iIndex;  // 0-3: 切换到某一路全画面显示, -1:切换到４路显示 (bStart == -1生效) 0-3 channel index 启动哪路直通(bStart == 0/1)
} BVPU_SDK_LiveVideoChannelChange;

/* 视频直通配置 */
typedef struct _st_BVPU_SDK_LiveVideoChannelConfig {
    int   bStart;       // 1: 配置此通道 0: 销毁此通道，再此使用需要重新配置
    /** 建议三个参数采用同一值 */
    int   iIndex;       // 第几路视频直通配置
    int   iInIndex;     // 输入通道号  (一般情况，与上一参数值相同)
    int   iOutIndex;    // 输出通道号  (一般情况，输入，输出一一对应)
} BVPU_SDK_LiveVideoChannelConfig;

/* 视频输出位置*/
typedef struct _st_BVPU_SDK_VideoOut {
    int              iChannel;    // 配置通道0/1/2/3
    BVPU_SDK_ImageRect   szImageRect;  // 位置和大小
} BVPU_SDK_VideoOut;

/*  Snapshot   */
typedef struct  _BVPU_SDK_SnapshotParam{
    BVPU_SDK_ImageSize stImageSize;//当前使用的抓拍图像分辨率
    int iQuality;//抓拍JPG压缩质量，取值范围1～100    
    int iSequencePicCount; //单次连拍图片数。
    int iSequenceInterval;//连拍的每张图片时间间隔。单位毫秒
    int iSequenceDelay;//一次连拍周期结束后延时时间。单位毫秒。
    int iMaxSequenceCount;//一次开始手动抓拍命令，连续抓拍的最大周期数，然后自动停止抓拍。 0：表示不限制最大周期数。
    int iOverlay;//叠加信息标志。BVPU_SDK_OVERLAY_*的组合
}BVPU_SDK_SnapshotParam;

typedef struct _st_BVPU_SDK_MediaStream_Storage{
    int iEnableInterLibManagement;	//SDK内部自动管理存储方式和策略； 1---使能，0---不使能
    
    /*当0 == iEnableInterLibManagement的时候使用,录像路径*/
    char szFilePath[BVPU_SDK_MAX_FILE_NAME_LEN+1];
    
    /*当0 == iEnableInterLibManagement的时候使用,每个录像文件的时间长度，单位秒。设置为<=0表示停止存储*/
    int   iFileLenInSeconds;
}BVPU_SDK_MediaStream_Storage;

/*
 * 流数据信息
 */
typedef struct _st_BVPU_SDK_MediaStreamData {
	/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_MediaStreamData)*/
    int iSize;

    void *pData;   // 视频数据指针
    int   iLen;    // 数据长度

    // 数据基本信息
    int   iChannel;
    enum BVPU_SDK_STREAM_TYPE  iAVType;  // audio/video
    // 在下面添加数据Codec信息，按需增加
} BVPU_SDK_MediaStreamData;

typedef struct _st_BVPU_SDK_MediaStreamParam {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_MediaStreamParam)*/
		int	iSize;
		
		int	iChannel;
		BVPU_SDK_EncoderStreamType iStreamType; /*BVPU_SDK_ENCODERSTREAMTYPE_xxxx*/
		
		BVPU_SDK_MediaDir iMediaDir;
		
		/*是否存储该视频流*/
		int	iEnableStorage;
		/*当使能存储该流的时候使用*/
		BVPU_SDK_MediaStream_Storage stMediaStreamStorage;
		
		/*
		使用ffmepg中的AVCodecContext和AVPacket定义
		pstAVCodecContext: Codec信息
    pstAVPacket：音视频数据：收到的原始媒体数据；纯数据：组好帧后的数据
    pUserData为指向BVPU_SDK_MediaStreamParam中szUserData的数组指针。
		*/
		//void (*AfterReadMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVPacket *pstAVPacket, int *pUserData);
		void (*AfterReadMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData, int *pUserData);
		/*
		使用ffmepg中的AVCodecContext和AVPacket定义
		pstAVCodecContext: Codec信息
    pstFrame：音视频数据：解码后得到的原始媒体数据；纯数据：组好帧后的数据
    pUserData为指向BVPU_SDK_MediaStreamParam中szUserData的数组指针。
		*/
		//void (*AfterDecodeMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVFrame* pstFrame, int *pUserData);
		void (*AfterDecodeMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData, int *pUserData);
		
		/* 
    事件回调。
    iEventCode:事件码
     pParam: 每个事件对应的参数，具体类型参考各个事件码的说明。如果pParam是NULL，表示无参数。
     */
    void (*OnEvent)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_Event iEventCode, void* pParam);
		
		int	szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
}BVPU_SDK_MediaStreamParam;

typedef struct _st_BVPU_SDK_MediaStreamVideoEncoderDynamicParam {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_MediaStreamVideoEncoderDynamicParam)*/
		int	iSize;
		
		BVPU_SDK_RateControl iRateControlType; /*编码码率控制方式*/
		int	iPictureQuality; /*图像质量等级， 0～5可调*/
		int iFramesPerSec;//单位1/1000帧。例如25fps，需要设置为25*1000。该帧率不能超过VideoIn.iFPSMax
		int	iBitRate;	//码率，单位bps
		int iKeyFrameInterval;//关键帧间隔
}BVPU_SDK_MediaStreamVideoEncoderDynamicParam;

typedef struct _st_BVPU_SDK_MediaStreamVideoInColorParam {
		/*本结构体的大小，分配者应初始化为sizeof(BVPU_SDK_MediaStreamVideoInColorParam)*/
		int	iSize;
		
		BVPU_SDK_VideoColorParam stColorParam;
}BVPU_SDK_MediaStreamVideoInColorParam;

/* ============================================== 云台======================== */
//=======================云台相关============================

//RS232串口
typedef struct _st_BVPU_SDK_RS232{
    int   iDataBit;    //数据位。5/6/7/8
    int   iStopBit;    //停止位。0:1位，1：1.5位，2：2位
    int   iParity;     //奇偶校验位。0:无，1：奇校验，2：偶校验
    int   iBaudRate;   //波特率.常见的包括1200，2400，4800，9600，19200，38400，57600，115200等
    int   iFlowControl; //流控。0:无，1：软流控，2：硬流控
} BVPU_SDK_RS232;

//串口
typedef struct _st_BVPU_SDK_SerialPort{
    BVPU_SDK_RS232 stRS232;
    int iAddress;//RS485地址，如果为-1，表明不是RS485串口
    int iType;//0-数据传输（例如PPP拨号）;1-控制台;2-透明串口
}BVPU_SDK_SerialPort;

//预置点
typedef struct _st_BVPU_SDK_Preset{
    int  iID;//预置点号。-1表示无效，有效值从0开始
    char szPreset[BVPU_SDK_PTZ_MAX_NAME_LEN+1]; //预置点名
}BVPU_SDK_Preset;
//巡航点
typedef struct _st_BVPU_SDK_CruisePoint{
    short iPreset;//预置点号。-1表示无效值
    short iSpeed;//转到下一巡航点的云台速度
    int   iDuration;//在本预置点停留时间，单位秒
}BVPU_SDK_CruisePoint;

//巡航路线
typedef struct _st_BVPU_SDK_Cruise{
    int  iID;//巡航路线号。-1表示无效，有效值从0开始
    char szName[BVPU_SDK_PTZ_MAX_NAME_LEN+1];//巡航路线名字。未设置的巡航路线名字为空

    //巡航路线的巡航点。约定：有效的巡航点放在数组前面，数组中第一个无效巡航点之后的点都被认为是无效点
    BVPU_SDK_CruisePoint stPoints[BVPU_SDK_PTZ_MAX_CRUISEPOINT_COUNT];
}BVPU_SDK_Cruise;

//云台属性
typedef struct _BVPU_SDK_PTZAttr{
    int iPTZProtocolAll[BVPU_SDK_PTZ_MAX_PROTOCOL_COUNT];  //支持的所有协议列表。BVCU_PTZ_PROTO_*。只读
    int iPTZProtocolIndex;   //当前使用的PTZ协议索引

    int iAddress;      //485地址，范围0～255。可写。
    BVPU_SDK_RS232 stRS232;  //232串口属性。可写

    //是否批量更改预置点。0-不更改，1-更改。不更改时szPreset的内容被忽略。 仅在配置命令时有意义，查询时无意义
    int bChangePreset;

    //是否批量更改巡航路线。0-不更改，1-更改。不更改时stCruise的内容被忽略。仅在配置命令时有意义，查询时无意义
    int bChangeCruise;

    //预置列表。
    //查询时返回所有预置点，设置时的作用是批量改预置点的名字和删除预置点
    //注意：预置点的位置只能用BVCU_PTZ_COMMAND_PRESET_SET命令设置
    //约定：所有有效的预置点放在数组最前面，如果总数目不到BVCU_PTZ_MAX_PRESET_COUNT，则第一个无效的Preset的iIndex为-1
    BVPU_SDK_Preset stPreset[BVPU_SDK_PTZ_MAX_PRESET_COUNT]; 

    //巡航路线。
    //查询时返回巡航路线。设置时批量更改巡航路线
    //约定：所有有效的巡航路线放在数组最前面，如果总数目不到BVCU_PTZ_MAX_CRUISE_COUNT，第一个无效的Cruise的iIndex为-1
    BVPU_SDK_Cruise stCruise[BVPU_SDK_PTZ_MAX_CRUISE_COUNT];
    
    //当前正在使用的巡航路线ID。-1表示没有活跃的巡航路线。
    int iActiveCruiseID;
}BVPU_SDK_PTZAttr;

typedef struct _BVPU_SDK_PTZControl{
    int iPTZCommand;    //BVCU_PTZ_COMMAND_*
    int bStop;//0-动作开始，1-动作停止。仅对方向操作/镜头操作/锁操作有效，其他操作应该设置为0。锁操作：0-开始锁定，1-停止锁定
    int iParam1,iParam2,iParam3;//参考BVCU_PTZ_COMMAND_*说明
    //注意：BVCU_PTZ_COMMAND_CRUISE_SET的iParam2是个指针，网络发送/接收时应发送/接收整个BVCU_PTZ_COMMAND_CRUISE_SET结构体
}BVPU_SDK_PTZControl;



/*========================================================================================================*/

/*====================================TODO：所有的Hardware Interface========================================================*/


/*hardware 功能接口
针对每一种device，SDK必须有是否用户提供的函数需要支持多线程安全的说明。
*/
typedef struct _st_BVPU_SDK_HardwareDeviceInterface {
	BVPU_SDK_HardwareDeviceType	iHardwareType;
	int	iIndex;	/*每一种device type都从0开始*/
	char	szName[BVPU_SDK_MAX_NAME_LEN+1];
    char             *deviceName;
	/*callback function*/
	/*BVPU_SDK_Result (*OnInit)();*/
	/*BVPU_SDK_Result (*OnProbe)();*/
	BVPU_SDK_Result (*OnOpen)(const char *device, int flag, int indx, void *priv);	/*不同的iHardwareType对应不同的Open输入参数*/
	BVPU_SDK_Result	(*OnRead)(void *priv, void *data, int len);
	BVPU_SDK_Result	(*OnWrite)(void *priv, void *data, int len);
	BVPU_SDK_Result	(*OnControl)(int iCmd, void *data); /*需要支持各种命令，比如说音视频通道需要支持获取/调整颜色、音量等等命令；报警输入输出通道需要支持输出占空比等命令*/
	BVPU_SDK_Result	(*OnSeek)(int whence, long offset);
	BVPU_SDK_Result	(*OnClose)();
	/*BVPU_SDK_Result (*OnDeInit)();*/
	int	szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
}BVPU_SDK_HardwareDeviceInterface;

#endif
