#ifndef __BVPU_SDK_DATATYPES_H__
#define __BVPU_SDK_DATATYPES_H__


/*===================================================included files====================================================================*/
#include "BVPU_SDK_Const.h"

#include "BVPU_SDK_Event.h"

/*===================================================Common Structs====================================================================*/

typedef void *	BVPU_SDK_HHandle;
typedef BVPU_SDK_HHandle BVPU_SDK_HMediaStreamHandle;


/*TODO��deviceinfo�μ������ͻ���SDK�е�NET_DVR_DEVICECFG_V40*/


typedef struct _st_BVPU_SDK_VideoColorParam {
		unsigned char cBrightness;  //����,ȡֵ��Χ[0,255]
    unsigned char cContrast;    //�Աȶ�,ȡֵ��Χ[0,255]
    unsigned char cSaturation;  //���Ͷ�,ȡֵ��Χ[0,255]
    unsigned char cHue;         //ɫ��,ȡֵ��Χ[0,255]
} BVPU_SDK_VideoColorParam;
//
///
///һ���е�һ��ʱ��Ƭ
/// Start time and end time
/// @warning ע�⣺����BVCU_DayTimeSlice stWeek[7][...]�ȱ�ʾһ���е�ʱ��Ƭ��stWeek[0]��ʾ���գ�stWeek[1]��ʾ��һ,...stWeek[6]��ʾ����
///
typedef struct _BVPU_SDK_DayTimeSlice{
    char cHourBegin;         ///< hour begin
    char cMinuteBegin;       ///< minute begin
    char cSecondBegin;       ///< second begin
    char cHourEnd;           ///< hour end
    char cMinuteEnd;         ///< minute end
    char cSecondEnd;         ///< second end
    char cReserved[2];       //����
}BVPU_SDK_DayTimeSlice;

///
///ǽ��ʱ��
///
typedef struct _BVPU_SDK_WallTime{
    short iYear;             ///< year
    char  iMonth;            ///< month
    char  iDay;              ///< day
    char  iHour;             ///< hour
    char  iMinute;           ///< minute
    char  iSecond;           ///< second
    char  cReserved[1];      //����
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
		int	iSupportAnonymousLogin;	/*�Ƿ�������¼*/
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
    BVPU_SDK_SAVCodec_ID iVideoCodecAll[4];  // ֧�ֵ���Ƶ����
    char iChannelCount[4];   // ������
    char iBitsPerSample[4];  // ��������
    char iSamplesPerSec[8];  // ������
    char iBitRate[8];        // ����
} BVPU_SDK_MediaAudioEncoderCapability;

typedef struct _st_BVPU_SDK_MediaVideoEncoderCapability {
    int   iMaxFrameRate;
    BVPU_SDK_ImageSize stVideoResolution[8];
    BVPU_SDK_SAVCodec_ID iVideoCodecAll[4]; // ֧�ֵ���Ƶ�������
    int   iKbpsLimitMin[BVPU_SDK_VIDEO_QUALITY_COUNT];
    int   iKbpsLimitMax;
} BVPU_SDK_MediaVideoEncoderCapability;

// ÿ��ͨ��������Ƶ��������
typedef struct _st_BVPU_SDK_MediaEncoderCapability {
    int   iChannel;
    // ��Ƶ����
    BVPU_SDK_MediaVideoEncoderCapability stMediaVideoEncoderCapability;

    // ��Ƶ����
    BVPU_SDK_MediaAudioEncoderCapability stMediaAudioEncoderCapability;

} BVPU_SDK_MediaEncoderCapability;

// ĳý���豸������
// ���ͨ��
typedef struct _st_BVPU_SDK_DeviceMediaCapability {
    int     iSize;     // sizeof(BVPU_SDK_DeviceMediaCapabilities)

    int     iCounts;   // How many
    
    BVPU_SDK_MediaEncoderCapability *stMediaEncoderCapability;
} BVPU_SDK_DeviceMediaCapability;



