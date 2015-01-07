/*************************************************************************
    > File Name: osdfilter.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月22日 星期一 10时37分25秒
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

#include <osdfilter.h>


static int init_filter(AVFilterGraph *filter_graph, BVOSDConfig *config)
{
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *drawtext_ctx;

    AVFilter *buffersrc;
    AVFilter *buffersink;
    AVFilter *drawtext;

    char args[1024];
    int ret = -1;
    enum AVPixelFormat pix_fmts[] = { config->dst_fmt, AV_PIX_FMT_NONE };
    memset(argc, 0, sizeof(args));
    buffersrc = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");
    drawtext = avfilter_get_by_name("drawtext");

    if (!buffersrc || !buffersink || !drawtext) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find filter %p %p %p\n", buffersrc, buffersink, drawtext);
        ret = AVERROR(EINVAL);
        goto end;
    }

    snprintf(args, sizeof(args),"video_size=%dx%d:pix_fmt=%d:time_base=1/%d", config->with, config->height, config->src_fmt, 25);
    if ((ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "osd_src", args, NULL, filter_graph)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffser source\n");
        goto end;
    }

    snprintf(args, sizeof(args), "fontfile=%s:text='NULL':fontcolor=%s:fontsize=%d:x=0:y=0", config->fontfile, config->fontcolor, config->fontsize);
    if (config->draw_box)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":boxcolor=%s", config->boxcolor);

    if (config->draw_border)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":bordercolor=%s:borderw=%d", config->bordercolor, config->borderw);

    if (config->draw_shadow)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":shadowcolor=%s:shadowx=%d:shadowy=%d", config->shadowcolor, config->shadowx, config->shadowy);

    av_log(NULL, AV_LOG_WARNING, "osd config %s\n", args);
    if ((ret = avfilter_graph_create_filter(&drawtext_ctx, drawtext, "osd_drawtext", args, NULL, filter_graph)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create drawtext filter\n");
        goto end;
    }

    if ((ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "osd_sink", NULL, NULL, filter_graph)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffser sink\n");
        goto end;
    }

    if ((ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0){
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    if((ret = avfilter_link(buffersrc_ctx, 0, drawtext_ctx, 0)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "link buffersrc to drawtext_ctx error\n");
        goto end;
    }

    if((ret = avfilter_link(drawtext_ctx, 0, buffersink_ctx, 0)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "link buffersrc to drawtext_ctx error\n");
        goto end;
    }
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "graph config error\n");
        goto end;
    }
    av_log(NULL, AV_LOG_ERROR, "ret %d\n", ret);
end:
    return ret;

}

AVFilterGraph *ff_osd_create(BVOSDConfig *config)
{
    AVFilterGraph *filter_graph = NULL;
    if (!config)
        return NULL;
    filter_graph = avfilter_graph_alloc();
    if (filter_graph == NULL) {
        av_log(NULL, AV_LOG_ERROR, "alloc filter error");
        return NULL;
    }

    if (init_filter(filter_graph, config) < 0) {
        av_log(NULL, AV_LOG_ERROR, "init filter_graph error");
        avfilter_graph_free(&filter_graph);
        filter_graph = NULL;
    }

    return filter_graph;
}

int ff_osd_update_config(AVFilterGraph *filter_graph, BVOSDConfig *config)
{
    char args[1024];
    if (!filter_graph || !config)
        return AVERROR(EINVAL);
    memset(args, 0, sizeof(args));
    snprintf(args, sizeof(args), "fontfile=%s:text='NULL':fontcolor=%s:fontsize=%d:x=0:y=0", config->fontfile, config->fontcolor, config->fontsize);
    if (config->draw_box)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":boxcolor=%s", config->boxcolor);

    if (config->draw_border)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":bordercolor=%s:borderw=%d", config->bordercolor, config->borderw);

    if (config->draw_shadow)
        snprintf(args + strlen(args), sizeof(args) - strlen(args), ":shadowcolor=%s:shadowx=%d:shadowy=%d", config->shadowcolor, config->shadowx, config->shadowy);

    av_log(NULL, AV_LOG_WARNING, "osd config %s\n", args);

    if (avfilter_graph_send_command(filter_graph, "osd_drawtext", "reinit", args, NULL, 0,  AVFILTER_CMD_FLAG_ONE) < 0) {
        av_log(filter_graph, AV_LOG_ERROR, "update config error\n");
        return AVERROR(ENOSYS);
    }

    return 0;
}

int ff_osd_update_data(AVFilterGraph *filter_graph, AVPacket *packet)
{
    AVFilterContext *osd_drawtext = NULL;
    if (!filter_graph || !packet || !packet->data)
        return AVERROR(EINVAL);
    if (!(osd_drawtext = avfilter_graph_get_filter(filter_graph, "osd_drawtext")))
        return AVERROR(ENOSYS);

    if (av_opt_set(osd_drawtext->priv, "text", packet->data, 0) < 0) {
        av_log(osd_drawtext, AV_LOG_ERROR, "set priv data text error\n");
        return -1;
    }

    return 0;
}

int ff_osd_process(AVFilterGraph *filter_graph, AVFrame *src_frm, AVFrame *dst_frm)
{
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    if (!filter_graph || !src_frm)
        return AVERROR(EINVAL);
    buffersrc_ctx = avfilter_graph_get_filter(filter_graph, "osd_src");
    buffersink_ctx = avfilter_graph_get_filter(filter_graph, "osd_sink");
    if (!buffersrc_ctx || !buffersink_ctx) {
        av_log(filter_graph, AV_LOG_ERROR, "get src sink error\n");
        return AVERROR(ENOSYS);
    }

    if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        goto end;
    }

    while (1) {
        ret = av_buffersink_get_frame(buffersink_ctx, dst_frm);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_log(NULL, AV_LOG_INFO, "end of sink frame\n");
            break;
        }
        if (ret < 0)
            goto end;
        av_log(NULL, AV_LOG_INFO, "got xxxx\n");
        av_frame_unref(dst_frm);
    }
    return 0;
end:
    return -1;
}

int ff_osd_destroy(AVFilterGraph **filter_graph)
{
    return avfilter_graph_free(filter_graph);
}
