/*
 * copyright (c) 2003 Fabrice Bellard
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

#ifndef BVUTIL_VERSION_H
#define BVUTIL_VERSION_H

#include "macros.h"

/**
 * @addtogroup version_utils
 *
 * Useful to check and match library version in order to maintain
 * backward compatibility.
 *
 * @{
 */

#define BV_VERSION_INT(a, b, c) (a<<16 | b<<8 | c)
#define BV_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define BV_VERSION(a, b, c) BV_VERSION_DOT(a, b, c)

/**
 * @}
 */

/**
 * @file
 * @ingroup lavu
 * Libbvutil version macros
 */

/**
 * @defgroup lavu_ver Version and Build diagnostics
 *
 * Macros and function useful to check at compiletime and at runtime
 * which version of libbvutil is in use.
 *
 * @{
 */

#define LIBBVUTIL_VERSION_MAJOR  0
#define LIBBVUTIL_VERSION_MINOR  0
#define LIBBVUTIL_VERSION_MICRO  1

#define LIBBVUTIL_VERSION_INT   BV_VERSION_INT(LIBBVUTIL_VERSION_MAJOR, \
                                               LIBBVUTIL_VERSION_MINOR, \
                                               LIBBVUTIL_VERSION_MICRO)
#define LIBBVUTIL_VERSION       BV_VERSION(LIBBVUTIL_VERSION_MAJOR,     \
                                           LIBBVUTIL_VERSION_MINOR,     \
                                           LIBBVUTIL_VERSION_MICRO)
#define LIBBVUTIL_BUILD         LIBBVUTIL_VERSION_INT

#define LIBBVUTIL_IDENT         "Lavu" BV_STRINGIFY(LIBBVUTIL_VERSION)

/**
 * @}
 *
 * @defgroup depr_guards Deprecation guards
 * BV_API_* defines may be placed below to indicate public API that will be
 * dropped at a future version bump. The defines themselves are not part of
 * the public API and may change, break or disappear at any time.
 *
 * @{
 */

#ifndef BV_API_OLD_BVOPTIONS
#define BV_API_OLD_BVOPTIONS            (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_PIX_FMT
#define BV_API_PIX_FMT                  (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_CONTEXT_SIZE
#define BV_API_CONTEXT_SIZE             (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_PIX_FMT_DESC
#define BV_API_PIX_FMT_DESC             (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_BV_REVERSE
#define BV_API_BV_REVERSE               (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_AUDIOCONVERT
#define BV_API_AUDIOCONVERT             (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_CPU_FLAG_MMX2
#define BV_API_CPU_FLAG_MMX2            (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_LLS_PRIVATE
#define BV_API_LLS_PRIVATE              (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_BVFRAME_LBVC
#define BV_API_BVFRAME_LBVC             (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_VDPAU
#define BV_API_VDPAU                    (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_GET_CHANNEL_LAYOUT_COMPAT
#define BV_API_GET_CHANNEL_LAYOUT_COMPAT (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_XVMC
#define BV_API_XVMC                     (LIBBVUTIL_VERSION_MAJOR < 55)
#endif
#ifndef BV_API_OPT_TYPE_METADATA
#define BV_API_OPT_TYPE_METADATA        (LIBBVUTIL_VERSION_MAJOR < 55)
#endif

#ifndef BV_CONST_BVUTIL55
#if LIBBVUTIL_VERSION_MAJOR >= 55
#define BV_CONST_BVUTIL55 const
#else
#define BV_CONST_BVUTIL55
#endif
#endif

/**
 * @}
 */

#endif /* BVUTIL_VERSION_H */

