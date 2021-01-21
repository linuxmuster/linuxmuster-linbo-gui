#!/bin/bash
## Linbo GUI global build srcipt
## Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU Affero General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Affero General Public License for more details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
## This script will build the linbo GUI for 32 and 64 bit.

# Check ubuntu version
if [[ $(lsb_release -rs) != "18.04" ]]; then
	echo "--------------------------------"
	echo "- Incompatible ubuntu version! -"
	echo "-   You have to be on 18.04    -"
	echo "--------------------------------"
	exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

cd build

BUILD_DEB=1

while test $# -gt 0
do
    case "$1" in
        --nodeb) BUILD_DEB=0
            ;;
    esac
    shift
done

Architecures=(64 32)

for ARCH in "${Architecures[@]}"
do
    echo "Building linbo_gui7 for $ARCH"

	# build for 64-Bit
	mkdir buildGUI$ARCH

	# create the buildfile
	cp build_Qt_and_Gui_generic buildGUI$ARCH/build.sh
	cd buildGUI$ARCH

	sed -i "s/## $ARCH: //g" build.sh

	# remove all other architectures to prevent errors
	for NOT_ARCH in "${Architecures[@]}"
	do
		sed -i "/## $NOT_ARCH: /d" build.sh
	done

	./build.sh "$@"

	if [[ $? -ne 0 ]]; then
	   echo "There was an error when building linbo_gui for $ARCH!"
	   exit 1
	fi
	
	# remove old linbofs folder
	rm -rf ../linbofs$ARCH
	
	# copy linbofs folder in place
	cp -r linbofs ../linbofs$ARCH

	cd ..

	# copy icons into linbofs folder
	cp -r ./icons  ./linbofs$ARCH

	echo "------------------------------------------------"
	echo "- linbo_gui7 for $ARCH was built successfully! -"
	echo "------------------------------------------------"

    sleep 5

done

rm -rf linbofs
mv linbofs32 linbofs

echo "--------------------------------------"
echo "- linbo_gui7 was built successfully! -"

if [[ $BUILD_DEB -ne 1 ]]; then
	echo "- Not building deb package, exiting  -"
	echo "--------------------------------------"
	exit 0
fi

echo "-   Now building debian package      -"
echo "--------------------------------------"

sudo apt update
sudo apt install debhelper zip -y

cd ..
./debian/mkdeb.sh

if [[ $? -ne 0 ]]; then
	echo "There was an error when building the deb package!"
	exit 1
fi

# copy all archives to a common dir and zip them
mkdir archives
cp ../linuxmuster-linbo-gui7_* ./archives

exit 0
