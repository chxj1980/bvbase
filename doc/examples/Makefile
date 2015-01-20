# use pkg-config for getting CFLAGS and LDLIBS
include $(SUBDIR)../../config.mak

BVBASE_DIR=$(shell pwd)/../../3rdparty/binary

PLANTFORM=hi3515

CFLAGS=  -I$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/include -I$(BVBASE_DIR)/onvif/$(PLANTFORM)/include -I$(BVBASE_DIR)/openssl/$(PLANTFORM)/include -std=c99 -fomit-frame-pointer -pthread  

LDFLAGS= -L$(BVBASE_DIR)/ffmpeg/$(PLANTFORM)/lib -lavformat -lavcodec -lavutil -L$(BVBASE_DIR)/onvif/$(PLANTFORM)/lib -lonvifc -L$(BVBASE_DIR)/openssl/$(PLANTFORM)/lib -lssl -lcrypto  

CFLAGS += -I$(BVBASE_DIR)/bvbase/$(PLANTFORM)/include
LDFLAGS += -L$(BVBASE_DIR)/bvbase/$(PLANTFORM)/lib -lbvdevice -lbvmedia -lbvcodec -lbvutil -lbvconfig
EXAMPLES=  exDevice device_scan exMedia a exConfig

CFLAGS += -Wall -g

OBJS=$(addsuffix .o,$(EXAMPLES))

#%.o:%.c
#	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
