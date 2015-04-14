/*************************************************************************
    > File Name: bvfilter.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月12日 星期日 19时39分08秒
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

#ifndef BV_FILTER_H
#define BV_FILTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>

/**
 *  --------------downstream ------->
 *  src -----> filter ----...----> sink
 *  <-------------upstream-------
 *
 *  数据流采用两种方式push和pull
 *  push方式是downstream方式
 *  pull方式是upstream方式
 */

enum BVFilterType {
    BV_FILTER_TYPE_NONE = 0,
    BV_FILTER_TYPE_SOURCE,      //只有输出
    BV_FILTER_TYPE_FILTER,      //既有输入又有输出
    BV_FILTER_TYPE_RENDER,      //只有输入
    BV_FILTER_TYPE_UNKNOWN,
};

enum BVFilterState {
    BV_FILTER_STATE_NONE = 0,
    BV_FILTER_STATE_START,
    BV_FILTER_STATE_PAUSE,
    BV_FILTER_STATE_STOP,
};

#if 0
typedef struct _BVFilterBus BVFilterBus;
typedef struct _BVFilterPad BVFilterPad;
typedef struct _BVFilterLink BVFilterLink;
typedef struct _BVFilter BVFilter;
typedef struct _BVFilterContext BVFilterContext;
typedef struct _BVFilterGraph BVFilterGraph;
#endif

/**
 *  Send Message control Filter
 */
enum BVFilterMessage {
    BV_FILTER_MESSAGE_START,
    BV_FILTER_MESSAGE_STOP,
    BV_FILTER_MESSAGE_PAUSE,
};

/**
 *  bus 中添加消息处理函数
 *  bv_filter_bus_add_watch();
 *  message quene
 *  pop memsage push message
 */
typedef struct _BVFilterBus {
    const char *name;
    struct _BVFilterGraph *graph;
    int32_t refcount;
} BVFilterBus;

typedef struct _BVFilterLink {
    struct _BVFilterContext *source;
    struct _BVFilterContext *dest;
    struct _BVFilterPad *srcpad;
    struct _BVFilterPad *dstpad;
} BVFilterLink;

typedef struct _BVFilterPad {
    const char *name;
    enum BVMediaType type;
} BVFilterPad;

typedef struct _BVFilterContext {
    const BVClass *bv_class;
    const uint32_t nb_sources;      //输入pad的个数
    const uint32_t nb_outputs;      //输出pad的个数
} BVFilterContext;

typedef struct _BVFilter {
    const char *name;

    const BVFilterPad *sources;     //输入的pad
    const BVFilterPad *outputs;     //输出的pad

    const BVClass *priv_class;
    int priv_data_size;

    int (*init)(BVFilterContext *ctx, BVDictionary **options);
    int (*exit)(BVFilterContext *ctx);
    int (*query_format)(BVFilterContext *ctx);
    struct _BVFilter *next;
} BVFilter;

typedef struct _BVFilterGraph {
    /* data */
} BVFilterGraph;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_FILTER_H */
