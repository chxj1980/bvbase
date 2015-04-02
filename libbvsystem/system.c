/*************************************************************************
    > File Name: system.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月10日 星期二 14时24分44秒
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

#line 25 "system.c"

#include "bvsystem.h"

static int init_system(BVSystemContext *s)
{
    /**
     *  Only One System Support
     *  FIXME
     */
    if (BV_CONFIG_HIS3515) {
        s->system = bv_system_find_system(BV_SYSTEM_TYPE_HIS3515);
    } else {
        s->system = bv_system_find_system(BV_SYSTEM_TYPE_X86);
    }

    if (!s->system)
        return BVERROR(ENOSYS);
    return 0;
}

int bv_system_init(BVSystemContext **h, BVSystem *sys, BVDictionary **options)
{
    int ret = 0;
    BVDictionary *tmp = NULL;
    BVSystemContext *s = *h;
    if (!s && !(s = bv_system_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", __FILE__, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);
    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;
    if (sys)
        s->system = sys;
    else
        ret = init_system(s);
    if (ret < 0) {
        ret = BVERROR(ENOSYS);
        goto fail;
    }
    if (s->system->priv_data_size > 0) {
        s->priv_data = bv_mallocz(s->system->priv_data_size);
        if (!s->priv_data)
            goto fail;
        if (s->system->priv_class) {
            *(const BVClass **) s->priv_data = s->system->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                bv_log(s, BV_LOG_ERROR, "set dict error\n");
                goto fail;
            }
        }
    }
    if (!s->system->sys_init || s->system->sys_init(s) < 0) {
        ret = BVERROR(ENOSYS);
        goto fail;
    }
    *h = s;
    bv_dict_free(&tmp);
    return ret;
fail:
    bv_dict_free(&tmp);
    bv_system_context_free(s);
    *h = NULL;
    return ret;
}

static int system_do_reboot(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int system_do_system_upgrade(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int system_do_config_upgrade(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

int bv_system_control(BVSystemContext *h, enum BVSystemMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    if (!h->system->sys_control) {
        return BVERROR(ENOSYS);
    }

    switch (type) {
        case BV_SYS_MESSAGE_TYPE_REBOOT:
        {
            ret = system_do_reboot(h, pkt_in, pkt_out);
            break;    
        }
        case BV_SYS_MESSAGE_TYPE_SYSUGD:
        {
            ret = system_do_system_upgrade(h, pkt_in, pkt_out);
            break;
        }
        case BV_SYS_MESSAGE_TYPE_CFGUGD:
        {
            ret = system_do_config_upgrade(h, pkt_in, pkt_out);
            break;
        }
        default:
        {
            ret = h->system->sys_control(h, type, pkt_in, pkt_out);
            break;
        }
    }

    return ret;
}

int bv_system_exit(BVSystemContext **h)
{
    BVSystemContext *s = *h;
    if (s->system && s->system->sys_exit)
        s->system->sys_exit(s);
    bv_system_context_free(s);
    *h = NULL;
    return 0;
}
