/*************************************************************************
    > File Name: sdp.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月21日 星期二 14时58分13秒
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

#ifndef BVPROTOCOL_SDP_H
#define BVPROTOCOL_SDP_H

/**
 *  TODO
 *  Session Description Protocol
 *  用来解析获取的SDP，或者封装SDP
 */
typedef struct _SDPOrigin {
    char *username;
    char *session_id;
    char *session_version;
    char *nettype;
    char *addrtype;
    char *address;
} SDPOrigin;

typedef struct _SDPMedia {
    char *media;
    char *port;
    char *num_of_port;
    char *proto;
} SDPMedia;

typedef struct _BVSDP {
    int version;    //0
    SDPOrigin origin;
} BVSDP;

BVSDP *bb_sdp_protoc_packed()
#endif /* end of include guard: BVPROTOCOL_SDP_H */
