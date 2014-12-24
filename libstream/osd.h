/*************************************************************************
	> File Name: osd.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月18日 星期四 09时05分58秒
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

#ifndef BV_OSD_H
#define BV_OSD_H

typedef struct _BVOSDConfig {
    uint8_t fontfile[128];
    uint8_t text[1024];     //UTF-8
    int     x, y;
    int     width, height;
    int     shadowx, shadowy;
    uint8_t draw_box;       //draw box around text --- 0 1
    uint8_t draw_border;
    uint8_t draw_shadow;
    uint8_t borderw;        //边框像素的大小
    char    fontcolor[32];  //white black ...#ADDD2F 0xADDD2F read@0.5 @alpha
    char    shadowcolor[32];
    char    boxcolor[32];
    char    bordercolor[32];
} BVOSDConfig;

int bv_osd_create(AVFormatContext *ic, int index, BVOSDConfig *config);

int bv_osd_update_config(AVFormatContext *ic, int index, BVOSDConfig *config);

int bv_osd_update_data(AVFormatContext *ic, AVPacket *pkt);

int bv_osd_hidden(AVFormatContext *ic, int index);

int bv_osd_destroy(AVFormatContext *ic, int index);
#endif /* end of include guard: BV_OSD_H */
