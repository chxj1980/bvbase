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

#ifndef BVUTIL_CRC_H
#define BVUTIL_CRC_H

#include <stdint.h>
#include <stddef.h>
#include "attributes.h"

/**
 * @defgroup lavu_crc32 CRC32
 * @ingroup lavu_crypto
 * @{
 */

typedef uint32_t BVCRC;

typedef enum {
    BV_CRC_8_ATM,
    BV_CRC_16_ANSI,
    BV_CRC_16_CCITT,
    BV_CRC_32_IEEE,
    BV_CRC_32_IEEE_LE,  /*< reversed bitorder version of BV_CRC_32_IEEE */
    BV_CRC_16_ANSI_LE,  /*< reversed bitorder version of BV_CRC_16_ANSI */
    BV_CRC_24_IEEE = 12,
    BV_CRC_MAX,         /*< Not part of public API! Do not use outside libbvutil. */
}BVCRCId;

/**
 * Initialize a CRC table.
 * @param ctx must be an array of size sizeof(BVCRC)*257 or sizeof(BVCRC)*1024
 * @param le If 1, the lowest bit represents the coefficient for the highest
 *           exponent of the corresponding polynomial (both for poly and
 *           actual CRC).
 *           If 0, you must swap the CRC parameter and the result of bv_crc
 *           if you need the standard representation (can be simplified in
 *           most cases to e.g. bswap16):
 *           bv_bswap32(crc << (32-bits))
 * @param bits number of bits for the CRC
 * @param poly generator polynomial without the x**bits coefficient, in the
 *             representation as specified by le
 * @param ctx_size size of ctx in bytes
 * @return <0 on failure
 */
int bv_crc_init(BVCRC *ctx, int le, int bits, uint32_t poly, int ctx_size);

/**
 * Get an initialized standard CRC table.
 * @param crc_id ID of a standard CRC
 * @return a pointer to the CRC table or NULL on failure
 */
const BVCRC *bv_crc_get_table(BVCRCId crc_id);

/**
 * Calculate the CRC of a block.
 * @param crc CRC of previous blocks if any or initial value for CRC
 * @return CRC updated with the data from the given block
 *
 * @see bv_crc_init() "le" parameter
 */
uint32_t bv_crc(const BVCRC *ctx, uint32_t crc,
                const uint8_t *buffer, size_t length) bv_pure;

/**
 * @}
 */

#endif /* BVUTIL_CRC_H */