/*================================================ onvif inference ============================================== */
/*=============================================structs of Global Param===========================================================*/
typedef struct _st_BVPU_SDK_GlobalParam {
	/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_GlobalParam��*/
	int	iSize;
	
	/**
		*���ڴ�����ڲ���ȫ���쳣�������¼����յ�������ȵ�
		*iEventCode	��Ӧ��ȫ���¼�����BVPU_SDK_EVENT_xxxx��
		*pParamBuf	ȫ���¼���Ӧ�Ĳ�����
		*iParamBufSize �����������Ĵ�С
		*pUserDataָ���û��Զ�������szUserData[4]
		*TODO:����Ժ�Event̫���˵Ļ������Կ��ǲο���������������Event���࣬ÿһ����һ��Event�Ļص���
		**/
	void (*OnEvent)(BVPU_SDK_Event iEventCode, void *pParamBuf, int iParamBufSize, int *pUserData);
	
	/*�û��Զ�������ݣ���OnEvent��ͨ����������*/
	int szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
	
} BVPU_SDK_GlobalParam;

/*==========================================structs of SubCommand=============================================================*/

typedef struct _st_BVPU_SDK_LogSetting {
	/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_Log_Setting)*/
	int	iSize;
		/*SDK�����log�ȼ���BVPU_SDK_LOG_LEVELxxx */
	BVPU_SDK_Log_level iLogLevel;
	
	/*SDK�����log�豸��BVPU_SDK_LOG_OUTPUT_DEVICE_xxx*/
	BVPU_SDK_Log_Output_Device iLogOutputDevice;

    // TODO: Log file name if output to file
}BVPU_SDK_LogSetting;

typedef struct _st_BVPU_SDK_SearchLocalNetworkDevice {
	/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_SearchDevice)*/
	int	iSize;
	
	/*����Э������ͣ�BVPU_SDK_SearchDeviceProtocol*/
	BVPU_SDK_SearchDeviceProtocol iSearchDeviceProtocolType;
	
	/*�����豸���ʱ�䣬Ĭ��Ϊ5s���������ʱ��û���������豸��ʧ��*/
	int iTimeOut;
	
}BVPU_SDK_SearchLocalNetworkDevice;

typedef struct _st_BVPU_SDK_RemoteServerCfg {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_RemoteServerCfg)*/
		int iSize;
				
		char szServerName[BVPU_SDK_MAX_NAME_LEN+1];
		char szServerAddress[BVPU_SDK_MAX_HOST_NAME_LEN+1];
		int	iPort;
		char szUserName[BVPU_SDK_MAX_NAME_LEN+1];
		char szUserPassword[BVPU_SDK_MAX_PASSWORD_LEN+1];
		int	iSupportAnonymousLogin;	/*�Ƿ�֧��������¼*/
}BVPU_SDK_RemoteServerCfg;

typedef struct _st_BVPU_SDK_LocalNetworkDevice {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_LocalNetDeviceList)*/
		int iSize;
		
        // Max support 16 devices.
		BVPU_SDK_RemoteServerCfg stLocalNetworkDeviceCfg[16];
		
}BVPU_SDK_LocalNetworkDevice;

typedef struct _st_BVPU_SDK_ChannelDeviceInfo {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_DeviceChannel)*/
		int iSize;
		
		int	iChannel;
		int	iChannelType;/*ͨ�����ͣ�BVPU_SDK_DEVICE_CHANNEL_TYPE_xxx*/
		
		/*��iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_CVBSʱ��ʹ���������*/
		
		/*��iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_COMSʱ��ʹ���������*/
		
		/*��iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_ONVIFʱ��ʹ���������*/
		BVPU_SDK_OnvifConnectInfo stOnvifConnectInfo;
		
		/*��iChannelType == BVPU_SDK_DEVICE_CHANNEL_TYPE_AHDʱ��ʹ���������*/
		
}BVPU_SDK_ChannelDeviceInfo;
/*==========================================structs of Media Stream========================================================*/

///
/// ��Ƶ����������
///
typedef struct _BVPU_SDK_AudioEncoderParam{
    BVPU_SDK_SAVCodec_ID iCodecID;              ///< ������ID
    char iChannelCount;           ///< ��ǰʹ�õ���������
    char iBitsPerSample;          ///< ��ǰʹ�õĲ�����������
    char iSamplesPerSec;          ///< ��ǰʹ�õĲ���������
    char iBitRate;                ///< ��ǰʹ�õ���������
}BVPU_SDK_AudioEncoderParam;

