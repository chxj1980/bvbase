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

#include <stdint.h>
#include <string.h>

#include "atomic.h"
#include "buffer_internal.h"
#include "common.h"
#include "mem.h"
#include "thread.h"

BVBufferRef *bv_buffer_create(uint8_t *data, int size,
                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags)
{
    BVBufferRef *ref = NULL;
    BVBuffer    *buf = NULL;

    buf = bv_mallocz(sizeof(*buf));
    if (!buf)
        return NULL;

    buf->data     = data;
    buf->size     = size;
    buf->free     = free ? free : bv_buffer_default_free;
    buf->opaque   = opaque;
    buf->refcount = 1;

    if (flags & BV_BUFFER_FLAG_READONLY)
        buf->flags |= BUFFER_FLAG_READONLY;

    ref = bv_mallocz(sizeof(*ref));
    if (!ref) {
        bv_freep(&buf);
        return NULL;
    }

    ref->buffer = buf;
    ref->data   = data;
    ref->size   = size;

    return ref;
}

void bv_buffer_default_free(void *opaque, uint8_t *data)
{
    bv_free(data);
}

BVBufferRef *bv_buffer_alloc(int size)
{
    BVBufferRef *ret = NULL;
    uint8_t    *data = NULL;

    data = bv_malloc(size);
    if (!data)
        return NULL;

    ret = bv_buffer_create(data, size, bv_buffer_default_free, NULL, 0);
    if (!ret)
        bv_freep(&data);

    return ret;
}

BVBufferRef *bv_buffer_allocz(int size)
{
    BVBufferRef *ret = bv_buffer_alloc(size);
    if (!ret)
        return NULL;

    memset(ret->data, 0, size);
    return ret;
}

BVBufferRef *bv_buffer_ref(BVBufferRef *buf)
{
    BVBufferRef *ret = bv_mallocz(sizeof(*ret));

    if (!ret)
        return NULL;

    *ret = *buf;

    bvpriv_atomic_int_add_and_fetch(&buf->buffer->refcount, 1);

    return ret;
}

void bv_buffer_unref(BVBufferRef **buf)
{
    BVBuffer *b;

    if (!buf || !*buf)
        return;
    b = (*buf)->buffer;
    bv_freep(buf);

    if (!bvpriv_atomic_int_add_and_fetch(&b->refcount, -1)) {
        b->free(b->opaque, b->data);
        bv_freep(&b);
    }
}

int bv_buffer_is_writable(const BVBufferRef *buf)
{
    if (buf->buffer->flags & BV_BUFFER_FLAG_READONLY)
        return 0;

    return bvpriv_atomic_int_get(&buf->buffer->refcount) == 1;
}

void *bv_buffer_get_opaque(const BVBufferRef *buf)
{
    return buf->buffer->opaque;
}

int bv_buffer_get_ref_count(const BVBufferRef *buf)
{
    return buf->buffer->refcount;
}

int bv_buffer_make_writable(BVBufferRef **pbuf)
{
    BVBufferRef *newbuf, *buf = *pbuf;

    if (bv_buffer_is_writable(buf))
        return 0;

    newbuf = bv_buffer_alloc(buf->size);
    if (!newbuf)
        return BVERROR(ENOMEM);

    memcpy(newbuf->data, buf->data, buf->size);
    bv_buffer_unref(pbuf);
    *pbuf = newbuf;

    return 0;
}

int bv_buffer_realloc(BVBufferRef **pbuf, int size)
{
    BVBufferRef *buf = *pbuf;
    uint8_t *tmp;

    if (!buf) {
        /* allocate a new buffer with bv_realloc(), so it will be reallocatable
         * later */
        uint8_t *data = bv_realloc(NULL, size);
        if (!data)
            return BVERROR(ENOMEM);

        buf = bv_buffer_create(data, size, bv_buffer_default_free, NULL, 0);
        if (!buf) {
            bv_freep(&data);
            return BVERROR(ENOMEM);
        }

        buf->buffer->flags |= BUFFER_FLAG_REALLOCATABLE;
        *pbuf = buf;

        return 0;
    } else if (buf->size == size)
        return 0;

    if (!(buf->buffer->flags & BUFFER_FLAG_REALLOCATABLE) ||
        !bv_buffer_is_writable(buf)) {
        /* cannot realloc, allocate a new reallocable buffer and copy data */
        BVBufferRef *new = NULL;

        bv_buffer_realloc(&new, size);
        if (!new)
            return BVERROR(ENOMEM);

        memcpy(new->data, buf->data, BBMIN(size, buf->size));

        bv_buffer_unref(pbuf);
        *pbuf = new;
        return 0;
    }

    tmp = bv_realloc(buf->buffer->data, size);
    if (!tmp)
        return BVERROR(ENOMEM);

    buf->buffer->data = buf->data = tmp;
    buf->buffer->size = buf->size = size;
    return 0;
}

BVBufferPool *bv_buffer_pool_init(int size, BVBufferRef* (*alloc)(int size))
{
    BVBufferPool *pool = bv_mallocz(sizeof(*pool));
    if (!pool)
        return NULL;

    ff_mutex_init(&pool->mutex, NULL);

    pool->size     = size;
    pool->alloc    = alloc ? alloc : bv_buffer_alloc;

    bvpriv_atomic_int_set(&pool->refcount, 1);

    return pool;
}

