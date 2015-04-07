/*************************************************************************
    > File Name: exParser.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月24日 星期二 11时19分53秒
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

#include <libbvcodec/bvcodec.h>
#include <libbvutil/bvutil.h>

int main(int argc, const char *argv[])
{
    BVCodecContext *ctx = NULL;
    BVCodecParserContext *parser = NULL;
    bv_codec_register_all();
    bv_log_set_level(BV_LOG_DEBUG);
    ctx = bv_codec_context_alloc(NULL);
    if (!ctx) {
        bv_log(NULL, BV_LOG_ERROR, "codec context alloc error\n");
        return BVERROR(ENOMEM);
    }

    ctx->codec_id = BV_CODEC_ID_H264;
    parser = bv_codec_parser_init(BV_CODEC_ID_H264);
    if (!parser) {
        bv_log(NULL, BV_LOG_ERROR, "not find h264 parser\n");
        bv_codec_context_free(ctx);
    }
    uint8_t data_in[] = {0,0,0,1,0x67, 0x42, 0xe0, 0x1E, 0xDB, 0x02, 0, 0, 0, 1, 
        0x68,0xce,0x32,0xa4,0, 0, 0, 1, 0x68,0xce,0x48,0x24,
        0,0,0,1,0x06,0xe5,0,0,0,1,0x65, 0xd8, 0x00, 0x02, 0x7c, 0x80};
    int size = sizeof(data_in)/ sizeof(data_in[0]);
    bv_codec_parser_parse(parser, ctx, data_in, size, NULL, NULL);
    int i = 0;
    for (i = 0; i < size; i++) {
        printf("%02x ", data_in[i]);
    }
    printf("\n");
    if (ctx->extradata_size) {
        for (i = 0; i < ctx->extradata_size; i++) {
            printf("%02x ", ctx->extradata[i]);
        }
        printf("\n");
    }

    bv_codec_parser_exit(parser);

    bv_codec_context_free(ctx);
    return 0;
}
