# use pkg-config for getting CFLAGS and LDLIBS
#include $(SUBDIR)../../config.mak

BVBASE_DIR=$(shell pwd)/../../3rdparty/binary

PLATFORM ?= x86

ifeq ($(PLATFORM), hi3515)
CROSS_COMPILE_PRE = arm-hisi-linux-
#LDFLAGS += -L$(BVBASE_DIR)/hissdk/his3515/lib -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lresampler
LDFLAGS += -L$(BVBASE_DIR)/hissdk/his3515/lib -lmpi -lhiaudio
endif

ifeq ($(PLATFORM), hi3518)
CROSS_COMPILE_PRE = arm-hisiv100nptl-linux-
endif

ifeq ($(PLATFORM), hi3535)
CROSS_COMPILE_PRE = arm-hisiv100nptl-linux-
CFLAGS += -DPLATFORM_HI3535 -march=armv7-a -mcpu=cortex-a9
endif

ifeq ($(PLATFORM), x86)
CFLAGS += -m32
LDFLAGS += -m32
endif

CC = $(CROSS_COMPILE_PRE)gcc
CXX = $(CROSS_COMPILE_PRE)g++
AR = $(CROSS_COMPILE_PRE)ar
STRIP = $(CROSS_COMPILE_PRE)strip

CFLAGS += -std=c99 -fomit-frame-pointer 
CFLAGS += -Wall -g

#CFLAGS += -I$(BVBASE_DIR)/ffmpeg/$(PLATFORM)/include 
#CFLAGS += -I$(BVBASE_DIR)/onvif/$(PLATFORM)/include 
#CFLAGS += -I$(BVBASE_DIR)/openssl/$(PLATFORM)/include 
CFLAGS += -I$(BVBASE_DIR)/bvbase/$(PLATFORM)/include
CFLAGS += -I$(BVBASE_DIR)/bvfs/$(PLATFORM)/include

LDFLAGS += -L$(BVBASE_DIR)/bvbase/$(PLATFORM)/lib -lbvdevice -lbvmedia -lbvcodec -lbvsystem -lbvconfig -lbvprotocol -lbvutil 
LDFLAGS += -L$(BVBASE_DIR)/ffmpeg/$(PLATFORM)/lib -lavformat -lavcodec -lavutil 
LDFLAGS += -L$(BVBASE_DIR)/onvif/$(PLATFORM)/lib -lonvifc 
LDFLAGS += -L$(BVBASE_DIR)/jansson/$(PLATFORM)/lib -ljansson
LDFLAGS += -L$(BVBASE_DIR)/openssl/$(PLATFORM)/lib -lssl -lcrypto  

LDFLAGS += -L$(BVBASE_DIR)/bvfs/$(PLATFORM)/lib -lbvfs -lpthread 

EXAMPLES=  exDevice device_scan exMedia exConfig exUrl exCfile exDisk exSystem exDecode exParser exFile exEncode exList a


OBJS=$(addsuffix .o,$(EXAMPLES))

%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS)
%:%.o
	$(CC) -o $@ $^ $(LDFLAGS)

.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
install:
	#cp $(EXAMPLES) /tftpboot/BVS-W8020-VF-A.filesys/usr/bin/
	cp $(EXAMPLES) /tftpboot/BVS-W4M10-VC-I.filesys/usr/bin/
	#cp $(EXAMPLES) /tftpboot/BVS-W4020-VC-E.filesys/usr/bin/
