/*************************************************************************
	> File Name: utils.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月30日 星期二 17时10分17秒
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

#include <libbvutil/atomic.h>

#include "bvmedia.h"

static BVInputMedia *first_ifmt = NULL;
static BVInputMedia **last_ifmt = &first_ifmt;

static BVOutputMedia *first_ofmt = NULL;
static BVOutputMedia **last_ofmt = &first_ofmt;

void bv_input_media_register(BVInputMedia *ifmt)
{
    BVInputMedia **p = last_ifmt;
    ifmt->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, ifmt))
        p = &(*p)->next;
    last_ifmt = &ifmt->next;
}

void bv_output_media_register(BVOutputMedia *media)
{
    BVOutputMedia **p = last_ofmt;

    media->next = NULL;
    while(*p || bvpriv_atomic_ptr_cas((void * volatile *)p, NULL, media))
        p = &(*p)->next;
    last_ofmt = &media->next;
}

BVInputMedia * bv_input_media_next(BVInputMedia *ifmt)
{
    if (ifmt)
        return ifmt->next;
    else
        return first_ifmt;
}

BVOutputMedia *bv_output_media_next(const BVOutputMedia *f)
{
    if (f)
        return f->next;
    else
        return first_ofmt;
}

