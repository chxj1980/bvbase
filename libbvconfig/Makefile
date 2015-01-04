include $(SUBDIR)../config.mak

NAME = bvconfig

HEADERS		=							bvconfig.h \
										common.h

OBJS		 = 							utils.o \
										allconfigs.o \
										options.o

OBJS-$(BV_CONFIG_ONVIF_CONFIG)				+= onvif.o
OBJS-$(BV_CONFIG_JSON_CONFIG)				+= json.o
