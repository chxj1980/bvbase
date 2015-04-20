include $(SUBDIR)../config.mak

NAME    = bvdevice

HEADERS = bvdevice.h version.h

OBJS    = utils.o alldevices.o options.o

OBJS-$(BV_CONFIG_ONVIF_DEV_DEVICE)          += onvif_dev.o
OBJS-$(BV_CONFIG_ONVIF_PTZ_DEVICE)          += onvif_ptz.o
OBJS-$(BV_CONFIG_DISK_DEV_DEVICE)           += disk.o
