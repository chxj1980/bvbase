/*************************************************************************
    > File Name: exCfile.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月04日 星期三 15时33分42秒
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

#include <libbvutil/bvutil.h>
#include <libbvconfig/common.h>
#include <libbvconfig/bvconfig.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>

int main(int argc, const char *argv[])
{
    BVConfigFileContext *fc = NULL;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *type = NULL;
    BVConfigObject *framerate = NULL;
    bv_log_set_level(BV_LOG_DEBUG);
    char value[128] = { 0 };
    int64_t lvalue;
    bv_config_register_all();
    if (bv_config_file_open(&fc, argv[1], NULL, NULL) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open config error\n");
        return BVERROR(EINVAL);
    }
#if 1
    obj = bv_config_file_lookup_from(fc, fc->root, "imaging_setting.[0].type");
    if (!obj) {
        bv_log(fc, BV_LOG_ERROR, "get member error\n");
    } else {
        bv_config_object_get_value(fc, obj, value);
        bv_log(fc, BV_LOG_WARNING, "member type %d value %s\n", obj->type, value);
        bv_config_object_decref(fc, obj);
    }
#else 
    obj = bv_config_get_member(fc, fc->root, "imaging_setting");
    if (!obj) {
        bv_log(fc, BV_LOG_ERROR, "get member error\n");
    } else {
        bv_log(fc, BV_LOG_WARNING, "member type %d\n", obj->type);
        bv_config_object_decref(fc, obj);
    }

    elem = bv_config_get_element(fc, obj, 0);
    type = bv_config_get_member(fc, elem, "type");
    bv_config_object_get_value(fc, type, value);
    bv_log(fc, BV_LOG_WARNING, "member type %d value %s\n", type->type, value);
    bv_config_object_decref(fc, type);
    bv_config_object_decref(fc, elem);
    bv_config_object_decref(fc, obj);
#endif
    framerate = bv_config_file_lookup_from(fc, fc->root, "video_encoders.[0].[0].framerate");
    if (framerate) {
        bv_config_object_get_value(fc, framerate, &lvalue);
        bv_log(fc, BV_LOG_WARNING, "member type %d value %lld\n", framerate->type, lvalue);
        lvalue = (lvalue + 15) % 30;
        bv_config_object_set_value(fc, framerate, &lvalue);
        bv_config_object_decref(fc, framerate);
    } else {
        bv_log(fc, BV_LOG_ERROR, "get member error\n");
    }
    bv_config_file_close(&fc);
    return 0;
}
