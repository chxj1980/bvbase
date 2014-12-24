/**
 *          File: bvpu_pj.c
 *
 *        Create: 2014年12月23日 星期二 16时15分59秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */
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
 * Copyright (C) @ BesoVideo, 2014
 */

#include <stdio.h>

#include "bvpu_pj.h"

static int bvpu_pj_inited = 0;

static pj_caching_pool pj_pool;

BVPU_SDK_Result bvpu_pj_init()
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    if (bvpu_pj_inited) return result;

    pj_status_t status = pj_init();
    if (status != PJ_SUCCESS) {
        return BVPU_SDK_RESULT_E_PJ_INIT_FAIL;
    }

    // Create caching pool.
    pj_caching_pool_init(&pj_pool, NULL, 1024*1024);

    bvpu_pj_inited = 1;
    return result;
}

BVPU_SDK_Result bvpu_pj_deinit()
{
    BVPU_SDK_Result result = BVPU_SDK_RESULT_S_OK;
    if (!bvpu_pj_inited) return result;
    
    pj_caching_pool_destroy(&pj_pool);

    bvpu_pj_inited = 0;
    return result;
}

pj_caching_pool *bvpu_pj_get_pj_pool()
{
    if (!bvpu_pj_inited) return NULL;

    return &pj_pool;
}


/*=============== End of file: bvpu_pj.c ==========================*/
