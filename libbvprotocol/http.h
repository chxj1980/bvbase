/*************************************************************************
    > File Name: http.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月16日 星期四 16时07分09秒
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

#ifndef BVPROTOCOL_HTT_H
#define BVPROTOCOL_HTT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "bvurl.h"

#define HTTP_HEADERS_SIZE 4096

enum HTTPStatusCode {
    HTTP_STATUS_CONTINUE             =100,
    HTTP_STATUS_OK                   =200,
    HTTP_STATUS_CREATED              =201,
    HTTP_STATUS_LOW_ON_STORAGE_SPACE =250,
    HTTP_STATUS_MULTIPLE_CHOICES     =300,
    HTTP_STATUS_MOVED_PERMANENTLY    =301,
    HTTP_STATUS_MOVED_TEMPORARILY    =302,
    HTTP_STATUS_SEE_OTHER            =303,
    HTTP_STATUS_NOT_MODIFIED         =304,
    HTTP_STATUS_USE_PROXY            =305,
    HTTP_STATUS_BAD_REQUEST          =400,
    HTTP_STATUS_UNAUTHORIZED         =401,
    HTTP_STATUS_PAYMENT_REQUIRED     =402,
    HTTP_STATUS_FORBIDDEN            =403,
    HTTP_STATUS_NOT_FOUND            =404,
    HTTP_STATUS_METHOD               =405,
    HTTP_STATUS_NOT_ACCEPTABLE       =406,
    HTTP_STATUS_PROXY_AUTH_REQUIRED  =407,
    HTTP_STATUS_REQ_TIME_OUT         =408,
    HTTP_STATUS_GONE                 =410,
    HTTP_STATUS_LENGTH_REQUIRED      =411,
    HTTP_STATUS_PRECONDITION_FAILED  =412,
    HTTP_STATUS_REQ_ENTITY_2LARGE    =413,
    HTTP_STATUS_REQ_URI_2LARGE       =414,
    HTTP_STATUS_UNSUPPORTED_MTYPE    =415,
    HTTP_STATUS_PARAM_NOT_UNDERSTOOD =451,
    HTTP_STATUS_CONFERENCE_NOT_FOUND =452,
    HTTP_STATUS_BANDWIDTH            =453,
    HTTP_STATUS_SESSION              =454,
    HTTP_STATUS_STATE                =455,
    HTTP_STATUS_INVALID_HEADER_FIELD =456,
    HTTP_STATUS_INVALID_RANGE        =457,
    HTTP_STATUS_RONLY_PARAMETER      =458,
    HTTP_STATUS_AGGREGATE            =459,
    HTTP_STATUS_ONLY_AGGREGATE       =460,
    HTTP_STATUS_TRANSPORT            =461,
    HTTP_STATUS_UNREACHABLE          =462,
    HTTP_STATUS_INTERNAL             =500,
    HTTP_STATUS_NOT_IMPLEMENTED      =501,
    HTTP_STATUS_BAD_GATEWAY          =502,
    HTTP_STATUS_SERVICE              =503,
    HTTP_STATUS_GATEWAY_TIME_OUT     =504,
    HTTP_STATUS_VERSION              =505,
    HTTP_STATUS_UNSUPPORTED_OPTION   =551,
};

/**
 * Initialize the authentication state based on another HTTP BVURLContext.
 * This can be used to pre-initialize the authentication parameters if
 * they are known beforehand, to avoid having to do an initial failing
 * request just to get the parameters.
 *
 * @param dest URL context whose authentication state gets updated
 * @param src URL context whose authentication state gets copied
 */
void bv_http_init_auth_state(BVURLContext *dest, const BVURLContext *src);

/**
 * Send a new HTTP request, reusing the old connection.
 *
 * @param h pointer to the resource
 * @param uri uri used to perform the request
 * @return a negative value if an error condition occurred, 0
 * otherwise
 */
int bv_http_do_new_request(BVURLContext *h, const char *uri);

int bv_http_averror(int status_code, int default_averror);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BVPROTOCOL_HTT_H */
