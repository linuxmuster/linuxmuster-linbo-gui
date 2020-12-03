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

cd build

# build for 64-Bit
mkdir buildGUI64

# create the buildfile
cp build_Qt_and_Gui_generic buildGUI64/build.sh
cd buildGUI64

sed -i '/## 32: /d' build.sh
sed -i 's/## 64: //g' build.sh

./build.sh

cd ..

sleep 10

# build for 32-Bit
mkdir buildGUI32

# create the buildfile
cp build_Qt_and_Gui_generic buildGUI32/build.sh
cd buildGUI32

sed -i '/## 64: /d' build.sh
sed -i 's/## 32: //g' build.sh

./build.sh

cd ..

