/**
 *          File: exStream.c
 *
 *        Create: 2014年12月11日 星期四 17时30分07秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>

#include <libconfig/common.h>
#include <libstream/stream.h>

int main(int argc, const char *argv[])
{
    AVFormatContext *avfmt = NULL;
    BVChannel channel;
    memset(&channel, 0, sizeof(BVChannel));

    bv_stream_init();

    channel.type = BV_CHANNEL_TYPE_ONVIFAVE;
    BVMediaProfile profile;
    BVCertification certification;
    memset(&profile, 0, sizeof(profile));
    memset(&certification, 0, sizeof(certification));

    av_strlcpy(profile.basic_info.name, "local_00_00/Profile_0000/0000", sizeof(profile.basic_info.name));
    av_strlcpy(profile.certification->server, "http://192.168.6.149:8989", sizeof(profile.certification->server));

    channel.profile = profile;
    if (bv_format_open_input(&avfmt, &channel, NULL, NULL) < 0) {
        printf("error\n");
    }
    return 0;
}

//
// example of stream get


/*=============== End of file: exStream.c ==========================*/
