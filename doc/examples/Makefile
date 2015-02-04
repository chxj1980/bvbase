# use pkg-config for getting CFLAGS and LDLIBS
#include $(SUBDIR)../../config.mak

BVBASE_DIR=$(shell pwd)/../../3rdparty/binary

PLANTFORM=x86

ifeq ($(os), hi3515)
CRosS_COMPILE_PRE = arm-hisi-linux-
PLANTFORM=hi3515
endif

ifeq ($(os), hi3518)
CRosS_COMPILE_PRE = arm-hisiv100nptl-linux-
PLANTFORM=hi3518
endif

ifeq ($(os), hi3535)
CRosS_COMPILE_PRE = arm-hisiv100nptl-linux-
CFLAGS += -DPLATFORM_HI3535 -march=armv7-a -mcpu=cortex-a9
PLANTFORM=hi3535
endif

CC = $(CRosS_COMPILE_PRE)gcc
CXX = $(CRosS_COMPILE_PRE)g++
AR = $(CRosS_COMPILE_PRE)ar
STRIP = $(CRosS_COMPILE_PRE)strip

CFLAGS=  -I$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/include -I$(BVBASE_DIR)/onvif/$(PLANTFORM)/include -I$(BVBASE_DIR)/openssl/$(PLANTFORM)/include -std=c99 -fomit-frame-pointer -pthread  

LDFLAGS= -L$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/lib -lavformat -lavcodec -lavutil -L$(BVBASE_DIR)/onvif/$(PLANTFORM)/lib -lonvifc -L$(BVBASE_DIR)/openssl/$(PLANTFORM)/lib -lssl -lcrypto  

CFLAGS += -I$(BVBASE_DIR)/bvbase/$(PLANTFORM)/include
LDFLAGS += -L$(BVBASE_DIR)/bvbase/$(PLANTFORM)/lib -lbvdevice -lbvmedia -lbvcodec -lbvutil -lbvconfig -lbvprotocol

CFLAGS += -I$(BVBASE_DIR)/bvfs/$(PLANTFORM)/include
LDFLAGS += -L$(BVBASE_DIR)/bvfs/$(PLANTFORM)/lib -lbvfs
EXAMPLES=  exDevice device_scan exMedia exConfig exUrl

CFLAGS += -Wall -g

OBJS=$(addsuffix .o,$(EXAMPLES))

%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
