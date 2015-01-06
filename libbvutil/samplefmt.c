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

#include "common.h"
#include "samplefmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SampleFmtInfo {
    char name[8];
    int bits;
    int planar;
    enum BVSampleFormat altform; ///< planar<->packed alternative form
} SampleFmtInfo;

/** this table gives more information about formats */
static const SampleFmtInfo sample_fmt_info[BV_SAMPLE_FMT_NB] = {
    [BV_SAMPLE_FMT_U8]   = { .name =   "u8", .bits =  8, .planar = 0, .altform = BV_SAMPLE_FMT_U8P  },
    [BV_SAMPLE_FMT_S16]  = { .name =  "s16", .bits = 16, .planar = 0, .altform = BV_SAMPLE_FMT_S16P },
    [BV_SAMPLE_FMT_S32]  = { .name =  "s32", .bits = 32, .planar = 0, .altform = BV_SAMPLE_FMT_S32P },
    [BV_SAMPLE_FMT_FLT]  = { .name =  "flt", .bits = 32, .planar = 0, .altform = BV_SAMPLE_FMT_FLTP },
    [BV_SAMPLE_FMT_DBL]  = { .name =  "dbl", .bits = 64, .planar = 0, .altform = BV_SAMPLE_FMT_DBLP },
    [BV_SAMPLE_FMT_U8P]  = { .name =  "u8p", .bits =  8, .planar = 1, .altform = BV_SAMPLE_FMT_U8   },
    [BV_SAMPLE_FMT_S16P] = { .name = "s16p", .bits = 16, .planar = 1, .altform = BV_SAMPLE_FMT_S16  },
    [BV_SAMPLE_FMT_S32P] = { .name = "s32p", .bits = 32, .planar = 1, .altform = BV_SAMPLE_FMT_S32  },
    [BV_SAMPLE_FMT_FLTP] = { .name = "fltp", .bits = 32, .planar = 1, .altform = BV_SAMPLE_FMT_FLT  },
    [BV_SAMPLE_FMT_DBLP] = { .name = "dblp", .bits = 64, .planar = 1, .altform = BV_SAMPLE_FMT_DBL  },
};

const char *bv_get_sample_fmt_name(enum BVSampleFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB)
        return NULL;
    return sample_fmt_info[sample_fmt].name;
}

enum BVSampleFormat bv_get_sample_fmt(const char *name)
{
    int i;

    for (i = 0; i < BV_SAMPLE_FMT_NB; i++)
        if (!strcmp(sample_fmt_info[i].name, name))
            return i;
    return BV_SAMPLE_FMT_NONE;
}

enum BVSampleFormat bv_get_alt_sample_fmt(enum BVSampleFormat sample_fmt, int planar)
{
    if (sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB)
        return BV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar == planar)
        return sample_fmt;
    return sample_fmt_info[sample_fmt].altform;
}

enum BVSampleFormat bv_get_packed_sample_fmt(enum BVSampleFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB)
        return BV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar)
        return sample_fmt_info[sample_fmt].altform;
    return sample_fmt;
}

enum BVSampleFormat bv_get_planar_sample_fmt(enum BVSampleFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB)
        return BV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar)
        return sample_fmt;
    return sample_fmt_info[sample_fmt].altform;
}

char *bv_get_sample_fmt_string (char *buf, int buf_size, enum BVSampleFormat sample_fmt)
{
    /* print header */
    if (sample_fmt < 0)
        snprintf(buf, buf_size, "name  " " depth");
    else if (sample_fmt < BV_SAMPLE_FMT_NB) {
        SampleFmtInfo info = sample_fmt_info[sample_fmt];
        snprintf (buf, buf_size, "%-6s" "   %2d ", info.name, info.bits);
    }

    return buf;
}

int bv_get_bytes_per_sample(enum BVSampleFormat sample_fmt)
{
     return sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB ?
        0 : sample_fmt_info[sample_fmt].bits >> 3;
}

int bv_sample_fmt_is_planar(enum BVSampleFormat sample_fmt)
{
     if (sample_fmt < 0 || sample_fmt >= BV_SAMPLE_FMT_NB)
         return 0;
     return sample_fmt_info[sample_fmt].planar;
}

int bv_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples,
                               enum BVSampleFormat sample_fmt, int align)
{
    int line_size;
    int sample_size = bv_get_bytes_per_sample(sample_fmt);
    int planar      = bv_sample_fmt_is_planar(sample_fmt);

    /* validate parameter ranges */
    if (!sample_size || nb_samples <= 0 || nb_channels <= 0)
        return BVERROR(EINVAL);

