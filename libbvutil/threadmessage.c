/*
 * Copyright (c) 2014 Nicolas George
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BVBase; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "fifo.h"
#include "threadmessage.h"
#if BV_HAVE_THREADS
#include "thread.h"
#endif

struct BVThreadMessageQueue {
#if BV_HAVE_THREADS
    BVFifoBuffer *fifo;
    BVMutex lock;
    BVCond cond;
    int err_send;
    int err_recv;
    unsigned elsize;
#else
    int dummy;
#endif
};

int bv_thread_message_queue_alloc(BVThreadMessageQueue **mq,
                                  unsigned nelem,
                                  unsigned elsize)
{
#if BV_HAVE_THREADS
    BVThreadMessageQueue *rmq;
    int ret = 0;

    if (nelem > INT_MAX / elsize)
        return BVERROR(EINVAL);
    if (!(rmq = bv_mallocz(sizeof(*rmq))))
        return BVERROR(ENOMEM);
    if ((ret = bv_mutex_init(&rmq->lock, NULL))) {
        bv_free(rmq);
        return BVERROR(ret);
    }
    if ((ret = bv_cond_init(&rmq->cond, NULL))) {
        bv_mutex_destroy(&rmq->lock);
        bv_free(rmq);
        return BVERROR(ret);
    }
    if (!(rmq->fifo = bv_fifo_alloc(elsize * nelem))) {
        bv_cond_destroy(&rmq->cond);
        bv_mutex_destroy(&rmq->lock);
        bv_free(rmq);
        return BVERROR(ret);
    }
    rmq->elsize = elsize;
    *mq = rmq;
    return 0;
#else
    *mq = NULL;
    return BVERROR(ENOSYS);
#endif /* BV_HAVE_THREADS */
}

void bv_thread_message_queue_free(BVThreadMessageQueue **mq)
{
#if BV_HAVE_THREADS
    if (*mq) {
        bv_fifo_freep(&(*mq)->fifo);
        bv_cond_destroy(&(*mq)->cond);
        bv_mutex_destroy(&(*mq)->lock);
        bv_freep(mq);
    }
#endif
}

#if BV_HAVE_THREADS

static int bv_thread_message_queue_send_locked(BVThreadMessageQueue *mq,
                                               void *msg,
                                               unsigned flags)
{
    while (!mq->err_send && bv_fifo_space(mq->fifo) < mq->elsize) {
        if ((flags & BV_THREAD_MESSAGE_NONBLOCK))
            return BVERROR(EAGAIN);
        bv_cond_wait(&mq->cond, &mq->lock);
    }
    if (mq->err_send)
        return mq->err_send;
    bv_fifo_generic_write(mq->fifo, msg, mq->elsize, NULL);
    bv_cond_signal(&mq->cond);
    return 0;
}

static int bv_thread_message_queue_recv_locked(BVThreadMessageQueue *mq,
                                               void *msg,
                                               unsigned flags)
{
    while (!mq->err_recv && bv_fifo_size(mq->fifo) < mq->elsize) {
        if ((flags & BV_THREAD_MESSAGE_NONBLOCK))
            return BVERROR(EAGAIN);
        bv_cond_wait(&mq->cond, &mq->lock);
    }
    if (bv_fifo_size(mq->fifo) < mq->elsize)
        return mq->err_recv;
    bv_fifo_generic_read(mq->fifo, msg, mq->elsize, NULL);
    bv_cond_signal(&mq->cond);
    return 0;
}

#endif /* BV_HAVE_THREADS */

int bv_thread_message_queue_send(BVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags)
{
#if BV_HAVE_THREADS
    int ret;

    bv_mutex_lock(&mq->lock);
    ret = bv_thread_message_queue_send_locked(mq, msg, flags);
    bv_mutex_unlock(&mq->lock);
    return ret;
#else
    return BVERROR(ENOSYS);
#endif /* BV_HAVE_THREADS */
}

int bv_thread_message_queue_recv(BVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags)
{
#if BV_HAVE_THREADS
    int ret;

    bv_mutex_lock(&mq->lock);
    ret = bv_thread_message_queue_recv_locked(mq, msg, flags);
    bv_mutex_unlock(&mq->lock);
    return ret;
#else
    return BVERROR(ENOSYS);
#endif /* BV_HAVE_THREADS */
}

void bv_thread_message_queue_set_err_send(BVThreadMessageQueue *mq,
                                          int err)
{
#if BV_HAVE_THREADS
    bv_mutex_lock(&mq->lock);
    mq->err_send = err;
    bv_cond_broadcast(&mq->cond);
    bv_mutex_unlock(&mq->lock);
#endif /* BV_HAVE_THREADS */
}

void bv_thread_message_queue_set_err_recv(BVThreadMessageQueue *mq,
                                          int err)
{
#if BV_HAVE_THREADS
    bv_mutex_lock(&mq->lock);
    mq->err_recv = err;
    bv_cond_broadcast(&mq->cond);
    bv_mutex_unlock(&mq->lock);
#endif /* BV_HAVE_THREADS */
}
