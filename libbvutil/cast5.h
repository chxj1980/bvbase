/*
 * An implementation of the CAST128 algorithm as mentioned in RFC2144
 * Copyright (c) 2014 Supraja Meedinti
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

#ifndef BVUTIL_CAST5_H
#define BVUTIL_CAST5_H

#include <stdint.h>


/**
  * @file
  * @brief Public header for libbvutil CAST5 algorithm
  * @defgroup lavu_cast5 CAST5
  * @ingroup lavu_crypto
  * @{
  */

extern const int bv_cast5_size;

struct BVCAST5;

/**
  * Allocate an BVCAST5 context
  * To free the struct: bv_free(ptr)
  */
struct BVCAST5 *bv_cast5_alloc(void);
/**
  * Initialize an BVCAST5 context.
  *
  * @param ctx an BVCAST5 context
  * @param key a key of 5,6,...16 bytes used for encryption/decryption
  * @param key_bits number of keybits: possible are 40,48,...,128
 */
int bv_cast5_init(struct BVCAST5 *ctx, const uint8_t *key, int key_bits);

/**
  * Encrypt or decrypt a buffer using a previously initialized context, ECB mode only
  *
  * @param ctx an BVCAST5 context
  * @param dst destination array, can be equal to src
  * @param src source array, can be equal to dst
  * @param count number of 8 byte blocks
  * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_cast5_crypt(struct BVCAST5 *ctx, uint8_t *dst, const uint8_t *src, int count, int decrypt);

/**
  * Encrypt or decrypt a buffer using a previously initialized context
  *
  * @param ctx an BVCAST5 context
  * @param dst destination array, can be equal to src
  * @param src source array, can be equal to dst
  * @param count number of 8 byte blocks
  * @param iv initialization vector for CBC mode, NULL for ECB mode
  * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_cast5_crypt2(struct BVCAST5 *ctx, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);
/**
 * @}
 */
#endif /* BVUTIL_CAST5_H */
