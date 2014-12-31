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

#include "bvformat.h"

/**
 *  @file
 *  Options definition for BVFormatContext
 */

#include "options_table.h"

static const char * format_to_name(void *ptr)
{
    BVFormatContext *fc = (BVFormatContext *)ptr;
    if (fc->iformat)
        return fc->iformat->name;
    if (fc->oformat)
        return fc->oformat->name;
    return "NULL";
}
static const BVClass bv_format_context_class = {
    .class_name     = "BVFormatContext",
    .item_name      = format_to_name,
    .option         = format_options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_MUXER,
};

static void bv_format_get_context_default(BVFormatContext *format)
{
    format->bv_class = &bv_format_context_class;
    bv_opt_set_defaults(format);
}

BVFormatContext *bv_format_context_alloc(void)
{
    BVFormatContext *s = bv_mallocz(sizeof(BVFormatContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVFormatContext error");
        return NULL;
    }
    bv_format_get_context_default(s);
    return s;
}

void bv_format_context_free(BVFormatContext * fmtctx)
{
    if (!fmtctx) 
        return;
    bv_opt_free(fmtctx);
    if (fmtctx->iformat && fmtctx->iformat->priv_class && fmtctx->priv_data)
        bv_opt_free(fmtctx->priv_data);
    if (fmtctx->oformat && fmtctx->oformat->priv_class && fmtctx->priv_data)
        bv_opt_free(fmtctx->priv_data);
    bv_freep(&fmtctx->priv_data);
    bv_free(fmtctx);
    return; 
}

