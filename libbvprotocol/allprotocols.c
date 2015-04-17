/*************************************************************************
    > File Name: allprotocols.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年09月25日 星期四 15时51分48秒
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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvurl.h"
#include "internal.h"
#include <config.h>

#if BV_CONFIG_LIBBVFS
#include <bvfs.h>
#endif

#define REGISTER_PROTOCOL(X, x)                                         \
    {                                                                   \
        extern BVURLProtocol bv_##x##_protocol;                           \
        if (BV_CONFIG_##X##_PROTOCOL)                                      \
            bv_url_register_protocol(&bv_##x##_protocol);                \
    }

void bv_protocol_register_all(void)
{
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    REGISTER_PROTOCOL(FILE, file);
    REGISTER_PROTOCOL(TCP, tcp);
    REGISTER_PROTOCOL(UDP, udp);
    REGISTER_PROTOCOL(BVFS, bvfs);
    REGISTER_PROTOCOL(HTTP, http);
    REGISTER_PROTOCOL(HTTPS, https);
    REGISTER_PROTOCOL(HTTPPROXY, httpproxy);
#if BV_CONFIG_LIBBVFS
    bvfs_init(1, 0);
#endif
}
