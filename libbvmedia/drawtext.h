/*************************************************************************
    > File Name: drawtext.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月09日 星期四 10时29分35秒
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

#ifndef BV_MEDIA_DRAW_TEXT_H
#define BV_MEDIA_DRAW_TEXT_H

#ifdef __cplusplus
extern "C"{
#endif

//FIXME

typedef struct _BVDrawTextContext {
    enum BVPixelFormat format;
    int16_t index;
    int16_t fontsize;
    int     x, y;
    int     width, height;
    char    fontfile[128];
    uint8_t draw_box;       //draw box around text --- 0 1
    uint8_t draw_border;
    uint8_t draw_shadow;
    uint8_t borderw;        //边框像素的大小
    char    fontcolor[32];  //white black ...#ADDD2F 0xADDD2F read@0.5 @alpha
    char    shadowcolor[32];
    char    boxcolor[32];
    char    bordercolor[32];
    void *priv_data;
} BVDrawTextContext;

int bv_draw_text_open(BVDrawTextContext *drawtext);

/**
 *  draw text content into frame data;
 *  if (*frame) is NULL
 *  alloc frame internal remeber to free it
 */
int bv_draw_text_process(BVDrawTextContext *drawtext, const char *text, BVFrame **frame);

int bv_draw_text_close(BVDrawTextContext *drawtext);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MEDIA_DRAW_TEXT_H */
