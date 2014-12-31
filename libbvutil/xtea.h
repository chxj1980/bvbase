/*
 * A 32-bit implementation of the XTEA algorithm
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

#ifndef BVUTIL_XTEA_H
#define BVUTIL_XTEA_H

#include <stdint.h>

/**
 * @file
 * @brief Public header for libbvutil XTEA algorithm
 * @defgroup lavu_xtea XTEA
 * @ingroup lavu_crypto
 * @{
 */

typedef struct BVXTEA {
    uint32_t key[16];
} BVXTEA;

/**
 * Initialize an BVXTEA context.
 *
 * @param ctx an BVXTEA context
 * @param key a key of 16 bytes used for encryption/decryption
 */
void bv_xtea_init(struct BVXTEA *ctx, const uint8_t key[16]);

/**
 * Encrypt or decrypt a buffer using a previously initialized context.
 *
 * @param ctx an BVXTEA context
 * @param dst destination array, can be equal to src
 * @param src source array, can be equal to dst
 * @param count number of 8 byte blocks
 * @param iv initialization vector for CBC mode, if NULL then ECB will be used
 * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_xtea_crypt(struct BVXTEA *ctx, uint8_t *dst, const uint8_t *src,
                   int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* BVUTIL_XTEA_H */
