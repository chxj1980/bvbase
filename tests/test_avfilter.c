/*************************************************************************
	> File Name: test_avfilter.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月12日 星期五 09时50分16秒
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

#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavutil/pixfmt.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define WIDTH (800)
#define HEIGHT (600)
#define SRC_FORMAT (AV_PIX_FMT_RGB24)
#define DST_FORMAT (AV_PIX_FMT_RGB555LE)
#define RATE (25)

static int init_filter(AVFilterGraph *filter_graph)
{

	AVFilterContext *buffersrc_ctx;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *drawtext_ctx;

	AVFilter *buffersrc;
	AVFilter *buffersink;
	AVFilter *drawtext;

	char args[1024];
	int ret;
    enum AVPixelFormat pix_fmts[] = { DST_FORMAT,AV_PIX_FMT_NONE };
	buffersrc = avfilter_get_by_name("buffer");
	buffersink = avfilter_get_by_name("buffersink");
	drawtext = avfilter_get_by_name("drawtext");

	if (!buffersrc || !buffersink || !drawtext) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find filter %p %p %p\n", buffersrc, buffersink, drawtext);
		ret = AVERROR(EINVAL);
		goto end;
	}


	snprintf(args, sizeof(args),"video_size=%dx%d:pix_fmt=%d:time_base=1/%d", WIDTH, HEIGHT, SRC_FORMAT, RATE);
	if ((ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "osd_src", args, NULL, filter_graph)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create buffser source\n");
		goto end;
	}

	snprintf(args, sizeof(args), "fontfile=font.ttf:text='好ABCHel你lo World':fontcolor=red:fontsize=32:borderw=2:bordercolor=black:x=20:y=400");
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
	int ret;
	AVFilterGraph *filter_graph;
	AVFilterContext *buffersrc_ctx;
	AVFilterContext *buffersink_ctx;

	avfilter_register_all();
	av_log_set_level(AV_LOG_DEBUG);

	if (!(filter_graph = avfilter_graph_alloc())) {
		ret = AVERROR(ENOMEM);
		goto end;
	}
	if ((ret = init_filter(filter_graph)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "init filter error\n");
		goto end;
	}
    buffersrc_ctx = avfilter_graph_get_filter(filter_graph, "osd_src");
    if (buffersrc_ctx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Error get src from the filtergraph\n");
        goto end;
    }

    buffersink_ctx = avfilter_graph_get_filter(filter_graph, "osd_sink");
    if (buffersink_ctx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Error get sink from the filtergraph\n");
        goto end;
    }
	AVFrame *frame = alloc_picture(SRC_FORMAT, 800, 600);
   // AVFrame *filt_frame = alloc_picture(DST_FORMAT, 800, 600);
    AVFrame *filt_frame = av_frame_alloc();
#if     1
    int i = 0;
    for (i = 0; i < 1000; i++) {
         if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
            goto end;
         }
        /* pull filtered frames from the filtergraph */
    }

    while (1) {
        ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_log(NULL, AV_LOG_INFO, "end of sink frame\n");
            break;
        }
        if (ret < 0)
            goto end;
        av_log(NULL, AV_LOG_INFO, "got xxxx\n");
        av_frame_unref(filt_frame);
    }
#endif
end:
    av_frame_free(&filt_frame);
    av_frame_free(&frame);
    avfilter_graph_free(&filter_graph);
	return ret;
}

