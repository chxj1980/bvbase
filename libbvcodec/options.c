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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvcodec.h"

/**
 *  @file
 *  Options definition for BVCodecContext
 */

#include "options_table.h"

static const char * codec_to_name(void *ptr)
{
    BVCodecContext *bvc = (BVCodecContext *)ptr;
    if (bvc->codec)
        return bvc->codec->name;
    return "NULL";
}

static void *codec_child_next(void *obj, void *prev)
{
    BVCodecContext *s = obj;
    if (!prev && s->codec && s->codec->priv_class && s->priv_data)
        return s->priv_data;
    return NULL;
}

static const BVClass *codec_child_class_next(const BVClass *prev)
{
    BVCodec *c = NULL;
#if 0
    /* find the codec that corresponds to prev */
    while (prev && (c = av_codec_next(c)))
        if (c->priv_class == prev)
            break;

    /* find next codec with priv options */
    while (c = av_codec_next(c))
        if (c->priv_class)
            return c->priv_class;
#endif
    return NULL;
}

static const BVClass bv_codec_context_class = {
    .class_name              = "BVCodecContext",
    .item_name               = codec_to_name,
    .option                  = codec_options,
    .version                 = LIBBVUTIL_VERSION_INT,
    .child_next              = codec_child_next,
    .child_class_next        = codec_child_class_next,
    .category                = BV_CLASS_CATEGORY_ENCODER,
};

static int bv_codec_context_get_default(BVCodecContext *s, const BVCodec *codec)
{
    s->bv_class = &bv_codec_context_class;
    s->codec_type = codec ? codec->type : BV_MEDIA_TYPE_UNKNOWN;
    if (codec) {
        s->codec = codec;
        s->codec_id = codec->id;
    }
    bv_opt_set_defaults(s);
    s->time_base = (BVRational){0, 1};
    s->pix_fmt = BV_PIX_FMT_NONE;
    s->sample_fmt = BV_SAMPLE_FMT_NONE;
    if(codec && codec->priv_data_size){
        if(!s->priv_data){
            s->priv_data= bv_mallocz(codec->priv_data_size);
            if (!s->priv_data) {
                return BVERROR(ENOMEM);
            }
        }
        if(codec->priv_class){
            *(const BVClass**)s->priv_data = codec->priv_class;
            bv_opt_set_defaults(s->priv_data);
        }
    }
    return 0;
}

BVCodecContext *bv_codec_context_alloc(const BVCodec *c)
{
    BVCodecContext *s = bv_mallocz(sizeof(BVCodecContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVCodecContext error");
        return NULL;
    }
    bv_codec_context_get_default(s, c);
    return s;
}

void bv_codec_context_free(BVCodecContext * codec)
{
    if (!codec) 
        return;
    bv_opt_free(codec);
    if (codec->codec && codec->codec->priv_class && codec->priv_data)
        bv_opt_free(codec->priv_data);
    bv_freep(&codec->extradata);
    bv_freep(&codec->priv_data);
    bv_free(codec);
    return; 
}

