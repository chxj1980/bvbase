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

/*TODO: 该SDK是否需要有配置文件，
存储的硬件设备的配置怎么办。
媒体流的编解码怎么办？
1. 配置文件由sdk控制，有至少两份，一份对应硬件属性，不可改，一份可配置，可由系统和调用者配置。
2. 取视频流的过程等价于媒体流的编解码
*/

/*SDK 相关接口*/
/*=========================================Init/DeInit============================================================*/
/*初始化相关接口*/
/*
初始化SDK，其他所有SDK接口都只有在该函数成功返回后才能使用。
该函数只能调用一次。
@param[in]	pstGlobalParam 
						初始化SDK的参数，BVPU_SDK_GlobalParam；
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Init(BVPU_SDK_GlobalParam *pstGlobalParam);

/*
停止SDK，其他所有SDK接口都必须在该函数之前调用。
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Deinit();

/*===========================================SDK Command=============================================================*/
/*获取/设置/订阅配置相关命令接口

@param[in]  iCommand
						命令类型，参见枚举BVPU_SDK_COMMAND_×××
@param[in]	iSubcommand
						子命令类型，参见枚举BVPU_SDK_SUBCOMMAND_×××
@param[in]	pInBuffer
						输入参数缓冲区地址。调用者分配的地址。
@param[in]	iInBufferSize
						输入参数缓冲区的大小。
@param[in,out]	pOutBuffer
								输出参数缓冲区地址。调用者分配的地址，返回是函数填充缓冲区的内容。
@param[in,out]	pOutBufferSize
								输入参数缓冲区的大小。调用者分配的缓冲区的大小，饭后是实际函数填充的数据大小。
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SendCommand(BVPU_SDK_Command iCommand, 
                                BVPU_SDK_Subcommand iSubCommand, 
                                void *pInBuffer, int iInBufferSize, 
                                void *pOutBuffer, int *pOutBuffeSize);

/*==========================================Media Stream========================================================*/
/*
通道的概念。
音视频通道需要屏蔽到CVBS/SDI/HDMI/AHD/TVI/ONVIF/私有协议IPC等等各种不同获取流方式不同的区别。
打开一个媒体流。
通道号，stream 类型，stream的media方向和信息；设置 回调函数。

@param[out]	phMediaStreamHandle
				 		返回媒体流的操作句柄，主要用于BVPU_SDK_MediaStream_Control（）和BVPU_SDK_MediaStream_Close（）；
@param[in]	pstStreamParam
						媒体流的参数。
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Open(BVPU_SDK_HMediaStreamHandle* phMediaStreamHandle, BVPU_SDK_MediaStreamParam *pstStreamParam);

/*
写入stream中方向为输入的媒体数据。使用ffmepg中的AVCodecContext和AVPacket定义
@param[in]	hMediaStreamHandle:媒体流的操作句柄
@param[in]	pstAVCodecContext: Codec信息
@param[in]	pstAVPacket：音视频数据：收到的原始媒体数据；纯数据：组好帧后的数据
@return	BVPU_SDK_Result
*/
//LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, AVCodecContext *pstAVCodecContext, AVPacket *pstAVPacket);
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, BVPU_SDK_MediaStreamData *pMediaStreamData);

/*
媒体流控制命令接口，
主要临时调整stream 的帧率、码率，用来调整传输策略；修改stream的media方向等等。

@param[in]	hMediaStreamHandle
				 		媒体流的操作句柄
@param[in]  iCommand
						命令类型，参见枚举BVPU_SDK_MEDIASTREAM_COMMAND×××
@param[in]	iSubcommand
						子命令类型，参见枚举BVPU_SDK_MEDIASTREAM_SUBCOMMAND×××
@param[in]	pInBuffer
						输入参数缓冲区地址。调用者分配的地址。
@param[in]	iInBufferSize
						输入参数缓冲区的大小。
@param[in,out]	pOutBuffer
								输出参数缓冲区地址。调用者分配的地址，返回是函数填充缓冲区的内容。
@param[in,out]	pOutBufferSize
								输入参数缓冲区的大小。调用者分配的缓冲区的大小，饭后是实际函数填充的数据大小。
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Command(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, 
																														BVPU_SDK_MediaStreamCommand iCommand, BVPU_SDK_MediaStreamSubcommand iSubCommand, 
																														void *pInBuffer, int iInBufferSize, 
																														void *pOutBuffer, int* pOutBufferSize);
																														
/*
关闭媒体流，释放open的时候申请的所有资源
@param[in]	hMediaStreamHandle
				 		媒体流的操作句柄
@return	BVPU_SDK_Result
*/
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Close(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle);

/*====================================TODO：所有的Hardware Interface========================================================*/
/*该SDK内部不实现媒体流的获取方式，调用者可以通过自定义Device接口来实现。比如说现在SDK不支持IOS的音视频采集，调用者可以使用该接口自己实现IOS的音视频采集，然后让该SDK使用这些接口来完成其他通道相关的通用功能；同时这样也相当于把获取流的方式插件化，避免SDK的一个修改导致多个产品的SDK重新编译。*/
/*av通道输入输出设备（包括音视频媒体数据的采集输出方式和其他控制接口，比如说video的对比度、亮度调节、audio的音量的调节）*/
/*存储的硬件设备的配置怎么办。*/

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SetHardwareInterface(BVPU_SDK_HardwareDeviceInterface *pstHardwareDeviceInterface);


/*====================================TODO：所有的Encode/Decode Interface========================================================*/


/*==============================================================需要进一步考虑的事情=============================================*/
/*
把SDK分成两层，最上一层面对第三方客户；第二层面对公司内部各个平台的开发人员。

0、是使用AVCodecContext/AVPacket这类ffmpeg里面用的复杂的结构体，还是用SAVPacket封装过的，再或者是自己再封装一个简单的结构体。


1.代码分割，尽量每一个模块能独立一个动态库或者静态库，每个模块有相对统一的接口。
2.使用CMake做到跨平台编译。
3、在SDK内部集成加密认证功能，控制使用该SDK的第三方厂商。
4、考虑能不能把
5. 要有类似misc ctrl的插件机制，方便不同产品和定制的需求。
*/
/*===============================================================================================================================*/
#endif
