# use pkg-config for getting CFLAGS and LDLIBS
#include $(SUBDIR)../../config.mak

BVBASE_DIR=$(shell pwd)/../../3rdparty/binary

PLANTFORM ?= x86

ifeq ($(PLANTFORM), hi3515)
CROSS_COMPILE_PRE = arm-hisi-linux-
endif

ifeq ($(PLANTFORM), hi3518)
CROSS_COMPILE_PRE = arm-hisiv100nptl-linux-
endif

ifeq ($(PLANTFORM), hi3535)
CROSS_COMPILE_PRE = arm-hisiv100nptl-linux-
CFLAGS += -DPLATFORM_HI3535 -march=armv7-a -mcpu=cortex-a9
endif

CC = $(CROSS_COMPILE_PRE)gcc
CXX = $(CROSS_COMPILE_PRE)g++
AR = $(CROSS_COMPILE_PRE)ar
STRIP = $(CROSS_COMPILE_PRE)strip

CFLAGS=  -I$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/include -I$(BVBASE_DIR)/onvif/$(PLANTFORM)/include -I$(BVBASE_DIR)/openssl/$(PLANTFORM)/include -std=c99 -fomit-frame-pointer -pthread  

LDFLAGS= -L$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/lib -lavformat -lavcodec -lavutil -L$(BVBASE_DIR)/onvif/$(PLANTFORM)/lib -lonvifc -L$(BVBASE_DIR)/openssl/$(PLANTFORM)/lib -lssl -lcrypto  

CFLAGS += -I$(BVBASE_DIR)/bvbase/$(PLANTFORM)/include
LDFLAGS += -L$(BVBASE_DIR)/bvbase/$(PLANTFORM)/lib -lbvdevice -lbvmedia -lbvcodec -lbvutil -lbvconfig -lbvprotocol

CFLAGS += -I$(BVBASE_DIR)/bvfs/$(PLANTFORM)/include
LDFLAGS += -L$(BVBASE_DIR)/bvfs/$(PLANTFORM)/lib -lbvfs -lpthread -ldl -lrt
 EXAMPLES=  exDevice device_scan exMedia exConfig exUrl exOutMedia

CFLAGS += -Wall -g

OBJS=$(addsuffix .o,$(EXAMPLES))

%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
install:
	cp $(EXAMPLES) /tftpboot/BVS-W4M10-VC-I.filesys2/usr/bin/
