/*************************************************************************
	> File Name: mtcsvr.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年09月29日 星期一 11时30分53秒
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

#include "libavformat/url.h"
#include "libavformat/mtcproto.h"
#include "server.h"

struct MTCServerContext {
	char author_num[1024];
    const AVClass *av_class;
	URLContext *uc;
	uint32_t pkt_seq;
	bool init;
};

static int mtc_connect(BVServerContext *svrctx)
{
	struct MTCServerContext *mtc = svrctx->priv_data;
	if (mtc->init == true) {
		return 0;
	}
	char url[512] = { 0 };
	ff_url_join(url, sizeof(url), "mtc", NULL, svrctx->server_ip, svrctx->port, NULL);
	int flags = URL_PROTOCOL_FLAG_NETWORK |  AVIO_FLAG_READ_WRITE;
	if (ffurl_open(&mtc->uc, url, flags, NULL, NULL) < 0) {
		av_log(NULL, AV_LOG_ERROR, "find protocol error");
		return -1;
	}
	return 0;
}

static int mtc_read(BVServerContext * svrctx, BVServerPacket *pkt)
{

}

static int write_register(struct MTCServerContext *mtc, const BVServerPacket *pkt, size_t count)
{
	BVMTCMsg msg;
	BVServerRegisterInfo *register_info = pkt->data;
	msg->msg_head.msg_id = TERMINAL_REGISTER;
	msg->msg_head.msg_seq = mtc->pkt_seq;
	msg->msg_head.msg_len = sizeof(BVMTCPURegister);

	BVMTCPURegister msg_body;
	msg_body.producter_id = register_info->producter_id;
	msg_body.city_id = register_info->city_id;
	msg_body.producter_id = register_info->producter_id;
	msg_body.pu_type = register_info->pu_type;
	msg_body.pu_id = register_info->pu_id;
	msg_body.car_color = register_info->car_color;
	strncpy(msg_body.car_num, register_info->car_num, sizeof(msg_body.car_num) - 1);
	msg->
	if (ffurl_write(mtc->uc, &msg, sizeof(msg_body)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "ffurl_write error\n");
		return -1;
	}
	return pkt->data_size;
}

/**
 * 发送命令或数据
 * 内部转成平台内部结构
 */
static int mtc_write(BVServerContext * svrctx, const BVServerPacket *pkt, size_t count)
{
	struct MTCServerContext *mtc = svrctx->priv_data;
	int error = 0;
	if (mtc->init == false) {
		av_log(NULL, AV_LOG_ERROR, "Not Connect Success");
		return -1;
	}
	if(pkt->packet_type == BV_SERVER_PACKET_TYPE_DATA) {
		return count;
	}
	
	if(pkt->packet_type == BV_SERVER_PACKET_TYPE_UNKNOWN || pkt->packet_type == BV_SERVER_TYPE_NONE) {
		return 0;
	}
	switch(pkt->cmd) {
	case BV_SERVER_EVENT_REGISTER:
		error = write_register(mtc, pkt, count);
		break;

	default:
		av_log(NULL, AV_LOG_WARNING, "Not Support This method");
		break;
	}
	return error;
}

BVServer bv_mtc_server {
	.name = "mtc",
	.connect = mtc_connect,
	.read = mtc_read,
	.write = mtc_write,
	.priv_data_size = sizeof(struct MTCServerContext),
};