    /* auto-select alignment if not specified */
    if (!align) {
        if (nb_samples > INT_MAX - 31)
            return BVERROR(EINVAL);
        align = 1;
        nb_samples = BBALIGN(nb_samples, 32);
    }

    /* check for integer overflow */
    if (nb_channels > INT_MAX / align ||
        (int64_t)nb_channels * nb_samples > (INT_MAX - (align * nb_channels)) / sample_size)
        return BVERROR(EINVAL);

    line_size = planar ? BBALIGN(nb_samples * sample_size,               align) :
                         BBALIGN(nb_samples * sample_size * nb_channels, align);
    if (linesize)
        *linesize = line_size;

    return planar ? line_size * nb_channels : line_size;
}

int bv_samples_fill_arrays(uint8_t **audio_data, int *linesize,
                           const uint8_t *buf, int nb_channels, int nb_samples,
                           enum BVSampleFormat sample_fmt, int align)
{
    int ch, planar, buf_size, line_size;

    planar   = bv_sample_fmt_is_planar(sample_fmt);
    buf_size = bv_samples_get_buffer_size(&line_size, nb_channels, nb_samples,
                                          sample_fmt, align);
    if (buf_size < 0)
        return buf_size;

    audio_data[0] = (uint8_t *)buf;
    for (ch = 1; planar && ch < nb_channels; ch++)
        audio_data[ch] = audio_data[ch-1] + line_size;

    if (linesize)
        *linesize = line_size;

    return buf_size;
}

int bv_samples_alloc(uint8_t **audio_data, int *linesize, int nb_channels,
                     int nb_samples, enum BVSampleFormat sample_fmt, int align)
{
    uint8_t *buf;
    int size = bv_samples_get_buffer_size(NULL, nb_channels, nb_samples,
                                          sample_fmt, align);
    if (size < 0)
        return size;

    buf = bv_malloc(size);
    if (!buf)
        return BVERROR(ENOMEM);

    size = bv_samples_fill_arrays(audio_data, linesize, buf, nb_channels,
                                  nb_samples, sample_fmt, align);
    if (size < 0) {
        bv_free(buf);
        return size;
    }

    bv_samples_set_silence(audio_data, 0, nb_samples, nb_channels, sample_fmt);

    return size;
}

int bv_samples_alloc_array_and_samples(uint8_t ***audio_data, int *linesize, int nb_channels,
                                       int nb_samples, enum BVSampleFormat sample_fmt, int align)
{
    int ret, nb_planes = bv_sample_fmt_is_planar(sample_fmt) ? nb_channels : 1;

    *audio_data = bv_calloc(nb_planes, sizeof(**audio_data));
    if (!*audio_data)
        return BVERROR(ENOMEM);
    ret = bv_samples_alloc(*audio_data, linesize, nb_channels,
                           nb_samples, sample_fmt, align);
    if (ret < 0)
        bv_freep(audio_data);
    return ret;
}

int bv_samples_copy(uint8_t **dst, uint8_t * const *src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    enum BVSampleFormat sample_fmt)
{
    int planar      = bv_sample_fmt_is_planar(sample_fmt);
    int planes      = planar ? nb_channels : 1;
    int block_align = bv_get_bytes_per_sample(sample_fmt) * (planar ? 1 : nb_channels);
    int data_size   = nb_samples * block_align;
    int i;

    dst_offset *= block_align;
    src_offset *= block_align;

    if((dst[0] < src[0] ? src[0] - dst[0] : dst[0] - src[0]) >= data_size) {
        for (i = 0; i < planes; i++)
            memcpy(dst[i] + dst_offset, src[i] + src_offset, data_size);
    } else {
        for (i = 0; i < planes; i++)
            memmove(dst[i] + dst_offset, src[i] + src_offset, data_size);
    }

    return 0;
}

int bv_samples_set_silence(uint8_t **audio_data, int offset, int nb_samples,
                           int nb_channels, enum BVSampleFormat sample_fmt)
{
    int planar      = bv_sample_fmt_is_planar(sample_fmt);
    int planes      = planar ? nb_channels : 1;
    int block_align = bv_get_bytes_per_sample(sample_fmt) * (planar ? 1 : nb_channels);
    int data_size   = nb_samples * block_align;
    int fill_char   = (sample_fmt == BV_SAMPLE_FMT_U8 ||
                     sample_fmt == BV_SAMPLE_FMT_U8P) ? 0x80 : 0x00;
    int i;

    offset *= block_align;

    for (i = 0; i < planes; i++)
        memset(audio_data[i] + offset, fill_char, data_size);

    return 0;
}
