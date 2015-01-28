/*************************************************************************
    > File Name: bvio.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月24日 星期六 16时44分07秒
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

#include <libbvutil/bvassert.h>

#include "bvio.h"
#include "bvurl.h"

#define BV_IO_BUFFER_SIZE (32768)   //32k

#define SHORT_SEEK_THRESHOLD 4096

static void *io_url_child_next(void *obj, void *prev)
{
    BVIOContext *s = obj;
    return prev ? NULL : s->opaque;
}

static const BVClass *io_url_child_class_next(const BVClass *prev)
{
    return prev ? NULL : &bv_url_context_class;
}

static const BVOption io_url_options[] = {
    { NULL },
};

const BVClass bv_io_class = {
    .class_name = "BVIOContext",
    .item_name  = bv_default_item_name,
    .version    = LIBBVUTIL_VERSION_INT,
    .option     = io_url_options,
    .child_next = io_url_child_next,
    .child_class_next = io_url_child_class_next,
};

static int io_reset_buf(BVIOContext *s, int flags)
{
    bv_assert1(flags == BV_IO_FLAG_WRITE || flags == BV_IO_FLAG_READ);

    if (flags & BV_IO_FLAG_WRITE) {
        s->buffer_end = s->buffer + s->buffer_size;
        s->write_flag = 1;
    } else {
        s->buffer_end = s->buffer;
        s->write_flag = 0;
    }
    return 0;
}

static int bv_io_init_context(BVIOContext *s, uint8_t *buffer, int buffer_size, int write_flag, void *opaque,
        int (*read)(void *opaque, uint8_t *buf, size_t size), 
        int (*write)(void *opaque, const uint8_t *buf, size_t size),
        int64_t (*seek)(void *opaque, int64_t offset, int whence), 
        int (*control)(void *opaque, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out)
        )
{
    s->buffer = buffer;
    s->orig_buffer_size = 
    s->buffer_size = buffer_size;
    s->buffer_ptr = buffer;
    s->opaque = opaque;
    s->direct = 0;

    io_reset_buf(s, write_flag ? BV_IO_FLAG_WRITE: BV_IO_FLAG_READ);
    s->io_read  = read;
    s->io_write = write;
    s->io_seek = seek;
    s->io_control = control;
    s->seek_able = seek ? BV_IO_SEEK_ABLE_NORMAL : 0;
    s->pos = 0;
    s->eof_reached = 0;
    s->must_flush = 0;
    s->max_packet_size = 0;
    s->update_checksum = NULL;

    if (!read && !write_flag) {
        s->pos = buffer_size;
        s->buffer_end = s->buffer + buffer_size;
    }
    return 0;
}

BVIOContext * bv_io_alloc_context(uint8_t *buffer, int buffer_size, int write_flag, void *opaque,
        int (*read)(void *opaque, uint8_t *buf, size_t size),
        int (*write)(void *opaque, const uint8_t *buf, size_t size),
        int64_t (*seek)(void *opaque, int64_t offset, int whence),
        int (*control)(void *opaque, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out)
        )
{
    BVIOContext *s = bv_mallocz(sizeof(BVIOContext));
    if (!s)
        return NULL;
    bv_io_init_context(s, buffer, buffer_size, write_flag, opaque, read, write, seek, control);
    return s;
}

static int bv_io_fdopen(BVIOContext **s, BVURLContext *h)
{
    uint8_t *buffer;
    BVIOContext *is;
    int buffer_size, max_packet_size;
    max_packet_size = h->max_packet_size;
    if (max_packet_size) {
        buffer_size = max_packet_size;
    } else {
        buffer_size = BV_IO_BUFFER_SIZE;
    }

    buffer = bv_malloc(buffer_size);
    if (!buffer)
        return BVERROR(ENOMEM);
    is = bv_io_alloc_context(buffer, buffer_size, h->flags & BV_IO_FLAG_WRITE, h, (void *)bv_url_read, (void *) bv_url_write, (void *)bv_url_seek, (void *)bv_url_control);

    if (!is) {
        bv_freep(buffer);
        return BVERROR(ENOMEM);
    }
    is->seek_able = h->is_streamed ? 0 : BV_IO_SEEK_ABLE_NORMAL;
    is->max_packet_size = max_packet_size;
    is->bv_class = &bv_io_class;
    *s = is;
    return 0;
}

int bv_io_open(BVIOContext **s, const char *filename, int flags, const BVIOInterruptCB *int_cb, BVDictionary **options)
{
    BVURLContext *h = NULL;
    int ret;
    ret = bv_url_open(&h, filename, flags, int_cb, options);
    if (ret < 0)
        return ret;
    ret = bv_io_fdopen(s, h);
    if (ret < 0) {
        bv_url_close(h);
        return ret;
    }
    return ret;
}

static void write_out(BVIOContext *s, const uint8_t *data, int len)
{
    if (s->write_flag && !s->error) {
        int ret = s->io_write(s->opaque, data, len);
        if (ret < 0) {
            s->error = ret;
        }
    }
    s->writeout_counts ++;
    s->pos += len;
}

static void fill_buffer(BVIOContext *s)
{
    int max_buffer_size = s->max_packet_size ? s->max_packet_size : BV_IO_BUFFER_SIZE;
    uint8_t *dst = s->buffer_end - s->buffer + max_buffer_size < s->buffer_size ? s->buffer_end : s->buffer;
    int len = s->buffer_size - (dst - s->buffer);
    if (!s->io_read && s->buffer_ptr >= s->buffer_end)
        s->eof_reached = 1;
    if (s->eof_reached)
        return;
    if (s->update_checksum && dst == s->buffer) {
        if (s->buffer_end > s->checksum_ptr) {
            s->checksum = s->update_checksum(s->checksum, s->checksum_ptr, s->buffer_end - s->checksum_ptr);
            s->checksum_ptr = s->buffer;
        }
    }

    if (s->io_read && s->orig_buffer_size && s->buffer_size > s->orig_buffer_size) {
        if (dst == s->buffer) {
            bv_io_set_buffer_size(s, s->orig_buffer_size);
            s->checksum_ptr = dst = s->buffer;
        }
        bv_assert0(len >= s->orig_buffer_size);
    }

    if (s->io_read)
        len = s->io_read(s->opaque, dst, len);
    else
        len = 0;
    if (len <= 0) {
        s->eof_reached = 1;
        if (len < 0)
            s->error = len;
    } else {
        s->pos += len;
        s->buffer_ptr = dst;
        s->buffer_end = dst + len;
        s->bytes_read += len;
    }
    return;
}

static void flush_buffer(BVIOContext *s)
{
    if (s->write_flag && s->buffer_ptr > s->buffer) {
        write_out(s, s->buffer, s->buffer_ptr - s->buffer);
        if (s->update_checksum) {
            s->checksum = s->update_checksum(s->checksum, s->checksum_ptr, s->buffer_ptr - s->checksum_ptr);
            s->checksum_ptr = s->buffer;
        }
    }
    s->buffer_ptr = s->buffer;
    if (!s->write_flag) {
        s->buffer_end = s->buffer;
    }
}

int bv_io_write(BVIOContext *s, const uint8_t *buffer, size_t size)
{
    size_t lsize = size;
    //写buf 若满flush
    if (s->direct && !s->update_checksum) {
        bv_io_flush(s);
        write_out(s, buffer, size);
        return size;
    }

    while (size > 0) {
        int len = BBMIN(s->buffer_end - s->buffer_ptr, size);
        memcpy(s->buffer_ptr, buffer, len);
        s->buffer_ptr += len;

        if (s->buffer_ptr >= s->buffer_end)
            flush_buffer(s);
        buffer += len;
        size -= len;
    }
    return lsize;
}

int bv_io_read(BVIOContext *s, uint8_t *buffer, size_t size)
{
    int len, lsize;
    lsize = size;
    while (size > 0) {
        len = s->buffer_end - s->buffer_ptr;
        if (len > size)
            len = size;
        if (len == 0 || s->write_flag) {
            if ((s->direct || size > s->buffer_size) && !s->update_checksum) {
                if (s->io_read)
                    len = s->io_read(s->opaque, buffer, size);
                if (len <= 0) {
                    s->eof_reached = 1;
                    if (len < 0)
                        s->error = len;
                    break;
                } else {
                    s->pos += len;
                    s->bytes_read += len;
                    size -= len;
                    buffer += len;
                    s->buffer_ptr = s->buffer;
                    s->buffer_end = s->buffer;
                }
            } else {
                fill_buffer(s);
                len = s->buffer_end - s->buffer_ptr;
                if (len == 0)
                    break;
            }
        } else {
            memcpy(buffer, s->buffer_ptr, len);
            buffer += len;
            s->buffer_ptr += len;
            size -= len;
        }
    }

    if (lsize == size) {
        if (s->error)
            return s->error;
        if (bv_io_feof(s))
            return BVERROR_EOF;
    }
    return lsize - size;
}

int bv_io_feof(BVIOContext *s)
{
    if (!s)
        return 0;
    if (s->eof_reached) {
        s->eof_reached = 0;
        fill_buffer(s);
    }
    return s->eof_reached;
}

void bv_io_flush(BVIOContext *s)
{
    flush_buffer(s);
    s->must_flush = 0;
    return ;
}

int bv_io_close(BVIOContext *s)
{
    BVURLContext *h;
    if (!s)
        return 0;
    h = s->opaque;
    bv_io_flush(s);
    bv_freep(&s->buffer);
    if (s->write_flag)
        bv_log(s, BV_LOG_DEBUG, "Statistics: %u seeks, %u writeouts\n", s->seek_counts, s->writeout_counts);
    else
        bv_log(s, BV_LOG_DEBUG, "Statistics: %"PRId64" bytes read %u seeks\n", s->bytes_read, s->seek_counts);
    bv_free(s);
    return bv_url_close(h);
}

int bv_io_closep(BVIOContext **s)
{
    BVIOContext *h;
    if (!s || !*s)
        return 0;
    h = *s;
    *s = NULL;
    return bv_io_close(h);
}

int bv_io_set_buffer_size(BVIOContext *s, int buf_size)
{
    uint8_t *buffer;
    buffer = bv_malloc(buf_size);
    if (!buffer)
        return BVERROR(ENOMEM);
    bv_free(s->buffer);
    s->buffer = buffer;
    s->orig_buffer_size =
    s->buffer_size = buf_size;
    s->buffer_ptr = buffer;
    io_reset_buf(s, s->write_flag ? BV_IO_FLAG_WRITE: BV_IO_FLAG_READ);
    return 0;
}

int64_t bv_io_seek(BVIOContext *s, int64_t offset, int whence)
{
    int64_t offset1;
    int64_t pos;
    int force = whence & BV_IO_SEEK_FORCE;
    int buffer_size;
    whence &= ~BV_IO_SEEK_FORCE;

    if(!s)
        return BVERROR(EINVAL);

    buffer_size = s->buffer_end - s->buffer;
    pos = s->pos - (s->write_flag ? 0 : buffer_size);

    if (whence != SEEK_CUR && whence != SEEK_SET)
        return BVERROR(EINVAL);

    if (whence == SEEK_CUR) {
        offset1 = pos + (s->buffer_ptr - s->buffer);
        if (offset == 0)
            return offset1;
        offset += offset1;
    }
    if (offset < 0)
        return BVERROR(EINVAL);

    offset1 = offset - pos;
    if (!s->must_flush && (!s->direct || !s->io_seek) &&
        offset1 >= 0 && offset1 <= buffer_size) {
        /* can do the seek inside the buffer */
        s->buffer_ptr = s->buffer + offset1;
    } else if ((!s->seek_able ||
               offset1 <= s->buffer_end + SHORT_SEEK_THRESHOLD - s->buffer) &&
               !s->write_flag && offset1 >= 0 &&
               (!s->direct || !s->io_seek) &&
              (whence != SEEK_END || force)) {
        while(s->pos < offset && !s->eof_reached)
            fill_buffer(s);
        if (s->eof_reached)
            return BVERROR_EOF;
        s->buffer_ptr = s->buffer_end + offset - s->pos;
    } else if(!s->write_flag && offset1 < 0 && -offset1 < buffer_size>>1 && s->io_seek && offset > 0) {
        int64_t res;

        pos -= BBMIN(buffer_size>>1, pos);
        if ((res = s->io_seek(s->opaque, pos, SEEK_SET)) < 0)
            return res;
        s->buffer_end =
        s->buffer_ptr = s->buffer;
        s->pos = pos;
        s->eof_reached = 0;
        fill_buffer(s);
        return bv_io_seek(s, offset, SEEK_SET | force);
    } else {
        int64_t res;
        if (s->write_flag) {
            flush_buffer(s);
            s->must_flush = 1;
        }
        if (!s->io_seek)
            return BVERROR(EPIPE);
        if ((res = s->io_seek(s->opaque, offset, SEEK_SET)) < 0)
            return res;
        s->seek_counts ++;
        if (!s->write_flag)
            s->buffer_end = s->buffer;
        s->buffer_ptr = s->buffer;
        s->pos = offset;
    }
    s->eof_reached = 0;
    return offset;
}

