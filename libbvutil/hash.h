/*
 * Copyright (C) 2013 Reimar Döffinger <Reimar.Doeffinger@gmx.de>
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

#ifndef BVUTIL_HASH_H
#define BVUTIL_HASH_H

#include <stdint.h>

struct BVHashContext;

/**
 * Allocate a hash context for the algorithm specified by name.
 *
 * @return  >= 0 for success, a negative error code for failure
 * @note  The context is not initialized, you must call bv_hash_init().
 */
int bv_hash_alloc(struct BVHashContext **ctx, const char *name);

/**
 * Get the names of available hash algorithms.
 *
 * This function can be used to enumerate the algorithms.
 *
 * @param i  index of the hash algorithm, starting from 0
 * @return   a pointer to a static string or NULL if i is out of range
 */
const char *bv_hash_names(int i);

/**
 * Get the name of the algorithm corresponding to the given hash context.
 */
const char *bv_hash_get_name(const struct BVHashContext *ctx);

/**
 * Maximum value that bv_hash_get_size will currently return.
 *
 * You can use this if you absolutely want or need to use static allocation
 * and are fine with not supporting hashes newly added to libbvutil without
 * recompilation.
 * Note that you still need to check against bv_hash_get_size, adding new hashes
 * with larger sizes will not be considered an ABI change and should not cause
 * your code to overflow a buffer.
 */
#define BV_HASH_MAX_SIZE 64

/**
 * Get the size of the resulting hash value in bytes.
 *
 * The pointer passed to bv_hash_final have space for at least this many bytes.
 */
int bv_hash_get_size(const struct BVHashContext *ctx);

/**
 * Initialize or reset a hash context.
 */
void bv_hash_init(struct BVHashContext *ctx);

/**
 * Update a hash context with additional data.
 */
void bv_hash_update(struct BVHashContext *ctx, const uint8_t *src, int len);

/**
 * Finalize a hash context and compute the actual hash value.
 */
void bv_hash_final(struct BVHashContext *ctx, uint8_t *dst);

/**
 * Finalize a hash context and compute the actual hash value.
 * If size is smaller than the hash size, the hash is truncated;
 * if size is larger, the buffer is padded with 0.
 */
void bv_hash_final_bin(struct BVHashContext *ctx, uint8_t *dst, int size);

/**
 * Finalize a hash context and compute the actual hash value as a hex string.
 * The string is always 0-terminated.
 * If size is smaller than 2 * hash_size + 1, the hex string is truncated.
 */
void bv_hash_final_hex(struct BVHashContext *ctx, uint8_t *dst, int size);

/**
 * Finalize a hash context and compute the actual hash value as a base64 string.
 * The string is always 0-terminated.
 * If size is smaller than BV_BASE64_SIZE(hash_size), the base64 string is
 * truncated.
 */
void bv_hash_final_b64(struct BVHashContext *ctx, uint8_t *dst, int size);

/**
 * Free hash context.
 */
void bv_hash_freep(struct BVHashContext **ctx);

#endif /* BVUTIL_HASH_H */
