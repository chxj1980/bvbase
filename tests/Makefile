CFlAGS += -I /home/albert/x86/ffmpeg2/include/ -Wall
LDFLAGS +=  -L /home/albert/x86/ffmpeg2/lib/  -L /home/albert/bvlive/3rdparty/onvif/lib/ -lavfilter -lavformat -lavcodec -lswscale -lavutil -lonvifc
all:
	gcc -o test_avfilter $(CFlAGS) $(LDFLAGS) test_avfilter.c
