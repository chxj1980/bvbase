include $(SUBDIR)../config.mak

NAME    = bvmedia

HEADERS = bvmedia.h

OBJS    = utils.o allmedias.o media.o driver.o options.o mux.o drawutils.o

include $(SUBDIR)/drivers/Makefile

OBJS-$(BV_CONFIG_ONVIFAVE_INDEV)            += onvifave.o
ifeq ($(BV_CONFIG_HIS3515), yes)
-include $(SUBDIR)/his3515/Makefile
endif

OBJS-$(BV_CONFIG_DAV_MUXER)                 += davmux.o
OBJS-$(BV_CONFIG_DAV_DEMUXER)               += davdmx.o
OBJS-$(BV_CONFIG_LIBFREETYPE)               += drawtext.o
