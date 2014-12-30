/*
 * RC4 encryption/decryption/pseudo-random number generator
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

#ifndef BVUTIL_RC4_H
#define BVUTIL_RC4_H

#include <stdint.h>

struct BVRC4 {
    uint8_t state[256];
    int x, y;
};

/**
 * @brief Initializes an BVRC4 context.
 *
 * @param key_bits must be a multiple of 8
 * @param decrypt 0 for encryption, 1 for decryption, currently has no effect
 */
int bv_rc4_init(struct BVRC4 *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * @brief Encrypts / decrypts using the RC4 algorithm.
 *
 * @param count number of bytes
 * @param dst destination array, can be equal to src
 * @param src source array, can be equal to dst, may be NULL
 * @param iv not (yet) used for RC4, should be NULL
 * @param decrypt 0 for encryption, 1 for decryption, not (yet) used
 */
void bv_rc4_crypt(struct BVRC4 *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

#endif /* BVUTIL_RC4_H */