///
/// ������ѹ������
///
typedef struct _BVPU_SDK_EncoderParam{
    BVPU_SDK_DayTimeSlice stTime; //ʱ��Ƭ����ͬ��ʱ����Բ��ò�ͬ�ı������ (Ŀǰֻ��һ�����ɺ���)

    //��Ƶ��������
    int bVideoEnable;//0-��ֹ��1-ʹ��
    BVPU_SDK_SAVCodec_ID iVideoCodec;//
    BVPU_SDK_RateControl iRateControl;//���ʿ�������
    BVPU_SDK_ImageRect iImageRect;//
    int iFramesPerSec;//��λ1/1000֡������25fps����Ҫ����Ϊ25*1000����֡�ʲ��ܳ���BVCU_PUCFG_VideoIn.iFPSMax
    int iKeyFrameInterval;//�ؼ�֡���
    int iImageQuality;//��Ƶ����������ȡֵ��Χ1~BVCU_VIDEO_QUALITY_COUNT
    int iKbpsLimitCurrent;//�������Ƶ�ǰֵ��
	int iOverlay;//������Ϣ��־��BVCU_PUCFG_SNAPSHOTPARAM_OVERLAY_*�����

    //��Ƶ��������
    int bAudioEnable;//0-��ֹ��1-ʹ��
    BVPU_SDK_AudioEncoderParam stAudioCodec;
}BVPU_SDK_EncoderParam;

///
/// Encoder Stream Param
///
typedef struct _BVPU_SDK_EncoderStreamParam{
    int iCount;//���������õ�ʱ��Ƭ����
    //BVCU_PUCFG_EncoderParam* pstParams;//���������ã�ÿ����Ա��Ӧһ��ʱ��Ƭ�ε�����
    BVPU_SDK_EncoderParam pstParams[2];//���������ã�ÿ����Ա��Ӧһ��ʱ��Ƭ�ε�����,Ŀǰ��֧��1��
    int iStreamType;//�����͡�BVCU_ENCODERSTREAMTYPE_*
    int bEnableTransfer;//�Ƿ������䡣0-������1-����
}BVPU_SDK_EncoderStreamParam;

///
/// ������ͨ��
///
typedef struct  _BVPU_SDK_EncoderChannel{
    char szName[BVPU_SDK_MAX_NAME_LEN+1];
    int iCount;//֧�ֵ�����������ֻ��
    //BVCU_PUCFG_EncoderStreamParam* pParams;
    BVPU_SDK_EncoderStreamParam pParams[4];
    char iVideoInIndex;//����ͷ������-1��ʾ��֧�֡�ֻ��
    char iAudioInIndex;//��Ƶ����������-1��ʾ��֧�֡�ֻ��
    char iAudioOutIndex;//��Ƶ���������-1��ʾ��֧�֡�ֻ��
    char iPTZIndex; //��̨������-1��ʾ��֧�֡�ֻ��
    char cReserved[16];
}BVPU_SDK_EncoderChannel;

/*ֱͨ*/
typedef struct _st_BVPU_SDK_LiveVideoChannelChange {
    int   bStart;  // 0: stop live video, 1: start livevideo, -1: 4/1������л����� 
    int   iIndex;  // 0-3: �л���ĳһ·ȫ������ʾ, -1:�л�����·��ʾ (bStart == -1��Ч) 0-3 channel index ������·ֱͨ(bStart == 0/1)
} BVPU_SDK_LiveVideoChannelChange;

/* ��Ƶֱͨ���� */
typedef struct _st_BVPU_SDK_LiveVideoChannelConfig {
    int   bStart;       // 1: ���ô�ͨ�� 0: ���ٴ�ͨ�����ٴ�ʹ����Ҫ��������
    /** ����������������ͬһֵ */
    int   iIndex;       // �ڼ�·��Ƶֱͨ����
    int   iInIndex;     // ����ͨ����  (һ�����������һ����ֵ��ͬ)
    int   iOutIndex;    // ���ͨ����  (һ����������룬���һһ��Ӧ)
} BVPU_SDK_LiveVideoChannelConfig;

