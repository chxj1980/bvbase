/*************************************************************************
    > File Name: media.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月30日 星期二 19时18分36秒
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

#line 25 "media.c"

#include "bvmedia.h"
#include <libbvutil/bvstring.h>
#include <libbvutil/opt.h>

BVInputMedia *bv_input_media_find(const char *short_name)
{
    BVInputMedia *fmt = NULL;
    while ((fmt = bv_input_media_next(fmt)))
        if (bv_match_name(short_name, fmt->name))
            return fmt;
    return NULL;
}

static int init_imedia(BVMediaContext *s, const char *url)
{
    BVProbeData pd = { url, NULL, 0 };
    int score = BV_PROBE_SCORE_RETRY;
    int score_max = score;
    BVInputMedia *im = NULL;
    BVInputMedia *im1 = NULL;

    while ((im1 = bv_input_media_next(im1))) {
        if (im1->read_probe) {
            score = im1->read_probe(s, &pd);
        } else if (im1->extensions) {
          if (bv_match_ext(pd.filename, im1->extensions))
              score = BV_PROBE_SCORE_EXTENSION;
        }
        if (bv_match_name(pd.mime_type, im1->mime_type))
            score = BBMAX(score, BV_PROBE_SCORE_MIME);
        if (score > score_max) {
            score_max = score;
            im = im1;
        } else if (score == score_max) {
            im = NULL;
        }
    }
    s->imedia = im;
    return im == NULL ? -1: 0;
}

static int input_media_open_internal(BVMediaContext **fmt, const char *url, BVInputMedia *media, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVMediaContext *s = *fmt;
    int ret = 0;
    if (!s && !(s = bv_media_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", __FILE__, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);

    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;

    if (media)
         s->imedia = media;
    else
        ret = init_imedia(s, url);
    if (ret < 0) {
        ret = BVERROR(EINVAL);
        goto fail;
    }
    if (s->imedia->priv_data_size > 0) {
        s->priv_data = bv_mallocz(s->imedia->priv_data_size);
        if (!s->priv_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        if (s->imedia->priv_class) {
            *(const BVClass **) s->priv_data = s->imedia->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                bv_log(s, BV_LOG_ERROR, "set dict error\n");
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }

#if 0
    //用户自己打开文件
    if (!s->pb && !(s->imedia->flags & BV_MEDIA_FLAGS_NOFILE)) {
        //open bvio
    }
#endif
    if (url)
        bv_strlcpy(s->filename, url, sizeof(s->filename));
    if (!s->imedia->read_header) {
        ret = BVERROR(ENOSYS); 
        goto fail;
    }
    *fmt = s;

    bv_dict_free(&tmp);
    ret = s->imedia->read_header(s);
    if (ret) {
        goto fail;
    }
    return 0;
fail:
    bv_dict_free(&tmp);
    bv_media_context_free(s);
    return ret;
}

int bv_input_media_open(BVMediaContext **fmt, const BVMediaChannel *channel, const char *url,
        BVInputMedia *media, BVDictionary **options)
{
    int ret;
    if (channel) {
        
    }
    ret = input_media_open_internal(fmt, url, media, options);
    if (ret < 0) {
        bv_log(NULL, BV_LOG_ERROR, "media open error\n");
    }
    return ret;
}

int bv_input_media_read(BVMediaContext *s, BVPacket *pkt)
{
    int ret = 0;
    if (!s->imedia || !s->imedia->read_packet)
        return BVERROR(ENOSYS);
    ret = s->imedia->read_packet(s, pkt);
    if (ret == BVERROR(EAGAIN)) {
        ret = 0;
    }
    return ret;
}

int bv_input_media_close(BVMediaContext **fmt)
{
    BVMediaContext *s = *fmt;
    if (s->imedia && s->imedia->read_close)
        s->imedia->read_close(s);
    bv_media_context_free(s);
    *fmt = NULL;
    return 0;
}

static BVMediaDriver *bv_media_driver_guess(const char *short_name)
{
    BVMediaDriver *driver = NULL, *driver_found;
    int score_max, score;
    driver_found = NULL;
    score_max = 0;
    while ((driver = bv_media_driver_next(driver))) {
        score = 0;
        if (driver->name && bv_match_name(short_name, driver->name)) {
            score += 100;
        }
        if (score > score_max) {
            score_max = score;
            driver_found = driver;
        }
    }
    return driver_found;
}

int bv_media_driver_open(BVMediaDriverContext **s, const char *url, const char *short_name, BVMediaDriver *driver, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVMediaDriverContext *driver_context = *s;
    int ret = 0;
    if (!driver_context && !(driver_context = bv_media_driver_context_alloc()))
        return BVERROR(ENOMEM);
    if (!driver_context->bv_class) {
        bv_log(driver_context, BV_LOG_ERROR, "Impossible run here %s %d\n", __FILE__, __LINE__);
        return BVERROR(EINVAL);
    }
    if (options)
        bv_dict_copy(&tmp, *options, 0);
    if (bv_opt_set_dict(driver_context, &tmp) < 0)
        goto fail;
    if (!driver) {
        if (!short_name)
            goto fail;
        driver = bv_media_driver_guess(short_name);
    }
    if (!driver){
        ret = BVERROR(EINVAL);
        goto fail;
    }
    driver_context->driver = driver;
    if (driver_context->driver->priv_data_size > 0) {
        driver_context->priv_data = bv_mallocz(driver_context->driver->priv_data_size);
        if (!driver_context->priv_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        if (driver_context->driver->priv_class) {
            *(const BVClass **) driver_context->priv_data = driver_context->driver->priv_class;
            bv_opt_set_defaults(driver_context->priv_data);
            if ((ret = bv_opt_set_dict(driver_context->priv_data, &tmp)) < 0) {
                bv_log(driver_context, BV_LOG_ERROR, "set dict error\n");
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }
    if (url)
        bv_strlcpy(driver_context->filename, url, sizeof(driver_context->filename));
    if (!driver_context->driver->driver_open) {
        ret = BVERROR(ENOSYS);
        goto fail;
    }
    if (driver_context->driver->driver_open(driver_context) < 0) {
        ret = BVERROR(EIO);
        goto fail;
    }
    *s = driver_context;
    bv_dict_free(&tmp);
    return 0;
fail:
    bv_dict_free(&tmp);
    bv_media_driver_context_free(driver_context);
    return ret;
}

int bv_media_driver_close(BVMediaDriverContext **s)
{
    BVMediaDriverContext *h = *s;
    if (!h) {
        return BVERROR(EINVAL);
    }
    if (h->driver && h->driver->driver_close) {
        h->driver->driver_close(h);
    }
    bv_media_driver_context_free(h);
    *s = NULL;
    return 0;
}

int bv_media_driver_control(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    if (!s) {
        return BVERROR(EINVAL);
    }
    if (!s->driver || !s->driver->driver_control) {
        return BVERROR(ENOSYS);
    }
    return s->driver->driver_control(s, type, pkt_in, pkt_out);
}

