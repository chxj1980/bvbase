/*************************************************************************
    > File Name: bvio.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 17时47分52秒
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

#ifndef BV_IO_H
#define BV_IO_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/packet.h>
#include <libbvutil/log.h>
#include <libbvutil/dict.h>

typedef struct _BVIOContext {
    const BVClass *bv_class;
    void *opaque;
    int direct;
    uint8_t *buffer;
    int orig_buffer_size;
    int buffer_size;
    uint8_t *buffer_ptr;
    uint8_t *buffer_end;
    int (*io_read)(void *opaque, uint8_t *buffer, size_t size);
    int (*io_write)(void *opaque, const uint8_t *buffer, size_t size);
    int64_t(*io_seek)(void *opaque, int64_t offset, int whence);
    int (*io_control)(void *opaque, int type, BVControlPacket *in, BVControlPacket *out);
    uint64_t pos;
    int eof_reached;
    int write_flag;
    int max_packet_size;
    int seek_able;
    int must_flush;
    uint32_t writeout_counts;
    uint32_t seek_counts;
    int64_t  bytes_read;
    uint32_t checksum;
    uint8_t *checksum_ptr;
    uint32_t (*update_checksum)(uint32_t checksum, const uint8_t *buf, size_t size);
    int error;
} BVIOContext;

typedef struct BVIOInterruptCB {
    int (*callback)(void*);
    void *opaque;
} BVIOInterruptCB;

#define BV_IO_FLAG_READ     1
#define BV_IO_FLAG_WRITE    2
#define BV_IO_FLAG_READ_WRITE   (BV_IO_FLAG_READ | BV_IO_FLAG_WRITE)

#define BV_IO_FLAG_NONBLOCK 8

#define BV_IO_SEEK_ABLE_NORMAL 0x0001
#define BV_IO_SEEK_FORCE    (32)

BVIOContext * bv_io_alloc_context(uint8_t *buffer, int buffer_size, int write_flag, void *opaque,
        int (*read)(void *opaque, uint8_t *buf, size_t size),
        int (*write)(void *opaque, const uint8_t *buf, size_t size),
        int64_t (*seek)(void *opaque, int64_t offset, int whence),
        int (*control)(void *opaque, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out)
        );

int bv_io_open(BVIOContext **s, const char *filename, int flags, const BVIOInterruptCB *int_cb, BVDictionary **options);

int bv_io_write(BVIOContext *s, const uint8_t *buffer, size_t size);

int bv_io_read(BVIOContext *s, uint8_t *buffer, size_t size);

int64_t bv_io_seek(BVIOContext *s, int64_t offset, int whence);

int bv_io_control(BVIOContext *s, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out);

void bv_io_flush(BVIOContext *s);

int bv_io_feof(BVIOContext *s);

int bv_io_close(BVIOContext *s);

int bv_io_closep(BVIOContext **s);

int bv_io_set_buffer_size(BVIOContext *s, int buf_size);

void bv_io_w8(BVIOContext *s, uint8_t val);
void bv_io_wl16(BVIOContext *s, uint16_t val);
void bv_io_wl24(BVIOContext *s, uint32_t val);
void bv_io_wl32(BVIOContext *s, uint32_t val);
void bv_io_wl64(BVIOContext *s, uint64_t val);

void bv_io_wb16(BVIOContext *s, uint16_t val);
void bv_io_wb24(BVIOContext *s, uint32_t val);
void bv_io_wb32(BVIOContext *s, uint32_t val);
void bv_io_wb64(BVIOContext *s, uint64_t val);

uint8_t bv_io_r8(BVIOContext *s);
uint16_t bv_io_rl16(BVIOContext *s);
uint32_t bv_io_rl24(BVIOContext *s);
uint32_t bv_io_rl32(BVIOContext *s);
uint64_t bv_io_rl64(BVIOContext *s);

uint16_t bv_io_rb16(BVIOContext *s);
uint32_t bv_io_rb24(BVIOContext *s);
uint32_t bv_io_rb32(BVIOContext *s);
uint64_t bv_io_rb64(BVIOContext *s);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_IO_H */