/* ��Ƶ���λ��*/
typedef struct _st_BVPU_SDK_VideoOut {
    int              iChannel;    // ����ͨ��0/1/2/3
    BVPU_SDK_ImageRect   szImageRect;  // λ�úʹ�С
} BVPU_SDK_VideoOut;

/*  Snapshot   */
typedef struct  _BVPU_SDK_SnapshotParam{
    BVPU_SDK_ImageSize stImageSize;//��ǰʹ�õ�ץ��ͼ��ֱ���
    int iQuality;//ץ��JPGѹ��������ȡֵ��Χ1��100    
    int iSequencePicCount; //��������ͼƬ����
    int iSequenceInterval;//���ĵ�ÿ��ͼƬʱ��������λ����
    int iSequenceDelay;//һ���������ڽ�������ʱʱ�䡣��λ���롣
    int iMaxSequenceCount;//һ�ο�ʼ�ֶ�ץ���������ץ�ĵ������������Ȼ���Զ�ֹͣץ�ġ� 0����ʾ�����������������
    int iOverlay;//������Ϣ��־��BVPU_SDK_OVERLAY_*�����
}BVPU_SDK_SnapshotParam;

typedef struct _st_BVPU_SDK_MediaStream_Storage{
    int iEnableInterLibManagement;	//SDK�ڲ��Զ�����洢��ʽ�Ͳ��ԣ� 1---ʹ�ܣ�0---��ʹ��
    
    /*��0 == iEnableInterLibManagement��ʱ��ʹ��,¼��·��*/
    char szFilePath[BVPU_SDK_MAX_FILE_NAME_LEN+1];
    
    /*��0 == iEnableInterLibManagement��ʱ��ʹ��,ÿ��¼���ļ���ʱ�䳤�ȣ���λ�롣����Ϊ<=0��ʾֹͣ�洢*/
    int   iFileLenInSeconds;
}BVPU_SDK_MediaStream_Storage;

/*
 * ��������Ϣ
 */
typedef struct _st_BVPU_SDK_MediaStreamData {
	/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_MediaStreamData)*/
    int iSize;

    void *pData;   // ��Ƶ����ָ��
    int   iLen;    // ���ݳ���

    // ���ݻ�����Ϣ
    int   iChannel;
    enum BVPU_SDK_STREAM_TYPE  iAVType;  // audio/video
    // �������������Codec��Ϣ����������
} BVPU_SDK_MediaStreamData;

typedef struct _st_BVPU_SDK_MediaStreamParam {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_MediaStreamParam)*/
		int	iSize;
		
		int	iChannel;
		BVPU_SDK_EncoderStreamType iStreamType; /*BVPU_SDK_ENCODERSTREAMTYPE_xxxx*/
		
		BVPU_SDK_MediaDir iMediaDir;
		
		/*�Ƿ�洢����Ƶ��*/
		int	iEnableStorage;
		/*��ʹ�ܴ洢������ʱ��ʹ��*/
		BVPU_SDK_MediaStream_Storage stMediaStreamStorage;
		
		/*
		ʹ��ffmepg�е�AVCodecContext��AVPacket����
		pstAVCodecContext: Codec��Ϣ
    pstAVPacket������Ƶ���ݣ��յ���ԭʼý�����ݣ������ݣ����֡�������
    pUserDataΪָ��BVPU_SDK_MediaStreamParam��szUserData������ָ�롣
		*/
		//void (*AfterReadMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVPacket *pstAVPacket, int *pUserData);
		void (*AfterReadMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData, int *pUserData);
		/*
		ʹ��ffmepg�е�AVCodecContext��AVPacket����
		pstAVCodecContext: Codec��Ϣ
    pstFrame������Ƶ���ݣ������õ���ԭʼý�����ݣ������ݣ����֡�������
    pUserDataΪָ��BVPU_SDK_MediaStreamParam��szUserData������ָ�롣
		*/
		//void (*AfterDecodeMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVFrame* pstFrame, int *pUserData);
		void (*AfterDecodeMediaData)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData, int *pUserData);
		
		/* 
    �¼��ص���
    iEventCode:�¼���
     pParam: ÿ���¼���Ӧ�Ĳ������������Ͳο������¼����˵�������pParam��NULL����ʾ�޲�����
     */
    void (*OnEvent)(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_Event iEventCode, void* pParam);
		
		int	szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
}BVPU_SDK_MediaStreamParam;

