# use pkg-config for getting CFLAGS and LDLIBS
include $(SUBDIR)../bvconfig.mak

CFLAGS += -Wall -g
#CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
#LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

CFLAGS = -I ../include/ -I /home/albert/x86/ffmpeg2/include

LDFLAGS = -L ../lib -ldevice -L /home/albert/x86/ffmpeg2/lib -lavutil

EXAMPLES=       exDevice

OBJS=$(addsuffix .o,$(EXAMPLES))

#%.o:%.c
#	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean-test:
	$(RM) test*.pgm test.h264 test.mp2 test.sw test.mpg

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
