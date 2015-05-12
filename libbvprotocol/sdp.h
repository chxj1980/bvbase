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

#include <libbvutil/list.h>

/**
 *  TODO
 *  Session Description Protocol
 *  用来解析获取的SDP，或者封装SDP
 *  RFC2327
 */

/**
 * Session description
 *    v= (protocol version)
 *    o= (owner/creator and session identifier).
 *    s= (session name)
 *    i=* (session information)
 *    u=* (URI of description)
 *    e=* (email address)
 *    p=* (phone number)
 *    c=* (connection information - not required if included in all media)
 *    b=* (bandwidth information)
 *    One or more time descriptions (see below)
 *    z=* (time zone adjustments)
 *    k=* (encryption key)
 *    a=* (zero or more session attribute lines)
 *    Zero or more media descriptions (see below)
 *
 * Time description
 *    t= (time the session is active)
 *    r=* (zero or more repeat times)
 *
 * Media description
 *    m= (media name and transport address)
 *    i=* (media title)
 *    c=* (connection information - optional if included at session-level)
 *    b=* (bandwidth information)
 *    k=* (encryption key)
 *    a=* (zero or more media attribute lines)
 *
 * An example SDP description is:
 *    v=0
 *    o=mhandley 2890844526 2890842807 IN IP4 126.16.64.4
 *    s=SDP Seminar
 *    i=A Seminar on the session description protocol
 *    u=http://www.cs.ucl.ac.uk/staff/M.Handley/sdp.03.ps
 *    e=mjh@isi.edu (Mark Handley)
 *    c=IN IP4 224.2.17.12/127
 *    Handley & Jacobson Standards Track [Page 8]
 *    RFC 2327 SDP April 1998
 *    t=2873397496 2873404696
 *    a=recvonly
 *    m=audio 49170 RTP/AVP 0
 *    m=video 51372 RTP/AVP 31
 *    m=application 32416 udp wb
 *    a=orient:portrait
 */

enum BVNetAddressType {
    BV_NET_ADDRESS_TYPE_IPV4 = 0,
    BV_NET_ADDRESS_TYPE_IPV6,
};

/**
 * o=<username> <session id> <version> <network type> <address type> <address>
 * o=albert 2899938405 2899938443 IN IP4 192.168.6.147
 */
typedef struct _BVSDPOrigin {
    char *username;
    char *session_id;
    char *version;
    char *network_type;     //IN
    char *address_type;
    char *address;
} BVSDPOrigin;

/**
 * c=<network type> <address type> <connect address>
 * c=IN IP4 224.2.17.12/127
 */
typedef struct _BVSDPConnection {
    char *network_type;
    char *address_type;
    char *connect_addr;
    char *addr_multicast_ttl;
    char *addr_multicast_int;
} BVSDPConnection;

/**
 * b=<modifier>:<bandwidth-value>
 * b=X-YZ:128
 */
typedef struct _BVSDPBandwidth {
    char *bwtype;       //bandwidth type
    char *bandwidth;    //bandwidth value
} BVSDPBandwidth;

/**
 * t=<start time> <stop time>
 */
typedef struct _BVSDPTimeDesc {
    char *start_time;
    char *stop_time;
    BVList *repeats;
} BVSDPTimeDesc;

typedef struct _BVSDPKey {
    char *key_type;      //key type (prompt, clear, base64, uri)
    char *key_data;      //key data
} BVSDPKey;

/**
 * m=<media> <port>/<number of ports> <transport> <fmt list>
 * m=video 49170/2 RTP/AVP 31
 */
typedef struct _BVSDPMedia {
    char *media;
    char *port;
    char *num_of_port;
    char *proto;
    BVList *payloads;
    char *info;
    BVList *connections;
    BVList *bandwidths;
    BVList *attributes;
    BVSDPKey key;
} BVSDPMedia;

typedef struct _BVSDPSession {
    char *version;    //0
    BVSDPOrigin origin;
    char *session_name;      //s=<session name>
    char *session_info;      //i=<session info>
    char *uri;               //u=<URI>
    BVList *email;           //e=<email address>
    BVList *phone;           //p=<phone number>
    BVSDPConnection connection;
    BVList *bandwidths;
} BVSDPSession;

typedef struct _BVSDP {
    BVSDPSession session;
    BVList *times;      //BVSDPTimeDesc
    BVList *medias;     //BVSDPMedia
} BVSDP;

BVSDP *bb_sdp_protoc_packed();

#endif /* end of include guard: BVPROTOCOL_SDP_H */
