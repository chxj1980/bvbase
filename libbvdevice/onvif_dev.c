/*************************************************************************
    > File Name: onvif_dev.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月29日 星期一 10时17分53秒
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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#define _GNU_SOURCE
#include <wsseapi.h>
#include <wsaapi.h>
#include <string.h>

#include "bvdevice.h"
#include <libbvutil/bvstring.h>

#define ONVIF_NVT "NetWorkVideoTransmitter"
#define ONVIF_NVS "NetWorkVideoStorage"
#define ONVIF_NVD "NetWorkVideoDisplay"
#define ONVIF_NVA "NetWorkVideoAnalytics"

#define ONVIF_TMO    (-500000)

typedef struct OnvifDeviceContext {
    BVClass *bv_class;
    struct soap *soap;
    char *user;
    char *passwd;
    char svrurl[1024];
    int timeout;
    int max_ipcs;
} OnvifDeviceContext;

static struct soap *bv_soap_new(int timeout)
{
    struct soap *soap = NULL;
    soap = soap_new();
    if (soap == NULL) {
        return NULL;
    }
    soap_set_namespaces(soap, namespaces);
    if (!timeout) {
        timeout = ONVIF_TMO;
    }
    soap->recv_timeout = timeout;
    soap->send_timeout = timeout;
    soap->connect_timeout = timeout;
    return soap;
}

static void bv_soap_free(struct soap *soap)
{
    if (soap == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "Soap is NULL\n");
        return;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    soap_free(soap);
}

static void set_device_info(BVMobileDevice *device_info, struct __wsdd__ProbeMatches *resp)
{
    char *input_str = resp->wsdd__ProbeMatches->ProbeMatch->Types;
#if 1
    bv_log(NULL, BV_LOG_INFO, "Target Service Address  : %s\r\n",
        resp->wsdd__ProbeMatches->ProbeMatch->XAddrs);
    bv_log(NULL, BV_LOG_INFO, "Target EP Address       : %s\r\n",
        resp->wsdd__ProbeMatches->ProbeMatch->wsa__EndpointReference.Address);
    bv_log(NULL, BV_LOG_INFO, "Target Type             : %s\r\n",
        resp->wsdd__ProbeMatches->ProbeMatch->Types);
    bv_log(NULL, BV_LOG_INFO, "Target Metadata Version : %d\r\n",
        resp->wsdd__ProbeMatches->ProbeMatch->MetadataVersion);
#endif
    strncpy(device_info->url, resp->wsdd__ProbeMatches->ProbeMatch->XAddrs, sizeof(device_info->url) -1);
    device_info->timeout = ONVIF_TMO;
    device_info->type = BV_MOBILE_DEVICE_TYPE_NONE;
    if(strcasestr(input_str,ONVIF_NVS)) {
        bv_log(NULL, BV_LOG_INFO, "device is Storage\n");
        device_info->type |= BV_MOBILE_DEVICE_TYPE_NVS;
    }
    if(strcasestr(input_str,ONVIF_NVT)) {
        bv_log(NULL, BV_LOG_INFO, "device is Transmitter\n");
        device_info->type |= BV_MOBILE_DEVICE_TYPE_NVT;
    }
    if(strcasestr(input_str,ONVIF_NVA)) {
        bv_log(NULL, BV_LOG_INFO, "device is Analytics\n");
        device_info->type |= BV_MOBILE_DEVICE_TYPE_NVA;
    }
    if(strcasestr(input_str,ONVIF_NVD)) {
        bv_log(NULL, BV_LOG_INFO, "device is Display\n");
        device_info->type |= BV_MOBILE_DEVICE_TYPE_NVD;
    }
}

static int onvif_device_scan(BVDeviceContext *h, BVMobileDevice *device, int *max_ret)
{
    OnvifDeviceContext *devctx = h->priv_data;
    BVMobileDevice *current = device;
    int device_num = 0;
    int retval = SOAP_OK;
    wsdd__ProbeType req;
    struct __wsdd__ProbeMatches resp;
    wsdd__ScopesType sScope;
    struct SOAP_ENV__Header header;
    struct soap *soap;

    //这个就是传递过去的组播的ip地址和对应的端口发送广播信息  
    const char *soap_endpoint = "soap.udp://239.255.255.250:3702/";

    //这个接口填充一些信息并new返回一个soap对象，本来可以不用额外接口，
    // 但是后期会作其他操作，此部分剔除出来后面的操作就相对简单了,只是调用接口就好
    //soap = bv_soap_new(&header, was_To, was_Action, 5);
    soap = devctx->soap;

    soap_default_SOAP_ENV__Header(soap, &header);

    header.wsa__MessageID = (char *)soap_wsa_rand_uuid(soap);
    header.wsa__To = (char *) "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
    header.wsa__Action = (char *) "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
    soap->header = &header;

    soap_default_wsdd__ScopesType(soap, &sScope);
    sScope.__item = (char *)"";
    soap_default_wsdd__ProbeType(soap, &req);
    req.Scopes = &sScope;
    req.Types = (char *)"";                //"dn:NetworkVideoTransmitter";
    retval = soap_send___wsdd__Probe(soap, soap_endpoint, NULL, &req);
    //发送组播消息成功后，开始循环接收各位设备发送过来的消息
    while (retval == SOAP_OK) {
        retval = soap_recv___wsdd__ProbeMatches(soap, &resp);
        if (retval == SOAP_OK) {
            if (resp.wsdd__ProbeMatches &&resp.wsdd__ProbeMatches->ProbeMatch != NULL
                && resp.wsdd__ProbeMatches->ProbeMatch->XAddrs != NULL) {
                current = device + device_num;
                set_device_info(current, &resp);
                device_num ++;
                if(device_num > *max_ret) {
                    bv_log(NULL, BV_LOG_INFO, "reach max ret %d\n", *max_ret);
                    break;
                }
            }
        } else if (soap->error) {
            bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error,
                *soap_faultcode(soap), *soap_faultstring(soap));
            retval = soap->error;
        }
    }

    *max_ret = device_num;
    return device_num > 0 ? device_num : -1;
}

static int onvif_device_open(BVDeviceContext *h)
{
    OnvifDeviceContext *devctx = h->priv_data;
    char *p = NULL;
    p = bv_sreplace(h->url, "onvif_dev", "http");
    if (!p) {
        return BVERROR(ENOMEM);
    }
    bv_strlcpy(devctx->svrurl, p, sizeof(devctx->svrurl));
    bv_free(p);
    devctx->soap = bv_soap_new(devctx->timeout);
    if (!devctx->soap) {
        return BVERROR(ENOMEM);
    }
    return 0;
}

static int onvif_device_probe(BVDeviceContext *h, const char *args)
{
    if (strcmp("onvif_dev", args) == 0) {
        return 100;
    }
    return 0;
}
static int onvif_search_ipc(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    OnvifDeviceContext *devctx = h->priv_data;
    int ret = 0, max_ret = devctx->max_ipcs;
    BVMobileDevice *device = bv_mallocz(sizeof(BVMobileDevice) * devctx->max_ipcs);
    if (!device) {
        return BVERROR(ENOMEM);
    }

    ret = onvif_device_scan(h, device, &max_ret);
    if (ret < 0) {
        bv_free(device);
        device = NULL;
    }
    pkt_out->data = (void *)device;
    pkt_out->size = max_ret;
    return ret;
}

static int onvif_detect_ipc(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    BVMobileDevice *device = pkt_in->data;
    int retval = SOAP_OK;
    wsdd__ProbeType req;
    struct __wsdd__ProbeMatches resp;
    wsdd__ScopesType sScope;
    struct SOAP_ENV__Header header;
    struct soap *soap;
    char hostname[1024], proto[10];
    char auth[1024], path[1024], soap_endpoint[1024];
    int port, timeout = 1;
    int status = BV_DEVICE_STATUS_IPC_OFF_LINE;

    bv_url_split(proto, sizeof(proto), auth, sizeof(auth),
                 hostname, sizeof(hostname), &port,
                 path, sizeof(path), device->url);
    bv_log(h, BV_LOG_DEBUG, "url %s proto %s auth %s hostname %s port %d path %s\n",
            device->url, proto, auth, hostname, port, path);
    port = 3702;    //FIXME
    bv_sprintf(soap_endpoint, sizeof(soap_endpoint), "soap.udp://%s:%d", hostname, port);
    timeout = BBMAX(timeout, device->timeout);
    soap = bv_soap_new(timeout);

    soap_default_SOAP_ENV__Header(soap, &header);

    header.wsa__MessageID = (char *)soap_wsa_rand_uuid(soap);
    header.wsa__To = (char *) "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
    header.wsa__Action = (char *) "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
    soap->header = &header;

    soap_default_wsdd__ScopesType(soap, &sScope);
    sScope.__item = (char *)"";
    soap_default_wsdd__ProbeType(soap, &req);
    req.Scopes = &sScope;
    req.Types = (char *)"";                //"dn:NetworkVideoTransmitter";
    retval = soap_send___wsdd__Probe(soap, soap_endpoint, NULL, &req);
    if (retval == SOAP_OK) {
        retval = soap_recv___wsdd__ProbeMatches(soap, &resp);
        if (retval != SOAP_OK) {
            bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error,
                *soap_faultcode(soap), *soap_faultstring(soap));
            retval = BVERROR(EIO);
        }
    }else{
        bv_log(NULL, BV_LOG_INFO, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error,
            *soap_faultcode(soap), *soap_faultstring(soap));
        retval = BVERROR(EIO);
    }
    bv_soap_free(soap);
    if (pkt_out) {
        pkt_out->data = bv_mallocz(sizeof(int));
        if (retval == 0) 
            status = BV_DEVICE_STATUS_IPC_ON_LINE;
        if (pkt_out->data) {
            *(int *)pkt_out->data = status;
        }
    }
    return retval;
}

static int onvif_set_remote_dp(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    OnvifDeviceContext *devctx = h->priv_data;
    BVEthernetInfo *info = pkt_in->data;
    struct soap *soap = devctx->soap;
    int retval = SOAP_OK, i =0;
    struct _tds__SetDPAddresses request;
    struct _tds__SetDPAddressesResponse responses;
    struct _tds__SetRemoteDiscoveryMode tds__SetRemoteDiscoveryMode;
    struct _tds__SetRemoteDiscoveryModeResponse tds__SetRemoteDiscoveryModeResponse;

    BBCLEAR_STRUCT(tds__SetRemoteDiscoveryMode);
    BBCLEAR_STRUCT(tds__SetRemoteDiscoveryModeResponse);

    BBCLEAR_STRUCT(request);
    BBCLEAR_STRUCT(responses);

    request.__sizeDPAddress = pkt_in->size;
    request.DPAddress = soap_malloc(soap, sizeof(struct tt__NetworkHost) * request.__sizeDPAddress);
    if (!request.DPAddress) {
        return BVERROR(ENOMEM);;
    }
    for (i = 0; i < pkt_in->size; i++) {
        request.DPAddress[i].Type = tt__NetworkHostType__IPv4;
        request.DPAddress[i].IPv4Address = info[i].address;
    }

    if (devctx->user && devctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", devctx->user, devctx->passwd);
    }

    retval = soap_call___tds__SetDPAddresses(soap, devctx->svrurl, NULL, &request, &responses);
    if (retval != SOAP_OK) {
        bv_log(devctx, BV_LOG_ERROR, "set remote discover proxy error\n");
        bv_log(devctx, BV_LOG_ERROR, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        bv_log(devctx, BV_LOG_INFO, "set remote discover proxy ok\n");
    }

    tds__SetRemoteDiscoveryMode.RemoteDiscoveryMode = tt__DiscoveryMode__Discoverable;

    if (devctx->user && devctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", devctx->user, devctx->passwd);
    }
    retval =soap_call___tds__SetRemoteDiscoveryMode(soap, devctx->svrurl, NULL, &tds__SetRemoteDiscoveryMode, &tds__SetRemoteDiscoveryModeResponse);
    if (retval != SOAP_OK) {
        bv_log(devctx, BV_LOG_ERROR, "set remote discover mode error\n");
        bv_log(devctx, BV_LOG_ERROR, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        bv_log(devctx, BV_LOG_INFO, "set remote discover mode ok\n");
    }
    return retval;
}

static int onvif_get_remote_dp(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    OnvifDeviceContext *devctx = h->priv_data;
    struct soap *soap = devctx->soap;
    int retval = SOAP_OK;
    struct _tds__GetDPAddresses request;
    struct _tds__GetDPAddressesResponse responses;
    struct _tds__GetRemoteDiscoveryMode tds__GetRemoteDiscoveryMode;
    struct _tds__GetRemoteDiscoveryModeResponse tds__GetRemoteDiscoveryModeResponse;

    BBCLEAR_STRUCT(tds__GetRemoteDiscoveryMode);
    BBCLEAR_STRUCT(tds__GetRemoteDiscoveryModeResponse);

    BBCLEAR_STRUCT(request);
    BBCLEAR_STRUCT(responses);

    if (devctx->user && devctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", devctx->user, devctx->passwd);
    }

    retval = soap_call___tds__GetDPAddresses(soap, devctx->svrurl, NULL, &request, &responses);
    if (retval != SOAP_OK) {
        bv_log(devctx, BV_LOG_ERROR, "set remote discover proxy error\n");
        bv_log(devctx, BV_LOG_ERROR, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        int i = 0;
        bv_log(devctx, BV_LOG_INFO, "set remote discover proxy ok\n");
        bv_log(devctx, BV_LOG_INFO, "remote dp size %d\n", responses.__sizeDPAddress);
        for (i = 0; i < responses.__sizeDPAddress; i++) {
            bv_log(devctx, BV_LOG_INFO, "remote dp addr %s\n", responses.DPAddress[i].IPv4Address);
        }
    }

    if (devctx->user && devctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", devctx->user, devctx->passwd);
    }
    retval = SOAP_FMAC6 soap_call___tds__GetRemoteDiscoveryMode(soap, devctx->svrurl, NULL, &tds__GetRemoteDiscoveryMode, &tds__GetRemoteDiscoveryModeResponse);
    if (retval != SOAP_OK) {
        bv_log(devctx, BV_LOG_ERROR, "get remote discover mode error\n");
        bv_log(devctx, BV_LOG_ERROR, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        bv_log(devctx, BV_LOG_INFO, "get remote discover mode ok\n");
        bv_log(devctx, BV_LOG_INFO, "get remote discover mode %d \n", tds__GetRemoteDiscoveryModeResponse.RemoteDiscoveryMode);
    }

    return retval;
}


static int onvif_device_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVDeviceMessageType type;
        int (*control)(BVDeviceContext *h, const BVControlPacket *, BVControlPacket *);
    } onvif_control[] = {
        { BV_DEV_MESSAGE_TYPE_SEARCH_IPC, onvif_search_ipc},
        { BV_DEV_MESSAGE_TYPE_DETECT_IPC, onvif_detect_ipc},
        { BV_DEV_MESSAGE_TYPE_SET_RMTDP,  onvif_set_remote_dp},
        { BV_DEV_MESSAGE_TYPE_GET_RMTDP,  onvif_get_remote_dp},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(onvif_control); i++) {
        if (onvif_control[i].type == type)
           return onvif_control[i].control(h, pkt_in, pkt_out); 
    }
    bv_log(h, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

static int onvif_device_close(BVDeviceContext *h)
{
    OnvifDeviceContext *devctx = h->priv_data;
    bv_soap_free(devctx->soap);
    return 0;
}

#define OFFSET(x) offsetof(OnvifDeviceContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"timeout", "read write time out", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
    {"max_ipcs", "", OFFSET(max_ipcs), BV_OPT_TYPE_INT, {.i64 =  128}, INT_MIN, INT_MAX, DEC},
    {"user", "", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {NULL}
};

static const BVClass onvif_class = {
    .class_name         = "onvif device",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVDevice bv_onvif_dev_device = {
    .name               = "onvif_dev",
    .type               = BV_DEVICE_TYPE_ONVIF_DEVICE,
    .priv_data_size     = sizeof(OnvifDeviceContext),
    .dev_open           = onvif_device_open,
    .dev_probe          = onvif_device_probe,
    .dev_close          = onvif_device_close,
    .dev_control        = onvif_device_control,
    .priv_class         = &onvif_class,
};
