#!/bin/sh
#
# builds the linbo_gui
#
# dorian@itsblue.de
# 202101221
#

src_dir="$(pwd)"
deb_dir="$src_dir/debian"

docker run --rm -v $src_dir:/workspace linuxmuster/linuxmuster-linbo-gui-build 
