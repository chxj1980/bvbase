/*
 * Copyright (C) 2012 Martin Storsjo
 *
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVbase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_HMAC_H
#define BVUTIL_HMAC_H

#include <stdint.h>

/**
 * @defgroup lavu_hmac HMAC
 * @ingroup lavu_crypto
 * @{
 */

enum BVHMACType {
    BV_HMAC_MD5,
    BV_HMAC_SHA1,
    BV_HMAC_SHA224 = 10,
    BV_HMAC_SHA256,
    BV_HMAC_SHA384,
    BV_HMAC_SHA512,
};

typedef struct BVHMAC BVHMAC;

/**
 * Allocate an BVHMAC context.
 * @param type The hash function used for the HMAC.
 */
BVHMAC *bv_hmac_alloc(enum BVHMACType type);

/**
 * Free an BVHMAC context.
 * @param ctx The context to free, may be NULL
 */
void bv_hmac_free(BVHMAC *ctx);

/**
 * Initialize an BVHMAC context with an authentication key.
 * @param ctx    The HMAC context
 * @param key    The authentication key
 * @param keylen The length of the key, in bytes
 */
void bv_hmac_init(BVHMAC *ctx, const uint8_t *key, unsigned int keylen);

/**
 * Hash data with the HMAC.
 * @param ctx  The HMAC context
 * @param data The data to hash
 * @param len  The length of the data, in bytes
 */
void bv_hmac_update(BVHMAC *ctx, const uint8_t *data, unsigned int len);

/**
 * Finish hashing and output the HMAC digest.
 * @param ctx    The HMAC context
 * @param out    The output buffer to write the digest into
 * @param outlen The length of the out buffer, in bytes
 * @return       The number of bytes written to out, or a negative error code.
 */
int bv_hmac_final(BVHMAC *ctx, uint8_t *out, unsigned int outlen);

/**
 * Hash an array of data with a key.
 * @param ctx    The HMAC context
 * @param data   The data to hash
 * @param len    The length of the data, in bytes
 * @param key    The authentication key
 * @param keylen The length of the key, in bytes
 * @param out    The output buffer to write the digest into
 * @param outlen The length of the out buffer, in bytes
 * @return       The number of bytes written to out, or a negative error code.
 */
int bv_hmac_calc(BVHMAC *ctx, const uint8_t *data, unsigned int len,
                 const uint8_t *key, unsigned int keylen,
                 uint8_t *out, unsigned int outlen);

/**
 * @}
 */

#endif /* BVUTIL_HMAC_H */
