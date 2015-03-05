/*************************************************************************
    > File Name: json.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月30日 星期四 09时59分34秒
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

#line 25 "json.c"

#include <libbvutil/opt.h>

#include "bvcfile.h"
#include "jansson.h"

typedef struct _JsonConfigFileContext {
    const BVClass *bv_class;
    json_t *root;
    int fd;
}JsonConfigFileContext;

static int save_root_tree(BVConfigFileContext *s)
{
    JsonConfigFileContext *json = s->priv_data;
    s->root = bv_mallocz(sizeof(BVConfigObject));
    if (!s->root) {
        json_decref(json->root);
        return BVERROR(ENOMEM);
    }
    s->root->priv_data =json->root;
    //递归
    return 0;
}

static int jansson_file_open(BVConfigFileContext *s)
{
    JsonConfigFileContext *json = s->priv_data;
    json_error_t error;
    if (s->filename[0] == '\0') {
        bv_log(s, BV_LOG_ERROR, "json file name is NULL\n");
        return BVERROR(EINVAL);
    }
    if (!(json->root = json_load_file(s->filename, 0, &error))) {
        bv_log(json, BV_LOG_ERROR, "open json file %s error at line %d column %d\n", s->filename,
            error.line, error.column);
        return BVERROR(EIO);
    }
    //save root
    if (save_root_tree(s) < 0) {
        bv_log(s, BV_LOG_ERROR, "save root tree error\n");
        return BVERROR(EIO);
    }
    return 0;
}

static int jansson_file_dump(BVConfigFileContext *s, const char *filename)
{
    JsonConfigFileContext *json = s->priv_data;
    if (!json->root)
        return BVERROR(EINVAL);
    if (!filename)
        filename = s->filename;
    if (json_dump_file(json->root, filename, JSON_INDENT(4) | JSON_PRESERVE_ORDER) < 0) {
        bv_log(s, BV_LOG_ERROR, "dump json file %s error\n", filename);
    }
    return 0;
}

static int jansson_file_close(BVConfigFileContext *s)
{
    int ret = 0;
    if (jansson_file_dump(s, NULL) < 0) {
        bv_log(s, BV_LOG_ERROR, "close file error\n");
        ret = BVERROR(EIO);
    }
//    json_decref(json->root);
    return ret;
}

static int jansson_decref(BVConfigFileContext *s, BVConfigObject *obj)
{
    json_t *t = obj->priv_data;
    if (!t)
        return 0;
    json_decref(t);
    obj->priv_data = NULL;
    return 0;
}

static BVConfigObject *jansson_lookup_from(BVConfigFileContext *s, BVConfigObject *obj, const char *path)
{
    return NULL;
}

static BVConfigObject *jansson_lookup(BVConfigFileContext *s, const char *path)
{
    JsonConfigFileContext *json = s->priv_data;
    return NULL;
}

static int jansson_get_value(BVConfigFileContext *s, BVConfigObject *obj, void *value)
{
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(JsonConfigFileContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass jansson_class = {
    .class_name     = "jsnsson config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfigFile bv_json_cfile = {
    .name           = "json",
    .extensions     = "json",
    .type           = BV_CONFIG_FILE_TYPE_JSON,
    .priv_data_size = sizeof(JsonConfigFileContext),
    .priv_class     = &jansson_class,
    .file_open      = jansson_file_open,
    .file_close     = jansson_file_close,
    .file_dump      = jansson_file_dump,
    .decref         = jansson_decref,
//    .lookup         = jansson_lookup,
    .lookup_from    = jansson_lookup_from,
    .get_value      = jansson_get_value,
};
