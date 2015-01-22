/*************************************************************************
    > File Name: utils.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月29日 星期三 11时00分19秒
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

#include <libbvutil/atomic.h>
#include "bvconfig.h"

static BVConfig *first_cfg = NULL;
static BVConfig **last_cfg = &first_cfg;

static const char FILE_NAME[] = "utils.c";

int bv_config_register(BVConfig * cfg)
{
    BVConfig **p = last_cfg;
    cfg->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, cfg))
        p = &(*p)->next;
    last_cfg = &cfg->next;
    return 0;
}

BVConfig *bv_config_next(BVConfig * cfg)
{
    if (cfg)
        return cfg->next;
    else
        return first_cfg;
}

BVConfig *bv_config_find(enum BVConfigType type)
{
    BVConfig *cfg = NULL;
    if (first_cfg == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVConfig Not RegisterAll");
        return NULL;
    }

    while ((cfg = bv_config_next(cfg))) {
        if (cfg->type == type) {
            return cfg;
        }
    }
    return NULL;
}

BVConfig *bv_config_find_by_name(const char *cfg_name)
{
    BVConfig *cfg = NULL;
    if (first_cfg == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVConfig Not RegisterAll");
        return NULL;
    }

    while ((cfg = bv_config_next(cfg))) {
        if (strncmp(cfg->name, cfg_name, strlen(cfg->name)) == 0) {
            return cfg;
        }
    }
    return NULL;
}

static BVConfigFile *first_file_config = NULL;
static BVConfigFile **last_file_config = &first_file_config;

int bv_config_file_register(BVConfigFile * config_file)
{
    BVConfigFile **p = last_file_config;
    config_file->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, config_file))
        p = &(*p)->next;
    last_file_config = &config_file->next;
    return 0;
}

BVConfigFile *bv_config_file_next(BVConfigFile * config_file)
{
    if (config_file)
        return config_file->next;
    else
        return first_file_config;
}

BVConfigFile *bv_config_file_find(enum BVConfigFileType config_file_type)
{
    BVConfigFile *config_file = NULL;
    if (first_file_config == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVConfigFile Not RegisterAll");
        return NULL;
    }

    while ((config_file = bv_config_file_next(config_file))) {
        if (config_file->type == config_file_type) {
            return config_file;
        }
    }
    return NULL;
}

BVConfigFile *bv_config_file_find_by_name(const char *file_config_name)
{
    BVConfigFile *config_file = NULL;
    if (first_file_config == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVConfigFile Not RegisterAll");
        return NULL;
    }

    while ((config_file = bv_config_file_next(config_file))) {
        if (strncmp(config_file->name, file_config_name, strlen(config_file->name)) == 0) {
            return config_file;
        }
    }
    return NULL;
}
