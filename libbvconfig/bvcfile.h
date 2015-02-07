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
    BV_CONFIG_OBJTYPE_OBJECT,
    BV_CONFIG_OBJTYPE_ARRAY,
    BV_CONFIG_OBJTYPE_STRING,
    BV_CONFIG_OBJTYPE_INTEGER,
    BV_CONFIG_OBJTYPE_FLOAT,
    BV_CONFIG_OBJTYPE_TRUE,
    BV_CONFIG_OBJTYPE_FALSE,
    BV_CONFIG_OBJTYPE_NULL
} BVConfigObjectType;

typedef enum {
    BV_SECTION_ID_NONE = -1,
    BV_SECTION_ID_ROOT,
} BVSectionID;

#define BV_CONFIG_SECTION_FLAGS_IS_WRAPPER              1 ///< the section only contains other sections, but has no data at its own level
#define BV_CONFIG_SECTION_FLAGS_IS_ARRAY                2 ///< the section contains an array of elements of the same type
#define BV_CONFIG_SECTION_FLAGS_HAS_VARIABLE_FIELDS     4 ///< the section may contain a variable number of fields with variable keys.
                                                          ///  For these sections the element_name field is mandatory.

typedef struct _BVConfigSection {
    int id;
    const char *name;
    int flags;
} BVConfigSection;

typedef struct _BVConfigFileContext {
    const BVClass *bv_class;
    struct _BVConfigFile *cfile;
    void *priv_data;
    char filename[1024];
    BVConfigSection *section;
} BVConfigFileContext;

typedef struct _BVConfigFile {
    const char *name;
    enum BVConfigFileType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVConfigFile *next;
    int (*init)(BVConfigFileContext *s);
    int (*uninit)(BVConfigFileContext *s);
    int (*goto_section)(BVConfigFileContext *s, BVConfigSection *section);
    int (*create_section)(BVConfigSection *s, BVConfigSection *section);
    int (*delete_section)(BVConfigSection *s, BVConfigSection *section);
    int (*read)(BVConfigFileContext *s, BVConfigObjectType type, const char *key, void *value);
    int (*write)(BVConfigFileContext *s, BVConfigObjectType type,  const char *key, void *value);
    int (*update)(BVConfigFileContext *s, BVConfigObjectType type, const char *key, void *nvalue);
    int (*delete)(BVConfigObjectType *s, BVConfigObjectType type, const char *key);
    int (*create)(BVConfigObjectType *s, BVConfigObjectType type, const char *after, const char *key, void *value);
} BVConfigFile;

int bv_config_file_register(BVConfigFile * config_file);

BVConfigFile *bv_config_file_next(BVConfigFile * config_file);

BVConfigFile *bv_config_file_find(enum BVConfigFileType config_file_type);

BVConfigFile *bv_config_file_find_by_name(const char *file_config_name);

BVConfigFileContext *bv_config_file_context_alloc(void);

void bv_config_file_context_free(BVConfigFileContext * s);

int bv_config_file_open(BVConfigFileContext **s, const char *url, BVConfigFile *config, BVDictionary **options);

int bv_config_file_close(BVConfigFileContext **s);

int bv_config_file_goto_section(BVConfigFileContext *s, BVConfigSection *section);

int bv_config_file_create_section(BVConfigSection *s, BVConfigSection *section);

int bv_config_file_delete_section(BVConfigSection *s, BVConfigSection *section);

int bv_config_file_read_string(BVConfigFileContext *s, const char *key, char *value);

int bv_config_file_read_int(BVConfigFileContext *s, const char *key, int *value);

int bv_config_file_read_double(BVConfigFileContext *s, const char *key, double *value);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_CFILE_H */

