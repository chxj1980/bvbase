/*************************************************************************
    > File Name: httpauth.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月16日 星期四 16时25分00秒
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

#line 25 "httpauth.c"

#include "httpauth.h"
#include "libbvutil/base64.h"
#include "libbvutil/bvstring.h"
#include "internal.h"
#include "libbvutil/random_seed.h"
#include "libbvutil/md5.h"

static void handle_basic_params(HTTPAuthState *state, const char *key,
                                int key_len, char **dest, int *dest_len)
{
    if (!strncmp(key, "realm=", key_len)) {
        *dest     =        state->realm;
        *dest_len = sizeof(state->realm);
    }
}

static void handle_digest_params(HTTPAuthState *state, const char *key,
                                 int key_len, char **dest, int *dest_len)
{
    DigestParams *digest = &state->digest_params;

    if (!strncmp(key, "realm=", key_len)) {
        *dest     =        state->realm;
        *dest_len = sizeof(state->realm);
    } else if (!strncmp(key, "nonce=", key_len)) {
        *dest     =        digest->nonce;
        *dest_len = sizeof(digest->nonce);
    } else if (!strncmp(key, "opaque=", key_len)) {
        *dest     =        digest->opaque;
        *dest_len = sizeof(digest->opaque);
    } else if (!strncmp(key, "algorithm=", key_len)) {
        *dest     =        digest->algorithm;
        *dest_len = sizeof(digest->algorithm);
    } else if (!strncmp(key, "qop=", key_len)) {
        *dest     =        digest->qop;
        *dest_len = sizeof(digest->qop);
    } else if (!strncmp(key, "stale=", key_len)) {
        *dest     =        digest->stale;
        *dest_len = sizeof(digest->stale);
    }
}

static void handle_digest_update(HTTPAuthState *state, const char *key,
                                 int key_len, char **dest, int *dest_len)
{
    DigestParams *digest = &state->digest_params;

    if (!strncmp(key, "nextnonce=", key_len)) {
        *dest     =        digest->nonce;
        *dest_len = sizeof(digest->nonce);
    }
}

static void choose_qop(char *qop, int size)
{
    char *ptr = strstr(qop, "auth");
    char *end = ptr + strlen("auth");

    if (ptr && (!*end || bv_isspace(*end) || *end == ',') &&
        (ptr == qop || bv_isspace(ptr[-1]) || ptr[-1] == ',')) {
        bv_strlcpy(qop, "auth", size);
    } else {
        qop[0] = 0;
    }
}

void bv_http_auth_handle_header(HTTPAuthState *state, const char *key,
                                const char *value)
{
    if (!strcmp(key, "WWW-Authenticate") || !strcmp(key, "Proxy-Authenticate")) {
        const char *p;
        if (bv_stristart(value, "Basic ", &p) &&
            state->auth_type <= HTTP_AUTH_BASIC) {
            state->auth_type = HTTP_AUTH_BASIC;
            state->realm[0] = 0;
            state->stale = 0;
            bb_parse_key_value(p, (bb_parse_key_val_cb) handle_basic_params,
                               state);
        } else if (bv_stristart(value, "Digest ", &p) &&
                   state->auth_type <= HTTP_AUTH_DIGEST) {
            state->auth_type = HTTP_AUTH_DIGEST;
            memset(&state->digest_params, 0, sizeof(DigestParams));
            state->realm[0] = 0;
            state->stale = 0;
            bb_parse_key_value(p, (bb_parse_key_val_cb) handle_digest_params,
                               state);
            choose_qop(state->digest_params.qop,
                       sizeof(state->digest_params.qop));
            if (!bv_strcasecmp(state->digest_params.stale, "true"))
                state->stale = 1;
        }
    } else if (!strcmp(key, "Authentication-Info")) {
        bb_parse_key_value(value, (bb_parse_key_val_cb) handle_digest_update,
                           state);
    }
}


static void update_md5_strings(struct BVMD5 *md5ctx, ...)
{
    va_list vl;

    va_start(vl, md5ctx);
    while (1) {
        const char* str = va_arg(vl, const char*);
        if (!str)
            break;
        bv_md5_update(md5ctx, str, strlen(str));
    }
    va_end(vl);
}

/* Generate a digest reply, according to RFC 2617. */
static char *make_digest_auth(HTTPAuthState *state, const char *username,
                              const char *password, const char *uri,
                              const char *method)
{
    DigestParams *digest = &state->digest_params;
    int len;
    uint32_t cnonce_buf[2];
    char cnonce[17];
    char nc[9];
    int i;
    char A1hash[33], A2hash[33], response[33];
    struct BVMD5 *md5ctx;
    uint8_t hash[16];
    char *authstr;

    digest->nc++;
    snprintf(nc, sizeof(nc), "%08x", digest->nc);

    /* Generate a client nonce. */
    for (i = 0; i < 2; i++)
        cnonce_buf[i] = bv_get_random_seed();
    bb_data_to_hex(cnonce, (const uint8_t*) cnonce_buf, sizeof(cnonce_buf), 1);
    cnonce[2*sizeof(cnonce_buf)] = 0;

    md5ctx = bv_md5_alloc();
    if (!md5ctx)
        return NULL;

    bv_md5_init(md5ctx);
    update_md5_strings(md5ctx, username, ":", state->realm, ":", password, NULL);
    bv_md5_final(md5ctx, hash);
    bb_data_to_hex(A1hash, hash, 16, 1);
    A1hash[32] = 0;

    if (!strcmp(digest->algorithm, "") || !strcmp(digest->algorithm, "MD5")) {
    } else if (!strcmp(digest->algorithm, "MD5-sess")) {
        bv_md5_init(md5ctx);
        update_md5_strings(md5ctx, A1hash, ":", digest->nonce, ":", cnonce, NULL);
        bv_md5_final(md5ctx, hash);
        bb_data_to_hex(A1hash, hash, 16, 1);
        A1hash[32] = 0;
    } else {
        /* Unsupported algorithm */
        bv_free(md5ctx);
        return NULL;
    }

    bv_md5_init(md5ctx);
    update_md5_strings(md5ctx, method, ":", uri, NULL);
    bv_md5_final(md5ctx, hash);
    bb_data_to_hex(A2hash, hash, 16, 1);
    A2hash[32] = 0;

    bv_md5_init(md5ctx);
    update_md5_strings(md5ctx, A1hash, ":", digest->nonce, NULL);
    if (!strcmp(digest->qop, "auth") || !strcmp(digest->qop, "auth-int")) {
        update_md5_strings(md5ctx, ":", nc, ":", cnonce, ":", digest->qop, NULL);
    }
    update_md5_strings(md5ctx, ":", A2hash, NULL);
    bv_md5_final(md5ctx, hash);
    bb_data_to_hex(response, hash, 16, 1);
    response[32] = 0;

    bv_free(md5ctx);

    if (!strcmp(digest->qop, "") || !strcmp(digest->qop, "auth")) {
    } else if (!strcmp(digest->qop, "auth-int")) {
        /* qop=auth-int not supported */
        return NULL;
    } else {
        /* Unsupported qop value. */
        return NULL;
    }

    len = strlen(username) + strlen(state->realm) + strlen(digest->nonce) +
              strlen(uri) + strlen(response) + strlen(digest->algorithm) +
              strlen(digest->opaque) + strlen(digest->qop) + strlen(cnonce) +
              strlen(nc) + 150;

    authstr = bv_malloc(len);
    if (!authstr)
        return NULL;
    snprintf(authstr, len, "Authorization: Digest ");

    /* TODO: Escape the quoted strings properly. */
    bv_strlcatf(authstr, len, "username=\"%s\"",   username);
    bv_strlcatf(authstr, len, ",realm=\"%s\"",     state->realm);
    bv_strlcatf(authstr, len, ",nonce=\"%s\"",     digest->nonce);
    bv_strlcatf(authstr, len, ",uri=\"%s\"",       uri);
    bv_strlcatf(authstr, len, ",response=\"%s\"",  response);

    // we are violating the RFC and use "" because all others seem to do that too.
    if (digest->algorithm[0])
        bv_strlcatf(authstr, len, ",algorithm=\"%s\"",  digest->algorithm);

    if (digest->opaque[0])
        bv_strlcatf(authstr, len, ",opaque=\"%s\"", digest->opaque);
    if (digest->qop[0]) {
        bv_strlcatf(authstr, len, ",qop=\"%s\"",    digest->qop);
        bv_strlcatf(authstr, len, ",cnonce=\"%s\"", cnonce);
        bv_strlcatf(authstr, len, ",nc=%s",         nc);
    }

    bv_strlcatf(authstr, len, "\r\n");

    return authstr;
}

