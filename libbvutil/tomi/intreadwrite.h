/*
 * Copyright (c) 2010 Mans Rullgard <mans@mansr.com>
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

#ifndef BVUTIL_TOMI_INTREADWRITE_H
#define BVUTIL_TOMI_INTREADWRITE_H

#include <stdint.h>

#include "config.h"
#include "libbvutil/attributes.h"

#define BV_RB16 BV_RB16
static bv_always_inline uint16_t BV_RB16(const void *p)
{
    uint16_t v;
    __asm__ ("loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "storeacc,  %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "add,       %0    \n\t"
             : "=r"(v), "+a"(p));
    return v;
}

#define BV_WB16 BV_WB16
static bv_always_inline void BV_WB16(void *p, uint16_t v)
{
    __asm__ volatile ("loadacc,   %1    \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (%0+) \n\t"
                      "loadacc,   %1    \n\t"
                      "storeacc,  (%0+) \n\t"
                      : "+&a"(p) : "r"(v));
}

#define BV_RL16 BV_RL16
static bv_always_inline uint16_t BV_RL16(const void *p)
{
    uint16_t v;
    __asm__ ("loadacc,   (%1+) \n\t"
             "storeacc,  %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             : "=r"(v), "+a"(p));
    return v;
}

#define BV_WL16 BV_WL16
static bv_always_inline void BV_WL16(void *p, uint16_t v)
{
    __asm__ volatile ("loadacc,   %1    \n\t"
                      "storeacc,  (%0+) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (%0+) \n\t"
                      : "+&a"(p) : "r"(v));
}

#define BV_RB32 BV_RB32
static bv_always_inline uint32_t BV_RB32(const void *p)
{
    uint32_t v;
    __asm__ ("loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "storeacc,  %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "add,       %0    \n\t"
             : "=r"(v), "+a"(p));
    return v;
}

#define BV_WB32 BV_WB32
static bv_always_inline void BV_WB32(void *p, uint32_t v)
{
    __asm__ volatile ("loadacc,   #4    \n\t"
                      "add,       %0    \n\t"
                      "loadacc,   %1    \n\t"
                      "storeacc,  (-%0) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (-%0) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (-%0) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (-%0) \n\t"
                      : "+&a"(p) : "r"(v));
}

#define BV_RL32 BV_RL32
static bv_always_inline uint32_t BV_RL32(const void *p)
{
    uint32_t v;
    __asm__ ("loadacc,   (%1+) \n\t"
             "storeacc,  %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             "loadacc,   (%1+) \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "rol8             \n\t"
             "add,       %0    \n\t"
             : "=r"(v), "+a"(p));
    return v;
}

#define BV_WL32 BV_WL32
static bv_always_inline void BV_WL32(void *p, uint32_t v)
{
    __asm__ volatile ("loadacc,   %1    \n\t"
                      "storeacc,  (%0+) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (%0+) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (%0+) \n\t"
                      "lsr8             \n\t"
                      "storeacc,  (%0+) \n\t"
                      : "+&a"(p) : "r"(v));
}

#endif /* BVUTIL_TOMI_INTREADWRITE_H */
