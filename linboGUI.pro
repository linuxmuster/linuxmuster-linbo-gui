TARGET = linbo_gui

VERSION = 7.0.1-Alpha03
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
    headers/frontend/dialogs \
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
    headers/frontend/components/qmoderntextbrowser.h \
    headers/frontend/dialogs/linboconfirmationdialog.h \
    headers/frontend/dialogs/linboimagecreationdialog.h \
    headers/frontend/dialogs/linbologindialog.h \
    headers/frontend/dialogs/linboregisterdialog.h \
    headers/frontend/dialogs/linboterminaldialog.h \
    headers/frontend/dialogs/linboupdatecachedialog.h \
    headers/frontend/linboclientinfo.h \
    headers/frontend/linbomainactions.h \
    headers/frontend/linbomainpage.h \
    headers/linbogui.h \
    headers/frontend/linboosselectbutton.h \
    headers/frontend/linboosselectionrow.h \
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
    sources/frontend/components/qmoderntextbrowser.cpp \
    sources/frontend/dialogs/linboconfirmationdialog.cpp \
    sources/frontend/dialogs/linboimagecreationdialog.cpp \
    sources/frontend/dialogs/linbologindialog.cpp \
    sources/frontend/dialogs/linboregisterdialog.cpp \
    sources/frontend/dialogs/linboterminaldialog.cpp \
    sources/frontend/dialogs/linboupdatecachedialog.cpp \
    sources/frontend/linboclientinfo.cpp \
    sources/frontend/linbomainactions.cpp \
    sources/frontend/linbomainpage.cpp \
    sources/linbogui.cpp \
    sources/frontend/linboosselectbutton.cpp \
    sources/frontend/linboosselectionrow.cpp \
    sources/main.cpp \
    sources/frontend/components/qmodernprogressbar.cpp \
    sources/frontend/components/qmodernpushbutton.cpp \
    sources/frontend/components/qmodernpushbuttonoverlay.cpp \
    sources/frontend/components/qmodernstackedwidget.cpp

RESOURCES += \
    resources/linbo.qrc

TRANSLATIONS += \
    resources/translations/blank.ts \
    resources/translations/de.ts
