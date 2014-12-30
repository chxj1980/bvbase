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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#define _GNU_SOURCE
#include <soapH.h>
#include <string.h>

#include "bvdevice.h"

#define ONVIF_NVT "NetWorkVideoTransmitter"
#define ONVIF_NVS "NetWorkVideoStorage"
#define ONVIF_NVD "NetWorkVideoDisplay"
#define ONVIF_NVA "NetWorkVideoAnalytics"

#define ONVIF_TMO	(-500000)

typedef struct OnvifDeviceContext {
    AVClass *av_class;
    int timeout;
} OnvifDeviceContext;

static int get_uuid(char *uuid, int length)
{
	unsigned int Flagrand;
	unsigned char macaddr[6];
	srand((int) time(NULL));
	Flagrand = rand() % 9000 + 1000;	//保证四位整数
	macaddr[0] = 0x11;
	macaddr[1] = 0x25;
	macaddr[2] = 0x33;
	macaddr[3] = 0x43;
	macaddr[4] = 0x57;
	macaddr[5] = 0x65;
	snprintf(uuid, length, "urn:uuid:%ud68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",
		Flagrand, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

	printf("get uuid is %s\n", uuid);
	return 0;
}

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
		return;
	}
	soap_destroy(soap);
	soap_end(soap);
	soap_free(soap);
}

static void set_device_info(BVMobileDevice *device_info, struct __wsdd__ProbeMatches *resp)
{
#if 0
	printf("Target Service Address  : %s\r\n",
		resp->wsdd__ProbeMatches->ProbeMatch->XAddrs);
	printf("Target EP Address       : %s\r\n",
		resp->wsdd__ProbeMatches->ProbeMatch->wsa__EndpointReference.Address);
	printf("Target Type             : %s\r\n",
		resp->wsdd__ProbeMatches->ProbeMatch->Types);
	printf("Target Metadata Version : %d\r\n",
		resp->wsdd__ProbeMatches->ProbeMatch->MetadataVersion);
#endif
	char *input_str = resp->wsdd__ProbeMatches->ProbeMatch->Types;
	strncpy(device_info->url, resp->wsdd__ProbeMatches->ProbeMatch->XAddrs, sizeof(device_info->url) -1);
	device_info->timeout = ONVIF_TMO;
	device_info->type = BV_MOBILE_DEVICE_TYPE_NONE;
	if(strcasestr(input_str,ONVIF_NVS)) {
		printf("device is Storage\n");
		device_info->type |= BV_MOBILE_DEVICE_TYPE_NVS;
	}
	if(strcasestr(input_str,ONVIF_NVT)) {
		printf("device is Transmitter\n");
		device_info->type |= BV_MOBILE_DEVICE_TYPE_NVT;
	}
	if(strcasestr(input_str,ONVIF_NVA)) {
		printf("device is Analytics\n");
		device_info->type |= BV_MOBILE_DEVICE_TYPE_NVA;
	}
	if(strcasestr(input_str,ONVIF_NVD)) {
		printf("device is Display\n");
		device_info->type |= BV_MOBILE_DEVICE_TYPE_NVD;
	}
}

static int onvif_device_scan(BVDeviceContext *h, BVMobileDevice *device, int *max_ret)
{
    OnvifDeviceContext *devctx = h->priv_data;
    int timeout = devctx->timeout;
	BVMobileDevice *current = device;
	char uuid[64] = { 0 };
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
	soap = bv_soap_new(timeout);

	soap_default_SOAP_ENV__Header(soap, &header);

	get_uuid(uuid, 64);
	header.wsa__MessageID = uuid;
	header.wsa__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
	header.wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
	soap->header = &header;

	soap_default_wsdd__ScopesType(soap, &sScope);
	sScope.__item = "";
	soap_default_wsdd__ProbeType(soap, &req);
	req.Scopes = &sScope;
	req.Types = "dn";				//"dn:NetworkVideoTransmitter";

	retval = soap_send___wsdd__Probe(soap, soap_endpoint, NULL, &req);
	//发送组播消息成功后，开始循环接收各位设备发送过来的消息
	while (retval == SOAP_OK) {
		retval = soap_recv___wsdd__ProbeMatches(soap, &resp);
		if (retval == SOAP_OK) {
			if (resp.wsdd__ProbeMatches->ProbeMatch != NULL
				&& resp.wsdd__ProbeMatches->ProbeMatch->XAddrs != NULL) {
				current = device + device_num;
				set_device_info(current, &resp);
				device_num ++;
				if(device_num > *max_ret) {
					printf("reach max ret %d\n", *max_ret);
					break;
				}
			}
		} else if (soap->error) {
			printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error,
				*soap_faultcode(soap), *soap_faultstring(soap));
			retval = soap->error;
		}
	}

	*max_ret = device_num;

	bv_soap_free(soap);
	return device_num > 0 ? device_num : -1;
}

#define OFFSET(x) offsetof(OnvifDeviceContext, x)
#define DEC AV_OPT_FLAG_DECODING_PARAM
static const AVOption options[] = {
	{"timeout", "read write time out", OFFSET(timeout), AV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
	{NULL}
};

static const AVClass onvif_class = {
	.class_name     = "onvif device",
	.item_name      = av_default_item_name,
	.option         = options,
	.version        = LIBAVUTIL_VERSION_INT,
	.category       = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

BVDevice bv_onvif_dev_device = {
    .name = "onvif_dev",
    .type = BV_DEVICE_TYPE_ONVIF_DEVICE,
    .priv_data_size = sizeof(OnvifDeviceContext),
    .dev_scan = onvif_device_scan,
    .priv_class = &onvif_class,
};
