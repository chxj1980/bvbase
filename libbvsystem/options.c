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

#include "bvsystem.h"

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
static const BVClass bv_system_context_class = {
    .class_name     = "BVSystemContext",
    .item_name      = system_to_name,
    .option         = system_options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       =   BV_CLASS_CATEGORY_SYSTEM,
};

static void bv_system_context_get_default(BVSystemContext *system)
{
    system->bv_class = &bv_system_context_class;
    bv_opt_set_defaults(system);
}

BVSystemContext *bv_system_context_alloc(void)
{
    BVSystemContext *s = bv_mallocz(sizeof(BVSystemContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVSystemContext error");
        return NULL;
    }
    bv_system_context_get_default(s);
    return s;
}

void bv_system_context_free(BVSystemContext * sysctx)
{
    if (!sysctx) 
        return;
    bv_opt_free(sysctx);
    if (sysctx->system && sysctx->system->priv_class && sysctx->priv_data)
        bv_opt_free(sysctx->priv_data);
    bv_freep(&sysctx->priv_data);
    bv_free(sysctx);
    return; 
}

