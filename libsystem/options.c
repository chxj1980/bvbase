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

#include "system.h"

/**
 *  @file
 *  Options definition for BVSystemContext
 */

#include "options_table.h"

static const char * system_to_name(void *ptr)
{
    BVSystemContext *sc = (BVSystemContext *)ptr;
    if (sc->system)
        return sc->system->name;
    return "NULL";
}
static const AVClass bv_system_context_class = {
    .class_name     = "BVSystemContext",
    .item_name      = system_to_name,
    .option         = system_options,
    .version        = LIBAVUTIL_VERSION_INT,
    .category       =   AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_system_get_context_default(BVSystemContext *system)
{
    system->av_class = &bv_system_context_class;
    av_opt_set_defaults(system);
}

BVSystemContext *bv_system_alloc_context(void)
{
    BVSystemContext *s = av_mallocz(sizeof(BVSystemContext));
    if (!s) {
        av_log(NULL, AV_LOG_ERROR, "malloc BVSystemContext error");
        return NULL;
    }
    bv_system_get_context_default(s);
    return s;
}

void bv_system_free_context(BVSystemContext * sysctx)
{
    if (!sysctx) 
        return;
    av_opt_free(sysctx);
    if (sysctx->system && sysctx->system->priv_class && sysctx->priv_data)
        av_opt_free(sysctx->priv_data);
    av_freep(&sysctx->priv_data);
    av_free(sysctx);
    return; 
}

