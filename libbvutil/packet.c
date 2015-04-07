/*************************************************************************
    > File Name: packet.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 21时47分18秒
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

#include "packet.h"
#include "bvutil.h"
#include "mem.h"

int bv_packet_init(BVPacket *pkt)
{
    pkt->pts = pkt->dts = BV_NOPTS_VALUE;
    pkt->flags = pkt->stream_index = 0;
    pkt->buf = NULL;
    pkt->data = NULL;
    pkt->size = 0;
    return 0;
}

static int packet_alloc(BVBufferRef **buf, int size)
{
    int ret;
    if ((unsigned)size >= (unsigned)size + BV_INPUT_BUFFER_PADDING_SIZE)
        return BVERROR(EINVAL);

    ret = bv_buffer_realloc(buf, size + BV_INPUT_BUFFER_PADDING_SIZE);
    if (ret < 0)
        return ret;

    memset((*buf)->data + size, 0, BV_INPUT_BUFFER_PADDING_SIZE);

    return 0;
}

int bv_packet_new(BVPacket *pkt, int size)
{
    BVBufferRef *buf = NULL;
    int ret = packet_alloc(&buf, size);
    if (ret < 0)
        return ret;

    bv_packet_init(pkt);
    pkt->buf      = buf;
    pkt->data     = buf->data;
    pkt->size     = size;
    return 0;
}

#define ALLOC_MALLOC(data, size) data = bv_malloc(size)
#define ALLOC_BUF(data, size)                \
do {                                         \
    bv_buffer_realloc(&pkt->buf, size);      \
    data = pkt->buf ? pkt->buf->data : NULL; \
} while (0)

#define DUP_DATA(dst, src, size, padding, ALLOC)                        \
    do {                                                                \
        void *data;                                                     \
        if (padding) {                                                  \
            if ((unsigned)(size) >                                      \
                (unsigned)(size) + BV_INPUT_BUFFER_PADDING_SIZE)        \
                goto failed_alloc;                                      \
            ALLOC(data, size + BV_INPUT_BUFFER_PADDING_SIZE);           \
        } else {                                                        \
            ALLOC(data, size);                                          \
        }                                                               \
        if (!data)                                                      \
            goto failed_alloc;                                          \
        memcpy(data, src, size);                                        \
        if (padding)                                                    \
            memset((uint8_t *)data + size, 0,                           \
                   BV_INPUT_BUFFER_PADDING_SIZE);                       \
        dst = data;                                                     \
    } while (0)

static int copy_packet_data(BVPacket *pkt, const BVPacket *src, int dup)
{
    pkt->data      = NULL;
   // pkt->side_data = NULL;
    if (pkt->buf) {
        BVBufferRef *ref = bv_buffer_ref(src->buf);
        if (!ref)
            return BVERROR(ENOMEM);
        pkt->buf  = ref;
        pkt->data = ref->data;
    } else {
        DUP_DATA(pkt->data, src->data, pkt->size, 1, ALLOC_BUF);
    }
#if 0
    if (pkt->side_data_elems && dup)
        pkt->side_data = src->side_data;
    if (pkt->side_data_elems && !dup) {
        return bv_copy_packet_side_data(pkt, src);
    }
#endif
    return 0;

failed_alloc:
    bv_packet_free(pkt);
    return BVERROR(ENOMEM);
}

int bv_packet_copy(BVPacket *dst, const BVPacket *src)
{
    *dst = *src;
    return copy_packet_data(dst, src, 0);
}

void bv_packet_free(BVPacket *pkt)
{
    if (!pkt)
        return;
    if (pkt->buf)
        bv_buffer_unref(&pkt->buf);
    pkt->size = 0;
    pkt->data = NULL;
    return;
}

