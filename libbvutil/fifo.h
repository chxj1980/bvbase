/*
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
 * a very simple circular buffer FIFO implementation
 */

#ifndef BVUTIL_FIFO_H
#define BVUTIL_FIFO_H

#include <stdint.h>
#include "bvutil.h"
#include "attributes.h"

typedef struct BVFifoBuffer {
    uint8_t *buffer;
    uint8_t *rptr, *wptr, *end;
    uint32_t rndx, wndx;
} BVFifoBuffer;

/**
 * Initialize an BVFifoBuffer.
 * @param size of FIFO
 * @return BVFifoBuffer or NULL in case of memory allocation failure
 */
BVFifoBuffer *bv_fifo_alloc(unsigned int size);

/**
 * Initialize an BVFifoBuffer.
 * @param nmemb number of elements
 * @param size  size of the single element
 * @return BVFifoBuffer or NULL in case of memory allocation failure
 */
BVFifoBuffer *bv_fifo_alloc_array(size_t nmemb, size_t size);

/**
 * Free an BVFifoBuffer.
 * @param f BVFifoBuffer to free
 */
void bv_fifo_free(BVFifoBuffer *f);

/**
 * Free an BVFifoBuffer and reset pointer to NULL.
 * @param f BVFifoBuffer to free
 */
void bv_fifo_freep(BVFifoBuffer **f);

/**
 * Reset the BVFifoBuffer to the state right after bv_fifo_alloc, in particular it is emptied.
 * @param f BVFifoBuffer to reset
 */
void bv_fifo_reset(BVFifoBuffer *f);

/**
 * Return the amount of data in bytes in the BVFifoBuffer, that is the
 * amount of data you can read from it.
 * @param f BVFifoBuffer to read from
 * @return size
 */
int bv_fifo_size(const BVFifoBuffer *f);

/**
 * Return the amount of space in bytes in the BVFifoBuffer, that is the
 * amount of data you can write into it.
 * @param f BVFifoBuffer to write into
 * @return size
 */
int bv_fifo_space(const BVFifoBuffer *f);

/**
 * Feed data from an BVFifoBuffer to a user-supplied callback.
 * @param f BVFifoBuffer to read from
 * @param buf_size number of bytes to read
 * @param func generic read function
 * @param dest data destination
 */
int bv_fifo_generic_read(BVFifoBuffer *f, void *dest, int buf_size, void (*func)(void*, void*, int));

/**
 * Feed data from a user-supplied callback to an BVFifoBuffer.
 * @param f BVFifoBuffer to write to
 * @param src data source; non-const since it may be used as a
 * modifiable context by the function defined in func
 * @param size number of bytes to write
 * @param func generic write function; the first parameter is src,
 * the second is dest_buf, the third is dest_buf_size.
 * func must return the number of bytes written to dest_buf, or <= 0 to
 * indicate no more data available to write.
 * If func is NULL, src is interpreted as a simple byte array for source data.
 * @return the number of bytes written to the FIFO
 */
int bv_fifo_generic_write(BVFifoBuffer *f, void *src, int size, int (*func)(void*, void*, int));

/**
 * Resize an BVFifoBuffer.
 * In case of reallocation failure, the old FIFO is kept unchanged.
 *
 * @param f BVFifoBuffer to resize
 * @param size new BVFifoBuffer size in bytes
 * @return <0 for failure, >=0 otherwise
 */
int bv_fifo_realloc2(BVFifoBuffer *f, unsigned int size);

/**
 * Enlarge an BVFifoBuffer.
 * In case of reallocation failure, the old FIFO is kept unchanged.
 * The new fifo size may be larger than the requested size.
 *
 * @param f BVFifoBuffer to resize
 * @param additional_space the amount of space in bytes to allocate in addition to bv_fifo_size()
 * @return <0 for failure, >=0 otherwise
 */
int bv_fifo_grow(BVFifoBuffer *f, unsigned int additional_space);

/**
 * Read and discard the specified amount of data from an BVFifoBuffer.
 * @param f BVFifoBuffer to read from
 * @param size amount of data to read in bytes
 */
void bv_fifo_drain(BVFifoBuffer *f, int size);

/**
 * Return a pointer to the data stored in a FIFO buffer at a certain offset.
 * The FIFO buffer is not modified.
 *
 * @param f    BVFifoBuffer to peek at, f must be non-NULL
 * @param offs an offset in bytes, its absolute value must be less
 *             than the used buffer size or the returned pointer will
 *             point outside to the buffer data.
 *             The used buffer size can be checked with bv_fifo_size().
 */
static inline uint8_t *bv_fifo_peek2(const BVFifoBuffer *f, int offs)
{
    uint8_t *ptr = f->rptr + offs;
    if (ptr >= f->end)
        ptr = f->buffer + (ptr - f->end);
    else if (ptr < f->buffer)
        ptr = f->end - (f->buffer - ptr);
    return ptr;
}

#endif /* BVUTIL_FIFO_H */
