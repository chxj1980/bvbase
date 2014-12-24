/**
 *          File: bvpustream.h
 *
 *        Create: 2014年12月16日 星期二 14时26分53秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
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
 * Copyright (C) @ BesoVideo, 2014
 */

#ifndef __BVPU_STREAM_H
#define __BVPU_STREAM_H

#include "BVPU_SDK_DataTypes.h"

///
// Stream Data define see
// BVPU_SDK_MediaStreamData;

typedef struct __st_BVPU_StreamParam {
    // Call back of read stream from
    // inner complete if set to null.
    // 如果设置，则从此函数或取音视频数据
    // 如果未设置，则内部自已取音视频
    BVPU_SDK_Result  (*bvpu_read_stream)(void *priv, BVPU_SDK_MediaStreamData *stMediaStreamData);

    // Call back of get stream.
    // inner complete to deal if set to null.
    // 如果设置，会调此函数返回音视频数据
    // 如果未设置，内部流程处理视频数据
    BVPU_SDK_Result (*bvpu_return_stream)(void *priv, BVPU_SDK_MediaStreamData *stMediaStreamData);

    void *priv;  // private data of caller.

} BVPU_StreamParam;

enum BVPU_STREAM_COMMAND {
    BVPU_STREAM_COMMAND_ENCODER_START = 1,
    BVPU_STREAM_COMMAND_ENCODER_STOP,
    BVPU_STREAM_COMMAND_ENCODER_CONFIG, // BVPU_SDK_EncoderSreamParam
    BVPU_STREAM_COMMAND_ENCODER_DYNAMIC, // BVPU_SDK_MediaStreamVideoEncoderDynamicParam
    BVPU_STREAM_COMMAND_VIDEOIN_COLOR, // BVPU_SDK_MediaStreamVideoInColorParam

    BVPU_STREAM_COMMAND_AUDIOIN = 0x50,  // BVPU_SDK_AudioEncoderParam
    BVPU_STREAM_COMMAND_AUDIOOUT,
    BVPU_STREAM_COMMAND_VIDEOOUT,   // BVPU_SDK_VideoOut
    BVPU_STREAM_COMMAND_LIVEVIDEO,
    BVPU_STREAM_COMMAND_LIVEVIDEO_START,

    BVPU_STREAM_COMMAND_SNAPSHOT_CONFIG = 0x100,  // BVPU_SDK_SnapshotParam
    BVPU_STREAM_COMMAND_SNAPSHOT_START,
    BVPU_STREAM_COMMAND_SNAPSHOT_STOP,
    BVPU_STREAM_COMMAND_SNAPSHOT_ONCE,
};

typedef struct __st_BVPU_StreamCommand {
    int           iChannel;   // which channel to control
    int           iAVType;    // BVPU_SDK_STREAM_TYPE

    // Diff structs with diff BVPU_STREAM_COMMAND
    void         *data;
} BVPU_StreamCommand;

////////////////////////////////////////////////////////////////
// Encoder and Decoder
////////////////////////////////////////////////////////////////

typedef struct tagBVPU_Stream_AVCodec *BVPU_Stream_AVCodec;

// 
// Init and deinit of avcodec.
BVPU_SDK_Result bvpu_stream_avcodec_open(BVPU_Stream_AVCodec *pAVCodec);
BVPU_SDK_Result bvpu_stream_avcodec_close(BVPU_Stream_AVCodec *pAVCodec);

// Encoder:
// Codec info is in BVPU_SDK_MediaStreamData with the real data.
BVPU_SDK_Result bvpu_stream_encoder(BVPU_Stream_AVCodec pAVCodec,
                BVPU_SDK_MediaStreamData *stMediaStreamDataIn,
                BVPU_SDK_MediaStreamData *stMediaStreamDataOut);

BVPU_SDK_Result bvpu_stream_decoder(BVPU_Stream_AVCodec pAVCodec,
                BVPU_SDK_MediaStreamData *stMediaStreamDataIn,
                BVPU_SDK_MediaStreamData *stMediaStreamDataOut);

#endif  // __BVPU_STREAM_H

/*=============== End of file: bvpustream.h =====================*/
