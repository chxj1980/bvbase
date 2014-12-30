/*************************************************************************
	> File Name: utils.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年09月25日 星期四 15时49分34秒
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

#include <libavutil/avstring.h>
#include <pthread.h>

#include "bvdevice.h"

const char FILE_NAME[] = "utils.c";

static pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER;

static BVDevice *first_dev = NULL;
static BVDevice **last_dev = &first_dev;

#define DEFAULT_BUFFER_SIZE (4096)

static void *bvpriv_atomic_ptr_cas(void * volatile *ptr, void *oldval, void *newval)
{
    void *ret;
    pthread_mutex_lock(&atomic_lock);
    ret = *ptr;
    if (*ptr == oldval)
        *ptr = newval;
    pthread_mutex_unlock(&atomic_lock);
    return ret;
}

int bv_device_register(BVDevice * dev)
{
    BVDevice **p = last_dev;
    dev->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, dev))
        p = &(*p)->next;
    last_dev = &dev->next;
    return 0;
}

BVDevice *bv_device_next(BVDevice * dev)
{
    if (dev)
        return dev->next;
    else
        return first_dev;
}

BVDevice *bv_device_find_device(enum BVDeviceType type)
{
    BVDevice *dev = NULL;
    if (first_dev == NULL) {
        av_log(NULL, AV_LOG_ERROR, "BVDevice Not RegisterAll");
        return NULL;
    }

    while ((dev = bv_device_next(dev))) {
        if (dev->type == type) {
            return dev;
        }
    }
    return NULL;
}

/**
 *  get BVDevice from url
 */
static int init_device(BVDeviceContext *s, const char *url)
{
    BVDevice *dev = NULL;
    BVDevice *dev2 = NULL;
    int max_score = 0;
    int score = 0;
    char args[64] = { 0 };
    if (!url) {
        av_log(s, AV_LOG_WARNING, "url is NULL\n");
        return -1;
    }
    if (sscanf(url, "%[^:]", args) < 0) {
        av_log(s, AV_LOG_ERROR, "INVAL url %s\n", url);
        return AVERROR(EINVAL);
    }

    while ((dev = bv_device_next(dev))) {
        if (dev->dev_probe) {
            score = dev->dev_probe(s, args);
            if (score > max_score) {
                max_score = score;
                dev2 = dev;
            }
        }
    }

    s->device = dev2;
    if (dev2 == NULL) {
        av_log(s, AV_LOG_WARNING, "Not Find Device\n");
        return -1;
    }
    return 0;
}

int bv_device_open(BVDeviceContext ** h, BVDevice *dev, const char *url, AVDictionary **options)
{
    AVDictionary *tmp = NULL;
    BVDeviceContext *s = *h;
    int ret = 0;
    if (!s && !(s = bv_device_alloc_context()))
        return AVERROR(ENOMEM);
    if (!s->av_class) {
        av_log(s, AV_LOG_ERROR, "Impossible run here %s %d\n", FILE_NAME, __LINE__);
        return AVERROR(EINVAL);
    }

    if (options)
        av_dict_copy(&tmp, *options, 0);

    if (av_opt_set_dict(s, &tmp) < 0)
        goto fail;
    if (s->buffer_size > 0) {
        s->buffer = av_malloc(s->buffer_size);
        if (!s->buffer) {
            goto fail;
        }
        s->buffer_ptr = s->buffer;
        s->buffer_end = s->buffer_ptr + s->buffer_size;
    }

    if (dev)
         s->device = dev;
    else
        ret = init_device(s, url);
    if (ret < 0) {
        return AVERROR(ENOSYS);
    }
    if (s->device->priv_data_size > 0) {
        s->priv_data = av_mallocz(s->device->priv_data_size);
        if (!s->priv_data)
            goto fail;
        if (s->device->priv_class) {
            *(const AVClass **) s->priv_data = s->device->priv_class;
            av_opt_set_defaults(s->priv_data);
            if ((ret = av_opt_set_dict(s->priv_data, &tmp)) < 0) {
                av_log(s, AV_LOG_ERROR, "set dict error\n");
                goto fail;
            }
        }
    }

    if (url)
        av_strlcpy(s->url, url, sizeof(s->url));
    if (!s->device->dev_open)
        goto fail;
    *h = s;

    av_dict_free(&tmp);
    return s->device->dev_open(s);
fail:
    av_dict_free(&tmp);
    bv_device_free_context(s);
    *h = NULL;
    return ret;
}

int bv_device_read(BVDeviceContext * h, void *buf, size_t size)
{
    if (!h->device || !h->device->dev_read)
        return AVERROR(ENOSYS);
    return h->device->dev_read(h, buf, size);
}

int bv_device_write(BVDeviceContext * h, const void *buf, size_t size)
{
    return size;
}

int64_t bv_device_seek(BVDeviceContext * h, int64_t pos, int whence)
{
    return 0;
}

int bv_device_control(BVDeviceContext * h, enum BVDeviceMessageType type, const BVDevicePacket *pkt_in, BVDevicePacket *pkt_out)
{
    int ret = 0;
    if (!h->device || !h->device->dev_control) {
        av_log(h, AV_LOG_ERROR, "Not Support Control\n");
        return AVERROR(ENOSYS);
    }
    ret = h->device->dev_control(h, type, pkt_in, pkt_out);
    return ret;
}

int bv_device_get_fd(BVDeviceContext *h)
{
    if (!h->device || !h->device->get_fd)
        return AVERROR(ENOSYS);
    return h->device->get_fd(h);
}

int bv_device_close(BVDeviceContext ** h)
{
    BVDeviceContext *s = *h;
    if (s->device && s->device->dev_close)
        s->device->dev_close(s);
    bv_device_free_context(s);
    *h = NULL;
    return 0;
}
