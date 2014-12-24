/**
 *          File: bvpustream.c
 *
 *        Create: 2014年12月16日 星期二 14时24分57秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
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

#include <stdio.h>

#include <unistd.h>

//
// Get stream and set stream and encoder/decoder stream
// All here.
// TODO:
// 1. If set need return stream callback, call callback function to return the stream. (user need stream do other thing)
// 2. If set get stream callback, call callback function to get stream, (user implement get stream).
// 3. If return stream didn't set, check daytimeslice and do transfer to our server.
// 4. If get stream callback didn't set, get stream from lower bvpusdk.
// 5. Provide function to encoder/decoder, with params of inputs and outputs.
// 6. Provide function to config videoin/videoout/audioin/audioout.
// 7. Provide function to config stream with diff channel.
// 8. Get stream, return stream, transfer with protocol though thread.
//


#include "bvpustream.h"

typedef struct __st_BVPU_Stream {
    int          bExit;
    int          iCommand;
} BVPU_Stream;

static BVPU_Stream  bvpu_in_stream;
static BVPU_Stream *inStream = &bvpu_in_stream;
/////////////////////////////////////////////////////////////////
///// Encoder and Decoder
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//// Stream operation
////////////////////////////////////////////////////////////////

// Operation of stream with @iComman,
// @data: the data with @iCommand.
int bvpu_stream_control(int iCommand, void *data)
{
    if (iCommand <= 0 || !data) return BVPU_SDK_RESULT_E_INVALIDPARAM;
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    BVPU_StreamCommand *streamCmd = (BVPU_StreamCommand*)data;

    switch (iCommand) {
    }

    return result;
}

static void bvpu_stream_thread(void *priv)
{
    while(!inStream->bExit) {
        switch(inStream->iCommand) {
            // With command
            default: break;
        }

        usleep(40000);
    }
}

int bvpu_stream_init(BVPU_StreamParam *stStreamParam)
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // From Param 
    // Init with module libstream


    // TODO: create thread to do something.    


    return result;
}

int bvpu_stream_deinit()
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;

    // call deinit for self
    // and libstream module to deinit something.

    return result;
}


/*=============== End of file: bvpustream.c ==========================*/