typedef struct _st_BVPU_SDK_MediaStreamVideoEncoderDynamicParam {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_MediaStreamVideoEncoderDynamicParam)*/
		int	iSize;
		
		BVPU_SDK_RateControl iRateControlType; /*�������ʿ��Ʒ�ʽ*/
		int	iPictureQuality; /*ͼ�������ȼ��� 0��5�ɵ�*/
		int iFramesPerSec;//��λ1/1000֡������25fps����Ҫ����Ϊ25*1000����֡�ʲ��ܳ���VideoIn.iFPSMax
		int	iBitRate;	//���ʣ���λbps
		int iKeyFrameInterval;//�ؼ�֡���
}BVPU_SDK_MediaStreamVideoEncoderDynamicParam;

typedef struct _st_BVPU_SDK_MediaStreamVideoInColorParam {
		/*���ṹ��Ĵ�С��������Ӧ��ʼ��Ϊsizeof(BVPU_SDK_MediaStreamVideoInColorParam)*/
		int	iSize;
		
		BVPU_SDK_VideoColorParam stColorParam;
}BVPU_SDK_MediaStreamVideoInColorParam;

/* ============================================== ��̨======================== */
//=======================��̨���============================

//RS232����
typedef struct _st_BVPU_SDK_RS232{
    int   iDataBit;    //����λ��5/6/7/8
    int   iStopBit;    //ֹͣλ��0:1λ��1��1.5λ��2��2λ
    int   iParity;     //��żУ��λ��0:�ޣ�1����У�飬2��żУ��
    int   iBaudRate;   //������.�����İ���1200��2400��4800��9600��19200��38400��57600��115200��
    int   iFlowControl; //���ء�0:�ޣ�1�������أ�2��Ӳ����
} BVPU_SDK_RS232;

//����
typedef struct _st_BVPU_SDK_SerialPort{
    BVPU_SDK_RS232 stRS232;
    int iAddress;//RS485��ַ�����Ϊ-1����������RS485����
    int iType;//0-���ݴ��䣨����PPP���ţ�;1-����̨;2-͸������
}BVPU_SDK_SerialPort;

//Ԥ�õ�
typedef struct _st_BVPU_SDK_Preset{
    int  iID;//Ԥ�õ�š�-1��ʾ��Ч����Чֵ��0��ʼ
    char szPreset[BVPU_SDK_PTZ_MAX_NAME_LEN+1]; //Ԥ�õ���
}BVPU_SDK_Preset;
//Ѳ����
typedef struct _st_BVPU_SDK_CruisePoint{
    short iPreset;//Ԥ�õ�š�-1��ʾ��Чֵ
    short iSpeed;//ת����һѲ�������̨�ٶ�
    int   iDuration;//�ڱ�Ԥ�õ�ͣ��ʱ�䣬��λ��
}BVPU_SDK_CruisePoint;

//Ѳ��·��
typedef struct _st_BVPU_SDK_Cruise{
    int  iID;//Ѳ��·�ߺš�-1��ʾ��Ч����Чֵ��0��ʼ
    char szName[BVPU_SDK_PTZ_MAX_NAME_LEN+1];//Ѳ��·�����֡�δ���õ�Ѳ��·������Ϊ��

    //Ѳ��·�ߵ�Ѳ���㡣Լ������Ч��Ѳ�����������ǰ�棬�����е�һ����ЧѲ����֮��ĵ㶼����Ϊ����Ч��
    BVPU_SDK_CruisePoint stPoints[BVPU_SDK_PTZ_MAX_CRUISEPOINT_COUNT];
}BVPU_SDK_Cruise;

