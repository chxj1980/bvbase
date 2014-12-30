/*
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVbase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_INTREADWRITE_H
#define BVUTIL_INTREADWRITE_H

#include <stdint.h>
#include "libbvutil/bvconfig.h"
#include "attributes.h"
#include "bswap.h"

typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t  u8 [8];
    double   f64;
    float    f32[2];
} bv_alias bv_alias64;

typedef union {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8 [4];
    float    f32;
} bv_alias bv_alias32;

typedef union {
    uint16_t u16;
    uint8_t  u8 [2];
} bv_alias bv_alias16;

/*
 * Arch-specific headers can provide any combination of
 * BV_[RW][BLN](16|24|32|48|64) and BV_(COPY|SWAP|ZERO)(64|128) macros.
 * Preprocessor symbols must be defined, even if these are implemented
 * as inline functions.
 *
 * R/W means read/write, B/L/N means big/little/native endianness.
 * The following macros require aligned access, compared to their
 * unaligned variants: BV_(COPY|SWAP|ZERO)(64|128), BV_[RW]N[8-64]A.
 * Incorrect usage may range from abysmal performance to crash
 * depending on the platform.
 *
 * The unaligned variants are BV_[RW][BLN][8-64] and BV_COPY*U.
 */

#ifdef HAVE_BV_CONFIG_H

#include "config.h"

#if   ARCH_ARM
#   include "arm/intreadwrite.h"
#elif ARCH_AVR32
#   include "avr32/intreadwrite.h"
#elif ARCH_MIPS
#   include "mips/intreadwrite.h"
#elif ARCH_PPC
#   include "ppc/intreadwrite.h"
#elif ARCH_TOMI
#   include "tomi/intreadwrite.h"
#elif ARCH_X86
#   include "x86/intreadwrite.h"
#endif

#endif /* HAVE_BV_CONFIG_H */

/*
 * Map BV_RNXX <-> BV_R[BL]XX for all variants provided by per-arch headers.
 */

#if BV_HAVE_BIGENDIAN

#   if    defined(BV_RN16) && !defined(BV_RB16)
#       define BV_RB16(p) BV_RN16(p)
#   elif !defined(BV_RN16) &&  defined(BV_RB16)
#       define BV_RN16(p) BV_RB16(p)
#   endif

#   if    defined(BV_WN16) && !defined(BV_WB16)
#       define BV_WB16(p, v) BV_WN16(p, v)
#   elif !defined(BV_WN16) &&  defined(BV_WB16)
#       define BV_WN16(p, v) BV_WB16(p, v)
#   endif

#   if    defined(BV_RN24) && !defined(BV_RB24)
#       define BV_RB24(p) BV_RN24(p)
#   elif !defined(BV_RN24) &&  defined(BV_RB24)
#       define BV_RN24(p) BV_RB24(p)
#   endif

#   if    defined(BV_WN24) && !defined(BV_WB24)
#       define BV_WB24(p, v) BV_WN24(p, v)
#   elif !defined(BV_WN24) &&  defined(BV_WB24)
#       define BV_WN24(p, v) BV_WB24(p, v)
#   endif

#   if    defined(BV_RN32) && !defined(BV_RB32)
#       define BV_RB32(p) BV_RN32(p)
#   elif !defined(BV_RN32) &&  defined(BV_RB32)
#       define BV_RN32(p) BV_RB32(p)
#   endif

#   if    defined(BV_WN32) && !defined(BV_WB32)
#       define BV_WB32(p, v) BV_WN32(p, v)
#   elif !defined(BV_WN32) &&  defined(BV_WB32)
#       define BV_WN32(p, v) BV_WB32(p, v)
#   endif

#   if    defined(BV_RN48) && !defined(BV_RB48)
#       define BV_RB48(p) BV_RN48(p)
#   elif !defined(BV_RN48) &&  defined(BV_RB48)
#       define BV_RN48(p) BV_RB48(p)
#   endif

