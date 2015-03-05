include $(SUBDIR)../config.mak

NAME    = bvconfig

HEADERS = bvconfig.h common.h bvcfile.h

OBJS    = utils.o allconfigs.o options.o config.o bvcfile.o

OBJS-$(BV_CONFIG_JSON_CFILE)				+= json.o
OBJS-$(BV_CONFIG_ONVIF_CONFIG)				+= onvif.o
OBJS-$(BV_CONFIG_LOCAL_CONFIG)				+= local.o
