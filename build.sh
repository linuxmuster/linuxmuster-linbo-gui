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

vercomp () {
    if [[ $1 == $2 ]]
    then
        return 0
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    # fill empty fields in ver1 with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++))
    do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++))
    do
        if [[ -z ${ver2[i]} ]]
        then
            # fill empty fields in ver2 with zeros
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]}))
        then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]}))
        then
            return 2
        fi
    done
    return 0
}

checkInstalledVersion () {
	BIN=$1
	MIN_VERSION=$2

	if ! command -v $BIN &> /dev/null
	then
		echo "----------------------------------"
		echo "- $BIN was not found!           -"
		echo "- Please install $BIN >= $MIN_VERSION -"
		echo "----------------------------------"
		exit 1
	fi

	INSTALLED_VERSION=$($BIN --version 2>&1 | head -n 1 | awk '{print $NF}')
	vercomp $INSTALLED_VERSION $MIN_VERSION
	COMPARE_RESULT=$?

	if [[ $COMPARE_RESULT != 0 ]] && [[ $COMPARE_RESULT != 1 ]]; then
		echo "----------------------------------"
		echo "- $BIN $INSTALLED_VERSION not compatible!   -"
		echo "- Please install cmake >= $MIN_VERSION -"
		echo "----------------------------------"
		exit 1
	fi
}

# Check ubuntu version
. /etc/os-release
if [[ $VERSION_ID != "22.04" ]]; then
	echo "--------------------------------"
	echo "- Incompatible ubuntu version! -"
	echo "- You have to be on 22.04      -"
	echo "--------------------------------"
	exit 1
fi

#checkInstalledVersion cmake 3.21.1
#checkInstalledVersion gcc 9.0.0
#checkInstalledVersion g++ 9.0.0

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

# New architectures (like arm) could be added here
AllArchitecures=(64)
BuildArchitecures=(64)

for ARCH in "${BuildArchitecures[@]}"
do
    echo "Building linbo_gui7 for $ARCH"

	# build for 64-Bit
	mkdir buildGUI$ARCH

	# create the buildfile
	cp build_Qt_and_Gui_generic buildGUI$ARCH/build.sh
	cd buildGUI$ARCH

	sed -i "s/## $ARCH: //g" build.sh

	# remove all other architectures to prevent errors
	for NOT_ARCH in "${AllArchitecures[@]}"
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
sudo apt install debhelper -y

cd ..
./debian/mkdeb.sh

if [[ $? -ne 0 ]]; then
	echo "There was an error when building the deb package!"
	exit 1
fi

# copy all archives to a common dir
mkdir archives
cp ../linuxmuster-linbo-gui7_* ./archives

exit 0
