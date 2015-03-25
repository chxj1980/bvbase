/*************************************************************************
    > File Name: allcodecs.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月23日 星期一 11时41分56秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

#line 25 "allcodecs.c"

#include "bvcodec.h"
#include <config.h>

#define REGISTER_PARSER(X, x)                                           \
    {                                                                   \
        extern BVCodecParser bv_##x##_parser;                           \
        if (BV_CONFIG_##X##_PARSER)                                     \
            bv_codec_parser_register(&bv_##x##_parser);                 \
    }

#if 0
#define REGISTER_ENCODER(X, x)                                          \
    {                                                                   \
        extern BVCodec bv_##x##_encoder;                                \
        if (BV_CONFIG_##X##_ENCODER)                                    \
            bv_codec_register(&bv_##x##_encoder);                       \
    }

#define REGISTER_DECODER(X, x)                                          \
    {                                                                   \
        extern BVCodec bv_##x##_decoder;                                \
        if (BV_CONFIG_##X##_DECODER)                                    \
            bv_codec_register(&bv_##x##_decoder);                       \
    }

#endif
void bv_codec_register_all(void)
{
    static int initialized;
    if (initialized)
        return;
    initialized = 1;

    REGISTER_PARSER(H264, h264);
}

