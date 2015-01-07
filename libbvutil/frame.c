/*
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

#include "channel_layout.h"
#include "avassert.h"
#include "buffer.h"
#include "common.h"
#include "dict.h"
#include "frame.h"
#include "imgutils.h"
#include "mem.h"
#include "samplefmt.h"

MAKE_ACCESSORS(BVFrame, frame, int64_t, best_effort_timestamp)
MAKE_ACCESSORS(BVFrame, frame, int64_t, pkt_duration)
MAKE_ACCESSORS(BVFrame, frame, int64_t, pkt_pos)
MAKE_ACCESSORS(BVFrame, frame, int64_t, channel_layout)
MAKE_ACCESSORS(BVFrame, frame, int,     channels)
MAKE_ACCESSORS(BVFrame, frame, int,     sample_rate)
MAKE_ACCESSORS(BVFrame, frame, BVDictionary *, metadata)
MAKE_ACCESSORS(BVFrame, frame, int,     decode_error_flags)
MAKE_ACCESSORS(BVFrame, frame, int,     pkt_size)
MAKE_ACCESSORS(BVFrame, frame, enum BVColorSpace, colorspace)
MAKE_ACCESSORS(BVFrame, frame, enum BVColorRange, color_range)

#define CHECK_CHANNELS_CONSISTENCY(frame) \
    bv_assert2(!(frame)->channel_layout || \
               (frame)->channels == \
               bv_get_channel_layout_nb_channels((frame)->channel_layout))

BVDictionary **bvpriv_frame_get_metadatap(BVFrame *frame) {return &frame->metadata;};

int bv_frame_set_qp_table(BVFrame *f, BVBufferRef *buf, int stride, int qp_type)
{
    bv_buffer_unref(&f->qp_table_buf);

    f->qp_table_buf = buf;

    f->qscale_table = buf->data;
    f->qstride      = stride;
    f->qscale_type  = qp_type;

    return 0;
}

int8_t *bv_frame_get_qp_table(BVFrame *f, int *stride, int *type)
{
    *stride = f->qstride;
    *type   = f->qscale_type;

    if (!f->qp_table_buf)
        return NULL;

    return f->qp_table_buf->data;
}

const char *bv_get_colorspace_name(enum BVColorSpace val)
{
    static const char * const name[] = {
        [BVCOL_SPC_RGB]       = "GBR",
        [BVCOL_SPC_BT709]     = "bt709",
        [BVCOL_SPC_FCC]       = "fcc",
        [BVCOL_SPC_BT470BG]   = "bt470bg",
        [BVCOL_SPC_SMPTE170M] = "smpte170m",
        [BVCOL_SPC_SMPTE240M] = "smpte240m",
        [BVCOL_SPC_YCOCG]     = "YCgCo",
    };
    if ((unsigned)val >= BV_ARRAY_ELEMS(name))
        return NULL;
    return name[val];
}

static void get_frame_defaults(BVFrame *frame)
{
    if (frame->extended_data != frame->data)
        bv_freep(&frame->extended_data);

    memset(frame, 0, sizeof(*frame));

    frame->pts                   =
    frame->pkt_dts               =
    frame->pkt_pts               = BV_NOPTS_VALUE;
    bv_frame_set_best_effort_timestamp(frame, BV_NOPTS_VALUE);
    bv_frame_set_pkt_duration         (frame, 0);
    bv_frame_set_pkt_pos              (frame, -1);
    bv_frame_set_pkt_size             (frame, -1);
    frame->key_frame           = 1;
    frame->sample_aspect_ratio = (BVRational){ 0, 1 };
    frame->format              = -1; /* unknown */
    frame->extended_data       = frame->data;
    frame->color_primaries     = BVCOL_PRI_UNSPECIFIED;
    frame->color_trc           = BVCOL_TRC_UNSPECIFIED;
    frame->colorspace          = BVCOL_SPC_UNSPECIFIED;
    frame->color_range         = BVCOL_RANGE_UNSPECIFIED;
    frame->chroma_location     = BVCHROMA_LOC_UNSPECIFIED;
}

static void free_side_data(BVFrameSideData **ptr_sd)
{
    BVFrameSideData *sd = *ptr_sd;

    bv_freep(&sd->data);
    bv_dict_free(&sd->metadata);
    bv_freep(ptr_sd);
}

static void wipe_side_data(BVFrame *frame)
{
    int i;

    for (i = 0; i < frame->nb_side_data; i++) {
        free_side_data(&frame->side_data[i]);
    }
    frame->nb_side_data = 0;

    bv_freep(&frame->side_data);
}

