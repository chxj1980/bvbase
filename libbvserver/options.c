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
static const BVClass bv_server_context_class = {
    .class_name     = "BVServerContext",
    .item_name      = server_to_name,
    .option         = server_options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       =   BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_server_context_get_default(BVServerContext *server)
{
    server->bv_class = &bv_server_context_class;
    bv_opt_set_defaults(server);
}

BVServerContext *bv_server_context_alloc(void)
{
    BVServerContext *s = bv_mallocz(sizeof(BVServerContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVServerContext error");
        return NULL;
    }
    bv_server_context_get_default(s);
    return s;
}

void bv_server_context_free(BVServerContext * svrctx)
{
    if (!svrctx) 
        return;
    bv_opt_free(svrctx);
    if (svrctx->server && svrctx->server->priv_class && svrctx->priv_data)
        bv_opt_free(svrctx->priv_data);
    bv_freep(&svrctx->priv_data);
    bv_free(svrctx);
    return; 
}

