/*************************************************************************
    > File Name: utils.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月23日 星期一 12时03分19秒
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

#line 25 "utils.c"

#include <libbvutil/atomic.h>
#include <libbvutil/bvassert.h>

#include "bvcodec.h"

static BVCodecParser *first_parser = NULL;
static BVCodecParser **last_parser = &first_parser;

void bv_codec_parser_register(BVCodecParser *parser)
{
    BVCodecParser **p = last_parser;
    parser->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, parser))
        p = &(*p)->next;
    last_parser = &parser->next;
}

BVCodecParser *bv_codec_parser_next(const BVCodecParser *p)
{
    if (p)
        return p->next;
    else
        return first_parser;
}

BVCodecParserContext *bv_codec_parser_alloc(void)
{
    BVCodecParserContext *s = bv_mallocz(sizeof(BVCodecParserContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "bv codec parser alloc error\n");
    }
    return s;
}

void bv_codec_parser_free(BVCodecParserContext *s)
{
    if (!s) {
        return;
    }
    bv_free(s->priv_data);
    bv_free(s);
    return;
}

BVCodecParserContext *bv_codec_parser_init(enum BVCodecID codec_id)
{
    BVCodecParserContext *parser_context = NULL;
    BVCodecParser *parser = NULL;
    int ret = 0;
    if (codec_id == BV_CODEC_ID_NONE) {
        return NULL;
    }
    while ((parser = bv_codec_parser_next(parser))) {
        if (parser->codec_ids[0] == codec_id ||
            parser->codec_ids[1] == codec_id ||
            parser->codec_ids[2] == codec_id ||
            parser->codec_ids[3] == codec_id ||
            parser->codec_ids[4] == codec_id) {
            goto found;
        }
    }
    return NULL;
found:
    parser_context = bv_codec_parser_alloc();
    if (!parser_context) {
        return NULL;
    }
    parser_context->parser = parser;
    if (parser->priv_data_size) {
        parser_context->priv_data = bv_mallocz(parser->priv_data_size);
        if (!parser_context->priv_data)
            goto fail;
    }

    if (parser->parser_init) {
        ret = parser->parser_init(parser_context);
        if (ret != 0)
            goto fail;
    }
    return parser_context;
fail:
    bv_codec_parser_free(parser_context);
    return NULL;
}

int bv_codec_parser_exit(BVCodecParserContext *s)
{
    if (!s)
        return BVERROR(EINVAL);
    if (s->parser && s->parser->parser_exit) {
        s->parser->parser_exit(s);
    }
    bv_codec_parser_free(s);
    return 0;
}

int bv_codec_parser_parse(BVCodecParserContext *s, BVCodecContext *codec, const uint8_t *data_in, int data_in_size, const uint8_t **data_out, int *data_out_size)
{
    if (!s || !s->parser) {
        return BVERROR(EINVAL);
    }
    if (!s->parser->parser_parse) {
        return BVERROR(ENOSYS);
    }
    if (!data_in || data_in_size <= 0) {
        bv_log(s, BV_LOG_ERROR, "input data size error\n");
        return BVERROR(EINVAL);
    }
    return s->parser->parser_parse(s, codec, data_in, data_in_size, data_out, data_out_size);
}