BVFrame *bv_frame_alloc(void)
{
    BVFrame *frame = bv_mallocz(sizeof(*frame));

    if (!frame)
        return NULL;

    frame->extended_data = NULL;
    get_frame_defaults(frame);

    return frame;
}

void bv_frame_free(BVFrame **frame)
{
    if (!frame || !*frame)
        return;

    bv_frame_unref(*frame);
    bv_freep(frame);
}

static int get_video_buffer(BVFrame *frame, int align)
{
    const BVPixFmtDescriptor *desc = bv_pix_fmt_desc_get(frame->format);
    int ret, i;

    if (!desc)
        return BVERROR(EINVAL);

    if ((ret = bv_image_check_size(frame->width, frame->height, 0, NULL)) < 0)
        return ret;

    if (!frame->linesize[0]) {
        for(i=1; i<=align; i+=i) {
            ret = bv_image_fill_linesizes(frame->linesize, frame->format,
                                          BBALIGN(frame->width, i));
            if (ret < 0)
                return ret;
            if (!(frame->linesize[0] & (align-1)))
                break;
        }

        for (i = 0; i < 4 && frame->linesize[i]; i++)
            frame->linesize[i] = BBALIGN(frame->linesize[i], align);
    }

    for (i = 0; i < 4 && frame->linesize[i]; i++) {
        int h = BBALIGN(frame->height, 32);
        if (i == 1 || i == 2)
            h = BV_CEIL_RSHIFT(h, desc->log2_chroma_h);

        frame->buf[i] = bv_buffer_alloc(frame->linesize[i] * h + 16 + 16/*STRIDE_ALIGN*/ - 1);
        if (!frame->buf[i])
            goto fail;

        frame->data[i] = frame->buf[i]->data;
    }
    if (desc->flags & BV_PIX_FMT_FLAG_PAL || desc->flags & BV_PIX_FMT_FLAG_PSEUDOPAL) {
        bv_buffer_unref(&frame->buf[1]);
        frame->buf[1] = bv_buffer_alloc(1024);
        if (!frame->buf[1])
            goto fail;
        frame->data[1] = frame->buf[1]->data;
    }

    frame->extended_data = frame->data;

    return 0;
fail:
    bv_frame_unref(frame);
    return BVERROR(ENOMEM);
}

static int get_audio_buffer(BVFrame *frame, int align)
{
    int channels;
    int planar   = bv_sample_fmt_is_planar(frame->format);
    int planes;
    int ret, i;

    if (!frame->channels)
        frame->channels = bv_get_channel_layout_nb_channels(frame->channel_layout);

    channels = frame->channels;
    planes = planar ? channels : 1;

    CHECK_CHANNELS_CONSISTENCY(frame);
    if (!frame->linesize[0]) {
        ret = bv_samples_get_buffer_size(&frame->linesize[0], channels,
                                         frame->nb_samples, frame->format,
                                         align);
        if (ret < 0)
            return ret;
    }

    if (planes > BV_NUM_DATA_POINTERS) {
        frame->extended_data = bv_mallocz_array(planes,
                                          sizeof(*frame->extended_data));
        frame->extended_buf  = bv_mallocz_array((planes - BV_NUM_DATA_POINTERS),
                                          sizeof(*frame->extended_buf));
        if (!frame->extended_data || !frame->extended_buf) {
            bv_freep(&frame->extended_data);
            bv_freep(&frame->extended_buf);
            return BVERROR(ENOMEM);
        }
        frame->nb_extended_buf = planes - BV_NUM_DATA_POINTERS;
    } else
        frame->extended_data = frame->data;

    for (i = 0; i < BBMIN(planes, BV_NUM_DATA_POINTERS); i++) {
        frame->buf[i] = bv_buffer_alloc(frame->linesize[0]);
        if (!frame->buf[i]) {
            bv_frame_unref(frame);
            return BVERROR(ENOMEM);
        }
        frame->extended_data[i] = frame->data[i] = frame->buf[i]->data;
    }
    for (i = 0; i < planes - BV_NUM_DATA_POINTERS; i++) {
        frame->extended_buf[i] = bv_buffer_alloc(frame->linesize[0]);
        if (!frame->extended_buf[i]) {
            bv_frame_unref(frame);
            return BVERROR(ENOMEM);
        }
        frame->extended_data[i + BV_NUM_DATA_POINTERS] = frame->extended_buf[i]->data;
    }
    return 0;

}

