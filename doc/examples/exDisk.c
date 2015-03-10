/* ==================================================================
 *       Filename:  disk.c
 *    Description:  
 *        Created:  2015年03月09日 15时11分35秒
 *       Compiler:  gcc/g++
 *         Author:  Silence (Liu Wei), 384439695@qq.com
 *        Company:  合肥优视科技有限公司
 * ==================================================================*/
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
#include <libbvdevice/bvdevice.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/log.h>
#include <libbvconfig/common.h>
#include <unistd.h>
#include <bvfs.h>
#include "time.h"

int main(int argc, const char *argv[])
{
    int ret;
    BVDevice *device = NULL;
    BVDeviceContext *device_context = NULL;
    BVDictionary *opn = NULL;

    bv_device_register_all();
    
    device = bv_device_find_device(BV_DEVICE_TYPE_HRDSK);
    
    if (!device) {
        bv_log(NULL, BV_LOG_ERROR, "Not Find This device\n");
        return BVERROR(EINVAL);
    }
    
    if ((ret = bv_device_open(&device_context, device, NULL, NULL))) {
        bv_log(NULL, BV_LOG_ERROR, "open device error %d\n", ret);
        bv_dict_free(&opn);
        return BVERROR(EIO);
    }
    
    BVControlPacket pkt_in;
    BVControlPacket pkt_out;
    
    BVSearchFileConditions cond;
    cond.start_time = ((time(NULL) - (long)3600*24*35));
    cond.end_time   = (time(NULL)) ;
    cond.channel_id = 2;
    //文件类型的存储类型见 libbvfs 头文件中的bvfs_type.h
    cond.file_type  = 255;
    cond.storage_type =2;

    pkt_in.data =&cond;
    bv_device_control(device_context, BV_DEV_MESSAGE_TYPE_SEARCH_FILE, &pkt_in, &pkt_out);
    
    int i;
    BVFileInfo *info = pkt_out.data;
    for(i=0;i < pkt_out.size;i++)
    {
        bv_log(device_context, BV_LOG_INFO, "file_name:%s\n",info->name);
        bv_log(device_context, BV_LOG_INFO, "start_time:%ld\n",info->start_time);
        bv_log(device_context, BV_LOG_INFO, "end_time:%ld\n",info->end_time);
        bv_log(device_context, BV_LOG_INFO, "channel_id:%d\n",info->channel_id);
        bv_log(device_context, BV_LOG_INFO, "file_type:%d\n",info->file_type);
        bv_log(device_context, BV_LOG_INFO, "disk_id:%d\n",info->disk_id);
        bv_log(device_context, BV_LOG_INFO, "storage_type:%d\n",info->storage_type);
        bv_log(device_context, BV_LOG_INFO, "file_size:%ld\n",info->file_size);
    }
    bv_free(pkt_out.data);//记得释放内存哦^_^

    BVDiskDevice disk;
	disk.index =0;
	disk.type=0;
    pkt_in.data = (void *)&disk;

    //when test format open it
    //	bv_device_control(device_context,BV_DEV_MESSAGE_TYPE_FORMAT_DISK,&pkt_in, NULL);
    
    bv_device_close(&device_context);
    return 0;
}

