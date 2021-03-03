TARGET = linbo_gui

VERSION = 7.0.0-rc06
DEFINES += GUI_VERSION=\"\\\"$${VERSION}\\\"\"

CONFIG += static
QT += gui widgets svg xml

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
    headers/frontend/components/linbocheckbox.h \
    headers/frontend/components/linbocombobox.h \
    headers/frontend/components/linbodialog.h \
    headers/frontend/components/linbolineedit.h \
    headers/frontend/components/linboprogressbar.h \
    headers/frontend/components/linbopushbutton.h \
    headers/frontend/components/linbopushbuttonoverlay.h \
    headers/frontend/components/linboradiobutton.h \
    headers/frontend/components/linbostackedwidget.h \
    headers/frontend/components/linbosvgwidget.h \
    headers/frontend/components/linboterminal.h \
    headers/frontend/components/linbotextbrowser.h \
    headers/frontend/components/linbotoolbutton.h \
    headers/frontend/dialogs/linboconfirmationdialog.h \
    headers/frontend/dialogs/linboimagecreationdialog.h \
    headers/frontend/dialogs/linboimageuploaddialog.h \
    headers/frontend/dialogs/linbologindialog.h \
    headers/frontend/dialogs/linboregisterdialog.h \
    headers/frontend/dialogs/linboterminaldialog.h \
    headers/frontend/dialogs/linboupdatecachedialog.h \
    headers/frontend/linboclientinfo.h \
    headers/frontend/linboguitheme.h \
    headers/frontend/linbomainactions.h \
    headers/frontend/linbomainpage.h \
    headers/linbogui.h \
    headers/frontend/linboosselectbutton.h \
    headers/frontend/linboosselectionrow.h

SOURCES += \
    sources/backend/linboconfig.cpp \
    sources/backend/linbodiskpartition.cpp \
    sources/backend/linboimage.cpp \
    sources/backend/linbologger.cpp \
    sources/backend/linboos.cpp \
    sources/backend/linbobackend.cpp \
    sources/frontend/components/linbocheckbox.cpp \
    sources/frontend/components/linbocombobox.cpp \
    sources/frontend/components/linbodialog.cpp \
    sources/frontend/components/linbolineedit.cpp \
    sources/frontend/components/linboprogressbar.cpp \
    sources/frontend/components/linbopushbutton.cpp \
    sources/frontend/components/linbopushbuttonoverlay.cpp \
    sources/frontend/components/linboradiobutton.cpp \
    sources/frontend/components/linbostackedwidget.cpp \
    sources/frontend/components/linbosvgwidget.cpp \
    sources/frontend/components/linboterminal.cpp \
    sources/frontend/components/linbotextbrowser.cpp \
    sources/frontend/components/linbotoolbutton.cpp \
    sources/frontend/dialogs/linboconfirmationdialog.cpp \
    sources/frontend/dialogs/linboimagecreationdialog.cpp \
    sources/frontend/dialogs/linboimageuploaddialog.cpp \
    sources/frontend/dialogs/linbologindialog.cpp \
    sources/frontend/dialogs/linboregisterdialog.cpp \
    sources/frontend/dialogs/linboterminaldialog.cpp \
    sources/frontend/dialogs/linboupdatecachedialog.cpp \
    sources/frontend/linboclientinfo.cpp \
    sources/frontend/linboguitheme.cpp \
    sources/frontend/linbomainactions.cpp \
    sources/frontend/linbomainpage.cpp \
    sources/linbogui.cpp \
    sources/frontend/linboosselectbutton.cpp \
    sources/frontend/linboosselectionrow.cpp \
    sources/main.cpp

RESOURCES += \
    resources/linbo.qrc \
    resources/translations/linbo-gui/translations.qrc

TRANSLATIONS += \
    resources/translations/linbo-gui/blank.ts
