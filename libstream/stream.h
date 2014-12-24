/*************************************************************************
	> File Name: stream.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年11月11日 星期二 18时11分58秒
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
 * Copyright (C) albert@BesoVideo, 2014
 */

#ifndef BV_STREAM_H
#define BV_STREAM_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libconfig/common.h>

#define BV_PKT_FLAG_KEY     0x0001 ///< The packet contains a keyframe
#define BV_PKT_FLAG_CORRUPT 0x0002 ///< The packet content is corrupted

typedef struct _BVAVPacket {
	uint64_t pts;
	uint64_t dts;
	uint8_t *data;
	uint32_t size;
	enum BVMediaType type;
	int      flags;			//BV_PKT_FLAG
} BVAVPacket;

typedef struct _BVSAVPacket {
	int nb_pkts;
	BVAVPacket **pkts;
} BVSAVPacket;

typedef struct _BVAVFrame {
#define AV_NUM_DATA_POINTERS 8
	uint8_t *data[AV_NUM_DATA_POINTERS];
	int linesize[AV_NUM_DATA_POINTERS];
	int width, height;

	int nb_samples;
	int pts;
	int key_frame;
} BVAVFrame;

typedef struct _BVSStream BVSStream;

typedef struct BVFormatContext BVFormatContext;

/**
 * @Synopsis  初始化ffmpeg环境
 */
void bv_stream_init();

/**
 * @Synopsis  去初始化ffmpeg
 */
void bv_stream_deinit();
/**
 * @Synopsis  打开一个流，可以是从编码器中，也可以是从文件中
 *
 * @Param stream  保存获取的stream结构体
 * @Param channel 编码通道
 * @Param uri 要打开的文件
 * @Param flags 1 输入流 2 
 *
 * @Returns   成功返回BVFormatContext Handle 失败返回NULL
 */
//bvfs://xxxx.dav/mkv
int bv_format_open_input(BVFormatContext **fmtctx, const BVEncodeChannel *channel, const char *uri);

void bv_spacket_init(BVSAVPacket *spkt);

void bv_spacket_free(BVSAVPacket *spkt);

int bv_format_read(BVFormatContext *fmtctx, BVSAVPacket *spkt);

int bv_format_close_input(BVFormatContext **fmtctx);


void bv_sstream_init(BVSStream **sstream);
BVStream * bv_new_stream(BVSStream *sstream, BVCodecContext *codec);

void bv_sstream_free(BVSStream **sstream);
int bv_format_open_output(BVFormatContext **fmtctx, void * decchn, const char *uri, const BVSStream *sstream);

int bv_format_write(BVFormatContext *fmtctx, BVSAVPacket *spkt);

int bv_format_close_output(BVFormatContext **fmtctx);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_STREAM_H */