#   if    defined(BV_WN48) && !defined(BV_WB48)
#       define BV_WB48(p, v) BV_WN48(p, v)
#   elif !defined(BV_WN48) &&  defined(BV_WB48)
#       define BV_WN48(p, v) BV_WB48(p, v)
#   endif

#   if    defined(BV_RN64) && !defined(BV_RB64)
#       define BV_RB64(p) BV_RN64(p)
#   elif !defined(BV_RN64) &&  defined(BV_RB64)
#       define BV_RN64(p) BV_RB64(p)
#   endif

#   if    defined(BV_WN64) && !defined(BV_WB64)
#       define BV_WB64(p, v) BV_WN64(p, v)
#   elif !defined(BV_WN64) &&  defined(BV_WB64)
#       define BV_WN64(p, v) BV_WB64(p, v)
#   endif

#else /* BV_HAVE_BIGENDIAN */

#   if    defined(BV_RN16) && !defined(BV_RL16)
#       define BV_RL16(p) BV_RN16(p)
#   elif !defined(BV_RN16) &&  defined(BV_RL16)
#       define BV_RN16(p) BV_RL16(p)
#   endif

#   if    defined(BV_WN16) && !defined(BV_WL16)
#       define BV_WL16(p, v) BV_WN16(p, v)
#   elif !defined(BV_WN16) &&  defined(BV_WL16)
#       define BV_WN16(p, v) BV_WL16(p, v)
#   endif

#   if    defined(BV_RN24) && !defined(BV_RL24)
#       define BV_RL24(p) BV_RN24(p)
#   elif !defined(BV_RN24) &&  defined(BV_RL24)
#       define BV_RN24(p) BV_RL24(p)
#   endif

#   if    defined(BV_WN24) && !defined(BV_WL24)
#       define BV_WL24(p, v) BV_WN24(p, v)
#   elif !defined(BV_WN24) &&  defined(BV_WL24)
#       define BV_WN24(p, v) BV_WL24(p, v)
#   endif

#   if    defined(BV_RN32) && !defined(BV_RL32)
#       define BV_RL32(p) BV_RN32(p)
#   elif !defined(BV_RN32) &&  defined(BV_RL32)
#       define BV_RN32(p) BV_RL32(p)
#   endif

#   if    defined(BV_WN32) && !defined(BV_WL32)
#       define BV_WL32(p, v) BV_WN32(p, v)
#   elif !defined(BV_WN32) &&  defined(BV_WL32)
#       define BV_WN32(p, v) BV_WL32(p, v)
#   endif

#   if    defined(BV_RN48) && !defined(BV_RL48)
#       define BV_RL48(p) BV_RN48(p)
#   elif !defined(BV_RN48) &&  defined(BV_RL48)
#       define BV_RN48(p) BV_RL48(p)
#   endif

#   if    defined(BV_WN48) && !defined(BV_WL48)
#       define BV_WL48(p, v) BV_WN48(p, v)
#   elif !defined(BV_WN48) &&  defined(BV_WL48)
#       define BV_WN48(p, v) BV_WL48(p, v)
#   endif

#   if    defined(BV_RN64) && !defined(BV_RL64)
#       define BV_RL64(p) BV_RN64(p)
#   elif !defined(BV_RN64) &&  defined(BV_RL64)
#       define BV_RN64(p) BV_RL64(p)
#   endif

#   if    defined(BV_WN64) && !defined(BV_WL64)
#       define BV_WL64(p, v) BV_WN64(p, v)
#   elif !defined(BV_WN64) &&  defined(BV_WL64)
#       define BV_WN64(p, v) BV_WL64(p, v)
#   endif

#endif /* !BV_HAVE_BIGENDIAN */

/*
 * Define BV_[RW]N helper macros to simplify definitions not provided
 * by per-arch headers.
 */

#if defined(__GNUC__) && !defined(__TI_COMPILER_VERSION__)

union unaligned_64 { uint64_t l; } __attribute__((packed)) bv_alias;
union unaligned_32 { uint32_t l; } __attribute__((packed)) bv_alias;
union unaligned_16 { uint16_t l; } __attribute__((packed)) bv_alias;