int bv_io_control(BVIOContext *s, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    if (!s)
        return BVERROR(EINVAL);
    if (!s->io_control)
        return BVERROR(ENOSYS);
    return s->io_control(s, type, pkt_in, pkt_out);
}

void bv_io_w8(BVIOContext *s, uint8_t val)
{
    *s->buffer_ptr++ = val;
    if (s->buffer_ptr >= s->buffer_end)
        flush_buffer(s);
    return;
}
void bv_io_wl16(BVIOContext *s, uint16_t val)
{
    bv_io_w8(s, val >> 8);
    bv_io_w8(s, (uint8_t)val);
}

void bv_io_wb16(BVIOContext *s, uint16_t val)
{
    bv_io_w8(s, (uint8_t)val);
    bv_io_w8(s, val >> 8);
}

void bv_io_wl24(BVIOContext *s, uint32_t val)
{
    bv_io_wl16(s, val & 0xffff);
    bv_io_w8(s, (uint8_t) val >> 16);
}

void bv_io_wb24(BVIOContext *s, uint32_t val)
{
    bv_io_wb16(s, (uint16_t)(val >>8));
    bv_io_w8(s, (uint8_t)val);
}

void bv_io_wl32(BVIOContext *s, uint32_t val)
{
    bv_io_w8(s, (uint8_t)val);
    bv_io_w8(s, (uint8_t)(val >> 8));
    bv_io_w8(s, (uint8_t)(val >> 16));
    bv_io_w8(s, (uint8_t)(val >> 24));
}
void bv_io_wb32(BVIOContext *s, uint32_t val)
{
    bv_io_w8(s,           val >> 24 );
    bv_io_w8(s, (uint8_t)(val >> 16));
    bv_io_w8(s, (uint8_t)(val >> 8 ));
    bv_io_w8(s, (uint8_t) val       );
}
void bv_io_wl64(BVIOContext *s, uint64_t val)
{
    bv_io_wl32(s, (uint32_t)(val & 0xffffffff));
    bv_io_wl32(s, (uint32_t)(val >> 32));
}

