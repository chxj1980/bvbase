/*************************************************************************
    > File Name: packet.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 21时35分16秒
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

#ifndef BV_PACKET_H
#define BV_PACKET_H

#ifdef __cplusplus
extern "C"{
#endif

#include "buffer.h"

/**
 * @ingroup lavc_decoding
 * Required number of additionally allocated bytes at the end of the input bitstream for decoding.
 * This is mainly needed because some optimized bitstream readers read
 * 32 or 64 bit at once and could read over the end.<br>
 * Note: If the first 23 bits of the additional bytes are not 0, then damaged
 * MPEG bitstreams could cause overread and segfault.
 */
#define BV_INPUT_BUFFER_PADDING_SIZE 32

/**
 * @ingroup lavc_encoding
 * minimum encoding buffer size
 * Used to avoid some checks during header writing.
 */
#define BV_MIN_BUFFER_SIZE 16384


#define BV_PKT_FLAG_KEY 0x0001
#define BV_PKT_FLAG_CORRUPT 0x0002
#define BV_PKT_FLAG_RAWDATA 0x0004

typedef struct _BVPacket {
    BVBufferRef *buf;
    int stream_index;
    int64_t pts;
    int64_t dts;
    uint8_t *data;
    int size;
    int flags;
} BVPacket;

typedef struct _BVControlPacket {
    int size;
    int flags;
    void *data;
} BVControlPacket;

int bv_packet_init(BVPacket *pkt);

int bv_packet_new(BVPacket *pkt, int size);

int bv_packet_copy(BVPacket *dst, const BVPacket *src);

void bv_packet_free(BVPacket *pkt);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_PACKET_H */

