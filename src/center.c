/**
 *          File: center.c
 *
 *        Create: 2014年12月09日 星期二 11时39分42秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>

#include "BVPU_SDK.h"

/////////////////////////////////////////////////////////////////////////////
// section - base operation of sdk
/////////////////////////////////////////////////////////////////////////////

LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Init(BVPU_SDK_GlobalParam *pstGlobalParam)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // TODO: Init every thing.
    // Read config .
    // Start log print.
    // Stream, record, peripheral, time, ...
    // Call the function of each module.

    return result;
}


LIBBVPU_SDK_API	BVPU_SDK_Result BVPU_SDK_Deinit()
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // Deinit everything.
    // Stream, record, peripheral, time, ...
    // Call the function of each module.

    return result;
}

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SendCommand(BVPU_SDK_Command iCommand, 
                                BVPU_SDK_Subcommand iSubCommand, 
                                void *pInBuffer, int iInBufferSize, 
                                void *pOutBuffer, int *pOutBuffeSize)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // Each iCommand and iSubcomand
    // To call each function of module.
    switch (iCommand) {
    case BVPU_SDK_COMMAND_CONTROL:
        switch (iSubCommand) {
        case BVPU_SDK_SUBCOMMAND_AVCHANNEL_DEVICE_INFO:
        // TODO: config device info with channel, program must remember this info.
        // Set to config and remember it.
        break;
        }
    break;
    case BVPU_SDK_COMMAND_QUERY:
        switch (iSubCommand) {
        case BVPU_SDK_SUBCOMMAND_CAPABILITIES_DEVICE:
        break;
        case BVPU_SDK_SUBCOMMAND_CAPABILITIES_PTZ:
        break;
        case BVPU_SDK_SUBCOMMAND_DEVICE_INFO:
        break;
        case BVPU_SDK_SUBCOMMAND_SEARCH_DEVICE:
        break;
        }
    break;
    case BVPU_SDK_COMMAND_SUBSCRIBE:
    break;

    default: result = BVPU_SDK_RESULT_E_UNSUPPORTED;
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////
// section - Media operation
// media of stream get and set call from stream module.
/////////////////////////////////////////////////////////////////////////////

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Open(BVPU_SDK_HMediaStreamHandle* phMediaStreamHandle, 
                BVPU_SDK_MediaStreamParam *pstStreamParam)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // Call module stream to ready everything.

    return result;
}

//LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, 
 //               AVCodecContext *pstAVCodecContext, AVPacket *pstAVPacket);
LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_WriteMediaData(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, 
                                BVPU_SDK_MediaStreamData *pMediaStreamData)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    return result;
}

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Command(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle, 
			    BVPU_SDK_MediaStreamCommand iCommand, BVPU_SDK_MediaStreamSubcommand iSubCommand, 
				void *pInBuffer, int iInBufferSize, 
			    void *pOutBuffer, int* pOutBufferSize)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // Change config of encoder when need.
    switch (iCommand) {
    case BVPU_SDK_MEDIASTREAM_COMMAND_CONTROL:
        switch (iSubCommand) {
        default: result = BVPU_SDK_RESULT_E_UNSUPPORTED;
        }
    break;
    case BVPU_SDK_MEDIASTREAM_COMMAND_QUERY:
    break;

    default: result = BVPU_SDK_RESULT_E_UNSUPPORTED;
    }

    return result;
}

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_MediaStream_Close(BVPU_SDK_HMediaStreamHandle hMediaStreamHandle)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    return result;
}

LIBBVPU_SDK_API BVPU_SDK_Result BVPU_SDK_SetHardwareInterface(BVPU_SDK_HardwareDeviceInterface *pstHardwareDeviceInterface)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    BVPU_SDK_HardwareDeviceType hdType = pstHardwareDeviceInterface->iHardwareType;
    switch (hdType) {
        case BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_VIDEOIN ... BVPU_SDK_HARDWARE_DEVICETYPE_ANALOG_AUDIOOUT:
        break;
        case BVPU_SDK_HARDWARE_DEVICETYPE_IP_VIDEOIN ... BVPU_SDK_HARDWARE_DEVICETYPE_IP_AUDIOOUT:
        // TODO: send info to peripheral for outside control of ip video / audio control.
        // recover the control of inner implement. (It will be set when init function be called)
        break;

        default: result = BVPU_SDK_RESULT_E_UNSUPPORTED;
    }

    return result;
}

/*=============== End of file: center.c ==========================*/
