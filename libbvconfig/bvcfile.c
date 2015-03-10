/*************************************************************************
    > File Name: bvcfile.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年02月04日 星期三 10时50分54秒
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

#line 25 "bvcfile.c"

#include <libbvutil/opt.h>
#include <libbvutil/bvstring.h>

#include "bvcfile.h"

static int init_config_file(BVConfigFileContext *s, const char *url)
{
    BVProbeData pd = { url, NULL, 0 };
    int score = BV_PROBE_SCORE_RETRY;
    int score_max = score;
    BVConfigFile *cf = NULL;
    BVConfigFile *cf1 = NULL;

    while ((cf1 = bv_config_file_next(cf1))) {
        if (cf1->extensions) {
          if (bv_match_ext(pd.filename, cf1->extensions))
              score = BV_PROBE_SCORE_EXTENSION;
        }
        if (score > score_max) {
            score_max = score;
            cf = cf1;
        } else if (score == score_max) {
            cf = NULL;
        }
    }
    s->cfile = cf;
    return cf == NULL ? -1: 0;
}

int bv_config_file_open(BVConfigFileContext **h, const char *url, BVConfigFile *config, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVConfigFileContext *s = *h;
    int ret = 0;
    if (!s && !(s = bv_config_file_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", __FILE__, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);
    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;
    if (config)
        s->cfile = config;
    else
        ret = init_config_file(s, url);
    if (ret < 0) {
        ret = BVERROR(EINVAL);
        goto fail;
    }
    if (s->cfile->priv_data_size > 0) {
        s->priv_data = bv_mallocz(s->cfile->priv_data_size);
        if (!s->priv_data) {
            ret = BVERROR(ENOMEM);
            goto fail;
        }
        if (s->cfile->priv_class) {
            *(const BVClass **) s->priv_data = s->cfile->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                ret = BVERROR(EINVAL);
                goto fail;
            }
        }
    }
    if (url)
        bv_strlcpy(s->filename, url, sizeof(s->filename));
    if (!s->cfile->file_open) {
        ret = BVERROR(ENOSYS);
        goto fail;
    }
    *h = s;
    bv_dict_free(&tmp);
    if ((ret = s->cfile->file_open(s)) < 0) {
        goto fail;
    }
    return 0;
fail:
    bv_dict_free(&tmp);
    bv_config_file_context_free(s);
    return ret;
}

int bv_config_file_dump(BVConfigFileContext *s, const char *filename)
{
    if (!s || !filename)
        return BVERROR(EINVAL);
    if (!s->cfile || !s->cfile->file_dump)
        return BVERROR(ENOSYS);
    return s->cfile->file_dump(s, filename);
}
int bv_config_file_close(BVConfigFileContext **h)
{
    BVConfigFileContext *s = *h;
    if (s->cfile && s->cfile->file_close)
        s->cfile->file_close(s);
    bv_config_file_context_free(s);
    *h = NULL;
    return 0;
}

int bv_config_object_decref(BVConfigFileContext *s, BVConfigObject *obj)
{
    bv_free(obj->name);
    if (s->cfile->decref)
        s->cfile->decref(s, obj);
    return 0;
}

BVConfigObject *bv_config_file_lookup_from(BVConfigFileContext *s, BVConfigObject *obj, const char *path)
{
    return NULL;
}
