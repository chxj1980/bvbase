/*************************************************************************
    > File Name: dav.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月21日 星期三 15时24分35秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

/**
 *  dav file struct used by BesoVideo
 *  It's a simple file container 
 */

#ifndef BV_MEDIA_DAV_H
#define BV_MEDIA_DAV_H

#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
    FRAME_TYPE_I_SLICE = 0xFD,
    FRAME_TYPE_P_SLICE = 0xFC,
    FRAME_TYPE_B_SLICE = 0xFE,
    FRAME_TYPE_STATIC_IMAGE = 0xFB,
    FRAME_TYPE_AUDIO = 0xF0,
	FRAME_TYPE_GPS = 0xF2	//fangqi adds
}FrameType;

typedef enum
{
    FRAME_CHILD_TYPE = 0x00,
}FrameChildType;

typedef struct DateTime
{
    unsigned int second : 6;
    unsigned int minute : 6;
    unsigned int hour : 5;
    unsigned int day : 5;
    unsigned int month : 4;
    unsigned int year : 6;
}DateTime;

typedef struct FrameHeader
{
    char sHeaderFlag[4];
    unsigned char enFrameType;
    unsigned char enFrameChildType;
    unsigned char iChannel;
    unsigned char iChildFrameSeq;
    int iFrameSeq;
    int iFrameLen;
    
	DateTime stDateTime;

    unsigned short iMSTimeStamp;
    unsigned char iAddedDataLen;
    unsigned char u8Crc;
}FrameHeader;

typedef enum
{
    FILED_TYPE_ONLY_ONE = 0,
    FILED_TYPE_DOUBLE = 1,
    FILED_TYPE_SEPARATE = 2
}FiledType;

typedef enum
{
    VIDEO_ENCODE_MPEG4 = 1,
    VIDEO_ENCODE_H264 = 2,
}VideoEncodeType;

typedef enum
{
    AUDIO_ENCODE_PCM8 = 7,
    AUDIO_ENCODE_G729,
    AUDIO_ENCODE_IMA_ADPCM,
    AUDIO_ENCODE_G711U,
    AUDIO_ENCODE_G721,
    AUDIO_ENCODE_PCM8_VWIS,
    AUDIO_ENCODE_MS_ADPCM,
    AUDIO_ENCODE_G711A,
    AUDIO_ENCODE_PCM16,
    AUDIO_ENCODE_G726
}AudioEncodeType;

typedef enum
{
    SAMPLE_FREQ_4000 = 1,
    SAMPLE_FREQ_8000,
    SAMPLE_FREQ_11025,
    SAMPLE_FREQ_16000,
    SAMPLE_FREQ_20000,
    SAMPLE_FREQ_22050,
    SAMPLE_FREQ_32000,
    SAMPLE_FREQ_44100,
    SAMPLE_FREQ_48000,
}AudioSampleRate;

typedef struct IDRFrameAddedHeader
{
    struct ImageSize
    {
        unsigned char iFlag;
        unsigned char enFiledFlag;
        unsigned char iWidth;
        unsigned char iHeight;
    }stImageSize;

    struct PlayBackType
    {
        unsigned char iFlag;
        unsigned char iReserve;
        unsigned char enCodedType; //1-MPEG4, 2-H.264
        unsigned char iFps;
    }stPlayBackType;
}IDRFrameAddedHeader;

typedef struct AudioFrameAddHeader
{
    unsigned char iFlag;
    unsigned char iChannelCount; //1-single, 2-mono
    unsigned char enCodedType;
    unsigned char enSampleRate;
}AudioFrameAddHeader;


typedef struct FrameEnder
{
    char sEnderFlag[4];
    int iFrameLen;
}FrameEnder;

typedef struct Frame{
    FrameHeader stFrameHeader;
    IDRFrameAddedHeader stIDRFrameHeader;
    AudioFrameAddHeader stAudioFrameHeader;
    FrameEnder stFrameEnder;
}Frame;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MEDIA_DAV_H */