/*
 * This function gets called when the pool has been uninited and
 * all the buffers returned to it.
 */
static void buffer_pool_free(BVBufferPool *pool)
{
    while (pool->pool) {
        BufferPoolEntry *buf = pool->pool;
        pool->pool = buf->next;

        buf->free(buf->opaque, buf->data);
        bv_freep(&buf);
    }
    ff_mutex_destroy(&pool->mutex);
    bv_freep(&pool);
}

void bv_buffer_pool_uninit(BVBufferPool **ppool)
{
    BVBufferPool *pool;

    if (!ppool || !*ppool)
        return;
    pool   = *ppool;
    *ppool = NULL;

    if (!bvpriv_atomic_int_add_and_fetch(&pool->refcount, -1))
        buffer_pool_free(pool);
}

#if USE_ATOMICS
/* remove the whole buffer list from the pool and return it */
static BufferPoolEntry *get_pool(BVBufferPool *pool)
{
    BufferPoolEntry *cur = *(void * volatile *)&pool->pool, *last = NULL;

    while (cur != last) {
        last = cur;
        cur = bvpriv_atomic_ptr_cas((void * volatile *)&pool->pool, last, NULL);
        if (!cur)
            return NULL;
    }

    return cur;
}

static void add_to_pool(BufferPoolEntry *buf)
{
    BVBufferPool *pool;
    BufferPoolEntry *cur, *end = buf;

    if (!buf)
        return;
    pool = buf->pool;

    while (end->next)
        end = end->next;

    while (bvpriv_atomic_ptr_cas((void * volatile *)&pool->pool, NULL, buf)) {
        /* pool is not empty, retrieve it and append it to our list */
        cur = get_pool(pool);
        end->next = cur;
        while (end->next)
            end = end->next;
    }
}
#endif

static void pool_release_buffer(void *opaque, uint8_t *data)
{
    BufferPoolEntry *buf = opaque;
    BVBufferPool *pool = buf->pool;

    if(BV_CONFIG_MEMORY_POISONING)
        memset(buf->data, BV_MEMORY_POISON, pool->size);

#if USE_ATOMICS
    add_to_pool(buf);
#else
    ff_mutex_lock(&pool->mutex);
    buf->next = pool->pool;
    pool->pool = buf;
    ff_mutex_unlock(&pool->mutex);
#endif

    if (!bvpriv_atomic_int_add_and_fetch(&pool->refcount, -1))
        buffer_pool_free(pool);
}

/* allocate a new buffer and override its free() callback so that
 * it is returned to the pool on free */
static BVBufferRef *pool_alloc_buffer(BVBufferPool *pool)
{
    BufferPoolEntry *buf;
    BVBufferRef     *ret;

    ret = pool->alloc(pool->size);
    if (!ret)
        return NULL;

    buf = bv_mallocz(sizeof(*buf));
    if (!buf) {
        bv_buffer_unref(&ret);
        return NULL;
    }

    buf->data   = ret->buffer->data;
    buf->opaque = ret->buffer->opaque;
    buf->free   = ret->buffer->free;
    buf->pool   = pool;

    ret->buffer->opaque = buf;
    ret->buffer->free   = pool_release_buffer;

#if USE_ATOMICS
    bvpriv_atomic_int_add_and_fetch(&pool->refcount, 1);
    bvpriv_atomic_int_add_and_fetch(&pool->nb_allocated, 1);
#endif

    return ret;
}

BVBufferRef *bv_buffer_pool_get(BVBufferPool *pool)
{
    BVBufferRef *ret;
    BufferPoolEntry *buf;

#if USE_ATOMICS
    /* check whether the pool is empty */
    buf = get_pool(pool);
    if (!buf && pool->refcount <= pool->nb_allocated) {
        bv_log(NULL, BV_LOG_DEBUG, "Pool race dectected, spining to avoid overallocation and eventual OOM\n");
        while (!buf && bvpriv_atomic_int_get(&pool->refcount) <= bvpriv_atomic_int_get(&pool->nb_allocated))
            buf = get_pool(pool);
    }

    if (!buf)
        return pool_alloc_buffer(pool);

    /* keep the first entry, return the rest of the list to the pool */
    add_to_pool(buf->next);
    buf->next = NULL;

    ret = bv_buffer_create(buf->data, pool->size, pool_release_buffer,
                           buf, 0);
    if (!ret) {
        add_to_pool(buf);
        return NULL;
    }
#else
    ff_mutex_lock(&pool->mutex);
    buf = pool->pool;
    if (buf) {
        ret = bv_buffer_create(buf->data, pool->size, pool_release_buffer,
                               buf, 0);
        if (ret) {
            pool->pool = buf->next;
            buf->next = NULL;
        }
    } else {
        ret = pool_alloc_buffer(pool);
    }
    ff_mutex_unlock(&pool->mutex);
#endif

    if (ret)
        bvpriv_atomic_int_add_and_fetch(&pool->refcount, 1);

    return ret;
}
