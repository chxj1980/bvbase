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

/**
 * @file
 * common internal API header
 */

#ifndef BVUTIL_INTERNAL_H
#define BVUTIL_INTERNAL_H

#if !defined(DEBUG) && !defined(NDEBUG)
#    define NDEBUG
#endif

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "config.h"
#include "attributes.h"
#include "timer.h"
#include "cpu.h"
#include "dict.h"
#include "pixfmt.h"
#include "version.h"

#if BV_ARCH_X86
#   include "x86/emms.h"
#endif

#ifndef emms_c
#   define emms_c()
#endif

#ifndef attribute_align_arg
#if BV_ARCH_X86_32 && BV_GCC_VERSION_AT_LEAST(4,2)
#    define attribute_align_arg __attribute__((force_align_arg_pointer))
#else
#    define attribute_align_arg
#endif
#endif

#if defined(_MSC_VER) && BV_CONFIG_SHARED
#    define bv_export __declspec(dllimport)
#else
#    define bv_export
#endif

#if BV_HAVE_PRAGMA_DEPRECATED
#    if defined(__ICL) || defined (__INTEL_COMPILER)
#        define BV_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable:1478))
#        define BV_ENABLE_DEPRECATION_WARNINGS  __pragma(warning(pop))
#    elif defined(_MSC_VER)
#        define BV_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable:4996))
#        define BV_ENABLE_DEPRECATION_WARNINGS  __pragma(warning(pop))
#    else
#        define BV_DISABLE_DEPRECATION_WARNINGS _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#        define BV_ENABLE_DEPRECATION_WARNINGS  _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
#    endif
#else
#    define BV_DISABLE_DEPRECATION_WARNINGS
#    define BV_ENABLE_DEPRECATION_WARNINGS
#endif


#define BV_MEMORY_POISON 0x2a

#define MAKE_ACCESSORS(str, name, type, field) \
    type bv_##name##_get_##field(const str *s) { return s->field; } \
    void bv_##name##_set_##field(str *s, type v) { s->field = v; }

// Some broken preprocessors need a second expansion
// to be forced to tokenize __VA_ARGS__
#define E1(x) x

#define BV_CONTAINER_OF(ptr_, type_, member_)  \
    ((type_ *)((char *)ptr_ - offsetof(type_, member_)))

/* Check if the hard coded offset of a struct member still matches reality.
 * Induce a compilation failure if not.
 */
#define BV_CHECK_OFFSET(s, m, o) struct check_##o {    \
        int x_##o[offsetof(s, m) == o? 1: -1];         \
    }

