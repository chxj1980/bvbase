# use pkg-config for getting CFLAGS and LDLIBS
include $(SUBDIR)../bvconfig.mak

CFLAGS += -Wall -g
#CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
#LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

CFLAGS = -I ../include/x86 -I ../3rdparty/binary/ffmpeg/x86/include 

LDFLAGS = -L ../3rdparty/binary/ffmpeg/x86/lib -lavutil -L ../3rdparty/binary/onvif/x86/lib/ -lonvifc -L ../lib/x86 -ldevice -lavformat -lavdevice -lavcodec -lavfilter -lswscale -lstream

EXAMPLES=       exDevice exStream

OBJS=$(addsuffix .o,$(EXAMPLES))

#%.o:%.c
#	$(CC) -c $^ -o $@ $(CFLAGS)
.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean-test:
	$(RM) test*.pgm test.h264 test.mp2 test.sw test.mpg

clean: 
	$(RM) $(EXAMPLES) $(OBJS)
