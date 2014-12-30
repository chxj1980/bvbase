/*************************************************************************
	> File Name: options.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月16日 星期二 10时25分30秒
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

#include "bvserver.h"

/**
 *  @file
 *  Options definition for BVServerContext
 */

#include "options_table.h"

static const char * server_to_name(void *ptr)
{
    BVServerContext *dc = (BVServerContext *)ptr;
    if (dc->server)
        return dc->server->name;
    return "NULL";
}
static const AVClass bv_server_context_class = {
    .class_name     = "BVServerContext",
    .item_name      = server_to_name,
    .option         = server_options,
    .version        = LIBAVUTIL_VERSION_INT,
    .category       =   AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_server_get_context_default(BVServerContext *server)
{
    server->av_class = &bv_server_context_class;
    av_opt_set_defaults(server);
}

BVServerContext *bv_server_alloc_context(void)
{
    BVServerContext *s = av_mallocz(sizeof(BVServerContext));
    if (!s) {
        av_log(NULL, AV_LOG_ERROR, "malloc BVServerContext error");
        return NULL;
    }
    bv_server_get_context_default(s);
    return s;
}

void bv_server_free_context(BVServerContext * svrctx)
{
    if (!svrctx) 
        return;
    av_opt_free(svrctx);
    if (svrctx->server && svrctx->server->priv_class && svrctx->priv_data)
        av_opt_free(svrctx->priv_data);
    av_freep(&svrctx->priv_data);
    av_free(svrctx);
    return; 
}

