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
#include <libbvutil/packet.h>

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
typedef struct _BVFilterPad BVFilterPad;
typedef struct _BVFilterLink BVFilterLink;
typedef struct _BVFilter BVFilter;
typedef struct _BVFilterContext BVFilterContext;
typedef struct _BVFilterGraph BVFilterGraph;
#endif

typedef struct _BVFilterBus BVFilterBus;
/**
 *  Send Message control Filter
 */
enum BVFilterMessage {
    BV_FILTER_MESSAGE_START,
    BV_FILTER_MESSAGE_STOP,
    BV_FILTER_MESSAGE_PAUSE,
};

typedef int (*BVFilterBusFunc) (const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

typedef struct _BVFilterLink {
    struct _BVFilterContext *srcctx;
    struct _BVFilterContext *dstctx;
    struct _BVFilterPad *srcpad;
    struct _BVFilterPad *dstpad;
    enum BVMediaType type;
    int width;
    int height;
    int sample_rate;
    int format;
    BVRational time_base;
} BVFilterLink;

typedef struct _BVFilterPad {
    const char *name;
    enum BVMediaType type;
} BVFilterPad;

typedef struct _BVFilterContext {
    const BVClass *bv_class;
    const uint32_t nb_sources;      //输入pad的个数
    const uint32_t nb_outputs;      //输出pad的个数
    BVFilterPad *source_pads;
    BVFilterPad *output_pads;
    BVFilterLink **source_links;
    BVFilterLink **output_links;
    void *priv_data;
    struct _BVFilterGraph *graph;
    
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
    const char *name;
    BVFilterBus *bus;
} BVFilterGraph;

BVFilterBus *bv_filter_bus_new(BVFilterGraph *graph);

BVFilterBus *bv_filter_bus_ref(BVFilterGraph *graph);

int bv_filter_bus_unref(BVFilterBus *bus);

int bv_filter_bus_add_watch(BVFilterBus *bus, BVFilterContext *filter, const char *name, BVFilterBusFunc *func);

int bv_filter_bus_del_watch(BVFilterBus *bus, const char *name);

int bv_filter_bus_push_message(BVFilterBus *bus);

int bv_filter_bus_pull_message(void);

int bv_filter_bus_destroy(BVFilterGraph *graph);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_FILTER_H */