int bv_frame_get_buffer(BVFrame *frame, int align)
{
    if (frame->format < 0)
        return BVERROR(EINVAL);

    if (frame->width > 0 && frame->height > 0)
        return get_video_buffer(frame, align);
    else if (frame->nb_samples > 0 && (frame->channel_layout || frame->channels > 0))
        return get_audio_buffer(frame, align);

    return BVERROR(EINVAL);
}

int bv_frame_ref(BVFrame *dst, const BVFrame *src)
{
    int i, ret = 0;

    dst->format         = src->format;
    dst->width          = src->width;
    dst->height         = src->height;
    dst->channels       = src->channels;
    dst->channel_layout = src->channel_layout;
    dst->nb_samples     = src->nb_samples;

    ret = bv_frame_copy_props(dst, src);
    if (ret < 0)
        return ret;

    /* duplicate the frame data if it's not refcounted */
    if (!src->buf[0]) {
        ret = bv_frame_get_buffer(dst, 32);
        if (ret < 0)
            return ret;

        ret = bv_frame_copy(dst, src);
        if (ret < 0)
            bv_frame_unref(dst);

        return ret;
    }

    /* ref the buffers */
    for (i = 0; i < BV_ARRAY_ELEMS(src->buf); i++) {
        if (!src->buf[i])
            continue;
        dst->buf[i] = bv_buffer_ref(src->buf[i]);
        if (!dst->buf[i]) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
    }

    if (src->extended_buf) {
        dst->extended_buf = bv_mallocz_array(sizeof(*dst->extended_buf),
                                       src->nb_extended_buf);
        if (!dst->extended_buf) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        dst->nb_extended_buf = src->nb_extended_buf;

        for (i = 0; i < src->nb_extended_buf; i++) {
            dst->extended_buf[i] = bv_buffer_ref(src->extended_buf[i]);
            if (!dst->extended_buf[i]) {
                ret = BVERROR(ENOMEM);
                goto fail;
            }
        }
    }

    /* duplicate extended data */
    if (src->extended_data != src->data) {
        int ch = src->channels;

        if (!ch) {
            ret = BVERROR(EINVAL);
            goto fail;
        }
        CHECK_CHANNELS_CONSISTENCY(src);

        dst->extended_data = bv_malloc_array(sizeof(*dst->extended_data), ch);
        if (!dst->extended_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        memcpy(dst->extended_data, src->extended_data, sizeof(*src->extended_data) * ch);
    } else
        dst->extended_data = dst->data;

    memcpy(dst->data,     src->data,     sizeof(src->data));
    memcpy(dst->linesize, src->linesize, sizeof(src->linesize));

    return 0;

fail:
    bv_frame_unref(dst);
    return ret;
}

BVFrame *bv_frame_clone(const BVFrame *src)
{
    BVFrame *ret = bv_frame_alloc();

    if (!ret)
        return NULL;

    if (bv_frame_ref(ret, src) < 0)
        bv_frame_free(&ret);

    return ret;
}

void bv_frame_unref(BVFrame *frame)
{
    int i;

    wipe_side_data(frame);

    for (i = 0; i < BV_ARRAY_ELEMS(frame->buf); i++)
        bv_buffer_unref(&frame->buf[i]);
    for (i = 0; i < frame->nb_extended_buf; i++)
        bv_buffer_unref(&frame->extended_buf[i]);
    bv_freep(&frame->extended_buf);
    bv_dict_free(&frame->metadata);
    bv_buffer_unref(&frame->qp_table_buf);

    get_frame_defaults(frame);
}

void bv_frame_move_ref(BVFrame *dst, BVFrame *src)
{
    *dst = *src;
    if (src->extended_data == src->data)
        dst->extended_data = dst->data;
    memset(src, 0, sizeof(*src));
    get_frame_defaults(src);
}

int bv_frame_is_writable(BVFrame *frame)
{
    int i, ret = 1;

    /* assume non-refcounted frames are not writable */
    if (!frame->buf[0])
        return 0;

    for (i = 0; i < BV_ARRAY_ELEMS(frame->buf); i++)
        if (frame->buf[i])
            ret &= !!bv_buffer_is_writable(frame->buf[i]);
    for (i = 0; i < frame->nb_extended_buf; i++)
        ret &= !!bv_buffer_is_writable(frame->extended_buf[i]);

    return ret;
}

int bv_frame_make_writable(BVFrame *frame)
{
    BVFrame tmp;
    int ret;

    if (!frame->buf[0])
        return BVERROR(EINVAL);

    if (bv_frame_is_writable(frame))
        return 0;

    memset(&tmp, 0, sizeof(tmp));
    tmp.format         = frame->format;
    tmp.width          = frame->width;
    tmp.height         = frame->height;
    tmp.channels       = frame->channels;
    tmp.channel_layout = frame->channel_layout;
    tmp.nb_samples     = frame->nb_samples;
    ret = bv_frame_get_buffer(&tmp, 32);
    if (ret < 0)
        return ret;

    ret = bv_frame_copy(&tmp, frame);
    if (ret < 0) {
        bv_frame_unref(&tmp);
        return ret;
    }

    ret = bv_frame_copy_props(&tmp, frame);
    if (ret < 0) {
        bv_frame_unref(&tmp);
        return ret;
    }

    bv_frame_unref(frame);

    *frame = tmp;
    if (tmp.data == tmp.extended_data)
        frame->extended_data = frame->data;

    return 0;
}

int bv_frame_copy_props(BVFrame *dst, const BVFrame *src)
{
    int i;

    dst->key_frame              = src->key_frame;
    dst->pict_type              = src->pict_type;
    dst->sample_aspect_ratio    = src->sample_aspect_ratio;
    dst->pts                    = src->pts;
    dst->repeat_pict            = src->repeat_pict;
    dst->interlaced_frame       = src->interlaced_frame;
    dst->top_field_first        = src->top_field_first;
    dst->palette_has_changed    = src->palette_has_changed;
    dst->sample_rate            = src->sample_rate;
    dst->opaque                 = src->opaque;
#if BV_API_BVFRAME_LBVC
    dst->type                   = src->type;
#endif
    dst->pkt_pts                = src->pkt_pts;
    dst->pkt_dts                = src->pkt_dts;
    dst->pkt_pos                = src->pkt_pos;
    dst->pkt_size               = src->pkt_size;
    dst->pkt_duration           = src->pkt_duration;
    dst->reordered_opaque       = src->reordered_opaque;
    dst->quality                = src->quality;
    dst->best_effort_timestamp  = src->best_effort_timestamp;
    dst->coded_picture_number   = src->coded_picture_number;
    dst->display_picture_number = src->display_picture_number;
    dst->flags                  = src->flags;
    dst->decode_error_flags     = src->decode_error_flags;
    dst->color_primaries        = src->color_primaries;
    dst->color_trc              = src->color_trc;
    dst->colorspace             = src->colorspace;
    dst->color_range            = src->color_range;
    dst->chroma_location        = src->chroma_location;

    bv_dict_copy(&dst->metadata, src->metadata, 0);

    memcpy(dst->error, src->error, sizeof(dst->error));

    for (i = 0; i < src->nb_side_data; i++) {
        const BVFrameSideData *sd_src = src->side_data[i];
        BVFrameSideData *sd_dst;
        if (   sd_src->type == BV_FRAME_DATA_PANSCAN
            && (src->width != dst->width || src->height != dst->height))
            continue;
        sd_dst = bv_frame_new_side_data(dst, sd_src->type,
                                                         sd_src->size);
        if (!sd_dst) {
            wipe_side_data(dst);
            return BVERROR(ENOMEM);
        }
        memcpy(sd_dst->data, sd_src->data, sd_src->size);
        bv_dict_copy(&sd_dst->metadata, sd_src->metadata, 0);
    }

    dst->qscale_table = NULL;
    dst->qstride      = 0;
    dst->qscale_type  = 0;
    if (src->qp_table_buf) {
        dst->qp_table_buf = bv_buffer_ref(src->qp_table_buf);
        if (dst->qp_table_buf) {
            dst->qscale_table = dst->qp_table_buf->data;
            dst->qstride      = src->qstride;
            dst->qscale_type  = src->qscale_type;
        }
    }

    return 0;
}

BVBufferRef *bv_frame_get_plane_buffer(BVFrame *frame, int plane)
{
    uint8_t *data;
    int planes, i;

    if (frame->nb_samples) {
        int channels = frame->channels;
        if (!channels)
            return NULL;
        CHECK_CHANNELS_CONSISTENCY(frame);
        planes = bv_sample_fmt_is_planar(frame->format) ? channels : 1;
    } else
        planes = 4;

    if (plane < 0 || plane >= planes || !frame->extended_data[plane])
        return NULL;
    data = frame->extended_data[plane];

    for (i = 0; i < BV_ARRAY_ELEMS(frame->buf) && frame->buf[i]; i++) {
        BVBufferRef *buf = frame->buf[i];
        if (data >= buf->data && data < buf->data + buf->size)
            return buf;
    }
    for (i = 0; i < frame->nb_extended_buf; i++) {
        BVBufferRef *buf = frame->extended_buf[i];
        if (data >= buf->data && data < buf->data + buf->size)
            return buf;
    }
    return NULL;
}

BVFrameSideData *bv_frame_new_side_data(BVFrame *frame,
                                        enum BVFrameSideDataType type,
                                        int size)
{
    BVFrameSideData *ret, **tmp;

    if (frame->nb_side_data > INT_MAX / sizeof(*frame->side_data) - 1)
        return NULL;

    tmp = bv_realloc(frame->side_data,
                     (frame->nb_side_data + 1) * sizeof(*frame->side_data));
    if (!tmp)
        return NULL;
    frame->side_data = tmp;

    ret = bv_mallocz(sizeof(*ret));
    if (!ret)
        return NULL;

    ret->data = bv_malloc(size);
    if (!ret->data) {
        bv_freep(&ret);
        return NULL;
    }

    ret->size = size;
    ret->type = type;

    frame->side_data[frame->nb_side_data++] = ret;

    return ret;
}

BVFrameSideData *bv_frame_get_side_data(const BVFrame *frame,
                                        enum BVFrameSideDataType type)
{
    int i;

    for (i = 0; i < frame->nb_side_data; i++) {
        if (frame->side_data[i]->type == type)
            return frame->side_data[i];
    }
    return NULL;
}

static int frame_copy_video(BVFrame *dst, const BVFrame *src)
{
    const uint8_t *src_data[4];
    int i, planes;

    if (dst->width  < src->width ||
        dst->height < src->height)
        return BVERROR(EINVAL);

    planes = bv_pix_fmt_count_planes(dst->format);
    for (i = 0; i < planes; i++)
        if (!dst->data[i] || !src->data[i])
            return BVERROR(EINVAL);

    memcpy(src_data, src->data, sizeof(src_data));
    bv_image_copy(dst->data, dst->linesize,
                  src_data, src->linesize,
                  dst->format, src->width, src->height);

    return 0;
}

static int frame_copy_audio(BVFrame *dst, const BVFrame *src)
{
    int planar   = bv_sample_fmt_is_planar(dst->format);
    int channels = dst->channels;
    int planes   = planar ? channels : 1;
    int i;

    if (dst->nb_samples     != src->nb_samples ||
        dst->channels       != src->channels ||
        dst->channel_layout != src->channel_layout)
        return BVERROR(EINVAL);

    CHECK_CHANNELS_CONSISTENCY(src);

    for (i = 0; i < planes; i++)
        if (!dst->extended_data[i] || !src->extended_data[i])
            return BVERROR(EINVAL);

    bv_samples_copy(dst->extended_data, src->extended_data, 0, 0,
                    dst->nb_samples, channels, dst->format);

    return 0;
}

int bv_frame_copy(BVFrame *dst, const BVFrame *src)
{
    if (dst->format != src->format || dst->format < 0)
        return BVERROR(EINVAL);

    if (dst->width > 0 && dst->height > 0)
        return frame_copy_video(dst, src);
    else if (dst->nb_samples > 0 && dst->channel_layout)
        return frame_copy_audio(dst, src);

    return BVERROR(EINVAL);
}

void bv_frame_remove_side_data(BVFrame *frame, enum BVFrameSideDataType type)
{
    int i;

    for (i = 0; i < frame->nb_side_data; i++) {
        BVFrameSideData *sd = frame->side_data[i];
        if (sd->type == type) {
            free_side_data(&frame->side_data[i]);
            frame->side_data[i] = frame->side_data[frame->nb_side_data - 1];
            frame->nb_side_data--;
        }
    }
}

const char *bv_frame_side_data_name(enum BVFrameSideDataType type)
{
    switch(type) {
    case BV_FRAME_DATA_PANSCAN:         return "BVPanScan";
    case BV_FRAME_DATA_A53_CC:          return "ATSC A53 Part 4 Closed Captions";
    case BV_FRAME_DATA_STEREO3D:        return "Stereoscopic 3d metadata";
    case BV_FRAME_DATA_MATRIXENCODING:  return "AVMatrixEncoding";
    case BV_FRAME_DATA_DOWNMIX_INFO:    return "Metadata relevant to a downmix procedure";
    case BV_FRAME_DATA_REPLAYGAIN:      return "BVReplayGain";
    case BV_FRAME_DATA_DISPLAYMATRIX:   return "3x3 displaymatrix";
    case BV_FRAME_DATA_MOTION_VECTORS:  return "Motion vectors";
    }
    return NULL;
}
