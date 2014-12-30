/*
 * Copyright (C) 2007 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (C) 2013 James Almer <jamrial@gmail.com>
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

#ifndef BVUTIL_SHA512_H
#define BVUTIL_SHA512_H

#include <stdint.h>

#include "attributes.h"
#include "version.h"

/**
 * @defgroup lavu_sha512 SHA512
 * @ingroup lavu_crypto
 * @{
 */

extern const int bv_sha512_size;

struct BVSHA512;

/**
 * Allocate an BVSHA512 context.
 */
struct BVSHA512 *bv_sha512_alloc(void);

/**
 * Initialize SHA-2 512 hashing.
 *
 * @param context pointer to the function context (of size bv_sha512_size)
 * @param bits    number of bits in digest (224, 256, 384 or 512 bits)
 * @return        zero if initialization succeeded, -1 otherwise
 */
int bv_sha512_init(struct BVSHA512* context, int bits);

/**
 * Update hash value.
 *
 * @param context hash function context
 * @param data    input data to update hash with
 * @param len     input data length
 */
void bv_sha512_update(struct BVSHA512* context, const uint8_t* data, unsigned int len);

/**
 * Finish hashing and output digest value.
 *
 * @param context hash function context
 * @param digest  buffer where output digest value is stored
 */
void bv_sha512_final(struct BVSHA512* context, uint8_t *digest);

/**
 * @}
 */

#endif /* BVUTIL_SHA512_H */
