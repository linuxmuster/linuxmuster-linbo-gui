TARGET = linbo_gui
DEPENDPATH += .
INCLUDEPATH += .
CFLAGS += -DQWS -static
QT += gui svg

QTPLUGIN.platforms = qlinuxfb
# have to set env: QT_QPA_EVDEV_MOUSE_PARAMETERS=/dev/input/event3
QTPLUGIN += qevdevmouseplugin

# define test Enviroment
TEST_ENV="\"$$PWD/fakeroot\""
DEFINES += TEST_ENV=\"\\\"$${TEST_ENV}\\\"\"

QMAKE_POST_LINK=strip $(TARGET)

# deployment
target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    headers/ \
    headers/backend \
    headers/frontend \
    headers/frontend/components

# Input
HEADERS += \
    headers/backend/linboconfig.h \
    headers/backend/linbodiskpartition.h \
    headers/backend/linboimage.h \
    headers/backend/linbologger.h \
    headers/backend/linboos.h \
    headers/backend/linbobackend.h \
    \
    headers/frontend/linbologindialog.h \
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
    sources/frontend/linbologindialog.cpp \
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

DISTFILES += \
    resources/fonts/Segoe UI Bold Italic.ttf \
    resources/fonts/Segoe UI Bold.ttf \
    resources/fonts/Segoe UI Italic.ttf \
    resources/fonts/Segoe UI.ttf