/*************************************************************************
    > File Name: bvserver.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年09月25日 星期四 09时48分36秒
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

#ifndef BV_SERVER_H
#define BV_SERVER_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @file
 *  libserver public API header
 *  DSS的核心服务器部分是由一个父进程所fork出的一个子进程构成，该父进程就构成了整合流媒体服务器。父进程会等待子进程的退出，如果在运行的时候子进程产生了错误从而退出，那么父进程就会fork出一个新的子进程。可以看出，网络客户和服务器直接的对接是由核心服务器来完成的。网络客户RTSPoverRTP来发送或者接受请求。服务器就通过模块来处理相应的请求并向客户端发送数据包。
 * 核心流媒体服务通过创建四种类型的线程来完成自己的工作，具体如下：
 * 服务器自己拥有的主线程。当服务器需要关闭检查，以及在关闭之前记录相关状态打印相关统计信息等任务处理时，一般都是通过这个线程来完成的。
 * 空闲任务线程。这个任务线程是用来对一个周期任务队列的管理，主要管理两种任务，超时任务和Socket任务。
 * 事件线程。套接口相关事件由事件线程负责监听，当有RTSP请求或者收到RTP数据包时，事件线程就会把这些实践交给任务线程来处理。
 * 任务线程。任务线程会把事件从事件线程中取出，并把处理请求传递到对应的服务器模块进行处理，比如把数据包发送给客户端的模块，在默认情况下，核心服务器会为每个处理器核创建一个任务线程。 
 */

#include "version.h"

#include "libbvutil/log.h"
#include "libbvutil/opt.h"
#include "libbvutil/dict.h"

#include "server_cmd.h"

#define MAX_NAME_LEN (128)

enum BVServerType{
    BV_SERVER_TYPE_NONE = -1,
    BV_SERVER_TYPE_BVC,        //Besovodeo Command Server
    BV_SERVER_TYPE_BVS,        //Besovideo Stream Server
    BV_SERVER_TYPE_BPU,        //Besovideo PU
    BV_SERVER_TYPE_MTC,        //Ministry of Transportation and Communication of China

    BV_SERVER_TYPE_UNKNOWN
};

typedef struct BVServerContext {
    const BVClass *bv_class;
    //URLContext *uc;    //not public strunct use BVIOContext instead
    //BVIOContext *pb;    //pb->opaque ponit to URLContext struct
    char url[1024];
    char server_name[MAX_NAME_LEN + 1];
    char server_ip[MAX_NAME_LEN + 1];
    struct _BVServer *server;
    char *authr_num;
    uint16_t port;
    uint16_t heartbeat_interval;    //心跳间隔 秒
    uint8_t protocol_type;    //udp tcp
    void *priv_data;
} BVServerContext;

typedef struct _BVServer {
    const char *name;
    enum BVServerType server_type;
    const BVClass *priv_class;
    int priv_data_size;
    struct _BVServer *next;
    int (*svr_open) (BVServerContext *svrctx);
    int (*svr_close) (BVServerContext *svrctx);
    int (*svr_connect) (BVServerContext * svrctx);
    int (*svr_disconnect) (BVServerContext * svrctx);
    int (*svr_read) (BVServerContext * svrctx, BVServerPacket *pkt);
    int (*svr_write) (BVServerContext * svrctx, const BVServerPacket *pkt);
    int (*get_fd)(BVServerContext *svrctx);
} BVServer;


void bv_server_register_all(void);

int bv_server_register(BVServer *svr);

BVServer * bv_server_find_server(enum BVServerType server_type);

BVServer * bv_server_find_server_by_name(const char *svr_name);

BVServer * bv_server_next(BVServer *svr);

BVServerContext * bv_server_context_alloc(void);

void bv_server_context_free(BVServerContext *svrctx);

int bv_server_open(BVServerContext **svrctx, BVServer *svr, const char *url, BVDictionary **options);

int bv_server_connect(BVServerContext *svrctx);

int bv_server_disconnect(BVServerContext *svrctx);

int bv_server_read(BVServerContext *svrctx, BVServerPacket *pkt);

int bv_server_write(BVServerContext *svrctx, const BVServerPacket *pkt);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_SERVER_H */

