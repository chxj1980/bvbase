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

/**
 * @file
 * misc image utilities
 */

#include "bvassert.h"
#include "common.h"
#include "imgutils.h"
#include "internal.h"
#include "intreadwrite.h"
#include "log.h"
#include "mathematics.h"
#include "pixdesc.h"
#include "rational.h"

void bv_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4],
                                const BVPixFmtDescriptor *pixdesc)
{
    int i;
    memset(max_pixsteps, 0, 4*sizeof(max_pixsteps[0]));
    if (max_pixstep_comps)
        memset(max_pixstep_comps, 0, 4*sizeof(max_pixstep_comps[0]));

    for (i = 0; i < 4; i++) {
        const BVComponentDescriptor *comp = &(pixdesc->comp[i]);
        if ((comp->step_minus1+1) > max_pixsteps[comp->plane]) {
            max_pixsteps[comp->plane] = comp->step_minus1+1;
            if (max_pixstep_comps)
                max_pixstep_comps[comp->plane] = i;
        }
    }
}

static inline
int image_get_linesize(int width, int plane,
                       int max_step, int max_step_comp,
                       const BVPixFmtDescriptor *desc)
{
    int s, shifted_w, linesize;

    if (!desc)
        return BVERROR(EINVAL);

    if (width < 0)
        return BVERROR(EINVAL);
    s = (max_step_comp == 1 || max_step_comp == 2) ? desc->log2_chroma_w : 0;
    shifted_w = ((width + (1 << s) - 1)) >> s;
    if (shifted_w && max_step > INT_MAX / shifted_w)
        return BVERROR(EINVAL);
    linesize = max_step * shifted_w;

    if (desc->flags & BV_PIX_FMT_FLAG_BITSTREAM)
        linesize = (linesize + 7) >> 3;
    return linesize;
}

int bv_image_get_linesize(enum BVPixelFormat pix_fmt, int width, int plane)
{
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    int max_step     [4];       /* max pixel step for each plane */
    int max_step_comp[4];       /* the component for each plane which has the max pixel step */

    if ((unsigned)pix_fmt >= BV_PIX_FMT_NB || desc->flags & BV_PIX_FMT_FLAG_HWACCEL)
        return BVERROR(EINVAL);

    bv_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    return image_get_linesize(width, plane, max_step[plane], max_step_comp[plane], desc);
}

int bv_image_fill_linesizes(int linesizes[4], enum BVPixelFormat pix_fmt, int width)
{
    int i, ret;
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    int max_step     [4];       /* max pixel step for each plane */
    int max_step_comp[4];       /* the component for each plane which has the max pixel step */

    memset(linesizes, 0, 4*sizeof(linesizes[0]));

    if (!desc || desc->flags & BV_PIX_FMT_FLAG_HWACCEL)
        return BVERROR(EINVAL);

    bv_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    for (i = 0; i < 4; i++) {
        if ((ret = image_get_linesize(width, i, max_step[i], max_step_comp[i], desc)) < 0)
            return ret;
        linesizes[i] = ret;
    }

    return 0;
}

int bv_image_fill_pointers(uint8_t *data[4], enum BVPixelFormat pix_fmt, int height,
                           uint8_t *ptr, const int linesizes[4])
{
    int i, total_size, size[4] = { 0 }, has_plane[4] = { 0 };

    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    memset(data     , 0, sizeof(data[0])*4);

    if (!desc || desc->flags & BV_PIX_FMT_FLAG_HWACCEL)
        return BVERROR(EINVAL);

    data[0] = ptr;
    if (linesizes[0] > (INT_MAX - 1024) / height)
        return BVERROR(EINVAL);
    size[0] = linesizes[0] * height;

    if (desc->flags & BV_PIX_FMT_FLAG_PAL ||
        desc->flags & BV_PIX_FMT_FLAG_PSEUDOPAL) {
        size[0] = (size[0] + 3) & ~3;
        data[1] = ptr + size[0]; /* palette is stored here as 256 32 bits words */
        return size[0] + 256 * 4;
    }

    for (i = 0; i < 4; i++)
        has_plane[desc->comp[i].plane] = 1;

    total_size = size[0];
    for (i = 1; i < 4 && has_plane[i]; i++) {
        int h, s = (i == 1 || i == 2) ? desc->log2_chroma_h : 0;
        data[i] = data[i-1] + size[i-1];
        h = (height + (1 << s) - 1) >> s;
        if (linesizes[i] > INT_MAX / h)
            return BVERROR(EINVAL);
        size[i] = h * linesizes[i];
        if (total_size > INT_MAX - size[i])
            return BVERROR(EINVAL);
        total_size += size[i];
    }

    return total_size;
}

