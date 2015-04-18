/*************************************************************************
    > File Name: bus.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月15日 星期三 15时18分28秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

#line 25 "bus.c"

#include <libbvutil/list.h>

#include "bvfilter.h"

typedef struct _BVFilterBusElement {
    const char *name;
    struct _BVFilterContext *filter;
    BVFilterBusFunc *cb;
} BVFilterBusElement;

/**
 *  bus 中添加消息处理函数
 *  bv_filter_bus_add_watch();
 *  message quene
 *  pop memsage push message
 */
struct _BVFilterBus {
    const char *name;
    struct _BVFilterGraph *graph;
    BVList *list;
    int32_t refcount;
};


