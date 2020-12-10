#!/bin/sh
#
# builds debian package
#
# thomas@linuxmuster.net
# 20201208
#
# to invoke in the buildroot: debian/mkdeb.sh

# with gpg signing key
#dpkg-buildpackage -rfakeroot -tc -sa -pgpg -k<fingerprint> \
dpkg-buildpackage -rfakeroot -tc -sa --no-sign \
  -i.git \
  -I.git \
  -Ibuild/*.tar.xz \
  -Ibuild/buildGUI* \
  -Ibuild/linbofs*