#   define BV_RN(s, p) (((const union unaligned_##s *) (p))->l)
#   define BV_WN(s, p, v) ((((union unaligned_##s *) (p))->l) = (v))

#elif defined(__DECC)

#   define BV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
#   define BV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))

#elif BV_HAVE_FAST_UNALIGNED

#   define BV_RN(s, p) (((const bv_alias##s*)(p))->u##s)
#   define BV_WN(s, p, v) (((bv_alias##s*)(p))->u##s = (v))

#else

#ifndef BV_RB16
#   define BV_RB16(x)                           \
    ((((const uint8_t*)(x))[0] << 8) |          \
      ((const uint8_t*)(x))[1])
#endif
#ifndef BV_WB16
#   define BV_WB16(p, darg) do {                \
        unsigned d = (darg);                    \
        ((uint8_t*)(p))[1] = (d);               \
        ((uint8_t*)(p))[0] = (d)>>8;            \
    } while(0)
#endif

#ifndef BV_RL16
#   define BV_RL16(x)                           \
    ((((const uint8_t*)(x))[1] << 8) |          \
      ((const uint8_t*)(x))[0])
#endif
#ifndef BV_WL16
#   define BV_WL16(p, darg) do {                \
        unsigned d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
    } while(0)
#endif

#ifndef BV_RB32
#   define BV_RB32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])
#endif
#ifndef BV_WB32
#   define BV_WB32(p, darg) do {                \
        unsigned d = (darg);                    \
        ((uint8_t*)(p))[3] = (d);               \
        ((uint8_t*)(p))[2] = (d)>>8;            \
        ((uint8_t*)(p))[1] = (d)>>16;           \
        ((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)
#endif

#ifndef BV_RL32
#   define BV_RL32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[3] << 24) |    \
               (((const uint8_t*)(x))[2] << 16) |    \
               (((const uint8_t*)(x))[1] <<  8) |    \
                ((const uint8_t*)(x))[0])
#endif
#ifndef BV_WL32
#   define BV_WL32(p, darg) do {                \
        unsigned d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
    } while(0)
#endif

#ifndef BV_RB64
#   define BV_RB64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[0] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[1] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[6] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[7])
#endif
#ifndef BV_WB64
#   define BV_WB64(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[7] = (d);               \
        ((uint8_t*)(p))[6] = (d)>>8;            \
        ((uint8_t*)(p))[5] = (d)>>16;           \
        ((uint8_t*)(p))[4] = (d)>>24;           \
        ((uint8_t*)(p))[3] = (d)>>32;           \
        ((uint8_t*)(p))[2] = (d)>>40;           \
        ((uint8_t*)(p))[1] = (d)>>48;           \
        ((uint8_t*)(p))[0] = (d)>>56;           \
    } while(0)
#endif

#ifndef BV_RL64
#   define BV_RL64(x)                                   \
    (((uint64_t)((const uint8_t*)(x))[7] << 56) |       \
     ((uint64_t)((const uint8_t*)(x))[6] << 48) |       \
     ((uint64_t)((const uint8_t*)(x))[5] << 40) |       \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |       \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |       \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |       \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |       \
      (uint64_t)((const uint8_t*)(x))[0])
#endif
#ifndef BV_WL64
#   define BV_WL64(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
        ((uint8_t*)(p))[4] = (d)>>32;           \
        ((uint8_t*)(p))[5] = (d)>>40;           \
        ((uint8_t*)(p))[6] = (d)>>48;           \
        ((uint8_t*)(p))[7] = (d)>>56;           \
    } while(0)
#endif

#if BV_HAVE_BIGENDIAN
#   define BV_RN(s, p)    BV_RB##s(p)
#   define BV_WN(s, p, v) BV_WB##s(p, v)
#else
#   define BV_RN(s, p)    BV_RL##s(p)
#   define BV_WN(s, p, v) BV_WL##s(p, v)
#endif

#endif /* HAVE_FAST_UNALIGNED */

