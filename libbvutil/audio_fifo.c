/*
 * Audio FIFO
 * Copyright (c) 2012 Justin Ruggles <justin.ruggles@gmail.com>
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
 * Audio FIFO
 */

#include "bvutil.h"
#include "audio_fifo.h"
#include "common.h"
#include "fifo.h"
#include "mem.h"
#include "samplefmt.h"

struct BVAudioFifo {
    BVFifoBuffer **buf;             /**< single buffer for interleaved, per-channel buffers for planar */
    int nb_buffers;                 /**< number of buffers */
    int nb_samples;                 /**< number of samples currently in the FIFO */
    int allocated_samples;          /**< current allocated size, in samples */

    int channels;                   /**< number of channels */
    enum BVSampleFormat sample_fmt; /**< sample format */
    int sample_size;                /**< size, in bytes, of one sample in a buffer */
};

void bv_audio_fifo_free(BVAudioFifo *af)
{
    if (af) {
        if (af->buf) {
            int i;
            for (i = 0; i < af->nb_buffers; i++) {
                if (af->buf[i])
                    bv_fifo_free(af->buf[i]);
            }
            bv_freep(&af->buf);
        }
        bv_free(af);
    }
}

BVAudioFifo *bv_audio_fifo_alloc(enum BVSampleFormat sample_fmt, int channels,
                                 int nb_samples)
{
    BVAudioFifo *af;
    int buf_size, i;

    /* get channel buffer size (also validates parameters) */
    if (bv_samples_get_buffer_size(&buf_size, channels, nb_samples, sample_fmt, 1) < 0)
        return NULL;

    af = bv_mallocz(sizeof(*af));
    if (!af)
        return NULL;

    af->channels    = channels;
    af->sample_fmt  = sample_fmt;
    af->sample_size = buf_size / nb_samples;
    af->nb_buffers  = bv_sample_fmt_is_planar(sample_fmt) ? channels : 1;

    af->buf = bv_mallocz_array(af->nb_buffers, sizeof(*af->buf));
    if (!af->buf)
        goto error;

    for (i = 0; i < af->nb_buffers; i++) {
        af->buf[i] = bv_fifo_alloc(buf_size);
        if (!af->buf[i])
            goto error;
    }
    af->allocated_samples = nb_samples;

    return af;

error:
    bv_audio_fifo_free(af);
    return NULL;
}

int bv_audio_fifo_realloc(BVAudioFifo *af, int nb_samples)
{
    int i, ret, buf_size;

    if ((ret = bv_samples_get_buffer_size(&buf_size, af->channels, nb_samples,
                                          af->sample_fmt, 1)) < 0)
        return ret;

    for (i = 0; i < af->nb_buffers; i++) {
        if ((ret = bv_fifo_realloc2(af->buf[i], buf_size)) < 0)
            return ret;
    }
    af->allocated_samples = nb_samples;
    return 0;
}

int bv_audio_fifo_write(BVAudioFifo *af, void **data, int nb_samples)
{
    int i, ret, size;

    /* automatically reallocate buffers if needed */
    if (bv_audio_fifo_space(af) < nb_samples) {
        int current_size = bv_audio_fifo_size(af);
        /* check for integer overflow in new size calculation */
        if (INT_MAX / 2 - current_size < nb_samples)
            return BVERROR(EINVAL);
        /* reallocate buffers */
        if ((ret = bv_audio_fifo_realloc(af, 2 * (current_size + nb_samples))) < 0)
            return ret;
    }

    size = nb_samples * af->sample_size;
    for (i = 0; i < af->nb_buffers; i++) {
        ret = bv_fifo_generic_write(af->buf[i], data[i], size, NULL);
        if (ret != size)
            return BVERROR_BUG;
    }
    af->nb_samples += nb_samples;

    return nb_samples;
}

int bv_audio_fifo_read(BVAudioFifo *af, void **data, int nb_samples)
{
    int i, ret, size;

    if (nb_samples < 0)
        return BVERROR(EINVAL);
    nb_samples = FFMIN(nb_samples, af->nb_samples);
    if (!nb_samples)
        return 0;

    size = nb_samples * af->sample_size;
    for (i = 0; i < af->nb_buffers; i++) {
        if ((ret = bv_fifo_generic_read(af->buf[i], data[i], size, NULL)) < 0)
            return BVERROR_BUG;
    }
    af->nb_samples -= nb_samples;

    return nb_samples;
}

int bv_audio_fifo_drain(BVAudioFifo *af, int nb_samples)
{
    int i, size;

    if (nb_samples < 0)
        return BVERROR(EINVAL);
    nb_samples = FFMIN(nb_samples, af->nb_samples);

    if (nb_samples) {
        size = nb_samples * af->sample_size;
        for (i = 0; i < af->nb_buffers; i++)
            bv_fifo_drain(af->buf[i], size);
        af->nb_samples -= nb_samples;
    }
    return 0;
}

void bv_audio_fifo_reset(BVAudioFifo *af)
{
    int i;

    for (i = 0; i < af->nb_buffers; i++)
        bv_fifo_reset(af->buf[i]);

    af->nb_samples = 0;
}

int bv_audio_fifo_size(BVAudioFifo *af)
{
    return af->nb_samples;
}

int bv_audio_fifo_space(BVAudioFifo *af)
{
    return af->allocated_samples - af->nb_samples;
}
