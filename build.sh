#!/bin/bash

usage()
{
    echo "$0 x86/x64/dm6446/dm365/hi3515/hi3535"
	exit 1
}

if [ $# != 1 ]; then
	usage
fi

BVBASE_DIR=$(pwd)/3rdparty/binary
NAME=bvbase

ARCH=$1
#export PKG_CONFIG_PATH=$BVBASE_DIR/freetype/$1/lib/pkgconfig

enable_bvfs()
{
	LD_LIBRARY_PATH+=$BVBASE_DIR/bvfs/$ARCH/lib:
	EXENABLE+="--enable-libbvfs "
	EXTCFLAGS+="-I$BVBASE_DIR/bvfs/$ARCH/include "
	EXTLFLAGS+="-L$BVBASE_DIR/bvfs/$ARCH/lib -lbvfs "
}

enable_onvifc()
{
	LD_LIBRARY_PATH+=$BVBASE_DIR/onvif/$ARCH/lib:$BVBASE_DIR/openssl/$ARCH/lib:
	EXENABLE+="--enable-libonvifc "
	EXTCFLAGS+="-I$BVBASE_DIR/onvif/$ARCH/include -I$BVBASE_DIR/openssl/$ARCH/include "
	EXTLFLAGS+="-L$BVBASE_DIR/onvif/$ARCH/lib -lonvifc -L$BVBASE_DIR/openssl/$ARCH/lib -lssl -lcrypto "
}

enable_ffmpeg()
{
	LD_LIBRARY_PATH+=$BVBASE_DIR/ffmpeg/$ARCH/lib:
	EXENABLE+="--enable-libavformat "
	EXTCFLAGS+="-I$BVBASE_DIR/ffmpeg/$ARCH/include "
	EXTLFLAGS+="-L$BVBASE_DIR/ffmpeg/$ARCH/lib -lavformat -lavcodec -lavutil "
}

enable_jansson()
{
	LD_LIBRARY_PATH+=$BVBASE_DIR/jansson/$ARCH/lib:
	EXENABLE+="--enable-libjansson "
	EXTCFLAGS+="-I$BVBASE_DIR/jansson/$ARCH/include "
	EXTLFLAGS+="-L$BVBASE_DIR/jansson/$ARCH/lib -ljansson "
}

enable_his3515()
{
	CROSS_COMPILE=arm-hisi-linux-
	HOST=arm-hisi-linux
	EXTRA="--cpu=arm926ej-s --arch=armv5te"
	EXENABLE+="--enable-his3515 "
	EXTCFLAGS+="-I$BVBASE_DIR/hissdk/his3515/include "
	#FIXME
	EXTLFLAGS+="-L$BVBASE_DIR/hissdk/his3515/lib -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lresampler "
}

enable_x86()
{
	EXTCFLAGS+="-m32 "
	EXTLFLAGS+="-m32 "
}

case $1 in
    dm6446)
        CROSS_COMPILE=arm_v5t_le-
		HOST=arm_v5t_le
        ;;
	dm365)
		CROSS_COMPILE=arm-none-linux-gnueabi-
		HOST=arm-none-linux-gnueabi
		;;
	hi3515)
		enable_his3515
		;;
	hi3518)
		CROSS_COMPILE=arm-hisiv100nptl-linux-
		HOST=arm-hisiv100nptl-linux
        ;;
	hi3535)
		EXFLAGS="-march=armv7-a -mcpu=cortex-a9"
		CFLAGS+=$EXFLAGS
		CROSS_COMPILE=arm-hisiv100nptl-linux-
		HOST=arm-hisiv100nptl-linux
		;;
    x86)
		enable_x86
        ;;
	x64)
		;;
	*)
		usage
		;;
esac

enable_bvfs

enable_onvifc

enable_ffmpeg

enable_jansson

export LD_LIBRARY_PATH
CC=${CROSS_COMPILE}gcc
if [ "$1" != "x86" ];then
	./configure --prefix=$BVBASE_DIR/$NAME/$1 --enable-cross-compile --disable-doc --disable-debug --disable-manpages \
		--cross-prefix=$CROSS_COMPILE --target-os=linux  $EXTRA --extra-cflags="$EXTCFLAGS" --extra-ldflags="$EXTLFLAGS" \
		--disable-asm --disable-yasm --disable-zlib --disable-bzlib \
		--enable-shared --enable-small \
		$EXENABLE
else
./configure --prefix=$BVBASE_DIR/$NAME/$1 --disable-doc --disable-debug --disable-manpages \
		--disable-asm --disable-yasm  --disable-zlib --disable-bzlib \
		--enable-shared --enable-small  --extra-cflags="$EXTCFLAGS" --extra-ldflags="$EXTLFLAGS" \
		$EXENABLE
fi
#make && make install
