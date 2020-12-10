#!/bin/sh
#
# mkarchives.sh: Creates 32 & 64bit linbo_gui archives
#
# thomas@linuxmuster.net
# 20201208
#

src_dir="$(pwd)"
deb_dir="$src_dir/debian"
build_dir="$src_dir/build"
build32_dir="$build_dir/linbofs"
build64_dir="$build_dir/linbofs64"
arc_dir="$deb_dir/linuxmuster-linbo-gui7/srv/linbo"

rm -rf "$arc_dir"
mkdir -p "$arc_dir"

for i in "$build32_dir" "$build64_dir"; do
  case "$i" in
    *64) bits="64" ; source="$build64_dir" ;;
    *) bits="32" ; source="$build32_dir" ;;
  esac
  archive="${arc_dir}/linbo_gui${bits}_7.tar.lz"
  cd "$i"
  echo -n "Creating $(basename $archive) ... "
  tar --lzma -cf "$archive" * || exit 1
  md5sum "$archive" | awk '{print $1}' > "$archive.md5" || exit 1
  echo "Done!"
  cd "$src_dir"
done
