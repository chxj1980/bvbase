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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvmedia.h"

/**
 *  @file
 *  Options definition for BVMediaContext
 */

#include "options_table.h"

static const char * media_to_name(void *ptr)
{
    BVMediaContext *fc = (BVMediaContext *)ptr;
    if (fc->imedia)
        return fc->imedia->name;
    if (fc->omedia)
        return fc->omedia->name;
    return "NULL";
}
static const BVClass bv_media_context_class = {
    .class_name     = "BVMediaContext",
    .item_name      = media_to_name,
    .option         = media_options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_MUXER,
};

static void bv_media_context_get_default(BVMediaContext *media)
{
    media->bv_class = &bv_media_context_class;
    bv_opt_set_defaults(media);
}

BVMediaContext *bv_media_context_alloc(void)
{
    BVMediaContext *s = bv_mallocz(sizeof(BVMediaContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVMediaContext error");
        return NULL;
    }
    bv_media_context_get_default(s);
    return s;
}

void bv_media_context_free(BVMediaContext * s)
{
    int i;
    if (!s) 
        return;
    bv_opt_free(s);
    if (s->imedia && s->imedia->priv_class && s->priv_data)
        bv_opt_free(s->priv_data);
    if (s->omedia && s->omedia->priv_class && s->priv_data)
        bv_opt_free(s->priv_data);

    for (i = s->nb_streams - 1; i >= 0; i--) {
        bv_stream_free(s, s->streams[i]);
    }

    bv_freep(&s->priv_data);
    bv_freep(&s->streams);
    bv_free(s);
    return; 
}

