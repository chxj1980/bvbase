include $(SUBDIR)../config.mak

NAME = bvmedia

HEADERS = bvmedia.h

OBJS = utils.o allmedias.o media.o options.o mux.o

OBJS-$(BV_CONFIG_ONVIFAVE_INDEV)          	+= onvifave.o

OBJS-$(BV_CONFIG_DAV_MUXER)					+= davmux.o
