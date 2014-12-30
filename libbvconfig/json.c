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

#include "bvconfig.h"
#include "jansson.h"

struct JsonConfigContext {
	const BVClass *bv_class;
	json_t *root;
	json_t *video_dev;
	int fd;
};

static int open_url(BVConfigContext * cfgctx, const char *url, int flags)
{
	struct JsonConfigContext *json_context = cfgctx->priv_data;
	json_error_t error;
	if ((json_context->root = json_load_file(url, 0, &error)) == NULL) {
		bv_log(cfgctx, BV_LOG_ERROR, "load json file %s error at line %d column %d\n", url,
			error.line, error.column);
		return -1;
	}
	return 0;
}

static int close_url(BVConfigContext * cfgctx)
{
	struct JsonConfigContext *json_context = cfgctx->priv_data;
	if (json_context->root) {
		if (json_dump_file(json_context->root, cfgctx->url,
				JSON_INDENT(4) | JSON_PRESERVE_ORDER) < 0) {
			bv_log(cfgctx, BV_LOG_ERROR, "dump json file %s error\n", cfgctx->url);
		}
		json_decref(json_context->root);
	}

	return 0;
}

#if 0
static int get_system_info(BVConfigContext * cfgctx, BVSystemInfo * sys_info)
{
	struct JsonConfigContext *json_context = cfgctx->priv_data;
	if (json_context->root == NULL) {
		return -1;
	}
}
#endif

BVConfig bv_json_config = {
	.name = "json",
    .priv_data_size = sizeof(struct JsonConfigContext),
    .open = open_url,
	.close = close_url,
//	.get_system_info = get_system_info,
};
