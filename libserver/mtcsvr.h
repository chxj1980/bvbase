/*************************************************************************
	> File Name: mtcsvr.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年09月29日 星期一 17时51分34秒
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

#ifndef BV_MTC_SVR_H
#define BV_MTC_SVR_H

#ifdef __cplusplus
extern "C"{
#endif
/**
 *	MTC MSG ID
 */
#define TERMINAL_COMMON_RESP		(0X0001)
#define PLATFORM_COMMON_RESP		(0X8001)
#define TERMINAL_HEART_BEAT			(0X0002)
#define RESEND_SPLIT_REQST			(0X8003)
#define TERMINAL_REGISTER			(0X0100)
#define PLATFORM_REGISTER_RESP		(0X8100)
#define TERMINAL_AUTHENTICATION		(0X0102)
#define PLATFORM_SET_TERMINAL_PARM	(0X8103)
#define PLATFORM_QUERY_TERMINAL_PARM(0x8104)

typedef struct _BVMTCPURegister {
	uint16_t province_id;
	uint16_t city_id;
	uint32_t producter_id:5;
	uint32_t pu_type:20;
	uint32_t pu_id:7;
	uint8_t  car_color;
	uint8_t  car_num[15];
} BVMTCPURegister;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_MTC_SVR_H */


