/*
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

#include "config.h"
#include "bvutil.h"
#include "bvassert.h"
#include "samplefmt.h"
#include "pixdesc.h"

/**
 * @file
 * various utility functions
 */

#include "libbvutil/bbversion.h"
const char bv_util_ffversion[] = "BVBase version " BVBASE_VERSION;

unsigned bvutil_version(void)
{
    static int checks_done;
    if (checks_done)
        return LIBBVUTIL_VERSION_INT;

    bv_assert0(BV_PIX_FMT_VDA_VLD == 81); //check if the pix fmt enum has not had anything inserted or removed by mistake
    bv_assert0(BV_SAMPLE_FMT_DBLP == 9);
    bv_assert0(BV_MEDIA_TYPE_ATTACHMENT == 4);
    bv_assert0(BV_PICTURE_TYPE_BI == 7);
    bv_assert0(LIBBVUTIL_VERSION_MICRO >= 100);
    bv_assert0(BV_HAVE_MMX2 == BV_HAVE_MMXEXT);

    bv_assert0(((size_t)-1) > 0); // C guarantees this but if false on a platform we care about revert at least b284e1ffe343d6697fb950d1ee517bafda8a9844

    if (bv_sat_dadd32(1, 2) != 5) {
        bv_log(NULL, BV_LOG_FATAL, "Libbvutil has been build with a broken binutils, please upgrade binutils and rebuild\n");
        abort();
    }

    if (llrint(1LL<<60) != 1LL<<60) {
        bv_log(NULL, BV_LOG_ERROR, "Libbvutil has been linked to a broken llrint()\n");
    }

#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 0
    bb_check_pixfmt_descriptors();
#endif
    checks_done = 1;
    return LIBBVUTIL_VERSION_INT;
}

const char *bvutil_configuration(void)
{
    return BVBASE_CONFIGURATION;
}

const char *bvutil_license(void)
{
#define LICENSE_PREFIX "libbvutil license: "
    return LICENSE_PREFIX BVBASE_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}

const char *bv_get_media_type_string(enum BVMediaType media_type)
{
    switch (media_type) {
    case BV_MEDIA_TYPE_VIDEO:      return "video";
    case BV_MEDIA_TYPE_AUDIO:      return "audio";
    case BV_MEDIA_TYPE_DATA:       return "data";
    case BV_MEDIA_TYPE_SUBTITLE:   return "subtitle";
    case BV_MEDIA_TYPE_ATTACHMENT: return "attachment";
    default:                      return NULL;
    }
}

char bv_get_picture_type_char(enum BVPictureType pict_type)
{
    switch (pict_type) {
    case BV_PICTURE_TYPE_I:  return 'I';
    case BV_PICTURE_TYPE_P:  return 'P';
    case BV_PICTURE_TYPE_B:  return 'B';
    case BV_PICTURE_TYPE_S:  return 'S';
    case BV_PICTURE_TYPE_SI: return 'i';
    case BV_PICTURE_TYPE_SP: return 'p';
    case BV_PICTURE_TYPE_BI: return 'b';
    default:                 return '?';
    }
}

unsigned bv_int_list_length_for_size(unsigned elsize,
                                     const void *list, uint64_t term)
{
    unsigned i;

    if (!list)
        return 0;
#define LIST_LENGTH(type) \
    { type t = term, *l = (type *)list; for (i = 0; l[i] != t; i++); }
    switch (elsize) {
    case 1: LIST_LENGTH(uint8_t);  break;
    case 2: LIST_LENGTH(uint16_t); break;
    case 4: LIST_LENGTH(uint32_t); break;
    case 8: LIST_LENGTH(uint64_t); break;
    default: bv_assert0(!"valid element size");
    }
    return i;
}

BVRational bv_get_time_base_q(void)
{
    return (BVRational){1, BV_TIME_BASE};
}