//��̨����
typedef struct _BVPU_SDK_PTZAttr{
    int iPTZProtocolAll[BVPU_SDK_PTZ_MAX_PROTOCOL_COUNT];  //֧�ֵ�����Э���б�BVCU_PTZ_PROTO_*��ֻ��
    int iPTZProtocolIndex;   //��ǰʹ�õ�PTZЭ������

    int iAddress;      //485��ַ����Χ0��255����д��
    BVPU_SDK_RS232 stRS232;  //232�������ԡ���д

    //�Ƿ���������Ԥ�õ㡣0-�����ģ�1-���ġ�������ʱszPreset�����ݱ����ԡ� ������������ʱ�����壬��ѯʱ������
    int bChangePreset;

    //�Ƿ���������Ѳ��·�ߡ�0-�����ģ�1-���ġ�������ʱstCruise�����ݱ����ԡ�������������ʱ�����壬��ѯʱ������
    int bChangeCruise;

    //Ԥ���б�
    //��ѯʱ��������Ԥ�õ㣬����ʱ��������������Ԥ�õ�����ֺ�ɾ��Ԥ�õ�
    //ע�⣺Ԥ�õ��λ��ֻ����BVCU_PTZ_COMMAND_PRESET_SET��������
    //Լ����������Ч��Ԥ�õ����������ǰ�棬�������Ŀ����BVCU_PTZ_MAX_PRESET_COUNT�����һ����Ч��Preset��iIndexΪ-1
    BVPU_SDK_Preset stPreset[BVPU_SDK_PTZ_MAX_PRESET_COUNT]; 

    //Ѳ��·�ߡ�
    //��ѯʱ����Ѳ��·�ߡ�����ʱ��������Ѳ��·��
    //Լ����������Ч��Ѳ��·�߷���������ǰ�棬�������Ŀ����BVCU_PTZ_MAX_CRUISE_COUNT����һ����Ч��Cruise��iIndexΪ-1
    BVPU_SDK_Cruise stCruise[BVPU_SDK_PTZ_MAX_CRUISE_COUNT];
    
    //��ǰ����ʹ�õ�Ѳ��·��ID��-1��ʾû�л�Ծ��Ѳ��·�ߡ�
    int iActiveCruiseID;
}BVPU_SDK_PTZAttr;

typedef struct _BVPU_SDK_PTZControl{
    int iPTZCommand;    //BVCU_PTZ_COMMAND_*
    int bStop;//0-������ʼ��1-����ֹͣ�����Է������/��ͷ����/��������Ч����������Ӧ������Ϊ0����������0-��ʼ������1-ֹͣ����
    int iParam1,iParam2,iParam3;//�ο�BVCU_PTZ_COMMAND_*˵��
    //ע�⣺BVCU_PTZ_COMMAND_CRUISE_SET��iParam2�Ǹ�ָ�룬���緢��/����ʱӦ����/��������BVCU_PTZ_COMMAND_CRUISE_SET�ṹ��
}BVPU_SDK_PTZControl;



/*========================================================================================================*/

/*====================================TODO�����е�Hardware Interface========================================================*/


/*hardware ���ܽӿ�
���ÿһ��device��SDK�������Ƿ��û��ṩ�ĺ�����Ҫ֧�ֶ��̰߳�ȫ��˵����
*/
typedef struct _st_BVPU_SDK_HardwareDeviceInterface {
	BVPU_SDK_HardwareDeviceType	iHardwareType;
	int	iIndex;	/*ÿһ��device type����0��ʼ*/
	char	szName[BVPU_SDK_MAX_NAME_LEN+1];
    char             *deviceName;
	/*callback function*/
	/*BVPU_SDK_Result (*OnInit)();*/
	/*BVPU_SDK_Result (*OnProbe)();*/
	BVPU_SDK_Result (*OnOpen)(const char *device, int flag, int indx, void *priv);	/*��ͬ��iHardwareType��Ӧ��ͬ��Open�������*/
	BVPU_SDK_Result	(*OnRead)(void *priv, void *data, int len);
	BVPU_SDK_Result	(*OnWrite)(void *priv, void *data, int len);
	BVPU_SDK_Result	(*OnControl)(int iCmd, void *data); /*��Ҫ֧�ָ����������˵����Ƶͨ����Ҫ֧�ֻ�ȡ/������ɫ�������ȵ���������������ͨ����Ҫ֧�����ռ�ձȵ�����*/
	BVPU_SDK_Result	(*OnSeek)(int whence, long offset);
	BVPU_SDK_Result	(*OnClose)();
	/*BVPU_SDK_Result (*OnDeInit)();*/
	int	szUserData[BVPU_SDK_USER_DATA_ARRAY_SIZE];
}BVPU_SDK_HardwareDeviceInterface;

#endif
