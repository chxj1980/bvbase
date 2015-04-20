include $(SUBDIR)../config.mak

NAME    = bvsystem

HEADERS = bvsystem.h version.h

OBJS    = utils.o allsystems.o options.o system.o

ifeq ($(BV_CONFIG_HIS3515), yes)
-include $(SUBDIR)/his3515/Makefile
endif

OBJS-$(BV_CONFIG_X86_SYSTEM)        += x86.o
