/*************************************************************************
    > File Name: allmedias.c
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

#include "bvmedia.h"
#include <config.h>

#define REGISTER_OUTDEV(X, x)                                          \
    {                                                                  \
        extern BVOutputMedia bv_##x##_muxer;                           \
        if (BV_CONFIG_##X##_OUTDEV)                                    \
            bv_output_media_register(&bv_##x##_muxer);                 \
    }

#define REGISTER_INDEV(X, x)                                           \
    {                                                                  \
        extern BVInputMedia bv_##x##_demuxer;                          \
        if (BV_CONFIG_##X##_INDEV)                                     \
            bv_input_media_register(&bv_##x##_demuxer);                \
    }

#define REGISTER_INOUTDEV(X, x) REGISTER_OUTDEV(X, x); REGISTER_INDEV(X, x)

#define REGISTER_MUXER(X, x)                                           \
    {                                                                  \
        extern BVOutputMedia bv_##x##_muxer;                           \
        if (BV_CONFIG_##X##_MUXER)                                     \
            bv_output_media_register(&bv_##x##_muxer);                 \
    }

#define REGISTER_DEMUXER(X, x)                                         \
    {                                                                  \
        extern BVInputMedia bv_##x##_demuxer;                          \
        if (BV_CONFIG_##X##_DEMUXER)                                   \
            bv_input_media_register(&bv_##x##_demuxer);                \
    }

#define REGISTER_DRIVER(X, x)                                         \
    {                                                                 \
        extern BVMediaDriver bv_##x##_driver;                         \
        if (BV_CONFIG_##X##_DRIVER)                                   \
            bv_media_driver_register(&bv_##x##_driver);               \
    }


#if BV_CONFIG_ONVIFAVE_INDEV
#include <libavformat/avformat.h>
#endif
void bv_media_register_all(void)
{
    static int initialized;
    if (initialized)
        return ;
    initialized = 1;
    REGISTER_INDEV(HISAVI, hisavi);
    REGISTER_INDEV(HISAVE, hisave);
    REGISTER_INDEV(ONVIFAVE, onvifave);
    REGISTER_MUXER(DAV, dav);
    REGISTER_DEMUXER(DAV, dav);
    REGISTER_OUTDEV(HISAVO, hisavo);
    REGISTER_OUTDEV(HISAVD, hisavd);
#if BV_CONFIG_ONVIFAVE_INDEV
    av_register_all();
    avformat_network_init();
#endif

    REGISTER_DRIVER(TW2866, tw2866);
}
