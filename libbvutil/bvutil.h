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

#ifndef BVUTIL_BVUTIL_H
#define BVUTIL_BVUTIL_H

/**
 * @file
 * external API header
 */

/**
 * @mainpage
 *
 * @section ffmpeg_intro Introduction
 *
 * This document describes the usage of the different libraries
 * provided by BVBase.
 *
 * @li @ref libavc "libavcodec" encoding/decoding library
 * @li @ref lavfi "libavfilter" graph-based frame editing library
 * @li @ref libavf "libavformat" I/O and muxing/demuxing library
 * @li @ref lavd "libavdevice" special devices muxing/demuxing library
 * @li @ref lavu "libbvutil" common utility library
 * @li @ref lswr "libswresample" audio resampling, format conversion and mixing
 * @li @ref lpp  "libpostproc" post processing library
 * @li @ref libsws "libswscale" color conversion and scaling library
 *
 * @section ffmpeg_versioning Versioning and compatibility
 *
 * Each of the BVBase libraries contains a version.h header, which defines a
 * major, minor and micro version number with the
 * <em>LIBRARYNAME_VERSION_{MAJOR,MINOR,MICRO}</em> macros. The major version
 * number is incremented with backward incompatible changes - e.g. removing
 * parts of the public API, reordering public struct members, etc. The minor
 * version number is incremented for backward compatible API changes or major
 * new features - e.g. adding a new public function or a new decoder. The micro
 * version number is incremented for smaller changes that a calling program
 * might still want to check for - e.g. changing behavior in a previously
 * unspecified situation.
 *
 * BVBase guarantees backward API and ABI compatibility for each library as long
 * as its major version number is unchanged. This means that no public symbols
 * will be removed or renamed. Types and names of the public struct members and
 * values of public macros and enums will remain the same (unless they were
 * explicitly declared as not part of the public API). Documented behavior will
 * not change.
 *
 * In other words, any correct program that works with a given BVBase snapshot
 * should work just as well without any changes with any later snapshot with the
 * same major versions. This applies to both rebuilding the program against new
 * BVBase versions or to replacing the dynamic BVBase libraries that a program
 * links against.
 *
 * However, new public symbols may be added and new members may be appended to
 * public structs whose size is not part of public ABI (most public structs in
 * BVBase). New macros and enum values may be added. Behavior in undocumented
 * situations may change slightly (and be documented). All those are accompanied
 * by an entry in doc/APIchanges and incrementing either the minor or micro
 * version number.
 */

/**
 * @defgroup lavu Common utility functions
 *
 * @brief
 * libbvutil contains the code shared across all the other BVBase
 * libraries
 *
 * @note In order to use the functions provided by bvutil you must include
 * the specific header.
 *
 * @{
 *
 * @defgroup lavu_crypto Crypto and Hashing
 *
 * @{
 * @}
 *
 * @defgroup lavu_math Maths
 * @{
 *
 * @}
 *
 * @defgroup lavu_string String Manipulation
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_mem Memory Management
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_data Data Structures
 * @{
 *
 * @}
 *
 * @defgroup lavu_audio Audio related
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_error Error Codes
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_log Logging Facility
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_misc Other
 *
 * @{
 *
 * @defgroup lavu_internal Internal
 *
 * Not exported functions, for internal usage only
 *
 * @{
 *
 * @}
 *
 * @defgroup preproc_misc Preprocessor String Macros
 *
 * @{
 *
 * @}
 *
 * @defgroup version_utils Library Version Macros
 *
 * @{
 *
 * @}
 */


/**
 * @addtogroup lavu_ver
 * @{
 */

/**
 * Return the LIBBVUTIL_VERSION_INT constant.
 */
unsigned bvutil_version(void);

/**
 * Return the libbvutil build-time configuration.
 */
const char *bvutil_configuration(void);

/**
 * Return the libbvutil license.
 */
const char *bvutil_license(void);

/**
 * @}
 */

/**
 * @addtogroup lavu_media Media Type
 * @brief Media Type
 */

