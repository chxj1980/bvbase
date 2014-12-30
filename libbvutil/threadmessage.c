/*
 * Copyright (c) 2014 Nicolas George
 *
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BVbase; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "fifo.h"
#include "threadmessage.h"
#if HAVE_THREADS
#if HAVE_PTHREADS
#include <pthread.h>
#elif HAVE_W32THREADS
#include "compat/w32pthreads.h"
#elif HAVE_OS2THREADS
#include "compat/os2threads.h"
#else
#error "Unknown threads implementation"
#endif
#endif

struct AVThreadMessageQueue {
#if HAVE_THREADS
    BVFifoBuffer *fifo;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int err_send;
    int err_recv;
    unsigned elsize;
#else
    int dummy;
#endif
};

int bv_thread_message_queue_alloc(AVThreadMessageQueue **mq,
                                  unsigned nelem,
                                  unsigned elsize)
{
#if HAVE_THREADS
    AVThreadMessageQueue *rmq;
    int ret = 0;

    if (nelem > INT_MAX / elsize)
        return BVERROR(EINVAL);
    if (!(rmq = bv_mallocz(sizeof(*rmq))))
        return BVERROR(ENOMEM);
    if ((ret = pthread_mutex_init(&rmq->lock, NULL))) {
        bv_free(rmq);
        return BVERROR(ret);
    }
    if ((ret = pthread_cond_init(&rmq->cond, NULL))) {
        pthread_mutex_destroy(&rmq->lock);
        bv_free(rmq);
        return BVERROR(ret);
    }
    if (!(rmq->fifo = bv_fifo_alloc(elsize * nelem))) {
        pthread_cond_destroy(&rmq->cond);
        pthread_mutex_destroy(&rmq->lock);
        bv_free(rmq);
        return BVERROR(ret);
    }
    rmq->elsize = elsize;
    *mq = rmq;
    return 0;
#else
    *mq = NULL;
    return BVERROR(ENOSYS);
#endif /* HAVE_THREADS */
}

void bv_thread_message_queue_free(AVThreadMessageQueue **mq)
{
#if HAVE_THREADS
    if (*mq) {
        bv_fifo_freep(&(*mq)->fifo);
        pthread_cond_destroy(&(*mq)->cond);
        pthread_mutex_destroy(&(*mq)->lock);
        bv_freep(mq);
    }
#endif
}

#if HAVE_THREADS

static int bv_thread_message_queue_send_locked(AVThreadMessageQueue *mq,
                                               void *msg,
                                               unsigned flags)
{
    while (!mq->err_send && bv_fifo_space(mq->fifo) < mq->elsize) {
        if ((flags & BV_THREAD_MESSAGE_NONBLOCK))
            return BVERROR(EAGAIN);
        pthread_cond_wait(&mq->cond, &mq->lock);
    }
    if (mq->err_send)
        return mq->err_send;
    bv_fifo_generic_write(mq->fifo, msg, mq->elsize, NULL);
    pthread_cond_signal(&mq->cond);
    return 0;
}

static int bv_thread_message_queue_recv_locked(AVThreadMessageQueue *mq,
                                               void *msg,
                                               unsigned flags)
{
    while (!mq->err_recv && bv_fifo_size(mq->fifo) < mq->elsize) {
        if ((flags & BV_THREAD_MESSAGE_NONBLOCK))
            return BVERROR(EAGAIN);
        pthread_cond_wait(&mq->cond, &mq->lock);
    }
    if (bv_fifo_size(mq->fifo) < mq->elsize)
        return mq->err_recv;
    bv_fifo_generic_read(mq->fifo, msg, mq->elsize, NULL);
    pthread_cond_signal(&mq->cond);
    return 0;
}

#endif /* HAVE_THREADS */

int bv_thread_message_queue_send(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags)
{
#if HAVE_THREADS
    int ret;

    pthread_mutex_lock(&mq->lock);
    ret = bv_thread_message_queue_send_locked(mq, msg, flags);
    pthread_mutex_unlock(&mq->lock);
    return ret;
#else
    return BVERROR(ENOSYS);
#endif /* HAVE_THREADS */
}

int bv_thread_message_queue_recv(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags)
{
#if HAVE_THREADS
    int ret;

    pthread_mutex_lock(&mq->lock);
    ret = bv_thread_message_queue_recv_locked(mq, msg, flags);
    pthread_mutex_unlock(&mq->lock);
    return ret;
#else
    return BVERROR(ENOSYS);
#endif /* HAVE_THREADS */
}

void bv_thread_message_queue_set_err_send(AVThreadMessageQueue *mq,
                                          int err)
{
#if HAVE_THREADS
    pthread_mutex_lock(&mq->lock);
    mq->err_send = err;
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);
#endif /* HAVE_THREADS */
}

void bv_thread_message_queue_set_err_recv(AVThreadMessageQueue *mq,
                                          int err)
{
#if HAVE_THREADS
    pthread_mutex_lock(&mq->lock);
    mq->err_recv = err;
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);
#endif /* HAVE_THREADS */
}
