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

static const AVClass bv_config_context_class = {
	.class_name = "BVConfigContext",
	.item_name = config_to_name,
	.option = config_options,
	.version = LIBAVUTIL_VERSION_INT,
	.category = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_config_get_context_default(BVConfigContext * config)
{
	config->av_class = &bv_config_context_class;
	av_opt_set_defaults(config);
}

BVConfigContext *bv_config_alloc_context(void)
{
	BVConfigContext *s = av_mallocz(sizeof(BVConfigContext));
	if (!s) {
		av_log(NULL, AV_LOG_ERROR, "malloc BVConfigContext error");
		return NULL;
	}
	bv_config_get_context_default(s);
	return s;
}

void bv_config_free_context(BVConfigContext * cfgctx)
{
	if (!cfgctx)
		return;
	av_opt_free(cfgctx);
	if (cfgctx->config && cfgctx->config->priv_class && cfgctx->priv_data)
		av_opt_free(cfgctx->priv_data);
	av_freep(&cfgctx->priv_data);
	av_free(cfgctx);
	return;
}
