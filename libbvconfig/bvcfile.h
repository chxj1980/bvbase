/*************************************************************************
    > File Name: bvcfile.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年02月04日 星期三 10时52分20秒
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

#ifndef BV_CFILE_H
#define BV_CFILE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/log.h>
#include <libbvutil/dict.h>

enum BVConfigFileType {
    BV_CONFIG_FILE_TYPE_NONE = -1,
    BV_CONFIG_FILE_TYPE_JSON,    //json file type now only support this type;
    BV_CONFIG_FILE_TYPE_XML,
    BV_CONFIG_FILE_TYPE_PRIV,

    BV_CONFIG_FILE_TYPE_UNKNOWN
};

typedef enum BVConfigObjectType {
    BV_CONFIG_OBJTYPE_NONE,
    BV_CONFIG_OBJTYPE_GROUP,    /*{ }*/
    BV_CONFIG_OBJTYPE_ARRAY,    /*[ ] root.sss[1].name */
    BV_CONFIG_OBJTYPE_STRING,
    BV_CONFIG_OBJTYPE_INTEGER,
    BV_CONFIG_OBJTYPE_FLOAT,
    BV_CONFIG_OBJTYPE_BOOL,
    BV_CONFIG_OBJTYPE_NULL,
    BV_CONFIG_OBJTYPE_UNKNOWN
} BVConfigObjectType;

typedef struct _BVConfigObject {
    BVConfigObjectType type;
    int flags;
    char *name;
    struct _BVConfigObject *parent;
#if 0
    union {
        int    ival;
        double fval;
        char * sval;
        struct _BVConfigArray *aval;
    } value;
//    void *value;
#endif
    void *priv_data;
} BVConfigObject;

typedef struct _BVConfigArray {
    int nb_elements;
    BVConfigObject **elements;
} BVConfigArray;

typedef struct _BVConfigFileContext {
    const BVClass *bv_class;
    struct _BVConfigFile *cfile;
    BVConfigObject *root;
    void *priv_data;
    char filename[1024];
} BVConfigFileContext;

typedef struct _BVConfigFile {
    const char *name;
    const char *extensions;
    enum BVConfigFileType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVConfigFile *next;
    int (*file_open)(BVConfigFileContext *s);
    int (*file_close)(BVConfigFileContext *s);
    int (*file_dump)(BVConfigFileContext *s, const char *filename);
    int (*decref)(BVConfigFileContext *s, BVConfigObject *obj);
    BVConfigObject *(*get_element)(BVConfigFileContext *s, BVConfigObject *parent, int index);
    BVConfigObject *(*get_member)(BVConfigFileContext *s, BVConfigObject *parent, const char *key);
    int (*get_value)(BVConfigFileContext *s, BVConfigObject *obj, void *value);
    int (*set_value)(BVConfigFileContext *s, BVConfigObject *obj, void *value);
    int (*remove)(BVConfigFileContext *s, BVConfigObject *parent, const char *key);
    BVConfigObject *(*add)(BVConfigFileContext *s, BVConfigObject *parent, const char *key, BVConfigObjectType type);
} BVConfigFile;

int bv_config_file_register(BVConfigFile * config_file);

BVConfigFile *bv_config_file_next(BVConfigFile * config_file);

BVConfigFile *bv_config_file_find(enum BVConfigFileType config_file_type);

BVConfigFile *bv_config_file_find_by_name(const char *file_config_name);

BVConfigFileContext *bv_config_file_context_alloc(void);

void bv_config_file_context_free(BVConfigFileContext * s);

int bv_config_file_open(BVConfigFileContext **s, const char *url, BVConfigFile *config, BVDictionary **options);

int bv_config_file_dump(BVConfigFileContext *s, const char *filename);

int bv_config_file_close(BVConfigFileContext **s);

BVConfigObject *bv_config_file_lookup(BVConfigFileContext *s, const char *path);

BVConfigObject *bv_config_file_lookup_from(BVConfigFileContext *s, BVConfigObject *obj, const char *path);

BVConfigObject *bv_config_get_element(BVConfigFileContext *s, BVConfigObject *parent, int index);

BVConfigObject *bv_config_get_member(BVConfigFileContext *s, BVConfigObject *parent, const char *key);

int bv_config_object_decref(BVConfigFileContext *s, BVConfigObject *obj);

int bv_config_object_get_value(BVConfigFileContext *s, BVConfigObject *obj, void *value);

int bv_config_object_set_value(BVConfigFileContext *s, BVConfigObject *obj, void *value);

int bv_config_object_remove(BVConfigFileContext *s, BVConfigObject *parent, const char *key);

BVConfigObject *bv_config_object_add(BVConfigFileContext *s, BVConfigObject *parent, const char *key, BVConfigObjectType type);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_CFILE_H */

