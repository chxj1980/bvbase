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

#ifndef BVUTIL_MURMUR3_H
#define BVUTIL_MURMUR3_H

#include <stdint.h>

struct AVMurMur3 *bv_murmur3_alloc(void);
void bv_murmur3_init_seeded(struct AVMurMur3 *c, uint64_t seed);
void bv_murmur3_init(struct AVMurMur3 *c);
void bv_murmur3_update(struct AVMurMur3 *c, const uint8_t *src, int len);
void bv_murmur3_final(struct AVMurMur3 *c, uint8_t dst[16]);

#endif /* BVUTIL_MURMUR3_H */
