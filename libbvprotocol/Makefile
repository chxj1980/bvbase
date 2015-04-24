include $(SUBDIR)../config.mak

NAME    = bvprotocol

HEADERS = bvio.h bvurl.h version.h

OBJS    = bvurl.o allprotocols.o bvio.o internal.o


OBJS-$(BV_CONFIG_FILE_PROTOCOL)         += file.o
OBJS-$(BV_CONFIG_TCP_PROTOCOL)          += tcp.o
OBJS-$(BV_CONFIG_UDP_PROTOCOL)          += udp.o
OBJS-$(BV_CONFIG_BVFS_PROTOCOL)         += bvfsproto.o
OBJS-$(BV_CONFIG_HTTP_PROTOCOL)         += http.o httpauth.o
OBJS-$(BV_CONFIG_HTTPS_PROTOCOL)        += http.o httpauth.o
OBJS-$(BV_CONFIG_HTTPPROXY_PROTOCOL)    += http.o httpauth.o
OBJS-$(BV_CONFIG_RTP_PROTOCOL)          += rtp.o
OBJS-$(BV_CONFIG_RTSP_PROTOCOL)         += rtsp.o
OBJS-$(BV_CONFIG_SDP_PROTOCOL)          += sdp.o

