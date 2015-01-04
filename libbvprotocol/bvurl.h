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

typedef struct _BVURLContext {
    BVClass *bv_class;
    struct _BVURLProtocol *prot;
	void *priv_data;
    char *filename;
} BVURLContext;

typedef struct _BVURLProtocol {
    const char *name;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVURLProtocol *next;
    int (*url_open)(BVURLContext *h, const char *url, int flags);
    int (*url_read)(BVURLContext *h, void *buf, size_t size);
    int (*url_write)(BVURLContext *h, const void *buf, size_t size);
    int (*url_seek)(BVURLContext *h, int64_t pos, int whence);
    int (*url_control)(BVURLContext *h, int type, BVControlPacket *in, BVControlPacket *out);
    int (*url_fd)(BVURLContext *h);
    int (*url_check)(BVURLContext *h, int mask);
    int (*url_close)(BVURLContext *h);
} BVURLProtocol;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_URL_H */
