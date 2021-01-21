#!/bin/sh
#
# copies the os icons into place
#
# dorian@itsblue.de
# 202101221
#

src_dir="$(pwd)"
deb_dir="$src_dir/debian"
icon_source_dir="$src_dir/resources/os-icons"
icon_dir="$deb_dir/linuxmuster-linbo-gui7/srv/linbo/icons"

mkdir -p "$icon_dir"

cp "$icon_source_dir"/* "$icon_dir"