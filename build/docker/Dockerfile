FROM ubuntu:22.04

MAINTAINER dorian@linuxmuster.net

ARG QT_EVERYWHERE_URL="https://download.qt.io/archive/qt/6.2/6.2.2/single/qt-everywhere-src-6.2.2.tar.xz"

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
  ls -l . && \
  export QT_QPA_PLATFORM=linuxfb && \
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
     -no-feature-calendarwidget \
     -no-feature-printpreviewwidget \
     -no-feature-colordialog \
     -no-feature-filedialog \
     -no-feature-fontdialog \
     -no-feature-printpreviewdialog \
     -no-feature-progressdialog \
     -no-feature-wizard \
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
     -skip qtlocation \
     -skip qtmultimedia \
     -skip qtquickcontrols \
     -skip qtquickcontrols2 \
     -skip qtsensors \
     -skip qtwebsockets \
     -skip qtwinextras \
     -skip qtwebchannel \
     -skip qtwebengine \
     -skip qttools \
     -skip qtserialbus \
     -skip qtserialport \
     -skip qtspeech \
     -skip qtx11extras \
     -skip qt5compat \
     -skip qtlottie \
     -skip qtmqtt \
     -skip qtopcua \
     -skip qtquicktimeline \
     -skip qtquick3d \
     -skip qtscxml \
     -skip qttranslations \
     -skip qtvirtualkeyboard \
     -skip qtwebview \
     -skip qtpositioning \
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

COPY ./build-linbo-gui /usr/local/bin/

ENTRYPOINT ["/usr/local/bin/build-linbo-gui"]