#ifndef BV_RN16
#   define BV_RN16(p) BV_RN(16, p)
#endif

#ifndef BV_RN32
#   define BV_RN32(p) BV_RN(32, p)
#endif

#ifndef BV_RN64
#   define BV_RN64(p) BV_RN(64, p)
#endif

#ifndef BV_WN16
#   define BV_WN16(p, v) BV_WN(16, p, v)
#endif

#ifndef BV_WN32
#   define BV_WN32(p, v) BV_WN(32, p, v)
#endif

#ifndef BV_WN64
#   define BV_WN64(p, v) BV_WN(64, p, v)
#endif

#if BV_HAVE_BIGENDIAN
#   define BV_RB(s, p)    BV_RN##s(p)
#   define BV_WB(s, p, v) BV_WN##s(p, v)
#   define BV_RL(s, p)    bv_bswap##s(BV_RN##s(p))
#   define BV_WL(s, p, v) BV_WN##s(p, bv_bswap##s(v))
#else
#   define BV_RB(s, p)    bv_bswap##s(BV_RN##s(p))
#   define BV_WB(s, p, v) BV_WN##s(p, bv_bswap##s(v))
#   define BV_RL(s, p)    BV_RN##s(p)
#   define BV_WL(s, p, v) BV_WN##s(p, v)
#endif

#define BV_RB8(x)     (((const uint8_t*)(x))[0])
#define BV_WB8(p, d)  do { ((uint8_t*)(p))[0] = (d); } while(0)

#define BV_RL8(x)     BV_RB8(x)
#define BV_WL8(p, d)  BV_WB8(p, d)

#ifndef BV_RB16
#   define BV_RB16(p)    BV_RB(16, p)
#endif
#ifndef BV_WB16
#   define BV_WB16(p, v) BV_WB(16, p, v)
#endif

#ifndef BV_RL16
#   define BV_RL16(p)    BV_RL(16, p)
#endif
#ifndef BV_WL16
#   define BV_WL16(p, v) BV_WL(16, p, v)
#endif

#ifndef BV_RB32
#   define BV_RB32(p)    BV_RB(32, p)
#endif
#ifndef BV_WB32
#   define BV_WB32(p, v) BV_WB(32, p, v)
#endif

#ifndef BV_RL32
#   define BV_RL32(p)    BV_RL(32, p)
#endif
#ifndef BV_WL32
#   define BV_WL32(p, v) BV_WL(32, p, v)
#endif

#ifndef BV_RB64
#   define BV_RB64(p)    BV_RB(64, p)
#endif
#ifndef BV_WB64
#   define BV_WB64(p, v) BV_WB(64, p, v)
#endif

#ifndef BV_RL64
#   define BV_RL64(p)    BV_RL(64, p)
#endif
#ifndef BV_WL64
#   define BV_WL64(p, v) BV_WL(64, p, v)
#endif

#ifndef BV_RB24
#   define BV_RB24(x)                           \
    ((((const uint8_t*)(x))[0] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[2])
#endif
#ifndef BV_WB24
#   define BV_WB24(p, d) do {                   \
        ((uint8_t*)(p))[2] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[0] = (d)>>16;           \
    } while(0)
#endif

#ifndef BV_RL24
#   define BV_RL24(x)                           \
    ((((const uint8_t*)(x))[2] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[0])
#endif
#ifndef BV_WL24
#   define BV_WL24(p, d) do {                   \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
    } while(0)
#endif

#ifndef BV_RB48
#   define BV_RB48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[0] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[1] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[4] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[5])
#endif
#ifndef BV_WB48
#   define BV_WB48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[5] = (d);               \
        ((uint8_t*)(p))[4] = (d)>>8;            \
        ((uint8_t*)(p))[3] = (d)>>16;           \
        ((uint8_t*)(p))[2] = (d)>>24;           \
        ((uint8_t*)(p))[1] = (d)>>32;           \
        ((uint8_t*)(p))[0] = (d)>>40;           \
    } while(0)
#endif

