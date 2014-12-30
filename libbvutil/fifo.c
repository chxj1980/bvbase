/*
 * a very simple circular buffer FIFO implementation
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
 * Copyright (c) 2006 Roman Shaposhnik
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

#include "avassert.h"
#include "common.h"
#include "fifo.h"

static BVFifoBuffer *fifo_alloc_common(void *buffer, size_t size)
{
    BVFifoBuffer *f;
    if (!buffer)
        return NULL;
    f = bv_mallocz(sizeof(BVFifoBuffer));
    if (!f) {
        bv_free(buffer);
        return NULL;
    }
    f->buffer = buffer;
    f->end    = f->buffer + size;
    bv_fifo_reset(f);
    return f;
}

BVFifoBuffer *bv_fifo_alloc(unsigned int size)
{
    void *buffer = bv_malloc(size);
    return fifo_alloc_common(buffer, size);
}

BVFifoBuffer *bv_fifo_alloc_array(size_t nmemb, size_t size)
{
    void *buffer = bv_malloc_array(nmemb, size);
    return fifo_alloc_common(buffer, nmemb * size);
}

void bv_fifo_free(BVFifoBuffer *f)
{
    if (f) {
        bv_freep(&f->buffer);
        bv_free(f);
    }
}

void bv_fifo_freep(BVFifoBuffer **f)
{
    if (f) {
        bv_fifo_free(*f);
        *f = NULL;
    }
}

void bv_fifo_reset(BVFifoBuffer *f)
{
    f->wptr = f->rptr = f->buffer;
    f->wndx = f->rndx = 0;
}

int bv_fifo_size(const BVFifoBuffer *f)
{
    return (uint32_t)(f->wndx - f->rndx);
}

int bv_fifo_space(const BVFifoBuffer *f)
{
    return f->end - f->buffer - bv_fifo_size(f);
}

int bv_fifo_realloc2(BVFifoBuffer *f, unsigned int new_size)
{
    unsigned int old_size = f->end - f->buffer;

    if (old_size < new_size) {
        int len          = bv_fifo_size(f);
        BVFifoBuffer *f2 = bv_fifo_alloc(new_size);

        if (!f2)
            return BVERROR(ENOMEM);
        bv_fifo_generic_read(f, f2->buffer, len, NULL);
        f2->wptr += len;
        f2->wndx += len;
        bv_free(f->buffer);
        *f = *f2;
        bv_free(f2);
    }
    return 0;
}

int bv_fifo_grow(BVFifoBuffer *f, unsigned int size)
{
    unsigned int old_size = f->end - f->buffer;
    if(size + (unsigned)bv_fifo_size(f) < size)
        return BVERROR(EINVAL);

    size += bv_fifo_size(f);

    if (old_size < size)
        return bv_fifo_realloc2(f, FFMAX(size, 2*size));
    return 0;
}

/* src must NOT be const as it can be a context for func that may need
 * updating (like a pointer or byte counter) */
int bv_fifo_generic_write(BVFifoBuffer *f, void *src, int size,
                          int (*func)(void *, void *, int))
{
    int total = size;
    uint32_t wndx= f->wndx;
    uint8_t *wptr= f->wptr;

    do {
        int len = FFMIN(f->end - wptr, size);
        if (func) {
            if (func(src, wptr, len) <= 0)
                break;
        } else {
            memcpy(wptr, src, len);
            src = (uint8_t *)src + len;
        }
// Write memory barrier needed for SMP here in theory
        wptr += len;
        if (wptr >= f->end)
            wptr = f->buffer;
        wndx    += len;
        size    -= len;
    } while (size > 0);
    f->wndx= wndx;
    f->wptr= wptr;
    return total - size;
}

int bv_fifo_generic_read(BVFifoBuffer *f, void *dest, int buf_size,
                         void (*func)(void *, void *, int))
{
// Read memory barrier needed for SMP here in theory
    do {
        int len = FFMIN(f->end - f->rptr, buf_size);
        if (func)
            func(dest, f->rptr, len);
        else {
            memcpy(dest, f->rptr, len);
            dest = (uint8_t *)dest + len;
        }
// memory barrier needed for SMP here in theory
        bv_fifo_drain(f, len);
        buf_size -= len;
    } while (buf_size > 0);
    return 0;
}

/** Discard data from the FIFO. */
void bv_fifo_drain(BVFifoBuffer *f, int size)
{
    bv_assert2(bv_fifo_size(f) >= size);
    f->rptr += size;
    if (f->rptr >= f->end)
        f->rptr -= f->end - f->buffer;
    f->rndx += size;
}

#ifdef TEST

int main(void)
{
    /* create a FIFO buffer */
    BVFifoBuffer *fifo = bv_fifo_alloc(13 * sizeof(int));
    int i, j, n;

    /* fill data */
    for (i = 0; bv_fifo_space(fifo) >= sizeof(int); i++)
        bv_fifo_generic_write(fifo, &i, sizeof(int), NULL);

    /* peek at FIFO */
    n = bv_fifo_size(fifo) / sizeof(int);
    for (i = -n + 1; i < n; i++) {
        int *v = (int *)bv_fifo_peek2(fifo, i * sizeof(int));
        printf("%d: %d\n", i, *v);
    }
    printf("\n");

    /* read data */
    for (i = 0; bv_fifo_size(fifo) >= sizeof(int); i++) {
        bv_fifo_generic_read(fifo, &j, sizeof(int), NULL);
        printf("%d ", j);
    }
    printf("\n");

    bv_fifo_free(fifo);

    return 0;
}

#endif
