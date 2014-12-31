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

#include "bvformat.h"

static BVInputFormat *first_ifmt = NULL;
static BVInputFormat **last_ifmt = &first_ifmt;

static BVOutputFormat *first_ofmt = NULL;
static BVOutputFormat **last_ofmt = &first_ofmt;

void bv_input_format_register(BVInputFormat *ifmt)
{
    BVInputFormat **p = last_ifmt;
    ifmt->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, ifmt))
        p = &(*p)->next;
    last_ifmt = &ifmt->next;
}

void bv_output_format_register(BVOutputFormat *format)
{
    BVOutputFormat **p = last_ofmt;

    format->next = NULL;
    while(*p || bvpriv_atomic_ptr_cas((void * volatile *)p, NULL, format))
        p = &(*p)->next;
    last_ofmt = &format->next;
}

BVInputFormat * bv_input_format_next(BVInputFormat *ifmt)
{
    if (ifmt)
        return ifmt->next;
    else
        return first_ifmt;
}

BVOutputFormat *bv_output_format_next(const BVOutputFormat *f)
{
    if (f)
        return f->next;
    else
        return first_ofmt;
}

