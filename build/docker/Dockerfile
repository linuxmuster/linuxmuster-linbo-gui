FROM ubuntu:22.04

MAINTAINER dorian@linuxmuster.net

ARG QT_EVERYWHERE_URL="https://download.qt.io/archive/qt/6.5/6.5.2/single/qt-everywhere-src-6.5.2.tar.xz"

WORKDIR /workspace

RUN echo "**** install dependencies ****" && \
  apt update && \
  DEBIAN_FRONTEND=noninteractive apt-get install -y --allow-downgrades \
    wget \
    build-essential \
    cmake \
    gcc-9 \
    g++-9 \
    libudev-dev \
    libudev1 \
    libinput-dev \
    libxkbcommon-dev \
    libmtdev-dev \
    libwacom-dev && \
  rm -rf /var/lib/apt/lists/*

# Download and build Qt
RUN echo "**** download Qt-everywhere ****" && \
  wget -O qt-everywhere.tar.xz ${QT_EVERYWHERE_URL} && \
  mkdir qt-everywhere && \
  echo "**** extract Qt-everywhere ****" && \
  tar -xf qt-everywhere.tar.xz --strip-components=1 -C ./qt-everywhere && \
  cd qt-everywhere && \
  export QT_QPA_DEFAULT_PLATFORM=linuxfb && \
  \
  echo "**** configure Qt ****" && \
  ./configure \
     -static \
     -prefix /usr/lib/Qt \
     -opensource \
     -confirm-license \
     -release \
     -qt-zlib \
     -qt-libpng \
     -libudev \
     -libinput \
     -feature-linguist \
     -feature-printdialog \
     -nomake examples \
     -no-gif \
     -no-libjpeg \
     -no-openssl \
     -no-dbus \
     -no-opengl \
     -no-glib \
     -no-cups \
     -no-accessibility \
     -no-libproxy \
     -no-sql-sqlite \
     -no-feature-textodfwriter \
     -no-feature-udpsocket \
     -no-feature-networkproxy \
     -no-feature-socks5 \
     -no-feature-networkdiskcache \
     -skip qt3d \
     -skip qtandroidextras \
     -skip qtcanvas3d \
     -skip qtcharts \
     -skip qtconnectivity \
     -skip qtdatavis3d \
     -skip qtdoc \
     -skip qtgamepad \
     -skip qtnetworkauth \
     -skip qtpurchasing \
     -skip qtwayland \
     -skip qtlocation \
     -skip qtscript \
     -skip qtdeclarative \
     -skip qtgrpc \
     -skip qtmultimedia \
     -skip qtquick \
     -skip qtquickcontrols \
     -skip qtquickcontrols2 \
     -skip qtsensors \
     -skip qtwinextras \
     -skip qtwebchannel \
     -skip qtwebengine \
     -skip qtserialbus \
     -skip qtserialport \
     -skip qtspeech \
     -skip qtx11extras \
     -skip qt5compat \
     -skip qtlottie \
     -skip qtshadertools \
     -skip qtmqtt \
     -skip qthttpserver \
     -skip qtwebsockets \
     -skip qtremoteobjects \
     -skip qtopcua \
     -skip qtquicktimeline \
     -skip qtquick3d \
     -skip qtquick3dphysics \
     -skip qtquickeffectmaker \
     -skip qtscxml \
     -skip qttranslations \
     -skip qtvirtualkeyboard \
     -skip qtwebview \
     -skip qtpositioning \
     -skip qtlanguageserver \
     -skip qtcoap && \
  \
  echo "**** build Qt ****" && \
  make -j8 && \	
  \
  echo "**** install Qt ****" && \
  make install && \
  \
  echo "**** clean sources ****" && \
  cd .. && \
  rm -rf qt-everywhere*

ENV CMAKE_PREFIX_PATH=/usr/lib/Qt
COPY ./linbo-gui-* /usr/local/bin/

CMD ["/usr/local/bin/linbo-gui-build"]