#ifndef BV_RL48
#   define BV_RL48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[5] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[0])
#endif
#ifndef BV_WL48
#   define BV_WL48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
        ((uint8_t*)(p))[4] = (d)>>32;           \
        ((uint8_t*)(p))[5] = (d)>>40;           \
    } while(0)
#endif

/*
 * The BV_[RW]NA macros access naturally aligned data
 * in a type-safe way.
 */

#define BV_RNA(s, p)    (((const bv_alias##s*)(p))->u##s)
#define BV_WNA(s, p, v) (((bv_alias##s*)(p))->u##s = (v))

#ifndef BV_RN16A
#   define BV_RN16A(p) BV_RNA(16, p)
#endif

#ifndef BV_RN32A
#   define BV_RN32A(p) BV_RNA(32, p)
#endif

#ifndef BV_RN64A
#   define BV_RN64A(p) BV_RNA(64, p)
#endif

#ifndef BV_WN16A
#   define BV_WN16A(p, v) BV_WNA(16, p, v)
#endif

#ifndef BV_WN32A
#   define BV_WN32A(p, v) BV_WNA(32, p, v)
#endif

#ifndef BV_WN64A
#   define BV_WN64A(p, v) BV_WNA(64, p, v)
#endif

/*
 * The BV_COPYxxU macros are suitable for copying data to/from unaligned
 * memory locations.
 */

#define BV_COPYU(n, d, s) BV_WN##n(d, BV_RN##n(s));

#ifndef BV_COPY16U
#   define BV_COPY16U(d, s) BV_COPYU(16, d, s)
#endif

#ifndef BV_COPY32U
#   define BV_COPY32U(d, s) BV_COPYU(32, d, s)
#endif

#ifndef BV_COPY64U
#   define BV_COPY64U(d, s) BV_COPYU(64, d, s)
#endif

#ifndef BV_COPY128U
#   define BV_COPY128U(d, s)                                    \
    do {                                                        \
        BV_COPY64U(d, s);                                       \
        BV_COPY64U((char *)(d) + 8, (const char *)(s) + 8);     \
    } while(0)
#endif

/* Parameters for BV_COPY*, BV_SWAP*, BV_ZERO* must be
 * naturally aligned. They may be implemented using MMX,
 * so emms_c() must be called before using any float code
 * afterwards.
 */

#define BV_COPY(n, d, s) \
    (((bv_alias##n*)(d))->u##n = ((const bv_alias##n*)(s))->u##n)

#ifndef BV_COPY16
#   define BV_COPY16(d, s) BV_COPY(16, d, s)
#endif

#ifndef BV_COPY32
#   define BV_COPY32(d, s) BV_COPY(32, d, s)
#endif

#ifndef BV_COPY64
#   define BV_COPY64(d, s) BV_COPY(64, d, s)
#endif

#ifndef BV_COPY128
#   define BV_COPY128(d, s)                    \
    do {                                       \
        BV_COPY64(d, s);                       \
        BV_COPY64((char*)(d)+8, (char*)(s)+8); \
    } while(0)
#endif

#define BV_SWAP(n, a, b) FFSWAP(bv_alias##n, *(bv_alias##n*)(a), *(bv_alias##n*)(b))

#ifndef BV_SWAP64
#   define BV_SWAP64(a, b) BV_SWAP(64, a, b)
#endif

#define BV_ZERO(n, d) (((bv_alias##n*)(d))->u##n = 0)

#ifndef BV_ZERO16
#   define BV_ZERO16(d) BV_ZERO(16, d)
#endif

#ifndef BV_ZERO32
#   define BV_ZERO32(d) BV_ZERO(32, d)
#endif

#ifndef BV_ZERO64
#   define BV_ZERO64(d) BV_ZERO(64, d)
#endif

#ifndef BV_ZERO128
#   define BV_ZERO128(d)         \
    do {                         \
        BV_ZERO64(d);            \
        BV_ZERO64((char*)(d)+8); \
    } while(0)
#endif

#endif /* BVUTIL_INTREADWRITE_H */
