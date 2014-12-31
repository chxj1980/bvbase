/*************************************************************************
	> File Name: server_cmd.h
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年09月29日 星期一 09时58分41秒
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

#ifndef BV_SERVER_CMD_H
#define BV_SERVER_CMD_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

/**
 *	命令分为发送和接收
 *	控制 查询 事件 应答几个大类
 */
enum BVServerCmd{
	//控制命令
	BV_SERVER_CONTROL_UNKNOWN = 0,
	BV_SERVER_CONTROL_REBOOT,
	BV_SERVER_CONTROL_SHUTDOWN,
	BV_SERVER_CONTROL_SBVECONFIG,

	//查询命令
	BV_SERVER_QUERY_UNKNOWN = 0x10000,
	BV_SERVER_QUERY_,

	//事件命令
	BV_SERVER_EVENT_UNKONWN = 0x20000,
	BV_SERVER_EVENT_REGISTER,

	//应答命令
	BV_SERVER_RESPONSE_UNKOWN = 0x30000,
	BV_SERVER_RESPONSE_REGISTER,

};

enum BVServerPacketType{
	BV_SERVER_PACKET_TYPE_NONE,
	BV_SERVER_PACKET_TYPE_CMD,
	BV_SERVER_PACKET_TYPE_DATA,
	
	BV_SERVER_PACKET_TYPE_UNKNOWN
};

typedef struct _BVServerPacket {
	enum BVServerCmd cmd;
	enum BVServerPacketType packet_type;
	void *data;
	int data_size;
} BVServerPacket;

typedef struct _BVServerRegisterInfo {
	uint16_t province_id;
	uint16_t city_id;
	uint32_t producter_id;
	uint32_t pu_type;
	uint32_t pu_id;
	uint8_t  car_color;
	uint8_t  car_num[15];
} BVServerRegisterInfo;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_SERVER_CMD_H */
