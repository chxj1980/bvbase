/**
 *          File: bvpu_pj.h
 *
 *        Create: 2014年12月23日 星期二 16时12分13秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
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

#ifndef  __BVPU_PJ_H
#define  __BVPU_PJ_H

#include "BVPU_SDK_DataTypes.h"

#include "pjlib.h"

// Init all thing of pj
BVPU_SDK_Result bvpu_pj_init();

BVPU_SDK_Result bvpu_pj_deinit();

// Get global caching pool. 
// Use it to malloc 
pj_caching_pool *bvpu_pj_get_pj_pool();

#endif   // __BVPU_PJ_H

/*=============== End of file: bvpu_pj.h =====================*/
