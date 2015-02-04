include $(SUBDIR)../config.mak

NAME    = bvsystem

HEADERS = bvsystem.h

OBJS    = utils.o allsystems.o options.o

ifeq ($(BV_CONFIG_HIS3515), yes)
-include $(SUBDIR)/his3515/Makefile
endif
