/*
 * Audio FIFO
 * Copyright (c) 2012 Justin Ruggles <justin.ruggles@gmail.com>
 *
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

/**
 * @file
 * Audio FIFO Buffer
 */

#ifndef BVUTIL_AUDIO_FIFO_H
#define BVUTIL_AUDIO_FIFO_H

#include "bvutil.h"
#include "fifo.h"
#include "samplefmt.h"

/**
 * @addtogroup lavu_audio
 * @{
 *
 * @defgroup lavu_audiofifo Audio FIFO Buffer
 * @{
 */

/**
 * Context for an Audio FIFO Buffer.
 *
 * - Operates at the sample level rather than the byte level.
 * - Supports multiple channels with either planar or packed sample format.
 * - Automatic reallocation when writing to a full buffer.
 */
typedef struct BVAudioFifo BVAudioFifo;

/**
 * Free an BVAudioFifo.
 *
 * @param af  BVAudioFifo to free
 */
void bv_audio_fifo_free(BVAudioFifo *af);

/**
 * Allocate an BVAudioFifo.
 *
 * @param sample_fmt  sample format
 * @param channels    number of channels
 * @param nb_samples  initial allocation size, in samples
 * @return            newly allocated BVAudioFifo, or NULL on error
 */
BVAudioFifo *bv_audio_fifo_alloc(enum BVSampleFormat sample_fmt, int channels,
                                 int nb_samples);

/**
 * Reallocate an BVAudioFifo.
 *
 * @param af          BVAudioFifo to reallocate
 * @param nb_samples  new allocation size, in samples
 * @return            0 if OK, or negative BVERROR code on failure
 */
int bv_audio_fifo_realloc(BVAudioFifo *af, int nb_samples);

/**
 * Write data to an BVAudioFifo.
 *
 * The BVAudioFifo will be reallocated automatically if the available space
 * is less than nb_samples.
 *
 * @see enum BVSampleFormat
 * The documentation for BVSampleFormat describes the data layout.
 *
 * @param af          BVAudioFifo to write to
 * @param data        audio data plane pointers
 * @param nb_samples  number of samples to write
 * @return            number of samples actually written, or negative BVERROR
 *                    code on failure. If successful, the number of samples
 *                    actually written will always be nb_samples.
 */
int bv_audio_fifo_write(BVAudioFifo *af, void **data, int nb_samples);

/**
 * Read data from an BVAudioFifo.
 *
 * @see enum BVSampleFormat
 * The documentation for BVSampleFormat describes the data layout.
 *
 * @param af          BVAudioFifo to read from
 * @param data        audio data plane pointers
 * @param nb_samples  number of samples to read
 * @return            number of samples actually read, or negative BVERROR code
 *                    on failure. The number of samples actually read will not
 *                    be greater than nb_samples, and will only be less than
 *                    nb_samples if bv_audio_fifo_size is less than nb_samples.
 */
int bv_audio_fifo_read(BVAudioFifo *af, void **data, int nb_samples);

/**
 * Drain data from an BVAudioFifo.
 *
 * Removes the data without reading it.
 *
 * @param af          BVAudioFifo to drain
 * @param nb_samples  number of samples to drain
 * @return            0 if OK, or negative BVERROR code on failure
 */
int bv_audio_fifo_drain(BVAudioFifo *af, int nb_samples);

/**
 * Reset the BVAudioFifo buffer.
 *
 * This empties all data in the buffer.
 *
 * @param af  BVAudioFifo to reset
 */
void bv_audio_fifo_reset(BVAudioFifo *af);

/**
 * Get the current number of samples in the BVAudioFifo available for reading.
 *
 * @param af  the BVAudioFifo to query
 * @return    number of samples available for reading
 */
int bv_audio_fifo_size(BVAudioFifo *af);

/**
 * Get the current number of samples in the BVAudioFifo available for writing.
 *
 * @param af  the BVAudioFifo to query
 * @return    number of samples available for writing
 */
int bv_audio_fifo_space(BVAudioFifo *af);

/**
 * @}
 * @}
 */

#endif /* BVUTIL_AUDIO_FIFO_H */
