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

#include "bvconfig.h"

/**
 *  @file
 *  Options definition for BVConfigContext
 */

#include "options_table.h"

static const char *config_to_name(void *ptr)
{
    BVConfigContext *cc = (BVConfigContext *) ptr;
    if (cc->config)
        return cc->config->name;
    return "NULL";
}

static const BVClass bv_config_context_class = {
    .class_name = "BVConfigContext",
    .item_name = config_to_name,
    .option = config_options,
    .version = LIBBVUTIL_VERSION_INT,
    .category = BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_config_context_get_default(BVConfigContext * config)
{
    config->bv_class = &bv_config_context_class;
    bv_opt_set_defaults(config);
}

BVConfigContext *bv_config_context_alloc(void)
{
    BVConfigContext *s = bv_mallocz(sizeof(BVConfigContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVConfigContext error");
        return NULL;
    }
    bv_config_context_get_default(s);
    return s;
}

void bv_config_context_free(BVConfigContext * cfgctx)
{
    if (!cfgctx)
        return;
    bv_opt_free(cfgctx);
    if (cfgctx->config && cfgctx->config->priv_class && cfgctx->priv_data)
        bv_opt_free(cfgctx->priv_data);
    bv_freep(&cfgctx->priv_data);
    bv_free(cfgctx);
    return;
}
