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
#include <libbvutil/bvstring.h>

#include "bvcfile.h"
#include "jansson.h"

typedef struct _JsonConfigFileContext {
    const BVClass *bv_class;
    json_t *root;
    int fd;
}JsonConfigFileContext;

static BVConfigObjectType json_type_objtype(json_type type)
{
    int i;
    struct {
        json_type type;
        BVConfigObjectType bvtype;
    } types[] = {
        { JSON_OBJECT, BV_CONFIG_OBJTYPE_GROUP },
        { JSON_ARRAY,  BV_CONFIG_OBJTYPE_ARRAY },
        { JSON_STRING, BV_CONFIG_OBJTYPE_STRING},
        { JSON_INTEGER, BV_CONFIG_OBJTYPE_INTEGER},
        { JSON_REAL, BV_CONFIG_OBJTYPE_FLOAT},
        { JSON_TRUE, BV_CONFIG_OBJTYPE_BOOL},
        { JSON_FALSE, BV_CONFIG_OBJTYPE_BOOL},
        { JSON_NULL, BV_CONFIG_OBJTYPE_NULL},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(types); i++) {
        if (types[i].type == type)
            return types[i].bvtype;
    }
    return BV_CONFIG_FILE_TYPE_NONE;
}

static int save_root_tree(BVConfigFileContext *s)
{
    JsonConfigFileContext *json = s->priv_data;
    s->root = bv_mallocz(sizeof(BVConfigObject));
    if (!s->root) {
        json_decref(json->root);
        json->root = NULL;
        return BVERROR(ENOMEM);
    }
    s->root->parent = NULL;
    s->root->type = json_type_objtype(json_typeof(json->root));
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
    if (json_dump_file(json->root, filename, JSON_INDENT(1) | JSON_PRESERVE_ORDER) < 0) {
        bv_log(s, BV_LOG_ERROR, "dump json file %s error\n", filename);
    }
    return 0;
}

static int jansson_file_close(BVConfigFileContext *s)
{
    JsonConfigFileContext *json = s->priv_data;
    int ret = 0;
    if (jansson_file_dump(s, NULL) < 0) {
        bv_log(s, BV_LOG_ERROR, "close file error\n");
        ret = BVERROR(EIO);
    }
    if (json->root)
        json_decref(json->root);
    return ret;
}

static int jansson_decref(BVConfigFileContext *s, BVConfigObject *obj)
{
#if 0
    json_t *t = obj->priv_data;
    if (!t)
        return 0;
    json_decref(t);
    obj->priv_data = NULL;
#endif
    return 0;
}

static BVConfigObject *jansson_get_element(BVConfigFileContext *s, BVConfigObject *parent, int index)
{
    BVConfigObject *obj = NULL;
    json_t *array = parent->priv_data;
    json_t *elem = NULL;
    if (!array)
        return NULL;
    elem = json_array_get(array, index);
    if (!elem)
        return NULL;
    obj = bv_mallocz(sizeof(BVConfigObject));
    if (!obj) {
        return NULL;
    }
    obj->type = json_type_objtype(json_typeof(elem));
    obj->priv_data = elem;
    obj->parent = parent;
    obj->name = bv_mallocz(256);
    if (obj->name) {
        if (parent->name) {
            bv_strlcpy(obj->name, parent->name, 256);
        }
        bv_sprintf(obj->name + strlen(obj->name), 256 - strlen(obj->name), ".[%d]", index);
    }
    return obj;
}

static BVConfigObject *jansson_get_member(BVConfigFileContext *s, BVConfigObject *parent, const char *key)
{
    BVConfigObject *obj = NULL;
    json_t *group = parent->priv_data;
    json_t *member = NULL;
    if (!group)
        return NULL;
    member = json_object_get(group, key);
    if (!member)
        return NULL;
    obj = bv_mallocz(sizeof(BVConfigObject));
    if (!obj)
        return NULL;
    obj->type = json_type_objtype(json_typeof(member));
    obj->priv_data = member;
    obj->parent = parent;
    obj->name = bv_strdup(key);
    return obj;
}

static int jansson_get_elements(BVConfigFileContext *s, BVConfigObject *parent)
{
    json_t *array = parent->priv_data;
    if (!array)
        return -1;
    return json_array_size(array);
}

/**
 *  FIXME 
 *  Be Careful with get string value 
 *  caller should have enough room for the result
 */
static int jansson_get_value(BVConfigFileContext *s, BVConfigObject *obj, void *value)
{
    json_t *t = obj->priv_data;
    switch (obj->type) {
        case BV_CONFIG_OBJTYPE_STRING:
            strcpy(value, json_string_value(t));    //FIXME
            break;
        case BV_CONFIG_OBJTYPE_FLOAT:
            *(double *)value = json_real_value(t);
            break;
        case BV_CONFIG_OBJTYPE_BOOL:
        case BV_CONFIG_OBJTYPE_INTEGER:
            *(int64_t *)value = json_integer_value(t);
            break;
        default:
            return -1;
    }
    return 0;
}

static int jansson_set_value(BVConfigFileContext *s, BVConfigObject *obj, void *value)
{
    json_t *t = obj->priv_data;
    int ret = 0;
    switch (obj->type) {
        case BV_CONFIG_OBJTYPE_STRING:
            ret = json_string_set(t, (const char *)value);
            break;
        case BV_CONFIG_OBJTYPE_FLOAT:
            ret = json_real_set(t, *(double *)value);
            break;
        case BV_CONFIG_OBJTYPE_BOOL:
        case BV_CONFIG_OBJTYPE_INTEGER:
            ret = json_integer_set(t, *(json_int_t *)value);
            break;
        default:
            ret = -1;
    }
    return ret;
}

#define OFFSET(x) offsetof(JsonConfigFileContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass jansson_class = {
    .class_name         = "jsnsson config",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_CONFIG,
};

BVConfigFile bv_json_cfile = {
    .name               = "json",
    .extensions         = "json",
    .type               = BV_CONFIG_FILE_TYPE_JSON,
    .priv_data_size     = sizeof(JsonConfigFileContext),
    .priv_class         = &jansson_class,
    .file_open          = jansson_file_open,
    .file_close         = jansson_file_close,
    .file_dump          = jansson_file_dump,
    .decref             = jansson_decref,
    .get_element        = jansson_get_element,
    .get_member         = jansson_get_member,
    .get_elements       = jansson_get_elements,
    .get_value          = jansson_get_value,
    .set_value          = jansson_set_value,
};
