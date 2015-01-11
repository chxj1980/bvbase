/*************************************************************************
    > File Name: bvurl.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 17时48分24秒
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

#ifndef BV_URL_H
#define BV_URL_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/packet.h>
#include <libbvutil/log.h>

struct _BVURLProtocol;
#include "bvio.h"
typedef struct _BVURLContext {
    const BVClass *bv_class;
    struct _BVURLProtocol *prot;
    void *priv_data;
    char *filename;
    BVIOInterruptCB interrupt_callback;
    int rw_timeout;
    int flags;
    int is_streamed;
    int is_connected;
    int max_packet_size;
} BVURLContext;
#define BV_URL_PROTOCOL_FLAG_NETWORK 0x01
#define BV_URL_PROTOCOL_FLAG_NESTED_SCHEME  0x02
#define BV_SEEK_SIZE    (INT_MIN)
#define BV_SEEK_FORCE    (32)
typedef struct _BVURLProtocol {
    const char *name;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVURLProtocol *next;
    int (*url_open)(BVURLContext *h, const char *url, int flags);
    int (*url_open2)(BVURLContext *h, const char *url, int flags, BVDictionary **options);
    int (*url_read)(BVURLContext *h, void *buf, size_t size);
    int (*url_write)(BVURLContext *h, const void *buf, size_t size);
    int64_t (*url_seek)(BVURLContext *h, int64_t pos, int whence);
    int (*url_control)(BVURLContext *h, int type, BVControlPacket *in, BVControlPacket *out);
    int (*url_get_file_handle)(BVURLContext *h);
    int (*url_get_multi_file_handle)(BVURLContext *h, int **handles, int *numhandles);
    int (*url_check)(BVURLContext *h, int mask);
    int (*url_shutdown)(BVURLContext *h, int flags);
    int (*url_close)(BVURLContext *h);
    int flags;
} BVURLProtocol;

BVURLProtocol *bv_url_protocol_next(const BVURLProtocol *prev);

const char *avio_enum_protocols(void **opaque, int output);


int bv_url_register_protocol(BVURLProtocol *protocol);

int bv_url_connect(BVURLContext *uc, BVDictionary **options);

int bv_url_alloc(BVURLContext **puc, const char *filename, int flags,
                const BVIOInterruptCB *int_cb);

int bv_url_open(BVURLContext **puc, const char *filename, int flags,
               const BVIOInterruptCB *int_cb, BVDictionary **options);


int bv_url_read(BVURLContext *h, void *buf, size_t size);
int bv_url_read_complete(BVURLContext *h, void *buf, size_t size);
int bv_url_write(BVURLContext *h, const void *buf, size_t size);
int64_t bv_url_seek(BVURLContext *h, int64_t pos, int whence);
int bv_url_closep(BVURLContext **hh);
int bv_url_close(BVURLContext *h);
const char *avio_find_protocol_name(const char *url);
int avio_check(const char *url, int flags);
int64_t bv_url_size(BVURLContext *h);
int bv_url_get_file_handle(BVURLContext *h);
int bv_url_get_multi_file_handle(BVURLContext *h, int **handles, int *numhandles);

int bv_url_shutdown(BVURLContext *h, int flags);

int bv_check_interrupt(BVIOInterruptCB *cb);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_URL_H */
