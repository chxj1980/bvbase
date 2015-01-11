# use pkg-config for getting CFLAGS and LDLIBS
include $(SUBDIR)../../config.mak

BVBASE_DIR=/home/albert/selfgit/binary

CFLAGS=  -I/home/albert/selfgit/binary/ffmpeg/x86/include -I/home/albert/selfgit/binary/onvif/x86/include -I/home/albert/selfgit/binary/openssl/x86/include -std=c99 -fomit-frame-pointer -pthread  

LDFLAGS= -L/home/albert/selfgit/binary/ffmpeg/x86/lib -lavformat -lavcodec -lavutil -L/home/albert/selfgit/binary/onvif/x86/lib -lonvifc -L/home/albert/selfgit/binary/openssl/x86/lib -lssl -lcrypto  

CFLAGS += -I$(BVBASE_DIR)/bvbase/x86/include
LDFLAGS += -L$(BVBASE_DIR)/bvbase/x86/lib -lbvdevice -lbvmedia -lbvcodec -lbvutil
EXAMPLES=  exDevice device_scan exMedia

CFLAGS += -Wall -g

OBJS=$(addsuffix .o,$(EXAMPLES))

#%.o:%.c
#	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
