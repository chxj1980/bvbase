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

// This header should only be used to simplify code where
// threading is optional, not as a generic threading abstraction.

#ifndef BVUTIL_THREAD_H
#define BVUTIL_THREAD_H

#include "config.h"

#if BV_HAVE_PTHREADS || BV_HAVE_W32THREADS || BV_HAVE_OS2THREADS

#define USE_ATOMICS 0

#if BV_HAVE_PTHREADS
#include <pthread.h>
#elif BV_HAVE_W32THREADS
#include "compat/w32pthreads.h"
#elif BV_HAVE_OS2THREADS
#include "compat/os2threads.h"
#else
#error "Unknown threads implementation"
#endif

#define BVMutex pthread_mutex_t
#define BVCond  pthread_cond_t

#define bv_mutex_init       pthread_mutex_init
#define bv_mutex_lock       pthread_mutex_lock
#define bv_mutex_unlock     pthread_mutex_unlock
#define bv_mutex_destroy    pthread_mutex_destroy

#define bv_cond_init        pthread_cond_init
#define bv_cond_wait        pthread_cond_wait
#define bv_cond_signal      pthread_cond_signal
#define bv_cond_broadcast   pthread_cond_broadcast
#define bv_cond_destroy     pthread_cond_destroy

#else

#define USE_ATOMICS 1

#define BVMutex char
#define BVCond  char

#define bv_mutex_init(mutex, attr)      (0)
#define bv_mutex_lock(mutex)            (0)
#define bv_mutex_unlock(mutex)          (0)
#define bv_mutex_destroy(mutex)         (0)

#define bv_cond_init(cond, attr)        (0) 
#define bv_cond_wait(cond, attr)        (0)
#define bv_cond_signal(cond)            (0) 
#define bv_cond_broadcast(cond)         (0)
#define bv_cond_destroy(cond)           (0)

#endif

#endif /* BVUTIL_THREAD_H */
