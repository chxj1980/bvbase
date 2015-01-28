include $(SUBDIR)../config.mak

NAME = bvprotocol

HEADERS = bvio.h bvurl.h

#OBJS = utils.o allformats.o format.o options.o
OBJS  = os_support.o network.o bvurl.o allprotocols.o bvio.o


OBJS-$(BV_CONFIG_FILE_PROTOCOL)          += file.o
OBJS-$(BV_CONFIG_TCP_PROTOCOL)           += tcp.o
OBJS-$(BV_CONFIG_UDP_PROTOCOL)           += udp.o
OBJS-$(BV_CONFIG_BVFS_PROTOCOL)          += bvfsproto.o
