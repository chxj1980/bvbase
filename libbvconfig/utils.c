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

#include <pthread.h>
#include "bvconfig.h"

static pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER;
static BVConfig *first_cfg = NULL;
static BVConfig **last_cfg = &first_cfg;

static const char FILE_NAME[] = "utils.c";

static void *bvpriv_atomic_ptr_cas(void *volatile *ptr, void *oldval, void *newval)
{
	void *ret;
	pthread_mutex_lock(&atomic_lock);
	ret = *ptr;
	if (*ptr == oldval)
		*ptr = newval;
	pthread_mutex_unlock(&atomic_lock);
	return ret;
}

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

BVConfig *bv_config_find_config(enum BVConfigType config_type)
{
	BVConfig *cfg = NULL;
	if (first_cfg == NULL) {
		av_log(NULL, AV_LOG_ERROR, "BVConfig Not RegisterAll");
		return NULL;
	}

	while ((cfg = bv_config_next(cfg))) {
		if (cfg->config_type == config_type) {
			return cfg;
		}
	}
	return NULL;
}

BVConfig *bv_config_find_config_by_name(const char *cfg_name)
{
	BVConfig *cfg = NULL;
	if (first_cfg == NULL) {
		av_log(NULL, AV_LOG_ERROR, "BVConfig Not RegisterAll");
		return NULL;
	}

	while ((cfg = bv_config_next(cfg))) {
		if (strncmp(cfg->name, cfg_name, strlen(cfg->name)) == 0) {
			return cfg;
		}
	}
	return NULL;
}
