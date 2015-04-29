/*************************************************************************
    > File Name: httpauth.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月16日 星期四 16时32分57秒
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

#ifndef BVPROTOCOL_HTTP_AUTH_H
#define BVPROTOCOL_HTTP_AUTH_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Authentication types, ordered from weakest to strongest.
 */
typedef enum HTTPAuthType {
    HTTP_AUTH_NONE = 0,    /**< No authentication specified */
    HTTP_AUTH_BASIC,       /**< HTTP 1.0 Basic auth from RFC 1945
                             *  (also in RFC 2617) */
    HTTP_AUTH_DIGEST,      /**< HTTP 1.1 Digest auth from RFC 2617 */
} HTTPAuthType;

typedef struct DigestParams {
    char nonce[300];       /**< Server specified nonce */
    char algorithm[10];    /**< Server specified digest algorithm */
    char qop[30];          /**< Quality of protection, containing the one
                             *  that we've chosen to use, from the
                             *  alternatives that the server offered. */
    char opaque[300];      /**< A server-specified string that should be
                             *  included in authentication responses, not
                             *  included in the actual digest calculation. */
    char stale[10];        /**< The server indicated that the auth was ok,
                             * but needs to be redone with a new, non-stale
                             * nonce. */
    int nc;                /**< Nonce count, the number of earlier replies
                             *  where this particular nonce has been used. */
} DigestParams;

/**
 * HTTP Authentication state structure. Must be zero-initialized
 * before used with the functions below.
 */
typedef struct HTTPAuthState {
    /**
     * The currently chosen auth type.
     */
    int auth_type;
    /**
     * Authentication realm
     */
    char realm[200];
    /**
     * The parameters specifiec to digest authentication.
     */
    DigestParams digest_params;
    /**
     * Auth ok, but needs to be resent with a new nonce.
     */
    int stale;
} HTTPAuthState;

void bb_http_auth_handle_header(HTTPAuthState *state, const char *key,
                                const char *value);
char *bb_http_auth_create_response(HTTPAuthState *state, const char *auth,
                                   const char *path, const char *method);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BVPROTOCOL_HTTP_AUTH_H */
