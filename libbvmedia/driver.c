/*************************************************************************
    > File Name: driver.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月08日 星期三 13时50分41秒
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

#line 25 "driver.c"

#include <libbvutil/bvstring.h>

#include "driver.h"

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
    int ret = BVERROR(EINVAL);
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

