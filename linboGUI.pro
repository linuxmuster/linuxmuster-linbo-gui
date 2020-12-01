TARGET = linbo_gui
DEPENDPATH += .
INCLUDEPATH += .
CFLAGS += -DQWS -static
QT += gui svg

# define test Enviroment
TEST_ENV="\"$$PWD/fakeroot\""
message($$TEST_ENV)
DEFINES += TEST_ENV=\"\\\"$${TEST_ENV}\\\"\"

QMAKE_POST_LINK=strip $(TARGET)

# deployment
target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    headers/ \
    headers/backend

# Input
HEADERS += \
    headers/backend/linboconfig.h \
    headers/backend/linbodiskpartition.h \
    headers/backend/linboimage.h \
    headers/backend/linbologger.h \
    headers/backend/linboos.h \
    headers/backend/linbobackend.h \
    headers/linbogui.h \
    headers/linboosselectbutton.h \
    headers/linboosselectionrow.h \
    headers/linbostartactions.h \
    headers/linbostartpage.h \
    headers/qmodernprogressbar.h \
    headers/qmodernpushbutton.h \
    headers/qmodernpushbuttonoverlay.h \
    headers/qmodernstackedwidget.h

SOURCES += \
    sources/backend/linboconfig.cpp \
    sources/backend/linbodiskpartition.cpp \
    sources/backend/linboimage.cpp \
    sources/backend/linbologger.cpp \
    sources/backend/linboos.cpp \
    sources/backend/linbobackend.cpp \
    sources/linbogui.cpp \
    sources/linboosselectbutton.cpp \
    sources/linboosselectionrow.cpp \
    sources/linbostartactions.cpp \
    sources/linbostartpage.cpp \
    sources/main.cpp \
    sources/qmodernprogressbar.cpp \
    sources/qmodernpushbutton.cpp \
    sources/qmodernpushbuttonoverlay.cpp \
    sources/qmodernstackedwidget.cpp

RESOURCES += \
    resources/linbo.qrc

DISTFILES += \
    resources/fonts/Segoe UI Bold Italic.ttf \
    resources/fonts/Segoe UI Bold.ttf \
    resources/fonts/Segoe UI Italic.ttf \
    resources/fonts/Segoe UI.ttf