int bvpriv_set_systematic_pal2(uint32_t pal[256], enum BVPixelFormat pix_fmt)
{
    int i;

    for (i = 0; i < 256; i++) {
        int r, g, b;

        switch (pix_fmt) {
        case BV_PIX_FMT_RGB8:
            r = (i>>5    )*36;
            g = ((i>>2)&7)*36;
            b = (i&3     )*85;
            break;
        case BV_PIX_FMT_BGR8:
            b = (i>>6    )*85;
            g = ((i>>3)&7)*36;
            r = (i&7     )*36;
            break;
        case BV_PIX_FMT_RGB4_BYTE:
            r = (i>>3    )*255;
            g = ((i>>1)&3)*85;
            b = (i&1     )*255;
            break;
        case BV_PIX_FMT_BGR4_BYTE:
            b = (i>>3    )*255;
            g = ((i>>1)&3)*85;
            r = (i&1     )*255;
            break;
        case BV_PIX_FMT_GRAY8:
            r = b = g = i;
            break;
        default:
            return BVERROR(EINVAL);
        }
        pal[i] = b + (g << 8) + (r << 16) + (0xFFU << 24);
    }

    return 0;
}

int bv_image_alloc(uint8_t *pointers[4], int linesizes[4],
                   int w, int h, enum BVPixelFormat pix_fmt, int align)
{
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    int i, ret;
    uint8_t *buf;

    if (!desc)
        return BVERROR(EINVAL);

    if ((ret = bv_image_check_size(w, h, 0, NULL)) < 0)
        return ret;
    if ((ret = bv_image_fill_linesizes(linesizes, pix_fmt, align>7 ? BBALIGN(w, 8) : w)) < 0)
        return ret;

    for (i = 0; i < 4; i++)
        linesizes[i] = BBALIGN(linesizes[i], align);

    if ((ret = bv_image_fill_pointers(pointers, pix_fmt, h, NULL, linesizes)) < 0)
        return ret;
    buf = bv_malloc(ret + align);
    if (!buf)
        return BVERROR(ENOMEM);
    if ((ret = bv_image_fill_pointers(pointers, pix_fmt, h, buf, linesizes)) < 0) {
        bv_free(buf);
        return ret;
    }
    if (desc->flags & BV_PIX_FMT_FLAG_PAL || desc->flags & BV_PIX_FMT_FLAG_PSEUDOPAL)
        bvpriv_set_systematic_pal2((uint32_t*)pointers[1], pix_fmt);

    return ret;
}

typedef struct ImgUtils {
    const BVClass *class;
    int   log_offset;
    void *log_ctx;
} ImgUtils;

static const BVClass imgutils_class = { "IMGUTILS", bv_default_item_name, NULL, LIBBVUTIL_VERSION_INT, offsetof(ImgUtils, log_offset), offsetof(ImgUtils, log_ctx) };

int bv_image_check_size(unsigned int w, unsigned int h, int log_offset, void *log_ctx)
{
    ImgUtils imgutils = { &imgutils_class, log_offset, log_ctx };

    if ((int)w>0 && (int)h>0 && (w+128)*(uint64_t)(h+128) < INT_MAX/8)
        return 0;

    bv_log(&imgutils, BV_LOG_ERROR, "Picture size %ux%u is invalid\n", w, h);
    return BVERROR(EINVAL);
}

int bv_image_check_sar(unsigned int w, unsigned int h, BVRational sar)
{
    int64_t scaled_dim;

    if (!sar.den)
        return BVERROR(EINVAL);

    if (!sar.num || sar.num == sar.den)
        return 0;

    if (sar.num < sar.den)
        scaled_dim = bv_rescale_rnd(w, sar.num, sar.den, BV_ROUND_ZERO);
    else
        scaled_dim = bv_rescale_rnd(h, sar.den, sar.num, BV_ROUND_ZERO);

    if (scaled_dim > 0)
        return 0;

    return BVERROR(EINVAL);
}

void bv_image_copy_plane(uint8_t       *dst, int dst_linesize,
                         const uint8_t *src, int src_linesize,
                         int bytewidth, int height)
{
    if (!dst || !src)
        return;
    bv_assert0(abs(src_linesize) >= bytewidth);
    bv_assert0(abs(dst_linesize) >= bytewidth);
    for (;height > 0; height--) {
        memcpy(dst, src, bytewidth);
        dst += dst_linesize;
        src += src_linesize;
    }
}

