Headers=" -I$(pwd)/3rdparty/binary/ffmpeg/x86/include -I$(pwd)/3rdparty/binary/jansson/x86/include" 
Headers+=" -I$(pwd)/3rdparty/binary/onvif/x86/include "

./configure --libdir=$(pwd)/lib/x86/ --shlibdir=$(pwd)/lib/x86/ --incdir=$(pwd)/include/x86 --extra-cflags="$Headers" --enable-shared
