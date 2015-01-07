/*************************************************************************
    > File Name: osdfilter.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月22日 星期一 10时26分28秒
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

#ifndef BV_OSD_FILTER_H
#define BV_OSD_FILTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavutil/pixfmt.h>

typedef struct _BVOSDConfig {
    uint8_t fontfile[128];
//    uint8_t text[1024];     //UTF-8
    int     x, y;
    int     width, height;
    int     shadowx, shadowy;
    uint8_t draw_box;       //draw box around text --- 0 1
    uint8_t draw_border;
    uint8_t draw_shadow;
    uint8_t borderw;        //边框像素的大小
    uint32_t fontsize;
    char    fontcolor[32];  //white black ...#ADDD2F 0xADDD2F read@0.5 @alpha
    char    shadowcolor[32];
    char    boxcolor[32];
    char    bordercolor[32];
    AVPixelFormat src_fmt;  //输入格式  创建后不可更改
    AVPixelFormat dst_fmt;  //输出格式  创建后不可更改
} BVOSDConfig;


AVFilterGraph *ff_osd_create(BVOSDConfig *config);

int ff_osd_update_config(AVFilterGraph *filter_graph, BVOSDConfig *config);

int ff_osd_update_data(AVFilterGraph *filter_graph, AVPacket *packet);

int ff_osd_process(AVFilterGraph *filter_graph, AVFrame *src_frm, AVFrame *dst_frm);

int ff_osd_destroy(AVFilterGraph **filter_graph);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_OSD_FILTER_H */
