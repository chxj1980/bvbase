/*************************************************************************
    > File Name: bvio.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 17时47分52秒
 ************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#ifndef BV_IO_H
#define BV_IO_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>

typedef struct _BVIOContext {
    BVClass *bv_class;
} BVIOContext;


typedef struct BVIOInterruptCB {
    int (*callback)(void*);
    void *opaque;
} BVIOInterruptCB;

#define BV_IO_FLAG_READ     1
#define BV_IO_FLAG_WRITE    2
#define BV_IO_FLAG_READ_WRITE   (BV_IO_FLAG_READ | BV_IO_FLAG_WRITE)

#define BV_IO_FLAG_NONBLOCK 8

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_IO_H */
