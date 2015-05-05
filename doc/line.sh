#!/bin/bash
find ../libbvconfig/ ../libbvdevice/ ../libbvmedia/ ../libbvcodec/ ../libbvprotocol/ ../libbvserver/ ../libbvsystem/ ../libbvfilter -name "*.c" |xargs  wc -l
