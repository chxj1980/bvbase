#ifndef __BVPU_SDK_H__
#define __BVPU_SDK_H__

#ifdef _MSC_VER
#ifdef LIBBVPU_SDK_EXPORTS
#define LIBBVPU_SDK_API __declspec(dllexport)
#else
#define LIBBVPU_SDK_API __declspec(dllimport)
#endif
#else
#define LIBBVPU_SDK_API
#endif//_MSC_VER

/*===================================================included files====================================================================*/
#include "BVPU_SDK_Const.h"
#include "BVPU_SDK_DataTypes.h"

/*TODO: ��SDK�Ƿ���Ҫ�������ļ���
�洢��Ӳ���豸��������ô�졣
ý�����ı������ô�죿
1. �����ļ���sdk���ƣ����������ݣ�һ�ݶ�ӦӲ�����ԣ����ɸģ�һ�ݿ����ã�����ϵͳ�͵��������á�
2. ȡ��Ƶ���Ĺ��̵ȼ���ý�����ı����
*/

/*SDK ��ؽӿ�*/
/*=========================================Init/DeInit============================================================*/
/*��ʼ����ؽӿ�*/
/*
��ʼ��SDK����������SDK�ӿڶ�ֻ���ڸú����ɹ����غ����ʹ�á�
�ú���ֻ�ܵ���һ�Ρ�
@param[in]	pstGlobalParam 
						��ʼ��SDK�Ĳ�����BVPU_SDK_GlobalParam��
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Init(BVPU_SDK_GlobalParam *pstGlobalParam);

/*
ֹͣSDK����������SDK�ӿڶ������ڸú���֮ǰ���á�
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Deinit();

/*===========================================SDK Command=============================================================*/
/*��ȡ/����/���������������ӿ�

@param[in]  iCommand
						�������ͣ��μ�ö��BVPU_SDK_COMMAND_������
@param[in]	iSubcommand
						���������ͣ��μ�ö��BVPU_SDK_SUBCOMMAND_������
@param[in]	pInBuffer
						���������������ַ�������߷���ĵ�ַ��
@param[in]	iInBufferSize
						��������������Ĵ�С��
@param[in,out]	pOutBuffer
								���������������ַ�������߷���ĵ�ַ�������Ǻ�����仺���������ݡ�
@param[in,out]	pOutBufferSize
								��������������Ĵ�С�������߷���Ļ������Ĵ�С��������ʵ�ʺ����������ݴ�С��
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SendCommand(BVPU_SDK_Command iCommand, 
                                BVPU_SDK_Subcommand iSubCommand, 
                                void *pInBuffer, int iInBufferSize, 
                                void *pOutBuffer, int *pOutBuffeSize);

/*==========================================Media Stream========================================================*/
/*
ͨ���ĸ��
����Ƶͨ����Ҫ���ε�CVBS/SDI/HDMI/AHD/TVI/ONVIF/˽��Э��IPC�ȵȸ��ֲ�ͬ��ȡ����ʽ��ͬ������
��һ��ý������
ͨ���ţ�stream ���ͣ�stream��media�������Ϣ������ �ص�������

@param[out]	phMediaStreamHandle
				 		����ý�����Ĳ����������Ҫ����BVPU_SDK_MediaStream_Control������BVPU_SDK_MediaStream_Close������
@param[in]	pstStreamParam
						ý�����Ĳ�����
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Open(BVPU_SDK_HMediaStreamHandle* phMediaStreamHandle, BVPU_SDK_MediaStreamParam *pstStreamParam);

/*
д��stream�з���Ϊ�����ý�����ݡ�ʹ��ffmepg�е�AVCodecContext��AVPacket����
@param[in]	hMediaStreamHandle:ý�����Ĳ������
@param[in]	pstAVCodecContext: Codec��Ϣ
@param[in]	pstAVPacket������Ƶ���ݣ��յ���ԭʼý�����ݣ������ݣ����֡�������
@return	BVPU_SDK_Result
*/
//LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVPacket *pstAVPacket);
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData);

/*
ý������������ӿڣ�
��Ҫ��ʱ����stream ��֡�ʡ����ʣ���������������ԣ��޸�stream��media����ȵȡ�

@param[in]	hMediaStreamHandle
				 		ý�����Ĳ������
@param[in]  iCommand
						�������ͣ��μ�ö��BVPU_SDK_MEDIASTREAM_COMMAND������
@param[in]	iSubcommand
						���������ͣ��μ�ö��BVPU_SDK_MEDIASTREAM_SUBCOMMAND������
@param[in]	pInBuffer
						���������������ַ�������߷���ĵ�ַ��
@param[in]	iInBufferSize
						��������������Ĵ�С��
@param[in,out]	pOutBuffer
								���������������ַ�������߷���ĵ�ַ�������Ǻ�����仺���������ݡ�
@param[in,out]	pOutBufferSize
								��������������Ĵ�С�������߷���Ļ������Ĵ�С��������ʵ�ʺ����������ݴ�С��
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Command(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, 
																														BVPU_SDK_MediaStreamCommand iCommand, BVPU_SDK_MediaStreamSubcommand iSubCommand, 
																														void *pInBuffer, int iInBufferSize, 
																														void *pOutBuffer, int* pOutBufferSize);
																														
/*
�ر�ý�������ͷ�open��ʱ�������������Դ
@param[in]	hMediaStreamHandle
				 		ý�����Ĳ������
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Close(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle);

/*====================================TODO�����е�Hardware Interface========================================================*/
/*��SDK�ڲ���ʵ��ý�����Ļ�ȡ��ʽ�������߿���ͨ���Զ���Device�ӿ���ʵ�֡�����˵����SDK��֧��IOS������Ƶ�ɼ��������߿���ʹ�øýӿ��Լ�ʵ��IOS������Ƶ�ɼ���Ȼ���ø�SDKʹ����Щ�ӿ����������ͨ����ص�ͨ�ù��ܣ�ͬʱ����Ҳ�൱�ڰѻ�ȡ���ķ�ʽ�����������SDK��һ���޸ĵ��¶����Ʒ��SDK���±��롣*/
/*avͨ����������豸����������Ƶý�����ݵĲɼ������ʽ���������ƽӿڣ�����˵video�ĶԱȶȡ����ȵ��ڡ�audio�������ĵ��ڣ�*/
/*�洢��Ӳ���豸��������ô�졣*/

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SetHardwareInterface(BVPU_SDK_HardwareDeviceInterface *pstHardwareDeviceInterface);


/*====================================TODO�����е�Encode/Decode Interface========================================================*/


/*==============================================================��Ҫ��һ�����ǵ�����=============================================*/
/*
��SDK�ֳ����㣬����һ����Ե������ͻ����ڶ�����Թ�˾�ڲ�����ƽ̨�Ŀ�����Ա��

0����ʹ��AVCodecContext/AVPacket����ffmpeg�����õĸ��ӵĽṹ�壬������SAVPacket��װ���ģ��ٻ������Լ��ٷ�װһ���򵥵Ľṹ�塣


1.����ָ����ÿһ��ģ���ܶ���һ����̬����߾�̬�⣬ÿ��ģ�������ͳһ�Ľӿڡ�
2.ʹ��CMake������ƽ̨���롣
3����SDK�ڲ����ɼ�����֤���ܣ�����ʹ�ø�SDK�ĵ��������̡�
4�������ܲ��ܰ�
5. Ҫ������misc ctrl�Ĳ�����ƣ����㲻ͬ��Ʒ�Ͷ��Ƶ�����
*/
/*===============================================================================================================================*/
#endif
