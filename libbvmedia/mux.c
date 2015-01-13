/*************************************************************************
    > File Name: mux.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月11日 星期日 16时19分02秒
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

#include "bvmedia.h"
#include <libbvutil/bvstring.h>
#include <libbvutil/opt.h>

static const char *FILE_NAME = "mux.c";

int bv_output_media_open(BVMediaContext **fmt, const char *url, const char *format, BVOutputMedia *media, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVMediaContext *s = *fmt;
    int ret = 0;
    if (!s && !(s = bv_media_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", FILE_NAME, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);

    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;

    if (!media) {
        if (format) {
            media = bv_output_media_guess(format, NULL, NULL);
            if (!media) {
                bv_log(s, BV_LOG_ERROR, "cant find format %s in output media\n", format);
                ret = BVERROR(EINVAL);
                goto fail;
            }
        } else {
            media = bv_output_media_guess(NULL, url, NULL);
            if (!media) {
                bv_log(s, BV_LOG_ERROR, "cant find url %s suitable output media\n", url);
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }
    s->omedia = media;
    if (s->omedia->priv_data_size > 0) {
        s->priv_data = bv_mallocz(s->omedia->priv_data_size);
        if (!s->priv_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        if (s->omedia->priv_class) {
            *(const BVClass **) s->priv_data = s->omedia->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                bv_log(s, BV_LOG_ERROR, "set dict error\n");
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }

    if (url)
        bv_strlcpy(s->filename, url, sizeof(s->filename));
    *fmt = s;
    bv_dict_free(&tmp);
    return 0;
fail:
    bv_dict_free(&tmp);
    bv_media_context_free(s);
    return ret;
}

static int init_muxer(BVMediaContext *s, BVDictionary **options)
{
    int ret = 0;
    int i = 0;
    BVDictionary *tmp = NULL;
    BVStream *st;
    BVCodecContext *codec = NULL;
    BVOutputMedia *omedia = s->omedia;

    if (options)
        bv_dict_copy(&tmp, *options, 0);

    if (ret = bv_opt_set_dict(s, &tmp) < 0)
        goto fail;
    if (s->priv_data && s->omedia->priv_class && *(const BVClass **)s->priv_data == s->omedia->priv_class
            && (ret = bv_opt_set_dict2(s->priv_data, &tmp, BV_OPT_SEARCH_CHILDREN)) < 0)
        goto fail;

    if (s->nb_streams == 0 && !(omedia->flags & BV_MEDIA_FLAGS_NOSTREAMS)) {
        bv_log(s, BV_LOG_ERROR, "No Streams to mux\n");
        ret = BVERROR(EINVAL);
        goto fail;
    }

    for (i = 0; i < s->nb_streams; i++) {
       st = s->streams[i];
       codec = st->codec;
       if (!st->time_base.num) {
           if (codec->codec_type == BV_MEDIA_TYPE_AUDIO && codec->sample_rate) {
//               bvpriv_set_pts_info(st, 64, 1, codec->sample_rate);
           } else {
//               bvpriv_set_pts_info(st, 33, 1, 90000);
           }
       }

       switch (codec->codec_type) {
           case BV_MEDIA_TYPE_AUDIO:
               if (codec->sample_rate <= 0) {
                   bv_log(s, BV_LOG_ERROR, "sample rate not set\n");
                   ret = BVERROR(EINVAL);
                   goto fail;
               }
               break;
           case BV_MEDIA_TYPE_VIDEO:
               if ((codec->width <= 0) || codec->height <= 0) {
                   bv_log(s, BV_LOG_ERROR, "video size not set\n");
                   ret = BVERROR(EINVAL);
                   goto fail;
               }
               break;
           default:
               bv_log(s, BV_LOG_WARNING, "not support now\n");
               break;
       }
    }

fail:
    bv_dict_free(&tmp);
    return ret;
}

int bv_output_media_write_header(BVMediaContext *s, BVDictionary **options)
{
    int ret = 0;

    if (ret = init_muxer(s, options))
        return ret;

    if (s->omedia->write_header) {
        ret = s->omedia->write_header(s);
        if (ret < 0) {
            return ret;
        }
    }
    return 0;
}

int bv_output_media_write(BVMediaContext *s, BVPacket *pkt)
{
    return BVERROR(ENOSYS);
}

int bv_output_media_close(BVMediaContext **fmt)
{
    return BVERROR(ENOSYS);
}
