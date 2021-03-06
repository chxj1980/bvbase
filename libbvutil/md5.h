/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef BVUTIL_MD5_H
#define BVUTIL_MD5_H

#include <stdint.h>

#include "attributes.h"
#include "version.h"

/**
 * @defgroup lavu_md5 MD5
 * @ingroup lavu_crypto
 * @{
 */

extern const int bv_md5_size;

struct BVMD5;

/**
 * Allocate an BVMD5 context.
 */
struct BVMD5 *bv_md5_alloc(void);

/**
 * Initialize MD5 hashing.
 *
 * @param ctx pointer to the function context (of size bv_md5_size)
 */
void bv_md5_init(struct BVMD5 *ctx);

/**
 * Update hash value.
 *
 * @param ctx hash function context
 * @param src input data to update hash with
 * @param len input data length
 */
void bv_md5_update(struct BVMD5 *ctx, const uint8_t *src, int len);

/**
 * Finish hashing and output digest value.
 *
 * @param ctx hash function context
 * @param dst buffer where output digest value is stored
 */
void bv_md5_final(struct BVMD5 *ctx, uint8_t *dst);

/**
 * Hash an array of data.
 *
 * @param dst The output buffer to write the digest into
 * @param src The data to hash
 * @param len The length of the data, in bytes
 */
void bv_md5_sum(uint8_t *dst, const uint8_t *src, const int len);

/**
 * @}
 */

#endif /* BVUTIL_MD5_H */
