/*************************************************************************
    > File Name: bvconfig.h
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
 *    @file
 *    @ingroup libconfig
 *    libconfig external API header
 */

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/dict.h>

#include "common.h"

//#include "default/common.h"

enum BVConfigFileType {
    BV_CONFIG_FILE_TYPE_NONE = -1,
    BV_CONFIG_FILE_TYPE_JSON,    //json file type now only support this type;
    BV_CONFIG_FILE_TYPE_XML,
    BV_CONFIG_FILE_TYPE_PRIV,

    BV_CONFIG_FILE_TYPE_UNKNOWN
};

#if 0
enum BVConfigType {
    BV_CONFIG_TYPE_NONE = -1,
    BV_CONFIG_TYPE_LOCAL,
    BV_CONFIG_TYPE_ONVIF,
    BV_CONFIG_TYPE_UNKNOWN
};
#endif
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
    int (*read_string)(BVConfigFileContext *s, const char *key, char *value);
    int (*read_int)(BVConfigFileContext *s, const char *key, int *value);
    int (*read_double)(BVConfigSection *s, const char *key, double *value);
    int (*write_string)(BVConfigFileContext *s, const char *key, char *value);
    int (*write_int)(BVConfigFileContext *s, const char *key, int value);
    int (*write_double)(BVConfigFileContext *s, const char *key, double *value);
} BVConfigFile;

typedef struct _BVConfigContext {
    const BVClass *bv_class;
    struct _BVConfig *config;
    void *priv_data;
    BVConfigFileContext *pdb;
    char url[1024];
} BVConfigContext;

#define BV_CONFIG_FLAGS_NOFILE      0x0001

#define BV_CONFIG_FLAGS_NETWORK     0x1000

typedef struct _BVConfig {
    const char *name;
    enum BVConfigType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVConfig *next;
    int flags;
//////////////////////////////////////////////////////////////////////
    int (*config_probe)(BVConfigContext *s, BVProbeData *p);
    int (*config_open)(BVConfigContext *s);
    int (*config_close)(BVConfigContext *s);
    int (*get_device_info)(BVConfigContext *s, BVDeviceInfo *devinfo);
    int (*get_profiles)(BVConfigContext *s, BVMediaProfile *profiles, int *max_num);
    int (*get_video_encoder)(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);
    int (*set_video_encoder)(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);

    int (*get_video_encoder_options)(BVConfigContext *s, int channel, int index, BVVideoEncoderOption *config);

    int (*get_audio_encoder)(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);
    int (*set_audio_encoder)(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);
    int (*get_audio_encoder_options)(BVConfigContext *s, int channel, int index, BVAudioEncoderOption *config);
    int (*get_ptz_device)(BVConfigContext *s, int channel, int index, BVPTZDevice *config);
    int (*save_ptz_preset)(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);
    int (*dele_ptz_preset)(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);
} BVConfig;

void bv_config_register_all(void);
int bv_config_register(BVConfig * cfg);
BVConfig *bv_config_next(BVConfig * cfg);
BVConfig *bv_config_find(enum BVConfigType type);
BVConfig *bv_config_find_by_name(const char *cfg_name);
BVConfigContext *bv_config_context_alloc(void);
void bv_config_context_free(BVConfigContext * s);

int bv_config_open(BVConfigContext **h, const char *url, BVConfig *config, BVDictionary **options);

int bv_config_close(BVConfigContext **h);

int bv_config_file_register(BVConfigFile * config_file);
BVConfigFile *bv_config_file_next(BVConfigFile * config_file);
BVConfigFile *bv_config_file_find(enum BVConfigFileType config_file_type);
BVConfigFile *bv_config_file_find_by_name(const char *file_config_name);
BVConfigFileContext *bv_config_file_context_alloc(void);
void bv_config_file_context_free(BVConfigFileContext * s);
//////////////////////////////////////////////////////////////////////

int bv_config_get_device_info(BVConfigContext *s, BVDeviceInfo *devinfo);

int bv_config_get_media_profiles(BVConfigContext *s, BVMediaProfile *profiles, int *max_num);

int bv_config_get_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);

int bv_config_set_video_encoder(BVConfigContext *s, int channel, int index, BVVideoEncoder *config);

int bv_config_get_video_encoder_options(BVConfigContext *s, int channel, int index, BVVideoEncoderOption *config);

int bv_config_get_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);

int bv_config_set_audio_encoder(BVConfigContext *s, int channel, int index, BVAudioEncoder *config);

int bv_config_get_audio_encoder_options(BVConfigContext *s, int channel, int index, BVAudioEncoderOption *config);

int bv_config_get_ptz_device(BVConfigContext *s, int channel, int index, BVPTZDevice *config);

int bv_config_save_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);

int bv_config_dele_ptz_preset(BVConfigContext *s, int channel, int index, BVPTZPreset *preset);
#if 0
//VIDev && VODev
int bv_config_get_video_source_device(BVConfigContext *s, int index, BVVideoSourceDevice *video_source_device);
int bv_config_set_video_source_device(BVConfigContext *s, int index, BVVideoSourceDevice *video_source_device);
int bv_config_get_video_output_device(BVConfigContext *s, int index, BVVideoOutputDevice *video_output_device);
int bv_config_set_video_output_device(BVConfigContext *s, int index, BVVideoOutputDevice *video_output_device);

//VIChn && VOChn
int bv_config_get_video_source(BVConfigContext *s, int index);
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

