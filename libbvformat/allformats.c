/*************************************************************************
	> File Name: allformats.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月30日 星期二 16时56分59秒
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

#include "bvformat.h"
#include <config.h>

#define REGISTER_OUTDEV(X, x)                                           \
    {                                                                   \
        extern BVOutputFormat bv_##x##_muxer;                           \
        if (CONFIG_##X##_OUTDEV)                                        \
            bv_register_output_format(&bv_##x##_muxer);                 \
    }

#define REGISTER_INDEV(X, x)                                            \
    {                                                                   \
        extern BVInputFormat bv_##x##_demuxer;                          \
        if (CONFIG_##X##_INDEV)                                         \
            bv_register_input_format(&bv_##x##_demuxer);                \
    }

#define REGISTER_INOUTDEV(X, x) REGISTER_OUTDEV(X, x); REGISTER_INDEV(X, x)

void bv_format_register_all(void)
{
    static int initialized;
    if (initialized)
        return ;
    initialized = 1;
    REGISTER_INDEV(HISAVE, hisave);
}