enum BVMediaType {
    BVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as BVMEDIA_TYPE_DATA
    BVMEDIA_TYPE_VIDEO,
    BVMEDIA_TYPE_AUDIO,
    BVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    BVMEDIA_TYPE_SUBTITLE,
    BVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    BVMEDIA_TYPE_NB
};

/**
 * Return a string describing the media_type enum, NULL if media_type
 * is unknown.
 */
const char *bv_get_media_type_string(enum BVMediaType media_type);

/**
 * @defgroup lavu_const Constants
 * @{
 *
 * @defgroup lavu_enc Encoding specific
 *
 * @note those definition should move to avcodec
 * @{
 */

#define FF_LAMBDA_SHIFT 7
#define FF_LAMBDA_SCALE (1<<FF_LAMBDA_SHIFT)
#define FF_QP2LAMBDA 118 ///< factor to convert from H.263 QP to lambda
#define FF_LAMBDA_MAX (256*128-1)

#define FF_QUALITY_SCALE FF_LAMBDA_SCALE //FIXME maybe remove

/**
 * @}
 * @defgroup lavu_time Timestamp specific
 *
 * BVBase internal timebase and timestamp definitions
 *
 * @{
 */

/**
 * @brief Undefined timestamp value
 *
 * Usually reported by demuxer that work on containers that do not provide
 * either pts or dts.
 */

#define BV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

/**
 * Internal time base represented as integer
 */

#define BV_TIME_BASE            1000000

/**
 * Internal time base represented as fractional value
 */

#define BV_TIME_BASE_Q          (BVRational){1, BV_TIME_BASE}

/**
 * @}
 * @}
 * @defgroup lavu_picture Image related
 *
 * BVPicture types, pixel formats and basic image planes manipulation.
 *
 * @{
 */

enum BVPictureType {
    BV_PICTURE_TYPE_NONE = 0, ///< Undefined
    BV_PICTURE_TYPE_I,     ///< Intra
    BV_PICTURE_TYPE_P,     ///< Predicted
    BV_PICTURE_TYPE_B,     ///< Bi-dir predicted
    BV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG4
    BV_PICTURE_TYPE_SI,    ///< Switching Intra
    BV_PICTURE_TYPE_SP,    ///< Switching Predicted
    BV_PICTURE_TYPE_BI,    ///< BI type
};

typedef struct _BVControlPacket {
    int size;
    void *data;
} BVControlPacket;

/**
 * Return a single letter to describe the given picture type
 * pict_type.
 *
 * @param[in] pict_type the picture type @return a single character
 * representing the picture type, '?' if pict_type is unknown
 */
char bv_get_picture_type_char(enum BVPictureType pict_type);

/**
 * @}
 */

#include "common.h"
#include "error.h"
#include "rational.h"
#include "version.h"
#include "macros.h"
#include "mathematics.h"
#include "log.h"
#include "pixfmt.h"

/**
 * Return x default pointer in case p is NULL.
 */
static inline void *bv_x_if_null(const void *p, const void *x)
{
    return (void *)(intptr_t)(p ? p : x);
}

/**
 * Compute the length of an integer list.
 *
 * @param elsize  size in bytes of each list element (only 1, 2, 4 or 8)
 * @param term    list terminator (usually 0 or -1)
 * @param list    pointer to the list
 * @return  length of the list, in elements, not counting the terminator
 */
unsigned bv_int_list_length_for_size(unsigned elsize,
                                     const void *list, uint64_t term) bv_pure;

/**
 * Compute the length of an integer list.
 *
 * @param term  list terminator (usually 0 or -1)
 * @param list  pointer to the list
 * @return  length of the list, in elements, not counting the terminator
 */
#define bv_int_list_length(list, term) \
    bv_int_list_length_for_size(sizeof(*(list)), list, term)

/**
 * Open a file using a UTF-8 filename.
 * The API of this function matches POSIX fopen(), errors are returned through
 * errno.
 */
FILE *bv_fopen_utf8(const char *path, const char *mode);

/**
 * Return the fractional representation of the internal time base.
 */
BVRational bv_get_time_base_q(void);

/**
 * @}
 * @}
 */

#endif /* BVUTIL_BVUTIL_H */
