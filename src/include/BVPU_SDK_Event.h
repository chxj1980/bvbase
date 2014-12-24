/**
 *          File: BVPU_SDK_Event.h
 *
 *        Create: 2014年12月12日 星期五 16时07分44秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
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
 * Copyright (C) @ BesoVideo, 2014
 */

#ifndef __BVPU_SDK_EVENT_H
#define __BVPU_SDK_EVENT_H

#include "BVPU_SDK_Const.h"

/*
 * 基本事件及报警状态上报
 */
typedef enum {
    BVPU_SDK_EVENT_TYPE_NONE = 0,        //无效值
    
    //通用
    BVPU_SDK_EVENT_TYPE_DISKERROR = 0x0001,       //磁盘错误。源：PU/NRU的Storage
    
    //PU相关
    BVPU_SDK_EVENT_TYPE_VIDEOLOST = 0x1000,    //视频丢失。源：PU 的 VideoIn
    BVPU_SDK_EVENT_TYPE_VIDEOMD,               //运动检测。源：PU 的 VideoIn
    BVPU_SDK_EVENT_TYPE_VIDEOOCCLUSION,        //视频遮挡。源：PU 的 VideoIn
    BVPU_SDK_EVENT_TYPE_ALERTIN,               //报警输入。源：PU 的 AlertIn
    BVPU_SDK_EVENT_TYPE_PERIOD,                //周期报警。源：PU 的 周期报警    
    BVPU_SDK_EVENT_TYPE_PUONLINE,              //PU上线。源：PU
    BVPU_SDK_EVENT_TYPE_PUOFFLINE,             //PU下线。源：PU
    BVPU_SDK_EVENT_TYPE_LOWTEMPERATURE,        //低温报警。源：PU 的 TemperatureIn
    BVPU_SDK_EVENT_TYPE_HIGHTEMPERATURE,       //高温报警。源：PU 的 TemperatureIn
    BVPU_SDK_EVENT_TYPE_SLOWSPEED,             //低速报警。源：PU 的 GPS
    BVPU_SDK_EVENT_TYPE_OVERSPEED,             //超速报警。源：PU 的 GPS
    BVPU_SDK_EVENT_TYPE_INTOREGION,            //进入区域报警。源：PU 的 GPS
    BVPU_SDK_EVENT_TYPE_OUTREGION,             //出区域报警。源：PU 的 GPS
    BVPU_SDK_EVENT_TYPE_LOWVOLTAGE,            //低电压报警。源：PU 的 VoltageIn
    BVPU_SDK_EVENT_TYPE_HIGHVOLTAGE,           //高电压报警。源：PU 的 VoltageIn
    
//// unuse for bvpu
    //NRU相关
    BVPU_SDK_EVENT_TYPE_NRUONLINE = 0x2000,   //NRU上线
    BVPU_SDK_EVENT_TYPE_NRUOFFLINE,           //NRU下线
    
    //VTDU相关
    BVPU_SDK_EVENT_TYPE_VTDUONLINE = 0x3000,
    BVPU_SDK_EVENT_TYPE_VTDUOFFLINE,
    
    //CMS相关
    BVPU_SDK_EVENT_TYPE_CMSONLINE = 0x4000,
    BVPU_SDK_EVENT_TYPE_CMSOFFLINE,
    
    //用户(User)相关
    BVPU_SDK_EVENT_TYPE_USERLOGIN = 0x5000,   //用户登录
    BVPU_SDK_EVENT_TYPE_USERLOGOUT,           //用户注销
//// end unuse for bvpu
        
    //该值及往后的值为自定义类型
    BVPU_SDK_EVENT_TYPE_CUSTOM = 0x10000000,

    // 数据流获取过程中报警信息
    BVPU_SDK_EVENT_MEDIA_START,
	
}BVPU_SDK_Event;

#endif // __BVPU_SDK_EVENT_H

/*=============== End of file: BVPU_SDK_Event.h =====================*/
