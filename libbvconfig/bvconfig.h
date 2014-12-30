/*************************************************************************
	> File Name: config.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年10月23日 星期四 11时48分59秒
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

#ifndef BV_CONFIG_H
#define BV_CONFIG_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 *	@file
 *	@ingroup libconfig
 *	libconfig external API header
 */

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/dict.h>

//#include "default/common.h"

enum BVConfigFileType {
	BV_CONFIG_FILE_TYPE_NONE = -1,
	BV_CONFIG_FILE_TYPE_JSON,	//json file type now only support this type;
	BV_CONFIG_FILE_TYPE_XML,
	BV_CONFIG_FILE_TYPE_PRIV,

	BV_CONFIG_FILE_TYPE_UNKNOWN
};

enum BVConfigType {
	BV_CONFIG_TYPE_NONE = -1,
	BV_CONFIG_TYPE_LOCAL,
	BV_CONFIG_TYPE_ONVIF,
	BV_CONFIG_TYPE_UNKNOWN
};


typedef struct _BVConfigContext {
    const BVClass *bv_class;
	struct _BVConfig *config;
	void *priv_data;
	char url[1024];
} BVConfigContext;

typedef struct _BVConfig {
	const char *name;
	enum BVConfigType config_type;
	const BVClass *priv_class;
	int priv_data_size;
	struct _BVConfig *next;
//////////////////////////////////////////////////////////////////////
	int (*open)(BVConfigContext *cfgctx, const char *url, int flags);
	int (*close)(BVConfigContext *cfgctx);
//	int (*get_device_info)(BVConfigContext *cfgctx, BVDeviceInfo *devinfo);
} BVConfig;


void bv_config_register_all(void);
int bv_config_register(BVConfig * cfg);
BVConfig *bv_config_next(BVConfig * cfg);
BVConfig *bv_config_find_config(enum BVConfigType config_type);
BVConfig *bv_config_find_config_by_name(const char *cfg_name);
BVConfigContext *bv_config_alloc_context(void);
void bv_config_free_context(BVConfigContext * cfgctx);

//////////////////////////////////////////////////////////////////////
#if 0
//VIDev && VODev
int bv_config_get_video_source_device(BVConfigContext *ctxt, int index, BVVideoSourceDevice *video_source_device);
int bv_config_set_video_source_device(BVConfigContext *ctxt, int index, BVVideoSourceDevice *video_source_device);
int bv_config_get_video_output_device(BVConfigContext *ctxt, int index, BVVideoOutputDevice *video_output_device);
int bv_config_set_video_output_device(BVConfigContext *ctxt, int index, BVVideoOutputDevice *video_output_device);

//VIChn && VOChn
int bv_config_get_video_source(BVConfigContext *ctxt, int index);
int bv_config_set_video_source();
int bv_config_get_video_output();
int bv_config_set_video_output();

//VEChn && VDChn
int bv_config_get_video_encoder();
int bv_config_set_video_encoder();
int bv_config_get_video_decoder();
int bv_config_set_video_decoder();


int bv_config_get_audio_source_device();
int bv_config_set_audio_source_device();
int bv_config_get_audio_output_device();
int bv_config_set_audio_output_device();

int bv_config_get_audio_source();
int bv_config_set_audio_source();
int bv_config_get_audio_output();
int bv_config_set_audio_output();

int bv_config_get_audio_encoder();
int bv_config_set_audio_encoder();
int bv_config_get_audio_decoder();
int bv_config_set_audio_decoder();


int bv_config_get_osd();
int bv_config_set_osd();
#endif
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_CONFIG_H */

