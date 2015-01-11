#!/bin/bash

if [ $# != 1 ]; then
    echo "$0 x86/dm6446/dm365/hi3515/hi3535"
    exit
fi

BVBASE_DIR=/home/albert/selfgit/binary
NAME=bvbase

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
		CROSS_COMPILE=arm-hisi-linux-
		HOST=arm-hisi-linux
		EXTRA="--cpu=arm926ej-s --arch=armv5te"
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
        ;;
esac

#export PKG_CONFIG_PATH=$BVBASE_DIR/freetype/$1/lib/pkgconfig


EXTCFLAGS="-I$BVBASE_DIR/ffmpeg/$1/include -I$BVBASE_DIR/onvif/$1/include -I$BVBASE_DIR/openssl/$1/include"
EXTLFLAGS="-L$BVBASE_DIR/ffmpeg/$1/lib -lavformat -lavcodec -lavutil -L$BVBASE_DIR/onvif/$1/lib -lonvifc -L$BVBASE_DIR/openssl/$1/lib -lssl -lcrypto"

export LD_LIBRARY_PATH=$BVBASE_DIR/ffmpeg/$1/lib:$BVBASE_DIR/onvif/$1/lib:$BVBASE_DIR/openssl/$1/lib
EXENABLE="--enable-libavformat --enable-libonvifc "

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
