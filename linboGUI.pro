TARGET = linbo_gui

VERSION = 7.0.1-Alpha01
DEFINES += GUI_VERSION=\"\\\"$${VERSION}\\\"\"

CONFIG += static
QT += gui widgets svg

# Include Linux framebuffer plugin
QTPLUGIN.platforms = qlinuxfb

# Don't define when in release mode
CONFIG(debug, debug | release) {
    # Testing enviroment
    TEST_ENV="\"$$PWD/fakeroot\""
    DEFINES += TEST_ENV=\"\\\"$${TEST_ENV}\\\"\"
}

# deployment
target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    headers/ \
    headers/backend \
    headers/frontend \
    headers/frontend/components

HEADERS += \
    headers/backend/linboconfig.h \
    headers/backend/linbodiskpartition.h \
    headers/backend/linboimage.h \
    headers/backend/linbologger.h \
    headers/backend/linboos.h \
    headers/backend/linbobackend.h \
    headers/frontend/components/qmoderndialog.h \
    headers/frontend/components/qmodernlineedit.h \
    headers/frontend/linboconfirmationdialog.h \
    headers/frontend/linbologindialog.h \
    headers/frontend/linboregisterdialog.h \
    headers/frontend/linboterminaldialog.h \
    headers/frontend/linboupdatecachedialog.h \
    headers/linbogui.h \
    headers/frontend/linboosselectbutton.h \
    headers/frontend/linboosselectionrow.h \
    headers/frontend/linbostartactions.h \
    headers/frontend/linbostartpage.h \
    headers/frontend/components/qmodernprogressbar.h \
    headers/frontend/components/qmodernpushbutton.h \
    headers/frontend/components/qmodernpushbuttonoverlay.h \
    headers/frontend/components/qmodernstackedwidget.h

SOURCES += \
    sources/backend/linboconfig.cpp \
    sources/backend/linbodiskpartition.cpp \
    sources/backend/linboimage.cpp \
    sources/backend/linbologger.cpp \
    sources/backend/linboos.cpp \
    sources/backend/linbobackend.cpp \
    sources/frontend/components/qmoderndialog.cpp \
    sources/frontend/components/qmodernlineedit.cpp \
    sources/frontend/linboconfirmationdialog.cpp \
    sources/frontend/linbologindialog.cpp \
    sources/frontend/linboregisterdialog.cpp \
    sources/frontend/linboterminaldialog.cpp \
    sources/frontend/linboupdatecachedialog.cpp \
    sources/linbogui.cpp \
    sources/frontend/linboosselectbutton.cpp \
    sources/frontend/linboosselectionrow.cpp \
    sources/frontend/linbostartactions.cpp \
    sources/frontend/linbostartpage.cpp \
    sources/main.cpp \
    sources/frontend/components/qmodernprogressbar.cpp \
    sources/frontend/components/qmodernpushbutton.cpp \
    sources/frontend/components/qmodernpushbuttonoverlay.cpp \
    sources/frontend/components/qmodernstackedwidget.cpp

RESOURCES += \
    resources/linbo.qrc
