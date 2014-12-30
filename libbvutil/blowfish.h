/*
 * Blowfish algorithm
 * Copyright (c) 2012 Samuel Pitoiset
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_BLOWFISH_H
#define BVUTIL_BLOWFISH_H

#include <stdint.h>

/**
 * @defgroup lavu_blowfish Blowfish
 * @ingroup lavu_crypto
 * @{
 */

#define BV_BF_ROUNDS 16

typedef struct BVBlowfish {
    uint32_t p[BV_BF_ROUNDS + 2];
    uint32_t s[4][256];
} BVBlowfish;

/**
 * Initialize an BVBlowfish context.
 *
 * @param ctx an BVBlowfish context
 * @param key a key
 * @param key_len length of the key
 */
void bv_blowfish_init(struct BVBlowfish *ctx, const uint8_t *key, int key_len);

/**
 * Encrypt or decrypt a buffer using a previously initialized context.
 *
 * @param ctx an BVBlowfish context
 * @param xl left four bytes halves of input to be encrypted
 * @param xr right four bytes halves of input to be encrypted
 * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_blowfish_crypt_ecb(struct BVBlowfish *ctx, uint32_t *xl, uint32_t *xr,
                           int decrypt);

/**
 * Encrypt or decrypt a buffer using a previously initialized context.
 *
 * @param ctx an BVBlowfish context
 * @param dst destination array, can be equal to src
 * @param src source array, can be equal to dst
 * @param count number of 8 byte blocks
 * @param iv initialization vector for CBC mode, if NULL ECB will be used
 * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_blowfish_crypt(struct BVBlowfish *ctx, uint8_t *dst, const uint8_t *src,
                       int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* BVUTIL_BLOWFISH_H */