char *bv_http_auth_create_response(HTTPAuthState *state, const char *auth,
                                   const char *path, const char *method)
{
    char *authstr = NULL;

    /* Clear the stale flag, we assume the auth is ok now. It is reset
     * by the server headers if there's a new issue. */
    state->stale = 0;
    if (!auth || !strchr(auth, ':'))
        return NULL;

    if (state->auth_type == HTTP_AUTH_BASIC) {
        int auth_b64_len, len;
        char *ptr, *decoded_auth = bb_url_decode(auth);

        if (!decoded_auth)
            return NULL;

        auth_b64_len = BV_BASE64_SIZE(strlen(decoded_auth));
        len = auth_b64_len + 30;

        authstr = bv_malloc(len);
        if (!authstr) {
            bv_free(decoded_auth);
            return NULL;
        }

        snprintf(authstr, len, "Authorization: Basic ");
        ptr = authstr + strlen(authstr);
        bv_base64_encode(ptr, auth_b64_len, decoded_auth, strlen(decoded_auth));
        bv_strlcat(ptr, "\r\n", len - (ptr - authstr));
        bv_free(decoded_auth);
    } else if (state->auth_type == HTTP_AUTH_DIGEST) {
        char *username = bb_url_decode(auth), *password;

        if (!username)
            return NULL;

        if ((password = strchr(username, ':'))) {
            *password++ = 0;
            authstr = make_digest_auth(state, username, password, path, method);
        }
        bv_free(username);
    }
    return authstr;
}
