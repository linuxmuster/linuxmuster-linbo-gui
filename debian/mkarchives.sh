#!/bin/sh
#
# mkarchives.sh: Creates 32 & 64bit linbo_gui archives
#
# thomas@linuxmuster.net
# 20201219
#

src_dir="$(pwd)"
deb_dir="$src_dir/debian"
build_dir="$src_dir/build"
arc_dir="$deb_dir/linuxmuster-linbo-gui7/srv/linbo"
initramfs_base_url="https://raw.githubusercontent.com/linuxmuster/linuxmuster-linbo/master/conf"
tmp_dir="$deb_dir/tmp.$$"

rm -rf "$arc_dir"
mkdir -p "$arc_dir"
mkdir -p "$tmp_dir"

# libs to exclude in any case
libs_ex_any="libc.so.6\nlibpthread.so.0"
libs_ex="$tmp_dir/libs_ex"

initramfs_url="$initramfs_base_url/initramfs64.conf"
source="$build_dir/linbofs"

# get initramfs.conf
initramfs_conf="$tmp_dir/$(basename "$initramfs_url")"
wget "$initramfs_url" -O "$initramfs_conf"

# get existing libs
libs="$(ls -1 "$source/lib/")"
if [ -s "$initrams_conf" -a -n "$libs" ]; then
rm -f "$libs_ex"

# exclude libs which are already in linbofs by looking in initramfs.conf
for l in $libs; do
grep -q "$l" "$initramfs_conf" && echo "$l" >> "$libs_ex"
done
else
echo "$libs_ex_any" > "$libs_ex"
fi

# Make sure linbo_gui is executable (just in case)
chmod +x $source/usr/bin/linbo_gui

# Build archive
archive="${arc_dir}/linbo_gui64_7.tar.lz"
cd "$source"
echo -n "Creating $(basename $archive) ... "
tar --lzma -X "$libs_ex" -cf "$archive" * || exit 1
md5sum "$archive" | awk '{print $1}' > "$archive.md5" || exit 1

echo "Done!"

rm -rf "$tmp_dir"