void bv_image_copy(uint8_t *dst_data[4], int dst_linesizes[4],
                   const uint8_t *src_data[4], const int src_linesizes[4],
                   enum BVPixelFormat pix_fmt, int width, int height)
{
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);

    if (!desc || desc->flags & BV_PIX_FMT_FLAG_HWACCEL)
        return;

    if (desc->flags & BV_PIX_FMT_FLAG_PAL ||
        desc->flags & BV_PIX_FMT_FLAG_PSEUDOPAL) {
        bv_image_copy_plane(dst_data[0], dst_linesizes[0],
                            src_data[0], src_linesizes[0],
                            width, height);
        /* copy the palette */
        memcpy(dst_data[1], src_data[1], 4*256);
    } else {
        int i, planes_nb = 0;

        for (i = 0; i < desc->nb_components; i++)
            planes_nb = BBMAX(planes_nb, desc->comp[i].plane + 1);

        for (i = 0; i < planes_nb; i++) {
            int h = height;
            int bwidth = bv_image_get_linesize(pix_fmt, width, i);
            if (bwidth < 0) {
                bv_log(NULL, BV_LOG_ERROR, "bv_image_get_linesize failed\n");
                return;
            }
            if (i == 1 || i == 2) {
                h = BV_CEIL_RSHIFT(height, desc->log2_chroma_h);
            }
            bv_image_copy_plane(dst_data[i], dst_linesizes[i],
                                src_data[i], src_linesizes[i],
                                bwidth, h);
        }
    }
}

int bv_image_fill_arrays(uint8_t *dst_data[4], int dst_linesize[4],
                         const uint8_t *src,
                         enum BVPixelFormat pix_fmt, int width, int height, int align)
{
    int ret, i;

    if ((ret = bv_image_check_size(width, height, 0, NULL)) < 0)
        return ret;

    if ((ret = bv_image_fill_linesizes(dst_linesize, pix_fmt, width)) < 0)
        return ret;

    for (i = 0; i < 4; i++)
        dst_linesize[i] = BBALIGN(dst_linesize[i], align);

    return bv_image_fill_pointers(dst_data, pix_fmt, height, (uint8_t *)src, dst_linesize);
}

int bv_image_get_buffer_size(enum BVPixelFormat pix_fmt, int width, int height, int align)
{
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    uint8_t *data[4];
    int linesize[4];

    if (!desc)
        return BVERROR(EINVAL);
    if (bv_image_check_size(width, height, 0, NULL) < 0)
        return BVERROR(EINVAL);
    if (desc->flags & BV_PIX_FMT_FLAG_PSEUDOPAL)
        // do not include palette for these pseudo-paletted formats
        return width * height;
    return bv_image_fill_arrays(data, linesize, NULL, pix_fmt, width, height, align);
}

int bv_image_copy_to_buffer(uint8_t *dst, int dst_size,
                            const uint8_t * const src_data[4], const int src_linesize[4],
                            enum BVPixelFormat pix_fmt, int width, int height, int align)
{
    int i, j, nb_planes = 0, linesize[4];
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(pix_fmt);
    int size = bv_image_get_buffer_size(pix_fmt, width, height, align);

    if (size > dst_size || size < 0)
        return BVERROR(EINVAL);

    for (i = 0; i < desc->nb_components; i++)
        nb_planes = BBMAX(desc->comp[i].plane, nb_planes);
    nb_planes++;

    bv_image_fill_linesizes(linesize, pix_fmt, width);
    for (i = 0; i < nb_planes; i++) {
        int h, shift = (i == 1 || i == 2) ? desc->log2_chroma_h : 0;
        const uint8_t *src = src_data[i];
        h = (height + (1 << shift) - 1) >> shift;

        for (j = 0; j < h; j++) {
            memcpy(dst, src, linesize[i]);
            dst += BBALIGN(linesize[i], align);
            src += src_linesize[i];
        }
    }

    if (desc->flags & BV_PIX_FMT_FLAG_PAL) {
        uint32_t *d32 = (uint32_t *)(((size_t)dst + 3) & ~3);
        for (i = 0; i<256; i++)
            BV_WL32(d32 + i, BV_RN32(src_data[1] + 4*i));
    }

    return size;
}
