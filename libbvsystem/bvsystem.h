/*************************************************************************
    > File Name: bvsystem.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月14日 星期日 20时36分06秒
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

#ifndef BV_SYSTEM_H
#define BV_SYSTEM_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/dict.h>
#include <libbvutil/packet.h>

/**
 * FIXME
 *  不同平台所用字段不同， 字段冗余太大
 */
typedef struct _BVSystemVIDev {
    int index;      //index in BVSystemContext 
} BVSystemVIDev;

typedef struct _BVSystemVODev {
    int index;      //index in BVSystemContext
} BVSystemVODev;

enum BVSystemMessageType {
    BV_SYS_MESSAGE_TYPE_NONE = -1,
    BV_SYS_MESSAGE_TYPE_UNKNOW
};

typedef struct _BVSystemContext {
    const BVClass *bv_class;
    struct _BVSystem *system;
    BVSystemVIDev **videv;
    int nb_videv;
    void *priv_data;
} BVSystemContext;

enum BVSystemType {
    BV_SYSTEM_TYPE_NONE = 0,
    BV_SYSTEM_TYPE_HIS3515,
};

typedef struct _BVSystem {
    const char *name;
    enum BVSystemType type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVSystem *next;
    int (*sys_init)(BVSystemContext *sysctx);
    int (*sys_deinit)(BVSystemContext *sysctx);
    int (*sys_control)(BVSystemContext *sysctx, enum BVSystemMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);
    int (*sys_vienable)(BVSystemContext *sysctx, int index);
    int (*sys_vidisable)(BVSystemContext *sysctx, int index);
    int (*sys_voenable)(BVSystemContext *sysctx, int index);
    int (*sys_vodisable)(BVSystemContext *sysctx, int index);
} BVSystem;

void bv_system_register_all(void);

int bv_system_register(BVSystem * sys);

BVSystem *bv_system_next(BVSystem * sys);

BVSystem *bv_system_find_system(enum BVSystemType type);

BVSystemContext *bv_system_context_alloc(void);

void bv_system_context_free(BVSystemContext * sysctx);

int bv_system_new_videv(BVSystemContext *sysctx, BVSystemVIDev *vi);
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_SYSTEM_H */