void bv_io_wb64(BVIOContext *s, uint64_t val)
{
    bv_io_wb32(s, (uint32_t)(val >> 32));
    bv_io_wb32(s, (uint32_t)(val & 0xffffffff));
}

uint8_t bv_io_r8(BVIOContext *s)
{
    if (s->buffer_ptr >= s->buffer_end)
        fill_buffer(s);
    if (s->buffer_ptr < s->buffer_end)
        return *s->buffer_ptr++;
    return 0;
}
uint16_t bv_io_rl16(BVIOContext *s)
{
    uint16_t val;
    val = bv_io_r8(s);
    val |= bv_io_r8(s) << 8;
    return val;
}
uint32_t bv_io_rl24(BVIOContext *s)
{
    uint32_t val;
    val = bv_io_rl16(s);
    val |= bv_io_r8(s) << 16;
    return val;
}
uint32_t bv_io_rl32(BVIOContext *s)
{
    uint32_t val;
    val = bv_io_rl16(s);
    val |= bv_io_rl16(s) << 16;
    return val;
}
uint64_t bv_io_rl64(BVIOContext *s)
{
    uint64_t val;
    val = (uint64_t)bv_io_rl32(s);
    val |= (uint64_t)bv_io_rl32(s) << 32;
    return val;
}

uint16_t bv_io_rb16(BVIOContext *s)
{
    uint16_t val;
    val = bv_io_r8(s) << 8;
    val |= bv_io_r8(s);
    return val;

}
uint32_t bv_io_rb24(BVIOContext *s)
{
    uint32_t val;
    val = bv_io_rb16(s) << 8;
    val |= bv_io_r8(s);
    return val;
}
uint32_t bv_io_rb32(BVIOContext *s)
{
    uint32_t val;
    val = bv_io_rb16(s) << 16;
    val |= bv_io_rb16(s);
    return val;
}
uint64_t bv_io_rb64(BVIOContext *s)
{
    uint64_t val;
    val = (uint64_t)bv_io_rb32(s) << 32;
    val |= (uint64_t)bv_io_rb32(s);
    return val;
}

