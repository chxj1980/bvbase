# use pkg-config for getting CFLAGS and LDLIBS
include $(SUBDIR)../../config.mak

BVBASE_DIR=$(shell pwd)/../../3rdparty/binary/

CFLAGS=  -I$(BVBASE_DIR)/ffmpeg/x86/include -I$(BVBASE_DIR)/onvif/x86/include -I$(BVBASE_DIR)/openssl/x86/include -std=c99 -fomit-frame-pointer -pthread  

LDFLAGS= -L$(BVBASE_DIR)/ffmpeg/x86/lib -lavformat -lavcodec -lavutil -L$(BVBASE_DIR)/onvif/x86/lib -lonvifc -L$(BVBASE_DIR)/openssl/x86/lib -lssl -lcrypto  

CFLAGS += -I$(BVBASE_DIR)/bvbase/x86/include
LDFLAGS += -L$(BVBASE_DIR)/bvbase/x86/lib -lbvdevice -lbvmedia -lbvcodec -lbvutil
EXAMPLES=  exDevice device_scan exMedia a

CFLAGS += -Wall -g

OBJS=$(addsuffix .o,$(EXAMPLES))

#%.o:%.c
#	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
