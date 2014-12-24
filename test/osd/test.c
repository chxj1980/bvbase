/*************************************************************************
	> File Name: test.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月22日 星期一 11时44分44秒
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

#include "osdfilter.h"

#include <libavutil/avstring.h>

#include <string.h>

static void set_default_config(BVOSDConfig *config)
{
    av_strlcpy(config->fontfile, "font.ttf", sizeof(config->fontfile));
    config->width = 800;
    config->height = 600;
    config->fontsize = 32;
    config->draw_border = 1;
    config->borderw = 2;
    av_strlcpy(config->bordercolor, "black@0.9", sizeof(config->bordercolor));
    av_strlcpy(config->fontcolor, "white@0.8", sizeof(config->fontcolor));
    config->src_fmt = AV_PIX_FMT_RGB24;
    config->dst_fmt = AV_PIX_FMT_RGB24;
}

AVFrame * alloc_picture (enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    int ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

int main(int argc, const char *argv[])
{
    avfilter_register_all();
    AVFilterGraph *filter_graph = NULL;
    BVOSDConfig config;
    AVPacket pkt;
    memset(&config, 0 , sizeof(config));
    set_default_config(&config);
    if (!(filter_graph = ff_osd_create(&config))) {
        av_log(NULL, AV_LOG_ERROR, "create error\n");
        return -1;
    }
    av_new_packet(&pkt, 1024);

    const char *args = "你好联想ABCDEFG";

    av_strlcpy((char *)pkt.data, args, strlen(args));
    if (ff_osd_update_data(filter_graph, &pkt) < 0) {
        av_log(NULL, AV_LOG_ERROR, "update data error\n");
    }

    av_free_packet(&pkt);

    AVFrame *src_frm = alloc_picture(config.src_fmt, config.width, config.height);
    AVFrame *dst_frm = av_frame_alloc();

    if (!src_frm || !dst_frm) {
        av_log(NULL, AV_LOG_ERROR, "No MEM");
        return -1;
    }

    if (ff_osd_process(filter_graph, src_frm, dst_frm) < 0) {
        av_log(filter_graph, AV_LOG_ERROR, "process filter error\n");
        return -1;
    }
    //FIXME
    //

    av_frame_free(&src_frm);
    av_frame_free(&dst_frm);
    ff_osd_destroy(&filter_graph);
    return 0;
}