#define LOCAL_ALIGNED_A(a, t, v, s, o, ...)             \
    uint8_t la_##v[sizeof(t s o) + (a)];                \
    t (*v) o = (void *)BBALIGN((uintptr_t)la_##v, a)

#define LOCAL_ALIGNED_D(a, t, v, s, o, ...)             \
    BV_DECLARE_ALIGNED(a, t, la_##v) s o;                  \
    t (*v) o = la_##v

#define LOCAL_ALIGNED(a, t, v, ...) E1(LOCAL_ALIGNED_A(a, t, v, __VA_ARGS__,,))

#if BV_HAVE_LOCAL_ALIGNED_8
#   define LOCAL_ALIGNED_8(t, v, ...) E1(LOCAL_ALIGNED_D(8, t, v, __VA_ARGS__,,))
#else
#   define LOCAL_ALIGNED_8(t, v, ...) LOCAL_ALIGNED(8, t, v, __VA_ARGS__)
#endif

#if BV_HAVE_LOCAL_ALIGNED_16
#   define LOCAL_ALIGNED_16(t, v, ...) E1(LOCAL_ALIGNED_D(16, t, v, __VA_ARGS__,,))
#else
#   define LOCAL_ALIGNED_16(t, v, ...) LOCAL_ALIGNED(16, t, v, __VA_ARGS__)
#endif

#if BV_HAVE_LOCAL_ALIGNED_32
#   define LOCAL_ALIGNED_32(t, v, ...) E1(LOCAL_ALIGNED_D(32, t, v, __VA_ARGS__,,))
#else
#   define LOCAL_ALIGNED_32(t, v, ...) LOCAL_ALIGNED(32, t, v, __VA_ARGS__)
#endif

#define BV_ALLOC_OR_GOTO(ctx, p, size, label)\
{\
    p = bv_malloc(size);\
    if (!(p) && (size) != 0) {\
        bv_log(ctx, BV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#define BV_ALLOCZ_OR_GOTO(ctx, p, size, label)\
{\
    p = bv_mallocz(size);\
    if (!(p) && (size) != 0) {\
        bv_log(ctx, BV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#define BV_ALLOC_ARRAY_OR_GOTO(ctx, p, nelem, elsize, label)\
{\
    p = bv_malloc_array(nelem, elsize);\
    if (!p) {\
        bv_log(ctx, BV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#define BV_ALLOCZ_ARRAY_OR_GOTO(ctx, p, nelem, elsize, label)\
{\
    p = bv_mallocz_array(nelem, elsize);\
    if (!p) {\
        bv_log(ctx, BV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#include "libm.h"

#if defined(_MSC_VER)
#pragma comment(linker, "/include:"EXTERN_PREFIX"bvpriv_strtod")
#pragma comment(linker, "/include:"EXTERN_PREFIX"bvpriv_snprintf")
#endif

/**
 * Return NULL if BV_CONFIG_SMALL is true, otherwise the argument
 * without modification. Used to disable the definition of strings
 * (for example BVCodec long_names).
 */
#if BV_CONFIG_SMALL
#   define NULL_IF_BV_CONFIG_SMALL(x) NULL
#else
#   define NULL_IF_BV_CONFIG_SMALL(x) x
#endif

/**
 * Define a function with only the non-default version specified.
 *
 * On systems with ELF shared libraries, all symbols exported from
 * BVBase libraries are tagged with the name and major version of the
 * library to which they belong.  If a function is moved from one
 * library to another, a wrapper must be retained in the original
 * location to preserve binary compatibility.
 *
 * Functions defined with this macro will never be used to resolve
 * symbols by the build-time linker.
 *
 * @param type return type of function
 * @param name name of function
 * @param args argument list of function
 * @param ver  version tag to assign function
 */
#if BV_HAVE_SYMVER_ASM_LABEL
#   define BV_SYMVER(type, name, args, ver)                     \
    type bb_##name args __asm__ (EXTERN_PREFIX #name "@" ver);  \
    type bb_##name args
#elif BV_HAVE_SYMVER_GNU_ASM
#   define BV_SYMVER(type, name, args, ver)                             \
    __asm__ (".symver bb_" #name "," EXTERN_PREFIX #name "@" ver);      \
    type bb_##name args;                                                \
    type bb_##name args
#endif

/**
 * Return NULL if a threading library has not been enabled.
 * Used to disable threading functions in BVCodec definitions
 * when not needed.
 */
#if BV_HAVE_THREADS
#   define ONLY_IF_THREADS_ENABLED(x) x
#else
#   define ONLY_IF_THREADS_ENABLED(x) NULL
#endif

/**
 * Log a generic warning message about a missing feature.
 *
 * @param[in] avc a pointer to an arbitrary struct of which the first
 *                field is a pointer to an BVClass struct
 * @param[in] msg string containing the name of the missing feature
 */
void bvpriv_report_missing_feature(void *avc,
                                   const char *msg, ...) bv_printf_format(2, 3);

/**
 * Log a generic warning message about a missing feature.
 * Additionally request that a sample showcasing the feature be uploaded.
 *
 * @param[in] avc a pointer to an arbitrary struct of which the first field is
 *                a pointer to an BVClass struct
 * @param[in] msg string containing the name of the missing feature
 */
void bvpriv_request_sample(void *avc,
                           const char *msg, ...) bv_printf_format(2, 3);

#if BV_HAVE_LIBC_MSVCRT
#define bvpriv_open bb_open
#define PTRDIBV_SPECIFIER "Id"
#define SIZE_SPECIFIER "Iu"
#else
#define PTRDIBV_SPECIFIER "td"
#define SIZE_SPECIFIER "zu"
#endif

/**
 * A wrapper for open() setting O_CLOEXEC.
 */
int bvpriv_open(const char *filename, int flags, ...);

int bvpriv_set_systematic_pal2(uint32_t pal[256], enum BVPixelFormat pix_fmt);

#if BV_API_GET_CHANNEL_LAYOUT_COMPAT
uint64_t bb_get_channel_layout(const char *name, int compat);
#endif

#endif /* BVUTIL_INTERNAL_H */
