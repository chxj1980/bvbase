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
 * Macro definitions for various function/variable attributes
 */

#ifndef BVUTIL_ATTRIBUTES_H
#define BVUTIL_ATTRIBUTES_H

#ifdef __GNUC__
#    define BV_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#else
#    define BV_GCC_VERSION_AT_LEAST(x,y) 0
#endif

#ifndef bv_always_inline
#if BV_GCC_VERSION_AT_LEAST(3,1)
#    define bv_always_inline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define bv_always_inline __forceinline
#else
#    define bv_always_inline inline
#endif
#endif

#ifndef bv_extern_inline
#if defined(__ICL) && __ICL >= 1210 || defined(__GNUC_STDC_INLINE__)
#    define bv_extern_inline extern inline
#else
#    define bv_extern_inline inline
#endif
#endif

#if BV_GCC_VERSION_AT_LEAST(3,1)
#    define bv_noinline __attribute__((noinline))
#elif defined(_MSC_VER)
#    define bv_noinline __declspec(noinline)
#else
#    define bv_noinline
#endif

#if BV_GCC_VERSION_AT_LEAST(3,1)
#    define bv_pure __attribute__((pure))
#else
#    define bv_pure
#endif

#if BV_GCC_VERSION_AT_LEAST(2,6)
#    define bv_const __attribute__((const))
#else
#    define bv_const
#endif

#if BV_GCC_VERSION_AT_LEAST(4,3)
#    define bv_cold __attribute__((cold))
#else
#    define bv_cold
#endif

#if BV_GCC_VERSION_AT_LEAST(4,1) && !defined(__llvm__)
#    define bv_flatten __attribute__((flatten))
#else
#    define bv_flatten
#endif

#if BV_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif

/**
 * Disable warnings about deprecated features
 * This is useful for sections of code kept for backward compatibility and
 * scheduled for removal.
 */
#ifndef BV_NOWARN_DEPRECATED
#if BV_GCC_VERSION_AT_LEAST(4,6)
#    define BV_NOWARN_DEPRECATED(code) \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"") \
        code \
        _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#    define BV_NOWARN_DEPRECATED(code) \
        __pragma(warning(push)) \
        __pragma(warning(disable : 4996)) \
        code; \
        __pragma(warning(pop))
#else
#    define BV_NOWARN_DEPRECATED(code) code
#endif
#endif


#if defined(__GNUC__)
#    define bv_unused __attribute__((unused))
#else
#    define bv_unused
#endif

/**
 * Mark a variable as used and prevent the compiler from optimizing it
 * away.  This is useful for variables accessed only from inline
 * assembler without the compiler being aware.
 */
#if BV_GCC_VERSION_AT_LEAST(3,1)
#    define bv_used __attribute__((used))
#else
#    define bv_used
#endif

#if BV_GCC_VERSION_AT_LEAST(3,3)
#   define bv_alias __attribute__((may_alias))
#else
#   define bv_alias
#endif

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#    define bv_uninit(x) x=x
#else
#    define bv_uninit(x) x
#endif

#ifdef __GNUC__
#    define bv_builtin_constant_p __builtin_constant_p
#    define bv_printf_format(fmtpos, attrpos) __attribute__((__format__(__printf__, fmtpos, attrpos)))
#else
#    define bv_builtin_constant_p(x) 0
#    define bv_printf_format(fmtpos, attrpos)
#endif

#if BV_GCC_VERSION_AT_LEAST(2,5)
#    define bv_noreturn __attribute__((noreturn))
#else
#    define bv_noreturn
#endif

#endif /* BVUTIL_ATTRIBUTES_H */
